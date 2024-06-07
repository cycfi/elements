/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_MENU_JUNE_5_2016)
#define ELEMENTS_STYLE_MENU_JUNE_5_2016

#include <elements/element/indirect.hpp>
#include <elements/element/menu.hpp>
#include <elements/element/label.hpp>
#include <elements/element/align.hpp>
#include <elements/element/size.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/tile.hpp>
#include <elements/support/theme.hpp>
#include <elements/element/style/button.hpp>
#include <infra/string_view.hpp>
#include <string>
#include <vector>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Popup Button
   ////////////////////////////////////////////////////////////////////////////
   inline auto
   button_menu(
      std::string text
    , menu_position pos = menu_position::bottom_right
    , color body_color = get_theme().default_button_color
   )
   {
      auto icon =
         (pos == menu_position::bottom_right || pos == menu_position::bottom_left)?
         icons::down_dir : icons::up_dir
         ;
      auto menu = button<basic_button_menu>(std::move(text), icon, 1.0, body_color);
      menu.position(pos);
      return menu;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Menu Background
   ////////////////////////////////////////////////////////////////////////////
   using menu_background = panel; // We'll just reuse the panel now

   ////////////////////////////////////////////////////////////////////////////
   // Menu Items
   ////////////////////////////////////////////////////////////////////////////
   std::pair<std::string, std::string>
   diplay_shortcut(key_code k, int mod);

   inline std::pair<std::string, std::string>
   diplay_shortcut(key_info k)
   {
      return diplay_shortcut(k.key, k.modifiers);
   }

   inline auto menu_item_text(std::string text, float text_align = 0.0f /*align left*/)
   {
      return hmargin({20, 20}, halign(text_align, label(std::move(text))));
   }

   inline auto menu_item_text(std::string text, shortcut_key shortcut)
   {
      auto [mod, key] = diplay_shortcut(shortcut.key, shortcut.modifiers);
#if defined(__APPLE__)
      auto sk_font = get_theme().system_font;
#else
      auto sk_font = get_theme().label_font;
#endif
      return hmargin({20, 10},
         htile(
            htile(
               align_left(label(std::move(text)))
             , align_right(label(mod).font(sk_font))
            ),
#if defined(__APPLE__)
            margin_left(5, hsize(10, align_left(label(key))))
#else
            hsize(10, align_left(label(key)))
#endif
         )
      );
   }

   inline auto menu_item(std::string text, float text_align = 0.0f /*align left*/)
   {
      return basic_menu_item(menu_item_text(std::move(text), text_align));
   }

   inline auto menu_item(std::string text, shortcut_key shortcut)
   {
      auto r = basic_menu_item(menu_item_text(std::move(text), shortcut));
      r.shortcut = shortcut;
      return r;
   }

   struct menu_item_spacer_element : public element
   {
      view_limits          limits(basic_context const& ctx) const override;
      void                 draw(context const& ctx) override;
   };

   inline auto menu_item_spacer()
   {
      return menu_item_spacer_element{};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Selection Menu
   ////////////////////////////////////////////////////////////////////////////
   struct menu_selector
   {
      virtual ~menu_selector() = default;

      virtual std::size_t        size() const = 0;
      virtual string_view        operator[](std::size_t index) const = 0;
   };

   inline auto make_selection_menu_button(std::shared_ptr<basic_label> label)
   {
      return momentary_button<basic_button_menu>(
         layer(
            margin(
               get_theme().button_margin,
               htile(
                  align_left(hold(label)),
                  align_right(margin_left(12, icon(icons::down_dir, 1.0)))
               )
            ),
            frame{}
         )
      );
   }

   inline auto selection_menu(std::string init)
   {
      auto btn_text = share(label(std::move(init)).relative_font_size(1.0));
      auto menu_btn = make_selection_menu_button(btn_text);
      menu_btn.position(menu_position::bottom_right);
      return std::make_pair(menu_btn, btn_text);
   }

   using on_menu_item_function =
      std::function<void(basic_menu_item_element&, string_view item)>;

   inline auto selection_menu(
      std::function<void(string_view item)> on_select
    , menu_selector const& items
    , float text_align = 0.0f // align left
    , on_menu_item_function on_item = [](auto&, auto) {}
   )
   {
      auto r = selection_menu(items.size()? std::string(items[0]) : "");

      if (items.size())
      {
         vtile_composite list;
         for (std::size_t i = 0; i != items.size(); ++i)
         {
            auto e = share(menu_item(std::string(items[i]), text_align));
            auto label = find_subject<text_reader*>(e.get());
            if (label)
            {
               e->on_click = [btn_text = r.second, on_select, label]()
               {
                  auto text = label->get_text();
                  btn_text->set_text(text);
                  on_select(text);
               };
            }
            list.push_back(e);
            on_item(*e, items[i]);
         }

         auto menu = layer(list, panel{});
         r.first.menu(menu);
      }

      return r;
   }

   template <typename Sequence>
   inline auto selection_menu(
      std::function<void(string_view item)> on_select
    , Sequence const& seq
    , float text_align = 0.0f // align left
    , on_menu_item_function on_item = [](auto&, auto) {}
    , typename std::enable_if<!std::is_base_of<menu_selector, Sequence>::value>::type* = nullptr
   )
   {
      struct seq_menu_selector : menu_selector
      {
         seq_menu_selector(Sequence const& seq_)
          : _seq(seq_)
         {}

         std::size_t
         size() const override
         {
            return std::size(_seq);
         }

         string_view
         operator[](std::size_t index) const override
         {
            return _seq[index];
         }

         Sequence const& _seq;
      };

      return selection_menu(on_select, seq_menu_selector{seq}, text_align, on_item);
   }

   template <typename T>
   inline auto selection_menu(
      std::function<void(string_view item)> on_select
    , std::initializer_list<T> list
    , float text_align = 0.0f // align left
    , on_menu_item_function on_item = [](auto&, auto) {}
   )
   {
      struct init_list_menu_selector : menu_selector
      {
         init_list_menu_selector(std::initializer_list<T> list_)
          : _list(list_)
         {}

         std::size_t
         size() const override
         {
            return _list.size();
         }

         string_view
         operator[](std::size_t index) const override
         {
            return *(_list.begin()+index);
         }

         std::vector<T> _list;
      };

      return selection_menu(on_select, init_list_menu_selector{list}, text_align, on_item);
   }
}

#endif
