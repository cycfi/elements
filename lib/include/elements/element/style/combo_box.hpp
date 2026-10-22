/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_COMBO_BOX_OCTOBER_22_2026)
#define ELEMENTS_STYLE_COMBO_BOX_OCTOBER_22_2026

#include <elements/element/style/menu.hpp>
#include <elements/element/style/text_entry.hpp>
#include <elements/element/size.hpp>
#include <infra/utf8_utils.hpp>
#include <cctype>
#include <string>
#include <vector>

namespace cycfi::elements
{
   namespace detail
   {
      // An input box that completes what is typed from a list, as a font
      // box does: the first item that starts with the text (case aside)
      // fills in, its rest selected, so the next character types over it
      // and the right arrow or Enter takes it.
      class combo_input_box : public basic_input_box
      {
      public:

                              combo_input_box(
                                 std::string placeholder
                               , std::vector<std::string> const& items
                              )
                               : basic_input_box{std::move(placeholder)}
                              {
                                 for (auto const& item : items)
                                    _items.push_back(to_utf32(item));
                              }

         bool                 text(context const& ctx, text_info info) override
                              {
                                 if (!basic_input_box::text(ctx, info))
                                    return false;
                                 complete();
                                 return true;
                              }

      private:

         void                 complete();

         std::vector<std::u32string> _items;
      };

      // The combo box's arrow. Its list opens under the whole frame, the
      // frame's width, as if the frame were the button: the arrow's parent
      // tile spans the frame.
      class combo_arrow : public basic_button_menu
      {
      public:

         using list_ptr = std::shared_ptr<hmin_size_element<vtile_composite>>;

                              combo_arrow()
                               : basic_button_menu{menu_position::bottom_right}
                              {}

         bool                 click(context const& ctx, mouse_button btn) override
                              {
                                 if (btn.down && ctx.parent && _list)
                                 {
                                    auto frame = ctx.parent->bounds;
                                    _list->hmin_size(frame.width() - 6);
                                    return basic_button_menu::click(
                                       context{ctx, frame}, btn);
                                 }
                                 return basic_button_menu::click(ctx, btn);
                              }

         void                 list(list_ptr list_) { _list = std::move(list_); }

      private:

         list_ptr             _list;
      };

      inline void combo_input_box::complete()
      {
         // Only typing at the end completes
         std::u32string typed{get_text()};
         int n = typed.size();
         if (n == 0 || select_start() != n || select_end() != n)
            return;

         auto same = [](char32_t a, char32_t b)
         {
            if (a < 128 && b < 128)
               return std::tolower(int(a)) == std::tolower(int(b));
            return a == b;
         };
         for (auto const& item : _items)
         {
            if (item.size() > typed.size()
               && std::equal(typed.begin(), typed.end(), item.begin(), same))
            {
               set_text(item);
               select_start(n);
               select_end(item.size());
               return;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Combo Box
   //
   // An input box with a list of items behind a dropdown arrow, in one
   // frame. The text is typed freely, completed from the list as it is
   // typed, or picked from the list: a pick puts the item's text in the
   // box and calls the box's on_enter with it, so one function takes
   // both. Returns the combo box and the input box, as input_box does.
   ////////////////////////////////////////////////////////////////////////////
   inline auto combo_box(
      std::vector<std::string> const& items
    , std::string placeholder = ""
   )
   {
      std::shared_ptr<basic_input_box> tbox =
         share(detail::combo_input_box{std::move(placeholder), items});

      // The arrow takes the frame's full height
      auto pad = get_theme().button_margin;
      auto arrow = momentary_button<detail::combo_arrow>(
         margin({8, pad.top, pad.right, pad.bottom},
            icon(icons::down_dir, 1.0)));

      vtile_composite items_;
      for (auto const& item : items)
      {
         auto e = share(menu_item(item));
         e->on_click = [tbox, item]()
         {
            tbox->set_text(item);
            if (tbox->on_enter)
               tbox->on_enter(item);
         };
         items_.push_back(e);
      }
      auto list = share(hmin_size(0, std::move(items_)));
      arrow.list(list);
      arrow.menu(layer(hold(list), panel{}));

      auto combo = layer(
         htile(
            margin({pad.left, pad.top, 0, pad.bottom},
               scroller(hold(tbox), no_scrollbars | no_vscroll)),
            std::move(arrow)
         ),
         frame{}
      );
      return std::make_pair(std::move(combo), tbox);
   }
}

#endif
