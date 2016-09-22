/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   void view::draw(rect dirty_)
   {
      auto context_ = setup_context();

      _dirty = dirty_;

      canvas cnv{ *context_ };
      basic_context bctx{ *this, cnv };
      auto limits_ = content.limits(bctx);
      limits(limits_);

      auto size_ = size();
      clamp(size_.x, limits_.min.x, limits_.max.x);
      clamp(size_.y, limits_.min.y, limits_.max.y);
      if (size_ != size())
         size(size_);

      rect subj_bounds = { 0, 0, float(size_.x), float(size_.y) };

      // layout the subject only if the window bounds changes
      context ctx{ *this, cnv, &content, subj_bounds };
      if (subj_bounds != _current_bounds)
      {
         _current_bounds = subj_bounds;
         content.layout(ctx);
      }

      // draw the subject
      content.draw(ctx);

      cairo_destroy(context_);
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
         [p, status](auto const& ctx, auto& content) { content.cursor(ctx, p, status); },
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
}
