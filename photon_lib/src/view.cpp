/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/view.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   view::view(host_view* h)
    : base_view(h)
   {
      set_limits();
   }

   bool view::set_limits()
   {
      auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
      auto context_ = cairo_create(surface_);
      canvas cnv{ *context_ };
      bool resized = false;

      // Update the limits and constrain the window size to the limits
      basic_context bctx{ *this, cnv };
      auto limits_ = content.limits(bctx);
      if (limits_.min != _current_limits.min || limits_.max != _current_limits.max)
      {
         auto size_ = size();
         auto orig_size_ = size_;

         _current_limits = limits_;
         limits(limits_);

         clamp(size_.x, limits_.min.x, limits_.max.x);
         clamp(size_.y, limits_.min.y, limits_.max.y);

         if (size_ != orig_size_)
         {
            size(size_);
            size_ = size();
            resized = true;
         }
      }

      cairo_surface_destroy(surface_);
      cairo_destroy(context_);
      return resized;
   }

   void view::draw(cairo_t* context_, rect dirty_)
   {
      _dirty = dirty_;

      // Update the limits and constrain the window size to the limits
      if (set_limits())
         return; // return early if the window was resized.

      canvas cnv{ *context_ };
      auto size_ = size();
      rect subj_bounds = { 0, 0, size_.x, size_.y };
      context ctx{ *this, cnv, &content, subj_bounds };

      // layout the subject only if the window bounds changes
      if (subj_bounds != _current_bounds)
      {
         _current_bounds = subj_bounds;
         content.layout(ctx);
      }

      // draw the subject
      content.draw(ctx);
   }

   namespace
   {
      template <typename F, typename This>
      void call(F f, This& self, rect _current_bounds)
      {
         auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
         auto context_ = cairo_create(surface_);
         canvas cnv{ *context_ };
         context ctx { self, cnv, &self.content, _current_bounds };

         f(ctx, self.content);

         cairo_surface_destroy(surface_);
         cairo_destroy(context_);
      }
   }

   void view::refresh(element& element)
   {
      call(
         [&element](auto const& ctx, auto& content) { content.refresh(ctx, element); },
         *this, _current_bounds
      );
   }

   void view::refresh(context const& ctx)
   {
      refresh(ctx.bounds);
   }

   void view::click(mouse_button btn)
   {
      call(
         [btn](auto const& ctx, auto& content) { content.click(ctx, btn); },
         *this, _current_bounds
      );
   }

   void view::drag(mouse_button btn)
   {
      call(
         [btn](auto const& ctx, auto& content) { content.drag(ctx, btn); },
         *this, _current_bounds
      );
   }

   void view::cursor(point p, cursor_tracking status)
   {
      call(
         [p, status](auto const& ctx, auto& content)
         {
            if (!content.cursor(ctx, p, status))
               set_cursor(cursor_type::arrow);
         },
         *this, _current_bounds
      );
   }

   void view::scroll(point dir, point p)
   {
      call(
         [dir, p](auto const& ctx, auto& content) { content.scroll(ctx, dir, p); },
         *this, _current_bounds
      );
   }

   void view::key(key_info const& k)
   {
      call(
         [k](auto const& ctx, auto& content) { content.key(ctx, k); },
         *this, _current_bounds
      );
   }

   void view::text(text_info const& info)
   {
      call(
         [info](auto const& ctx, auto& content) { content.text(ctx, info); },
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
      content.focus(r);
      refresh();
   }
}
