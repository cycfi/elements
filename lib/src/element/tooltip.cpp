/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/tooltip.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   rect tooltip_element::tip_bounds(context const& ctx) const
   {
      auto limits_ = _tip->limits(ctx);
      auto w = limits_.min.x;
      auto h = limits_.min.y;
      return rect{ 0, 0, w, h }.move_to(ctx.bounds.left, ctx.bounds.top-h);
   }

   bool tooltip_element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (status != cursor_tracking::leaving)
      {
         if (_tip_status != tip_visible)
         {
            _tip_status = tip_delayed;
            _tip->bounds(tip_bounds(ctx));
            ctx.view.post(std::chrono::duration_cast<std::chrono::milliseconds>(_delay),
               [this, &view_ = ctx.view, bounds = ctx.bounds]()
               {
                  if (_tip_status == tip_delayed)
                  {
                     _tip->on_cursor =
                        [this, &view_, bounds](point p, cursor_tracking status)
                        {
                           _cursor_in_tip = status != cursor_tracking::leaving;
                           if (!_cursor_in_tip && !bounds.includes(p))
                              close_tip(view_);
                        };

                     on_hover(true);
                     _tip_status = tip_visible;
                     _tip->open(view_);
                  }
               }
            );
         }
      }
      else
      {
         ctx.view.post(
            [this, &view_ = ctx.view]()
            {
               if (!_cursor_in_tip)
                  close_tip(view_);
            }
         );
      }

      return base_type::cursor(ctx, p, status);
   }

   void tooltip_element::close_tip(view& view_)
   {
      if (_tip_status != tip_hidden)
      {
         if (_tip_status == tip_visible)
         {
            on_hover(false);
            _tip->close(view_);
         }
         _tip_status = tip_hidden;
         _cursor_in_tip = false;
      }
   }

   bool tooltip_element::key(context const& ctx, key_info k)
   {
      auto r = base_type::key(ctx, k);
      if (!r && k.key == key_code::escape)
         close_tip(ctx.view);
      return r;
   }
}}
