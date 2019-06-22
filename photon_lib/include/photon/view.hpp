/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_VIEW_AUGUST_15_2016)
#define CYCFI_PHOTON_GUI_LIB_VIEW_AUGUST_15_2016

#include <photon/base_view.hpp>
#include <photon/support/rect.hpp>
#include <photon/support/canvas.hpp>
#include <photon/support/theme.hpp>
#include <photon/element/element.hpp>
#include <photon/element/layer.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <unordered_map>
#include <chrono>

namespace cycfi { namespace photon
{
   struct context;
   class window;
   class idle_tasks;

   class view : public base_view
   {
   public:
                           view(host_window h);
                           view(window& win);
                           ~view();

      virtual void         draw(cairo_t* ctx, rect area) override;
      virtual void         click(mouse_button btn) override;
      virtual void         drag(mouse_button btn) override;
      virtual void         cursor(point p, cursor_tracking status) override;
      virtual void         scroll(point dir, point p) override;
      virtual void         key(key_info const& k) override;
      virtual void         text(text_info const& info) override;
      virtual void         focus(focus_request r) override;
      virtual void         poll() override;

      virtual void         refresh() override;
      virtual void         refresh(rect area) override;
      void                 refresh(element& element);
      void                 refresh(context const& ctx);
      rect                 dirty() const;

      struct undo_redo_task
      {
         std::function<void()> undo;
         std::function<void()> redo;
      };

      void                 add_undo(undo_redo_task t);
      bool                 has_undo();
      bool                 has_redo();
      bool                 undo();
      bool                 redo();

      using content_type = layer_composite;
      using layers_type = layer_composite::container_type;

      content_type&        content();
      content_type const&  content() const;
      void                 content(layers_type&& layers);
      view_limits          limits() const;
      mouse_button         current_button() const;

      using change_limits_function = std::function<void(view_limits limits_)>;
      change_limits_function on_change_limits;

      using io_context = boost::asio::io_context;
      io_context&          io();

                           template <typename T, typename F>
      void                 defer(T duration, F f);

   private:

      layer_composite      _content;

      void                 set_limits();

      rect                 _dirty;
      rect                 _current_bounds;
      view_limits          _current_limits = { { 0, 0 }, { full_extent, full_extent} };
      mouse_button         _current_button;

      using undo_stack_type = std::stack<undo_redo_task>;
      undo_stack_type      _undo_stack;
      undo_stack_type      _redo_stack;

      io_context           _io;
      io_context::work     _work;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
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
   inline void view::defer(T duration, F f)
   {
      auto timer = std::make_shared<boost::asio::steady_timer>(_io);
      timer->expires_from_now(duration);
      timer->async_wait(
         [timer, f](auto const& err)
         {
            if (!err)
               f();
         }
      );
   }
}}

#endif
