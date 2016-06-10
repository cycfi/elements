/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/menu.hpp>

namespace photon
{
   rect  menu_background::shadow_offset  = { -10, -10, +20, +30 };

   void menu_background::draw(context const& ctx)
   {
      auto&       canvas_ = ctx.canvas();
      auto const& bounds = ctx.bounds;

      // Panel fill
      canvas_.begin_path();
      canvas_.rect(ctx.bounds);
      canvas_.fill_color(ctx.theme().panel_color.opacity(0.95));
      canvas_.fill();

      // Drop shadow
      rect offs = shadow_offset;
      canvas_.begin_path();
      rect  shr =
         {  bounds.left+offs.left, bounds.top+offs.top,
            bounds.right+offs.right, bounds.bottom+offs.bottom
         };

      canvas_.rect(shr);
      canvas_.rect(bounds);
      canvas_.path_winding(canvas::hole);

      paint shadow_paint
         = canvas_.box_gradient(bounds.move(0, 2), 4, 10
          , color(0, 0, 0, 128), color(0, 0, 0, 0)
         );

      canvas_.fill_paint(shadow_paint);
      canvas_.fill();
   }

   namespace
   {
      auto find_menu(context const& ctx, widget_ptr _menu)
      {
         auto f = ctx.window.content.elements.begin();
         auto l = ctx.window.content.elements.end();
         return std::find(f, l, _menu);
      }

      void install_menu(context const& ctx, widget_ptr _menu, basic_dropdown_menu& owner)
      {
         if (auto p = std::dynamic_pointer_cast<menu_backdrop_widget>(_menu))
         {
            ctx.window.content.elements.push_back(_menu);
            p->owner(&owner);
         }
      }

      void layout_menu(context const& ctx, widget_ptr _menu)
      {
         if (auto p = std::dynamic_pointer_cast<menu_backdrop_widget>(_menu))
         {
            p->bounds(
               {
                  ctx.bounds.left+3, ctx.bounds.bottom,
                  ctx.bounds.right-3, full_extent
               }
            );

            rect  bounds = p->bounds();
            context new_ctx{ ctx.window, _menu.get(), bounds };
            _menu->layout(new_ctx);
         }
      }

      void remove_menu(context const& ctx, widget_ptr _menu)
      {
         auto i = find_menu(ctx, _menu);
         if (i != ctx.window.content.elements.end())
            ctx.window.content.elements.erase(i);
      }
   }

   void basic_dropdown_menu::layout(context const& ctx)
   {
      basic_button::layout(ctx);
      layout_menu(ctx, _menu);
   }

   widget* basic_dropdown_menu::click(context const& ctx, mouse_button btn)
   {
      if (btn.is_pressed)
      {
         if (state(!state()))
         {
            if (!_is_active)
            {
               install_menu(ctx, _menu, *this);
               _is_active = true;
            }
            ctx.window.draw();
         }
      }
      else
      {
         if (!state() || !hit_test(ctx, ctx.cursor_pos()))
         {
            remove_menu(ctx, _menu);
            state(false);
            _is_active = false;
            ctx.window.draw();
         }
      }
      return this;
   }

   void basic_dropdown_menu::drag(context const& ctx, mouse_button btn)
   {
      ctx.window.draw();
   }

   bool basic_dropdown_menu::key(context const& ctx, key_info const& k)
   {
      if (k.key == key_code::key_escape)
      {
         remove_menu(ctx, _menu);
         state(false);
         _is_active = false;
         ctx.window.draw();
         return true;
      }
      return false;
   }

   bool basic_dropdown_menu::focus(focus_request r)
   {
      return true;
   }

   widget* menu_backdrop_widget::hit_test(context const& ctx, point p)
   {
      return widget::hit_test(ctx, p);
   }

   widget* menu_backdrop_widget::click(context const& ctx, mouse_button btn)
   {
      auto r = floating_widget::click(ctx, btn);
      remove_menu(ctx, shared_from_this());
      _owner->state(0);
      _owner->_is_active = false;
      ctx.window.draw();
      return r;
   }

   void basic_menu_item_widget::draw(context const& ctx)
   {
      if (ctx.bounds.includes(ctx.cursor_pos()))
      {
         auto& canvas_ = ctx.canvas();

         canvas_.begin_path();
         canvas_.round_rect(ctx.bounds, 2);
         canvas_.fill_color(ctx.theme().indicator_color.opacity(0.6));
         canvas_.fill();
      }
      proxy_base::draw(ctx);
   }

   widget* basic_menu_item_widget::click(context const& ctx, mouse_button btn)
   {
      if (!btn.is_pressed && on_click)
         on_click();
      return proxy_base::click(ctx, btn);
   }

   bool basic_menu_item_widget::cursor(context const& ctx, point p)
   {
      if (ctx.bounds.includes(p))
         ctx.window.draw();
      return proxy_base::cursor(ctx, p);
   }

   bool basic_menu_item_widget::is_control() const
   {
      return true;
   }
}
