/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/port.hpp>
#include <photon/window.hpp>
#include <algorithm>
#include <nanovg.h>
#include <cmath>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // port
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect port_base::limits(basic_context const& ctx) const
   {
      rect e_limits = subject().limits(ctx);
      return rect{ 0, 0, e_limits.right, e_limits.bottom };
   }

   void port_base::prepare_subject(context& ctx)
   {
      rect     e_limits          = subject().limits(ctx);
      double   elem_width        = e_limits.left;
      double   elem_height       = e_limits.top;
      double   available_width   = ctx.parent->bounds.width();
      double   available_height  = ctx.parent->bounds.height();

      ctx.bounds.left -= (elem_width - available_width) * _halign;
      ctx.bounds.width(elem_width);
      ctx.bounds.top -= (elem_height - available_height) * _valign;
      ctx.bounds.height(elem_height);

      subject().layout(ctx);
   }

   void port_base::draw(context const& ctx)
   {
      nvgSave(ctx.canvas().context());
      nvgScissor(ctx.canvas().context(), ctx.bounds.left, ctx.bounds.top, ctx.bounds.width(), ctx.bounds.height());
      proxy_draw(ctx);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect scroller_base::limits(basic_context const& ctx) const
   {
      rect e_limits = subject().limits(ctx);
      return rect{
         allow_hscroll()? 0 : e_limits.left,
         allow_vscroll()? 0 : e_limits.top,
         e_limits.right,
         e_limits.bottom
      };
   }

   scroller_base::scrollbar_bounds
   scroller_base::get_scrollbar_bounds(context const& ctx)
   {
      scrollbar_bounds r;

      rect     e_limits = subject().limits(ctx);
      double   scroll_bar_width = ctx.theme().scroll_bar_width;

      r.has_h = e_limits.left > ctx.bounds.width() && allow_hscroll();
      r.has_v = e_limits.top > ctx.bounds.height() && allow_vscroll();

      if (r.has_v)
      {
         r.vscroll_bounds = rect{
            ctx.bounds.left + ctx.bounds.width() - scroll_bar_width,
            ctx.bounds.top,
            ctx.bounds.right,
            ctx.bounds.bottom - (r.has_h ? scroll_bar_width : 0)
         };
      }
      else
      {
         valign(0.0);
      }

      if (r.has_h)
      {
         r.hscroll_bounds = rect{
            ctx.bounds.left,
            ctx.bounds.top + ctx.bounds.height() - scroll_bar_width,
            ctx.bounds.right - (r.has_v ? scroll_bar_width : 0),
            ctx.bounds.bottom
         };
      }
      else
      {
         halign(0.0);
      }
      return r;
   }

   void scroller_base::draw(context const& ctx)
   {
      port_base::draw(ctx);

      if (has_scrollbars())
      {
         scrollbar_bounds  sb = get_scrollbar_bounds(ctx);
         rect              e_limits = subject().limits(ctx);

         if (sb.has_v)
            ctx.theme().draw_scroll_bar(valign(), e_limits.top, sb.vscroll_bounds);

         if (sb.has_h)
            ctx.theme().draw_scroll_bar(halign(), e_limits.left, sb.hscroll_bounds);
      }
   }

   bool scroller_base::scroll(context const& ctx, point const& p)
   {
      rect  e_limits = subject().limits(ctx);

      bool redraw = false;

      if (allow_hscroll())
      {
         double dx = (-p.x / (e_limits.left-ctx.bounds.width()));
         if ((dx > 0 && halign() < 1.0) || (dx < 0 && halign() > 0.0))
         {
            double alx = halign() + dx;
            limit(alx, 0.0, 1.0);
            halign(alx);
            redraw = true;
         }
      }

      if (allow_vscroll())
      {
         double dy = (-p.y / (e_limits.top-ctx.bounds.height()));
         if ((dy > 0 && valign() < 1.0) || (dy < 0 && valign() > 0.0))
         {
            double aly = valign() + dy;
            limit(aly, 0.0, 1.0);
            valign(aly);
            redraw = true;
         }
      }

      if (redraw)
         ctx.window.draw();
      return redraw;
   }

   widget* scroller_base::click(context const& ctx, mouse_button btn)
   {
      if (has_scrollbars())
      {
         if (btn.is_pressed)
         {
            _tracking = start;
            if (reposition(ctx))
               return &self();
         }
         _tracking = none;
      }
      return proxy_click(ctx, btn);
   }

   void scroller_base::drag(context const& ctx, mouse_button btn)
   {
      if (_tracking == none || !reposition(ctx))
      {
         // scroll cursor into view:
         auto mp = ctx.cursor_pos();
         if (!ctx.bounds.includes(mp))
         {
            point dp;
            if (mp.x > ctx.bounds.right)
               dp.x = ctx.bounds.right - mp.x;
            else if (mp.x < ctx.bounds.left)
               dp.x = ctx.bounds.left - mp.x;
            if (mp.y > ctx.bounds.bottom)
               dp.y = ctx.bounds.bottom - mp.y;
            else if (mp.y < ctx.bounds.top)
               dp.y = ctx.bounds.top - mp.y;
            scroll(ctx, dp);
         }
         proxy_drag(ctx, btn);
      }
   }

   bool scroller_base::reposition(context const& ctx)
   {
      if (!has_scrollbars())
         return false;

      point             p = ctx.cursor_pos();
      scrollbar_bounds  sb = get_scrollbar_bounds(ctx);
      rect              e_limits = subject().limits(ctx);

      auto valign_ = [&](double align)
      {
         limit(align, 0.0, 1.0);
         valign(align);
         ctx.window.draw();
      };

      auto halign_ = [&](double align)
      {
         limit(align, 0.0, 1.0);
         halign(align);
         ctx.window.draw();
      };

      if (sb.has_v)
      {
         // vertical scroll
         rect b = ctx.theme().scroll_bar_position(valign(), e_limits.top, sb.vscroll_bounds);
         if (_tracking == start)
         {
            if (b.includes(p))
            {
               // start tracking scroll-box
               _offset = point{ p.x-b.left, p.y-b.top };
               _tracking = tracking_v;
            }
            else if (sb.vscroll_bounds.includes(p))
            {
               // page up or down
               double page = b.height() / sb.vscroll_bounds.height();
               if (p.y < b.top)
               {
                  valign_(valign() - page);
                  return true;
               }
               else if (p.y > b.bottom)
               {
                  valign_(valign() + page);
                  return true;
               }
            }
         }

         // continue tracking scroll-box
         if (_tracking == tracking_v)
         {
            p.y -= _offset.y + ctx.bounds.top;
            valign_(p.y / (sb.vscroll_bounds.height() - b.height()));
            return true;
         }
      }

      if (sb.has_h)
      {
         // horizontal scroll
         rect b = ctx.theme().scroll_bar_position(halign(), e_limits.left, sb.hscroll_bounds);
         if (_tracking == start)
         {
            // start tracking scroll-box
            if (b.includes(p))
            {
               _offset = point{ p.x-b.left, p.y-b.top };
               _tracking = tracking_h;
            }
            else if (sb.hscroll_bounds.includes(p))
            {
               // page left or right
               double page = b.width() / sb.vscroll_bounds.width();
               if (p.x < b.left)
               {
                  halign_(valign() - page);
                  return true;
               }
               else if (p.x > b.right)
               {
                  halign_(valign() + page);
                  return true;
               }
            }
         }

         // continue tracking scroll-box
         if (_tracking == tracking_h)
         {
            p.x -= _offset.x + ctx.bounds.left;
            halign_(p.x / (sb.hscroll_bounds.width() - b.width()));
            return true;
         }
      }

      return false;
   }

   bool scroller_base::cursor(context const& ctx, point const& p)
   {
      if (has_scrollbars())
      {
         scrollbar_bounds sb = get_scrollbar_bounds(ctx);
         if (sb.hscroll_bounds.includes(p) || sb.vscroll_bounds.includes(p))
         {
            ctx.window.set_cursor(cursor::arrow);
            return true;
         }
      }
      return proxy_cursor(ctx, p);
   }

   bool scroller_base::is_control() const
   {
      return true;
   }

   bool scroller_base::scroll_into_view(context const& ctx, rect const& r)
   {
      constexpr auto scroll_clearance = 20;
      rect bounds = ctx.bounds;

      if (has_scrollbars())
      {
         scrollbar_bounds sb = get_scrollbar_bounds(ctx);

         if (sb.has_h)
            bounds.right -= ctx.theme().scroll_bar_width;
         if (sb.has_v)
            bounds.bottom -= ctx.theme().scroll_bar_width;
      }

      if (!bounds.includes(r))
      {
         // r is not in view, we need to scroll
         point dp;

         if (allow_vscroll())
         {
            if (r.top < bounds.top)
               dp.y = bounds.top-r.top;
            else if (r.bottom > bounds.bottom)
               dp.y = (bounds.bottom-r.bottom) - scroll_clearance;
         }

         if (allow_hscroll())
         {
            if (r.left < bounds.left)
               dp.x = bounds.left-r.left;
            else if (r.right > bounds.right)
               dp.x = bounds.right-r.right - scroll_clearance;
         }

         return scroll(ctx, dp);
      }
      return false;
   }
}
