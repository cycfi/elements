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

      basic_context bctx{ *this, *context_ };
      auto limits_ = content.limits(bctx);
      limits(limits_);

      auto size_ = size();
      clamp(size_.x, limits_.min.x, limits_.max.x);
      clamp(size_.y, limits_.min.y, limits_.max.y);
      if (size_ != size())
         size(size_);

      rect subj_bounds = { 0, 0, float(size_.x), float(size_.y) };

      // layout the subject only if the window bounds changes
      context ctx{ *this, *context_, &content, subj_bounds };
      if (subj_bounds != _current_bounds)
      {
         _current_bounds = subj_bounds;
         content.layout(ctx);
      }

      // draw the subject
      content.draw(ctx);

      cairo_destroy(context_);
   }

   void view::click(mouse_button btn)
   {
      auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
      auto context_ = cairo_create(surface_);
      context ctx { *this, *context_, &content, _current_bounds };

      content.click(ctx, btn);

      cairo_surface_destroy(surface_);
      cairo_destroy(context_);
   }

   void view::drag(mouse_button btn)
   {
      auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
      auto context_ = cairo_create(surface_);
      context ctx { *this, *context_, &content, _current_bounds };

      content.drag(ctx, btn);

      cairo_surface_destroy(surface_);
      cairo_destroy(context_);
   }

   void view::cursor(point p, cursor_tracking status)
   {
      auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
      auto context_ = cairo_create(surface_);
      context ctx { *this, *context_, &content, _current_bounds };

      content.cursor(ctx, p, status);

      cairo_surface_destroy(surface_);
      cairo_destroy(context_);
   }
   
   void view::scroll(point p)
   {
      auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
      auto context_ = cairo_create(surface_);
      context ctx { *this, *context_, &content, _current_bounds };

      content.scroll(ctx, p);

      cairo_surface_destroy(surface_);
      cairo_destroy(context_);
   }
}
