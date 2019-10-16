/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/port.hpp>
#include <elements/view.hpp>
#include <algorithm>
#include <cmath>

namespace cycfi { namespace elements
{
   constexpr auto min_size = 64;

   ////////////////////////////////////////////////////////////////////////////
   // port_base class implementation
   ////////////////////////////////////////////////////////////////////////////
   view_limits port_base::limits(basic_context const& ctx) const
   {
      view_limits e_limits = subject().limits(ctx);
      return { { min_size, min_size }, e_limits.max };
   }

   void port_base::prepare_subject(context& ctx)
   {
      view_limits    e_limits          = subject().limits(ctx);
      double         elem_width        = e_limits.min.x;
      double         elem_height       = e_limits.min.y;
      double         available_width   = ctx.parent->bounds.width();
      double         available_height  = ctx.parent->bounds.height();

      ctx.bounds.left -= (elem_width - available_width) * _halign;
      ctx.bounds.width(elem_width);
      ctx.bounds.top -= (elem_height - available_height) * _valign;
      ctx.bounds.height(elem_height);

      subject().layout(ctx);
   }

   void port_base::draw(context const& ctx)
   {
      auto state = ctx.canvas.new_state();
      ctx.canvas.rect(ctx.bounds);
      ctx.canvas.clip();
      proxy_base::draw(ctx);
   }

   ////////////////////////////////////////////////////////////////////////////
   // vport_base class implementation
   ////////////////////////////////////////////////////////////////////////////
   view_limits vport_base::limits(basic_context const& ctx) const
   {
      view_limits e_limits = subject().limits(ctx);
      return { { e_limits.min.x, min_size }, e_limits.max };
   }

   void vport_base::prepare_subject(context& ctx)
   {
      view_limits    e_limits          = subject().limits(ctx);
      double         elem_height       = e_limits.min.y;
      double         available_height  = ctx.parent->bounds.height();

      ctx.bounds.top -= (elem_height - available_height) * _valign;
      ctx.bounds.height(elem_height);

      subject().layout(ctx);
   }

   void vport_base::draw(context const& ctx)
   {
      auto state = ctx.canvas.new_state();
      ctx.canvas.rect(ctx.bounds);
      ctx.canvas.clip();
      proxy_base::draw(ctx);
   }

   ////////////////////////////////////////////////////////////////////////////
   // scrollable class implementation
   ////////////////////////////////////////////////////////////////////////////
   scrollable::scrollable_context scrollable::find(context const& ctx_)
   {
      auto const* ctx = &ctx_;
      while (ctx && ctx->element)
      {
         auto* sp = dynamic_cast<scrollable*>(ctx->element);
         if (sp)
            return { ctx, sp };
         else
            ctx = ctx->parent;
      }
      return { 0, 0 };
   }

   ////////////////////////////////////////////////////////////////////////////
   // scroller_base class implementation
   ////////////////////////////////////////////////////////////////////////////
   float scroller_base::width = 10;

   namespace
   {
      void draw_scrollbar_fill(canvas& _canvas, rect r, color fill_color)
      {
         _canvas.begin_path();
         _canvas.rect(r);
         _canvas.fill_style(fill_color);
         _canvas.fill();
      }

      void draw_scrollbar(
         canvas& _canvas, rect b, float radius,
         color outline_color, color fill_color, point mp,
         bool is_tracking
      )
      {
         _canvas.begin_path();
         _canvas.round_rect(b, radius);
         _canvas.fill_style(fill_color);

         if (is_tracking || _canvas.hit_test(mp))
            _canvas.fill_style(fill_color.opacity(0.8));

         _canvas.fill_preserve();

         _canvas.line_width(0.5);
         _canvas.stroke_style(outline_color);
         _canvas.stroke();
      }
   }

   void scroller_base::draw_scroll_bar(context const& ctx, scrollbar_info const& info, point mp)
   {
      theme const& thm = get_theme();

      float x = info.bounds.left;
      float y = info.bounds.top;
      float w = info.bounds.width();
      float h = info.bounds.height();

      draw_scrollbar_fill(ctx.canvas, info.bounds, thm.scrollbar_color);

      if (w > h)
      {
         w *= w / info.extent;
         clamp_min(w, 20);
         x += info.pos * (info.bounds.width()-w);
      }
      else
      {
         h *= h / info.extent;
         clamp_min(h, 20);
         y += info.pos * (info.bounds.height()-h);
      }

      draw_scrollbar(ctx.canvas, rect{ x, y, x+w, y+h }, scroller_base::width/3,
         thm.frame_color.opacity(0.5), thm.scrollbar_color.opacity(0.4), mp,
         _tracking == ((w > h)? tracking_h : tracking_v));
   }

   rect scroller_base::scroll_bar_position(context const& ctx, scrollbar_info const& info)
   {
      float x = info.bounds.left;
      float y = info.bounds.top;
      float w = info.bounds.width();
      float h = info.bounds.height();

      if (w > h)
      {
         w *= w  / info.extent;
         clamp_min(w, 20);
         x += info.pos * (info.bounds.width()-w);
      }
      else
      {
         h *= h / info.extent;
         clamp_min(h, 20);
         y += info.pos * (info.bounds.height()-h);
      }
      return rect{ x, y, x+w, y+h };
   }

   view_limits scroller_base::limits(basic_context const& ctx) const
   {
      view_limits e_limits = subject().limits(ctx);
      return view_limits{
         { allow_hscroll()? min_size : e_limits.min.x, allow_vscroll()? min_size : e_limits.min.y },
         { e_limits.max.x, e_limits.max.y }
      };
   }

   element* scroller_base::hit_test(context const& ctx, point p)
   {
      return element::hit_test(ctx, p);
   }

   scroller_base::scrollbar_bounds
   scroller_base::get_scrollbar_bounds(context const& ctx)
   {
      scrollbar_bounds r;
      view_limits      e_limits = subject().limits(ctx);

      r.has_h = e_limits.min.x > ctx.bounds.width() && allow_hscroll();
      r.has_v = e_limits.min.y > ctx.bounds.height() && allow_vscroll();

      if (r.has_v)
      {
         r.vscroll_bounds = rect{
            ctx.bounds.left + ctx.bounds.width() - width,
            ctx.bounds.top,
            ctx.bounds.right,
            ctx.bounds.bottom - (r.has_h ? scroller_base::width : 0)
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
            ctx.bounds.top + ctx.bounds.height() - scroller_base::width,
            ctx.bounds.right - (r.has_v ? scroller_base::width : 0),
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
         view_limits       e_limits = subject().limits(ctx);
         point             mp = ctx.view.cursor_pos();

         if (sb.has_v)
            draw_scroll_bar(ctx, { valign(), e_limits.min.y, sb.vscroll_bounds }, mp);

         if (sb.has_h)
            draw_scroll_bar(ctx, { halign(), e_limits.min.x, sb.hscroll_bounds }, mp);
      }
   }

   bool scroller_base::scroll(context const& ctx, point dir, point p)
   {
      view_limits e_limits = subject().limits(ctx);
      bool redraw = false;

      if (allow_hscroll())
      {
         double dx = (-dir.x / (e_limits.min.x - ctx.bounds.width()));
         if ((dx > 0 && halign() < 1.0) || (dx < 0 && halign() > 0.0))
         {
            double alx = halign() + dx;
            clamp(alx, 0.0, 1.0);
            halign(alx);
            redraw = true;
         }
      }

      if (allow_vscroll())
      {
         double dy = (-dir.y / (e_limits.min.y - ctx.bounds.height()));
         if ((dy > 0 && valign() < 1.0) || (dy < 0 && valign() > 0.0))
         {
            double aly = valign() + dy;
            clamp(aly, 0.0, 1.0);
            valign(aly);
            redraw = true;
         }
      }

      if (redraw)
         ctx.view.refresh(ctx);
      return redraw;
   }

   element* scroller_base::click(context const& ctx, mouse_button btn)
   {
      if (has_scrollbars())
      {
         if (btn.down)
         {
            _tracking = start;
            if (reposition(ctx, btn.pos))
               return ctx.element;
         }
         _tracking = none;
         refresh(ctx);
      }
      return port_base::click(ctx, btn);
   }

   void scroller_base::drag(context const& ctx, mouse_button btn)
   {
      if (_tracking == none || !reposition(ctx, btn.pos))
      {
         // scroll cursor into view:
         auto mp = btn.pos;
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
            scroll(ctx, dp, mp);
         }
         port_base::drag(ctx, btn);
      }
   }

   bool scroller_base::reposition(context const& ctx, point p)
   {
      if (!has_scrollbars())
         return false;

      scrollbar_bounds  sb = get_scrollbar_bounds(ctx);
      view_limits       e_limits = subject().limits(ctx);

      auto valign_ = [&](double align)
      {
         clamp(align, 0.0, 1.0);
         valign(align);
         ctx.view.refresh(ctx);
      };

      auto halign_ = [&](double align)
      {
         clamp(align, 0.0, 1.0);
         halign(align);
         ctx.view.refresh(ctx);
      };

      if (sb.has_v)
      {
         // vertical scroll
         rect b = scroll_bar_position(
            ctx, { valign(), e_limits.min.y, sb.vscroll_bounds });

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
         rect b = scroll_bar_position(
            ctx, { halign(), e_limits.min.x, sb.hscroll_bounds });

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

   bool scroller_base::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (has_scrollbars())
      {
         ctx.view.refresh(ctx);
         scrollbar_bounds sb = get_scrollbar_bounds(ctx);
         if (sb.hscroll_bounds.includes(p) || sb.vscroll_bounds.includes(p))
         {
            set_cursor(cursor_type::arrow);
            return true;
         }
      }
      return subject().cursor(ctx, p, status);
   }

   bool scroller_base::is_control() const
   {
      return true;
   }

   bool scroller_base::scroll_into_view(context const& ctx, rect r)
   {
      constexpr auto scroll_clearance = 20;
      rect bounds = ctx.bounds;

      if (has_scrollbars())
      {
         scrollbar_bounds sb = get_scrollbar_bounds(ctx);

         if (sb.has_h)
            bounds.right -= scroller_base::width;
         if (sb.has_v)
            bounds.bottom -= scroller_base::width;
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

         return scroll(ctx, dp, ctx.view.cursor_pos());
      }
      return false;
   }

   bool scroller_base::key(context const& ctx, key_info k)
   {
      auto valign_ = [&](double align)
      {
         clamp(align, 0.0, 1.0);
         valign(align);
         ctx.view.refresh(ctx);
      };

      bool handled = proxy_base::key(ctx, k);
      if (!handled)
      {
         switch (k.key)
         {
            case key_code::home:
               valign_(0);
               handled = true;
               break;

            case key_code::end:
               valign_(1);
               handled = true;
               break;

            case key_code::page_up:
            case key_code::page_down:
            {
               if (k.action == key_action::press)
               {
                  view_limits       e_limits = subject().limits(ctx);
                  scrollbar_bounds  sb = get_scrollbar_bounds(ctx);
                  rect b = scroll_bar_position(
                      ctx, { valign(), e_limits.min.y, sb.vscroll_bounds });
                  double page = b.height() / sb.vscroll_bounds.height();
                  valign_(valign() + ((k.key == key_code::page_down) ? page : -page));
               }
               handled = true;
               break;
            }

            default:
                break;
         }
      }
      return handled;
   }
}}
