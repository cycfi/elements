/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/menu.hpp>

namespace cycfi { namespace elements
{
   basic_menu
   button_menu(std::string_view text, menu_position pos, color body_color)
   {
      auto icon =
         (pos == menu_position::bottom_right || pos == menu_position::bottom_left)?
         icons::down_dir : icons::up_dir
         ;
      auto menu = make_button<basic_menu>(text, icon, 1.0, body_color);
      menu.position(pos);
      return menu;
   }

   basic_menu
   button_menu(menu_position pos, color body_color)
   {
      auto icon =
         (pos == menu_position::bottom_right || pos == menu_position::bottom_left)?
         icons::down_dir : icons::up_dir
         ;
      auto menu = make_button<basic_menu>(icon, 1.0, body_color);
      menu.position(pos);
      return menu;
   }

   basic_menu icon_menu(uint32_t code, float size, menu_position pos)
   {
      auto menu = text_button<basic_menu>(code, size, /*no_frame*/ true);
      menu.position(pos);
      return menu;
   }

   view_limits menu_item_spacer_element::limits(basic_context const& ctx) const
   {
      auto height = get_theme().label_font_size;
      return { { 0, height }, { full_extent, height } };
   }

   void menu_item_spacer_element::draw(context const& ctx)
   {
      auto& canvas_ = ctx.canvas;
      float y = ctx.bounds.top + ctx.bounds.height() / 2;

      canvas_.begin_path();
      canvas_.move_to({ ctx.bounds.left, y });
      canvas_.line_to({ ctx.bounds.right, y });
      canvas_.stroke_style(get_theme().frame_color.opacity(0.25));
      canvas_.line_width(1);
      canvas_.stroke();
   }

   std::pair<basic_menu, std::shared_ptr<label>>
   selection_menu(std::string_view init)
   {
      auto btn_text = share(label(init, 1.0));

      auto menu_btn = text_button<basic_menu>(
         margin(
            button_margin,
            htile(
               align_left(hold(btn_text)),
               align_right(left_margin(12, icon(icons::down_dir, 1.0)))
            )
         )
      );

      menu_btn.position(menu_position::bottom_right);
      return { std::move(menu_btn), btn_text };
   }

   std::pair<basic_menu, std::shared_ptr<label>>
   selection_menu(
      std::function<void(std::string_view item)> on_select
    , menu_selector const& items
   )
   {
      auto r = selection_menu(items.size()? items[0] : "");

      if (items.size())
      {
         vtile_composite list;
         for (auto i = 0; i != items.size(); ++i)
         {
            auto e = menu_item(items[i]);
            e.on_click = [btn_text = r.second, on_select, text = items[i]]()
            {
               btn_text->text(text);
               on_select(text);
            };
            list.push_back(share(e));
         }

         auto menu = layer(list, panel{});
         r.first.menu(menu);
      }

      return r;
   }
}}
