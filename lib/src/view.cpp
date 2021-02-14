/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/view.hpp>
#include <elements/window.hpp>
#include <elements/support/context.hpp>

 namespace cycfi { namespace elements
 {
   view::view(extent size_)
    : base_view(size_)
    , _main_element(make_scaled_content())
    , _work(_io)
   {}

   view::view(host_view_handle h)
    : base_view(h)
    , _main_element(make_scaled_content())
    , _work(_io)
   {}

   view::view(window& win)
    : base_view(win.host())
    , _main_element(make_scaled_content())
    , _work(_io)
   {
      on_change_limits = [&win](view_limits limits_)
      {
         win.limits(limits_);
      };
      win.limits(_current_limits);
   }

   view::~view()
   {
      _io.stop();
   }

   void view::set_limits()
   {
      if (_content.empty())
         return;

      auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
      auto context_ = cairo_create(surface_);
      canvas cnv{ *context_ };
      cnv.pre_scale(hdpi_scale());

      // Update the limits and constrain the window size to the limits
      basic_context bctx{ *this, cnv };
      auto limits_ = _main_element.limits(bctx);
      if (limits_.min != _current_limits.min || limits_.max != _current_limits.max)
      {
         _current_limits = limits_;
         if (on_change_limits)
            on_change_limits(limits_);
      }

      cairo_surface_destroy(surface_);
      cairo_destroy(context_);
   }

   void view::draw(cairo_t* context_, rect dirty_)
   {
      if (_content.empty())
         return;

      _dirty = dirty_;

      // Update the limits and constrain the window size to the limits
      set_limits();

      canvas cnv{ *context_ };
      cnv.pre_scale(hdpi_scale());
      auto size_ = size();
      rect subj_bounds = { 0, 0, size_.x, size_.y };
      context ctx{ *this, cnv, &_main_element, subj_bounds };

      // layout the subject only if the window bounds changes
      if (subj_bounds != _current_bounds)
      {
         _current_bounds = subj_bounds;
         _main_element.layout(ctx);
      }

      // draw the subject
      _main_element.draw(ctx);
   }

   namespace
   {
      template <typename F, typename This>
      void call(F f, This& self, rect _current_bounds)
      {
         auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
         auto context_ = cairo_create(surface_);
         canvas cnv{ *context_ };
         cnv.pre_scale(self.hdpi_scale());
         context ctx { self, cnv, &self.main_element(), _current_bounds };

         f(ctx, self.main_element());

         cairo_surface_destroy(surface_);
         cairo_destroy(context_);
      }
   }

   void view::layout()
   {
      if (_current_bounds.is_empty())
         return;

      call(
         [](auto const& ctx, auto& _main_element) { _main_element.layout(ctx); },
         *this, _current_bounds
      );

      refresh();
   }

   void view::layout(element &element)
   {
      if (_current_bounds.is_empty())
         return;

      call(
         [](auto const& ctx, auto& _main_element) { _main_element.layout(ctx); },
         *this, _current_bounds
      );

      refresh(element);
   }

   float view::scale() const
   {
      return _main_element.scale();
   }

   void view::scale(float val)
   {
      _main_element.scale(val);
      refresh();
   }

   void view::refresh()
   {
      // Allow refresh to be called from another thread
      _io.post(
         [this]()
         {
            base_view::refresh();
         }
      );
   }

   void view::refresh(rect area)
   {
      // Allow refresh to be called from another thread
      _io.post(
         [this, area]()
         {
            base_view::refresh(area);
         }
      );
   }

   void view::refresh(element& element, int outward)
   {
      if (_current_bounds.is_empty())
         return;

      _io.post(
         [this, &element, outward]()
         {
            call(
               [&element, outward](auto const& ctx, auto& _main_element)
               {
                  _main_element.refresh(ctx, element, outward);
               },
               *this, _current_bounds
            );
         }
      );
   }

   void view::refresh(context const& ctx, int outward)
   {
      context const* ctx_ptr = &ctx;
      while (outward > 0 && ctx_ptr)
      {
         --outward;
         ctx_ptr = ctx_ptr->parent;
      }
      if (ctx_ptr)
      {
         auto tl = ctx.canvas.user_to_device(ctx_ptr->bounds.top_left());
         auto br = ctx.canvas.user_to_device(ctx_ptr->bounds.bottom_right());
         refresh({ tl.x, tl.y, br.x, br.y });
      }
   }

   void view::click(mouse_button btn)
   {
      _current_button = btn;
      if (_content.empty())
         return;

      call(
         [btn, this](auto const& ctx, auto& _main_element)
         {
            _main_element.click(ctx, btn);
            _is_focus = _main_element.focus();
         },
         *this, _current_bounds
      );
   }

   void view::drag(mouse_button btn)
   {
      _current_button = btn;
      if (_content.empty())
         return;

      call(
         [btn](auto const& ctx, auto& _main_element)
         {
            _main_element.drag(ctx, btn);
         },
         *this, _current_bounds
      );
   }

   void view::cursor(point p, cursor_tracking status)
   {
      if (_content.empty())
         return;

      call(
         [p, status](auto const& ctx, auto& _main_element)
         {
            if (!_main_element.cursor(ctx, p, status))
               set_cursor(cursor_type::arrow);
         },
         *this, _current_bounds
      );
   }

   void view::scroll(point dir, point p)
   {
      if (_content.empty())
         return;

      call(
         [dir, p](auto const& ctx, auto& _main_element)
         {
            _main_element.scroll(ctx, dir, p);
         },
         *this, _current_bounds
      );
   }

   bool view::key(key_info const& k)
   {
      if (_content.empty())
         return false;

      bool handled = false;
      call(
         [k, &handled](auto const& ctx, auto& _main_element)
         {
             handled = _main_element.key(ctx, k);
         },
         *this, _current_bounds
      );
      return handled;
   }

   bool view::text(text_info const& info)
   {
      if (_content.empty())
         return false;

      bool handled = false;
      call(
         [info, &handled](auto const& ctx, auto& _main_element)
         {
             handled = _main_element.text(ctx, info);
         },
         *this, _current_bounds
      );
      return handled;
   }

   void view::add_undo(undo_redo_task f)
   {
      _undo_stack.push(f);
      if (has_redo())
      {
         // clear the redo stack
         undo_stack_type empty{};
         _redo_stack.swap(empty);
      }
   }

   bool view::undo()
   {
      if (has_undo())
      {
         auto t = _undo_stack.top();
         _undo_stack.pop();
         _redo_stack.push(t);
         t.undo();  // execute undo function
         return true;
      }
      return false;
   }

   bool view::redo()
   {
      if (has_redo())
      {
         auto t = _redo_stack.top();
         _undo_stack.push(t);
         _redo_stack.pop();
         t.redo();  // execute redo function
         return true;
      }
      return false;
   }

   void view::begin_focus()
   {
      if (_content.empty() || !_is_focus)
         return;

      _main_element.begin_focus();
      refresh();
   }

   void view::end_focus()
   {
      if (_content.empty() || !_is_focus)
         return;

      _main_element.end_focus();
      refresh();
   }

   void view::poll()
   {
      _io.poll();
      if (!_tracking.empty())
      {
         for (auto it = _tracking.cbegin(); it != _tracking.cend(); /**/)
         {
            using namespace std::chrono_literals;
            auto now = std::chrono::steady_clock::now();
            if ((now - it->second) > 1s)
            {
               on_tracking(*it->first, tracking::end_tracking);
               _tracking.erase(it++);
            }
            else
            {
               ++it;
            }
         }
      }
   }

   void view::manage_on_tracking(element& e, tracking state)
   {
      // Simulate a begin_tracking if needed
      if (_tracking.find(&e) == _tracking.end() && state == tracking::while_tracking)
         on_tracking(e, tracking::begin_tracking);

      _tracking[&e] = std::chrono::steady_clock::now();
      on_tracking(e, state);

      if (state == tracking::end_tracking)
         _tracking.erase(&e);
   }
}}
