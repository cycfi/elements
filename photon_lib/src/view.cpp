/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/view.hpp>
#include <photon/window.hpp>
#include <photon/support/context.hpp>

 namespace cycfi { namespace photon
 {
   view::view(host_window h)
    : base_view(h)
    , _work(_io)
   {}

   view::view(window& win)
    : base_view(win.host())
    , _work(_io)
   {
      on_change_limits = [&win](view_limits limits_)
      {
         win.limits(limits_);
      };
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
      bool resized = false;

      // Update the limits and constrain the window size to the limits
      basic_context bctx{ *this, cnv };
      auto limits_ = _content.limits(bctx);
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
      auto size_ = size();
      rect subj_bounds = { 0, 0, size_.x, size_.y };
      context ctx{ *this, cnv, &_content, subj_bounds };

      // layout the subject only if the window bounds changes
      if (subj_bounds != _current_bounds)
      {
         _current_bounds = subj_bounds;
         _content.layout(ctx);
      }

      // draw the subject
      _content.draw(ctx);
   }

   namespace
   {
      template <typename F, typename This>
      void call(F f, This& self, rect _current_bounds)
      {
         auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
         auto context_ = cairo_create(surface_);
         canvas cnv{ *context_ };
         context ctx { self, cnv, &self.content(), _current_bounds };

         f(ctx, self.content());

         cairo_surface_destroy(surface_);
         cairo_destroy(context_);
      }
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

   void view::refresh(element& element)
   {
      call(
         [&element](auto const& ctx, auto& _content) { _content.refresh(ctx, element); },
         *this, _current_bounds
      );
   }

   void view::refresh(context const& ctx)
   {
      refresh(ctx.bounds);
   }

   void view::click(mouse_button btn)
   {
      _current_button = btn;
      if (_content.empty())
         return;

      call(
         [btn](auto const& ctx, auto& _content) { _content.click(ctx, btn); },
         *this, _current_bounds
      );
   }

   void view::drag(mouse_button btn)
   {
      _current_button = btn;
      if (_content.empty())
         return;

      call(
         [btn](auto const& ctx, auto& _content) { _content.drag(ctx, btn); },
         *this, _current_bounds
      );
   }

   void view::cursor(point p, cursor_tracking status)
   {
      if (_content.empty())
         return;

      call(
         [p, status](auto const& ctx, auto& _content)
         {
            if (!_content.cursor(ctx, p, status))
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
         [dir, p](auto const& ctx, auto& _content) { _content.scroll(ctx, dir, p); },
         *this, _current_bounds
      );
   }

   void view::key(key_info const& k)
   {
      if (_content.empty())
         return;

      call(
         [k](auto const& ctx, auto& _content) { _content.key(ctx, k); },
         *this, _current_bounds
      );
   }

   void view::text(text_info const& info)
   {
      if (_content.empty())
         return;

      call(
         [info](auto const& ctx, auto& _content) { _content.text(ctx, info); },
         *this, _current_bounds
      );
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

   void view::focus(focus_request r)
   {
      if (_content.empty())
         return;

      _content.focus(r);
      refresh();
   }

   void view::content(layers_type&& layers)
   {
      _content = std::forward<layers_type>(layers);
      set_limits();
   }

   void view::poll()
   {
      _io.poll();
   }
}}
