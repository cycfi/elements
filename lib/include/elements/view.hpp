/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_VIEW_AUGUST_15_2016)
#define ELEMENTS_VIEW_AUGUST_15_2016

#include <elements/base_view.hpp>
#include <elements/support/rect.hpp>
#include <elements/support/canvas.hpp>
#include <elements/support/theme.hpp>
#include <elements/element/element.hpp>
#include <elements/element/layer.hpp>
#include <elements/element/size.hpp>
#include <elements/element/indirect.hpp>
#include <asio.hpp>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <map>

namespace cycfi { namespace elements
{
   class context;
   class window;
   class idle_tasks;

   class view : public base_view
   {
   public:
                              view(extent size_);
                              view(host_view_handle h);
                              view(window& win);
                              ~view();

      void                    draw(cairo_t* ctx, rect area) override;
      void                    click(mouse_button btn) override;
      void                    drag(mouse_button btn) override;
      void                    cursor(point p, cursor_tracking status) override;
      void                    scroll(point dir, point p) override;
      bool                    key(key_info const& k) override;
      bool                    text(text_info const& info) override;
      void                    begin_focus() override;
      void                    end_focus() override;
      void                    poll() override;

      void                    layout();
      void                    layout(element& element);
      float                   scale() const;
      void                    scale(float val);

      void                    refresh() override;
      void                    refresh(rect area) override;
      void                    refresh(element& element, int outward = 0);
      void                    refresh(context const& ctx, int outward = 0);
      rect                    dirty() const;

      struct undo_redo_task
      {
         std::function<void()> undo;
         std::function<void()> redo;
      };

      void                    add_undo(undo_redo_task t);
      bool                    has_undo();
      bool                    has_redo();
      bool                    undo();
      bool                    redo();

      using content_type = layer_composite;
      using layers_type = layer_composite::container_type;
      using scaled_content = scale_element<indirect<reference<layer_composite>>>;

      scaled_content&         main_element()         { return _main_element; }
      scaled_content const&   main_element() const   { return _main_element; }

      content_type&           content();
      content_type const&     content() const;
      void                    content(std::initializer_list<element_ptr> list);

                              template <typename... E>
      void                    content(E&&... elements);

      using layers_vector = std::vector<element_ptr>;

      void                    add(element_ptr e);
      void                    remove(element_ptr e);
      void                    move_to_front(element_ptr e);
      bool                    is_open(element_ptr e);
      layers_vector const&    layers() const;

      view_limits             limits() const;
      mouse_button            current_button() const;

      using change_limits_function = std::function<void(view_limits limits_)>;
      change_limits_function on_change_limits;

      using io_context = asio::io_context;
      io_context&             io();

                              template <typename T, typename F>
      void                    post(T duration, F f);

                              template <typename F>
      void                    post(F f);

      using tracking = element::tracking;

      using track_function = std::function<void(element& e, tracking state)>;
      track_function on_tracking = [](element& /* e */, tracking /* state */) {};

      void                    manage_on_tracking(element& e, tracking state);

   private:

      scaled_content          make_scaled_content() { return elements::scale(1.0, link(_content)); }

      layer_composite         _content;
      scaled_content          _main_element;

      void                    set_limits();

      rect                    _dirty;
      rect                    _current_bounds;
      view_limits             _current_limits = { { 0, 0 }, { full_extent, full_extent} };
      mouse_button            _current_button;
      bool                    _is_focus = false;

      using undo_stack_type = std::stack<undo_redo_task>;
      undo_stack_type         _undo_stack;
      undo_stack_type         _redo_stack;

      io_context              _io;
      io_context::work        _work;

      using time_point = std::chrono::steady_clock::time_point;
      using tracking_map = std::map<element*, time_point>;

      tracking_map            _tracking;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline point cursor_pos(view const& v) // declared in context.hpp
   {
      return v.cursor_pos();
   }

   inline rect view_bounds(view const& v) // declared in context.hpp
   {
      auto size = v.size();
      return rect{ 0, 0, size.x, size.y };
   }

   inline rect view::dirty() const
   {
      return _dirty;
   }

   inline bool view::has_undo()
   {
      return !_undo_stack.empty();
   }

   inline bool  view::has_redo()
   {
      return !_redo_stack.empty();
   }

   inline view::content_type& view::content()
   {
      return _content;
   }

   inline view::content_type const& view::content() const
   {
      return _content;
   }

   inline void view::content(std::initializer_list<element_ptr> list)
   {
      _content = list;
      std::reverse(_content.begin(), _content.end());
      set_limits();
   }

   namespace detail
   {
      template <typename E>
      inline element_ptr add_element(E&& e)
      {
         return share(std::forward<E>(e));
      }

      template <typename E>
      inline std::shared_ptr<E> add_element(std::shared_ptr<E> ep)
      {
         return ep;
      }
   }

   template <typename... E>
   inline void view::content(E&&... elements)
   {
      _content = { detail::add_element(std::forward<E>(elements))... };
      std::reverse(_content.begin(), _content.end());
      set_limits();
   }

   inline void view::add(element_ptr e)
   {
      // We'll defer this call just to be safe, to give the trigger that
      // initiated this call (e.g. button on_click) a chance to return.
      if (e)
      {
         if (_content.empty()
            || std::find(_content.begin(), _content.end(), e) != _content.end())
            return;

         io().post(
            [e, this]
            {
               end_focus();
               _content.push_back(e);
               layout(*e);
               begin_focus();
            }
         );
      }
   }

   inline void view::remove(element_ptr e)
   {
      // We want to dismiss the element, but we can't do it immediately
      // because we need to retain the trigger that initiated this call (e.g.
      // button on_click), otherwise there's nothing to return to. So, we
      // post a function that is called at idle time.
      if (e)
      {
         io().post(
            [e, this]
            {
               auto i = std::find(_content.begin(), _content.end(), e);
               if (i != _content.end())
               {
                  end_focus();
                  refresh(*e);
                  _content.erase(i);
                  _content.reset();
                  layout();
                  begin_focus();
               }
            }
         );
      }
   }

   inline bool view::is_open(element_ptr e)
   {
      auto i = std::find(_content.begin(), _content.end(), e);
      return i != _content.end();
   }

   inline void view::move_to_front(element_ptr e)
   {
      if (e && _content.back() != e)
      {
         io().post(
            [e, this]
            {
               auto i = std::find(_content.begin(), _content.end(), e);
               if (i != _content.end())
               {
                  end_focus();
                  _content.erase(i);
                  _content.insert(_content.end(), e);
                  _content.reset();
                  layout();
                  begin_focus();
               }
            }
         );
      }
   }

   inline view::layers_vector const& view::layers() const
   {
      return _content;
   }

   inline view_limits view::limits() const
   {
      return _current_limits;
   }

   inline view::io_context& view::io()
   {
      return _io;
   }

   inline mouse_button view::current_button() const
   {
      return _current_button;
   }

   template <typename T, typename F>
   inline void view::post(T duration, F f)
   {
      auto timer = std::make_shared<asio::steady_timer>(_io);
      timer->expires_from_now(duration);
      timer->async_wait(
         [timer, f](auto const& err)
         {
            if (!err)
               f();
         }
      );
   }

   template <typename F>
   inline void view::post(F f)
   {
      _io.post(f);
   }
}}

#endif
