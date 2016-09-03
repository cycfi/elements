/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   photon::canvas view::canvas()
   {
      return photon::canvas{ _context };
   }

   void view::draw(rect dirty_)
   {
      setup_context();
   
      _dirty = dirty_;

      basic_context bctx{ *this };
      auto limits_ = content.limits(bctx);
      limits(limits_);

      auto size_ = size();
      clamp(size_.x, limits_.left, limits_.right);
      clamp(size_.y, limits_.top, limits_.bottom);
      if (size_ != size())
         size(size_);

      rect subj_bounds = { 0, 0, float(size_.x), float(size_.y) };

      // layout the subject only if the window bounds changes
      context ctx{ *this, &content, subj_bounds };
      if (subj_bounds != _current_bounds)
      {
         _current_bounds = subj_bounds;
         content.layout(ctx);
      }

      // draw the subject
      content.draw(ctx);

      cairo_destroy(_context);
      cairo_surface_destroy(_surface);
   }

   void view::click(mouse_button btn)
   {
      context ctx { *this, &content, _current_bounds };
      content.click(ctx, btn);
   }

   void view::drag(mouse_button btn)
   {
      context ctx { *this, &content, _current_bounds };
      content.drag(ctx, btn);
   }
   
   void view::cursor(point p, cursor_tracking status)
   {
      context ctx { *this, &content, _current_bounds };
      content.cursor(ctx, p, status);
   }
}


