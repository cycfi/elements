/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016)
#define PHOTON_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016

#include <photon/widget/text.hpp>
#include <photon/widget/icon.hpp>
#include <photon/widget/tile.hpp>
#include <photon/widget/menu.hpp>
#include <photon/widget/align.hpp>
#include <photon/widget/basic.hpp>
#include <photon/widget/text_utils.hpp>
#include <photon/widget/icon_ids.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Buttons
   ////////////////////////////////////////////////////////////////////////////////////////////////
   auto const button_margin = rect{ 10, 5, 10, 5 };
   auto const default_button_color = color{ 0, 0, 0, 0 };

   class basic_button_body : public widget
   {
   public:

      static float corner_radius;

                     basic_button_body(color body_color);
      virtual void   draw(context const& ctx);

   private:

      color          body_color;
   };

   inline basic_button_body::basic_button_body(color body_color)
    : body_color(body_color)
   {}

   template <typename Button, typename Label>
   inline Button make_button(Label&& label, color body_color = default_button_color)
   {
      auto btn_body_off = basic_button_body(body_color.level(0.9));
      auto btn_body_on = basic_button_body(body_color.opacity(0.5));

      auto btn_img_off = layer(label, btn_body_off);
      auto btn_img_on = left_top_margin({1, 1}, layer(label, btn_body_on));

      return Button(std::move(btn_img_off), std::move(btn_img_on));
   }

   template <typename Button>
   inline Button make_button(
      std::string const& text
    , color body_color = default_button_color
   )
   {
      return make_button<Button>(
         margin(
            button_margin,
            align_center(label(text))
         ),
         body_color
      );
   }

   template <typename Button>
   inline Button make_button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color = default_button_color
   )
   {
      return make_button<Button>(
         margin(
            button_margin,
            align_center(
               htile(
                  right_margin(8, icon(icon_code)),
                  label(text)
               )
            )
         ),
         body_color
      );
   }

   template <typename Button>
   inline Button make_button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color = default_button_color
   )
   {
      return make_button<Button>(
         margin(
            button_margin,
            align_center(
               htile(
                  label(text),
                  left_margin(8, icon(icon_code))
               )
            )
         ),
         body_color
      );
   }

   basic_button
   button(
      std::string const& text
    , color body_color = default_button_color
   );

   basic_button
   button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color = default_button_color
   );

   basic_button
   button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color = default_button_color
   );

   basic_toggle_button
   toggle_button(
      std::string const& text
    , color body_color = default_button_color
   );

   basic_toggle_button
   toggle_button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color = default_button_color
   );

   basic_toggle_button
   toggle_button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color = default_button_color
   );

   basic_latching_button
   latching_button(
      std::string const& text
    , color body_color = default_button_color
   );

   basic_latching_button
   latching_button(
      std::uint32_t icon_code
    , std::string const& text
    , color body_color = default_button_color
   );

   basic_latching_button
   latching_button(
      std::string const& text
    , std::uint32_t icon_code
    , color body_color = default_button_color
   );

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Check Box
   ////////////////////////////////////////////////////////////////////////////////////////////////
   void draw_check_box(
      context const& ctx, std::string const& text, bool state, bool hilite
   );

   template <bool state>
   class check_box_widget : public widget
   {
   public:
                     check_box_widget(std::string const& text)
                      : _text(text)
                     {}

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   draw(context const& ctx);

   private:

      std::string    _text;
   };

   template <bool state>
   rect check_box_widget<state>::limits(basic_context const& ctx) const
   {
      auto& theme_ = ctx.theme();
      point s = text_utils(theme_)
         .measure_text(
            _text.c_str(), theme_.label_font, theme_.label_font_size);
      return { s.x + 45, s.y, s.x + 45, s.y };
   }

   template <bool state>
   void check_box_widget<state>::draw(context const& ctx)
   {
      draw_check_box(ctx, _text, state, ctx.bounds.includes(ctx.cursor_pos()));
   }

   inline basic_toggle_button check_box(std::string const& text)
   {
      return basic_toggle_button(
         check_box_widget<false>{ text }
       , check_box_widget<true>{ text }
      );
   }
   
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Icon Button
   ////////////////////////////////////////////////////////////////////////////////////////////////
   void draw_icon_button(context const& ctx, uint32_t code, float size, bool state, bool hilite);

   template <bool state>
   class icon_button_widget : public widget
   {
   public:
                        icon_button_widget(uint32_t code, float size)
                         : _code(code)
                         , _size(size)
                        {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);
      
      uint32_t          _code;
      float             _size;
   };

   template <bool state>
   rect icon_button_widget<state>::limits(basic_context const& ctx) const
   {
      auto  size = _size * 1.8f;
      return { size, size, size, size };
   }

   template <bool state>
   void icon_button_widget<state>::draw(context const& ctx)
   {
      draw_icon_button(ctx, _code, _size, state, ctx.bounds.includes(ctx.cursor_pos()));
   }

   inline basic_toggle_button icon_button(uint32_t code, float size)
   {
      return {
         icon_button_widget<false>{ code, size }
       , icon_button_widget<true>{ code, size }
      };
   }
   
   inline basic_toggle_button icon_button(uint32_t code1, uint32_t code2, float size)
   {
      return {
         icon_button_widget<false>{ code1, size }
       , icon_button_widget<true>{ code2, size }
      };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Dropdown Menu
   ////////////////////////////////////////////////////////////////////////////////////////////////
   basic_dropdown_menu
   dropdown_menu(
      std::string const& text
    , color body_color = default_button_color
   );

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Menu Items
   ////////////////////////////////////////////////////////////////////////////////////////////////
   inline auto menu_item_text(std::string const& text)
   {
      return xside_margin({ 20, 20 }, align_left(label(text)));
   }

   inline auto menu_item(std::string const& text)
   {
      return basic_menu_item(menu_item_text(text));
   }

   class menu_item_spacer_widget : public widget
   {
   public:

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   draw(context const& ctx);
   };

   inline auto menu_item_spacer()
   {
      return menu_item_spacer_widget{};
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Pane
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Heading, typename Content>
   inline auto pane(Heading&& heading, Content&& content)
   {
      return
        layer(
            align_top(
                layer(
                    align_center(margin({10, 4, 10, 4}, heading)),
                    title_bar{}
                )
            ),
            top_margin(30, std::forward<Content>(content)),
            panel{}
        );
   }

   template <typename Content>
   inline auto pane(std::string const& title, Content&& content, float title_size = 1.0)
   {
      return pane(heading(title, title_size), content);
   }

   template <typename Content>
   inline auto pane(char const* title, Content&& content, float title_size = 1.0)
   {
      return pane(heading(title, title_size), content);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Groups
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Heading, typename Content>
   inline auto group(Heading&& heading, Content&& content)
   {
      return
        layer(
            align_top(align_left(margin({10, 4, 10, 4}, heading))),
            top_margin(20, std::forward<Content>(content)),
            frame{}
        );
   }

   template <typename Content>
   inline auto group(std::string const& title, Content&& content, float label_size = 1.0)
   {
      return group(label(title, label_size), content);
   }

   template <typename Content>
   inline auto group(char const* title, Content&& content, float label_size = 1.0)
   {
      return group(label(title, label_size), content);
   }
   
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Captions
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Control>
   inline auto caption(Control&& control, std::string const& title)
   {
      return
         vtile(
            std::forward<Control>(control),
            align_center(top_margin(5.0, label(title, 0.8)))
         );
   }
}

#endif