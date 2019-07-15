/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016)
#define CYCFI_ELEMENTS_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016

#include <elements/support/theme.hpp>
#include <elements/support/text_utils.hpp>
#include <elements/element.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Box: A simple colored box.
   ////////////////////////////////////////////////////////////////////////////
   struct box_element : element
   {
      box_element(color color_)
       : _color(color_)
      {}

      void draw(context const& ctx)
      {
         auto& cnv = ctx.canvas;
         cnv.fill_style(_color);
         cnv.fill_rect(ctx.bounds);
      }

      color _color;
   };

   inline auto box(color color_)
   {
      return box_element{ color_ };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Pane
   ////////////////////////////////////////////////////////////////////////////
   template <typename Heading, typename Content>
   inline auto pane(
      Heading&& heading,
      Content&& content,
      bool center_heading = true
   )
   {
      auto align_ = center_heading? 0.5 : 0;
      return
        layer(
            align_top(
               layer(
                  halign(
                     align_,
                     margin({10, 4, 10, 4}, std::forward<Heading>(heading))
                  ),
                  title_bar{}
               )
            ),
            top_margin(40, std::forward<Content>(content)),
            panel{}
        );
   }

   template <typename Content>
   inline auto pane(
      std::string const& title,
      Content&& content,
      float title_size = 1.0,
      bool center_heading = true
   )
   {
      return pane(heading(title, title_size), content, center_heading);
   }

   template <typename Content>
   inline auto pane(
      char const* title,
      Content&& content,
      float title_size = 1.0,
      bool center_heading = true
   )
   {
      return pane(heading(title, title_size), content, center_heading);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Groups
   ////////////////////////////////////////////////////////////////////////////
   template <typename Heading, typename Content>
   inline auto make_group(
      Heading&&   heading,
      Content&&   content,
      bool        center_heading = true
   )
   {
      auto align_ = center_heading? 0.5 : 0;
      return
        layer(
            align_top(halign(align_, margin({ 10, 4, 10, 4 }, heading))),
            std::forward<Content>(content),
            frame{}
        );
   }

   template <typename Content>
   inline auto group(
      std::string    title,
      Content&&      content,
      float          label_size = 1.0,
      bool           center_heading = true
   )
   {
      return make_group(
         left_top_margin({ 10, 10 }, heading{ title, label_size }),
         std::forward<Content>(content), center_heading
      );
   }

   template <typename Heading, typename Content>
   inline auto make_unframed_group(
      Heading&&   heading,
      Content&&   content,
      bool        center_heading = true
   )
   {
      auto align_ = center_heading? 0.5 : 0;
      return
        layer(
            align_top(halign(align_, margin({ 10, 4, 10, 4 }, heading))),
            std::forward<Content>(content)
        );
   }

   template <typename Content>
   inline auto unframed_group(
      std::string    title,
      Content&&      content,
      float          label_size = 1.0,
      bool           center_heading = true
   )
   {
      return make_unframed_group(
         left_top_margin({ 10, 10 }, heading{ title, label_size }),
         std::forward<Content>(content), center_heading
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Captions
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   inline auto caption(Content&& content, std::string const& title, float size = 1.0)
   {
      return
         vtile(
            std::forward<Content>(content),
            align_center(top_margin(5.0, label(title, size)))
         );
   }

   template <typename Content>
   inline auto left_caption(Content&& content, std::string const& title, float size = 1.0)
   {
      return
         htile(
            align_middle(right_margin(5.0, label(title, size))),
            std::forward<Content>(content)
         );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Buttons
   ////////////////////////////////////////////////////////////////////////////
   auto constexpr button_margin = rect{ 10, 5, 10, 5 };

   struct basic_button_body : public element
   {
      static float corner_radius;

                              basic_button_body(color body_color);
      virtual void            draw(context const& ctx);

      color                   body_color;
   };

   inline basic_button_body::basic_button_body(color body_color)
    : body_color(body_color)
   {}

   template <typename Button, typename Label>
   inline Button make_button(
      Label&& label
    , color body_color = get_theme().default_button_color)
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
   , float size = 1.0
   , color body_color = get_theme().default_button_color
)
{
   return make_button<Button>(
      margin(
         button_margin,
         align_center(label(text, size))
      ),
      body_color
   );
}

   template <typename Button>
   inline Button make_button(
      std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   )
   {
      return make_button<Button>(
         margin(
            button_margin,
            align_center(
               icon(icon_code, size)
            )
         ),
         body_color
      );
   }

   template <typename Button>
   inline Button make_button(
      std::uint32_t icon_code
    , std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   )
   {
      return make_button<Button>(
         margin(
            button_margin,
            align_center(
               htile(
                  right_margin(8, icon(icon_code, size)),
                  label(text, size)
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
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   )
   {
      return make_button<Button>(
         margin(
            button_margin,
            align_center(
               htile(
                  label(text, size),
                  left_margin(12, icon(icon_code, size))
               )
            )
         ),
         body_color
      );
   }

   layered_button
   button(
      std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   layered_button
   button(
      std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   layered_button
   button(
      std::uint32_t icon_code
    , std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   layered_button
   button(
      std::string const& text
    , std::uint32_t icon_code
    , float size
    , color body_color = get_theme().default_button_color
   );

   basic_toggle_button<>
   toggle_button(
      std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_toggle_button<>
   toggle_button(
      std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_toggle_button<>
   toggle_button(
      std::uint32_t icon_code
    , std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_toggle_button<>
   toggle_button(
      std::string const& text
    , std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_latching_button<>
   latching_button(
      std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_latching_button<>
   latching_button(
      std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_latching_button<>
   latching_button(
      std::uint32_t icon_code
    , std::string const& text
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   basic_latching_button<>
   latching_button(
      std::string const& text
    , std::uint32_t icon_code
    , float size = 1.0
    , color body_color = get_theme().default_button_color
   );

   ////////////////////////////////////////////////////////////////////////////
   // Check Box
   ////////////////////////////////////////////////////////////////////////////
   void draw_check_box(
      context const& ctx, std::string const& text, bool state, bool hilite
   );

   template <bool state>
   struct check_box_element : element
   {
                              check_box_element(std::string const& text)
                               : _text(text)
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

      std::string             _text;
   };

   template <bool state>
   view_limits check_box_element<state>::limits(basic_context const& ctx) const
   {
      auto& thm = get_theme();
      auto  size = measure_text(ctx.canvas, _text.c_str(), thm.label_font, thm.label_font_size);
      size.x += 15 + size.y + 10 + 15;
      return { { size.x, size.y }, { size.x, size.y } };
   }

   template <bool state>
   void check_box_element<state>::draw(context const& ctx)
   {
      draw_check_box(ctx, _text, state, ctx.bounds.includes(ctx.view.cursor_pos()));
   }

   inline basic_toggle_button<> check_box(std::string const& text)
   {
      return basic_toggle_button(
         check_box_element<false>{ text }
       , check_box_element<true>{ text }
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Icon Button
   ////////////////////////////////////////////////////////////////////////////
   void draw_icon_button(
      context const& ctx
    , uint32_t code
    , float size
    , color body_color
    , bool state
    , bool hilite
   );

   template <bool state, bool hilite = state>
   struct icon_button_element : element
   {
                              icon_button_element(
                                 uint32_t code
                               , float size
                               , color body_color = get_theme().default_button_color
                              )
                               : _code(code)
                               , _size(size)
                               , _body_color(body_color)
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

      uint32_t                _code;
      float                   _size;
      color                   _body_color;
   };

   template <bool state, bool hilite>
   inline view_limits icon_button_element<state, hilite>::limits(basic_context const& ctx) const
   {
      auto  size = _size * get_theme().icon_font_size * 1.8f;
      return { { size, size }, { size, size } };
   }

   template <bool state, bool hilite>
   inline void icon_button_element<state, hilite>::draw(context const& ctx)
   {
      draw_icon_button(
         ctx, _code, _size * get_theme().icon_font_size, _body_color, state, hilite
      );
   }

   inline basic_toggle_button<> toggle_icon_button(
      uint32_t code
    , float size
    , color body_color = get_theme().default_button_color
   )
   {
      return {
         icon_button_element<false>{ code, size, body_color }
       , icon_button_element<true>{ code, size, body_color }
      };
   }

   inline basic_toggle_button<> toggle_icon_button(
      uint32_t code1
    , uint32_t code2
    , float size
    , color body_color = get_theme().default_button_color
   )
   {
      return {
         icon_button_element<true, true>{ code1, size, body_color }
       , icon_button_element<true, true>{ code2, size, body_color }
      };
   }

   inline layered_button icon_button(
      uint32_t code
    , float size
    , color body_color = get_theme().default_button_color
   )
   {
      return {
         icon_button_element<false>{ code, size, body_color }
       , icon_button_element<true>{ code, size, body_color }
      };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic buttons
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   inline basic_toggle_button<proxy<Subject, basic_button>>
   toggle_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline proxy<Subject, basic_button>
   momentary_button(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Popup Button
   ////////////////////////////////////////////////////////////////////////////
   basic_dropdown_menu
   dropdown_menu(
      std::string const &text
    , color body_color = get_theme().default_button_color
   );

   ////////////////////////////////////////////////////////////////////////////
   // Menu Background
   ////////////////////////////////////////////////////////////////////////////
   struct menu_background : element
   {
      virtual void            draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////
   // Menu Items
   ////////////////////////////////////////////////////////////////////////////
   inline auto menu_item_text(std::string const& text)
   {
      return xside_margin({ 20, 20 }, align_left(label(text)));
   }

   inline auto menu_item(std::string const& text)
   {
      return basic_menu_item(menu_item_text(text));
   }

   struct menu_item_spacer_element : public element
   {
      virtual view_limits limits(basic_context const& ctx) const;
      virtual void          draw(context const& ctx);
   };

   inline auto menu_item_spacer()
   {
      return menu_item_spacer_element{};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Text Entry
   ////////////////////////////////////////////////////////////////////////////
   template <typename InputBox>
   inline auto input_box(
      InputBox&& text_input
    , rect pad  = rect{ 5, 5, 5, 4 }
   )
   {
      return layer(
         margin(
            pad,
            scroller(
               hsize(16384, std::move(text_input)),
               no_scrollbars | no_vscroll
            )
         ),
         frame{}
      );
   }

   inline auto input_box(
      std::string const& placeholder
    , rect pad = rect{ 5, 5, 5, 4 }
   )
   {
      return input_box(
         basic_input_box{ placeholder }
       , pad
      );
   }

   inline auto input_box(
      char const* placeholder
    , rect pad = rect{ 5, 5, 5, 4 }
   )
   {
      return input_box(
         basic_input_box{ placeholder }
       , pad
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Dials
   //
   //    This function returns a std::pair<E, R>, where E is the element
   //    created and R is a reference to the dial_base which can be used
   //    to attach callbacks (e.g. on_change) when the dial is moved.
   ////////////////////////////////////////////////////////////////////////////
   inline auto dial(
      char const*          knob_sprite
    , float                knob_height
    , char const*          background_image
    , char const*          caption_text
    , float                scale = 1.0/4
    , float                caption_size = 1.0
   )
   {
      auto knob =  sprite{ knob_sprite, knob_height * scale, scale };
      auto lines = image{ background_image, scale };
      auto control = dial(std::move(knob));
      auto image = layer(
         align_center_middle(std::move(control)),
         std::move(lines)
      );

      return std::make_pair(
         align_center_middle(
            caption(
               image,
               caption_text,  // caption
               caption_size   // relative caption text size
            )
         ),
         link(static_cast<dial_base &>(image.at(1)))
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Messages and Alerts
   ////////////////////////////////////////////////////////////////////////////
   inline auto message_box0(
      char const* message
    , std::uint32_t icon_id
    , size size_ = get_theme().message_box_size
   )
   {
      auto textbox = static_text_box{ message };
      auto popup = share(
         align_center_middle(
            fixed_size(size_,
            layer(
               margin({ 20, 20, 20, 20 }, htile(
                  align_top(icon{ icon_id, 2.5 }),
                  left_margin(20, std::move(textbox))
               )),
               panel{}
         ))));

      return popup;
   }

   inline auto message_box1(
      char const* message
    , std::uint32_t icon_id
    , char const* ok_text = "OK"
    , size size_ = get_theme().message_box_size
    , color ok_color = get_theme().indicator_color
   )
   {
      auto textbox = static_text_box{ message };
      auto ok_button = share(button(ok_text, 1.0, ok_color));
      auto popup = share(
         key_intercept(align_center_middle(
            fixed_size(size_,
            layer(
               margin({ 20, 20, 20, 20 },
                  vtile(
                     htile(
                        align_top(icon{ icon_id, 2.5 }),
                        left_margin(20, std::move(textbox))
                     ),
                     align_right(hsize(100, hold(ok_button)))
                  )
               ),
               panel{}
         )))));

      popup->on_key =
         [ok_ = get(ok_button)](auto k)
         {
            if (k.key == key_code::enter)
            {
               if (auto ok = ok_.lock())
                  ok->value(true);
               return true;
            }
            return false;
         };

      return std::pair{ ok_button, popup };
   }

   inline auto message_box2(
      char const* message
    , std::uint32_t icon_id
    , char const* cancel_text = "Cancel"
    , char const* ok_text = "OK"
    , size size_ = get_theme().message_box_size
    , color ok_color = get_theme().indicator_color
   )
   {
      auto textbox = static_text_box{ message };
      auto cancel_button = share(button(cancel_text, 1.0));
      auto ok_button = share(button(ok_text, 1.0, ok_color));
      auto popup = share(
         key_intercept(align_center_middle(
            fixed_size(size_,
            layer(
               margin({ 20, 20, 20, 20 },
                  vtile(
                     htile(
                        align_top(icon{ icon_id, 2.5 }),
                        left_margin(20, std::move(textbox))
                     ),
                     align_right(
                        htile(
                           hsize(100, hold(cancel_button)),
                           left_margin(20, hsize(100, hold(ok_button)))
                        )
                     )
                  )
               ),
               panel{}
         )))));

      popup->on_key =
         [ok_ = get(ok_button), cancel_ = get(cancel_button)](auto k)
         {
            if (k.action == key_action::release)
               return false;

            if (k.key == key_code::enter)
            {
               if (auto ok = ok_.lock())
                  ok->value(true);
               return true;
            }
            else if (k.key == key_code::escape)
            {
               if (auto cancel = cancel_.lock())
                  cancel->value(true);
               return true;
            }
            return false;
         };

      return std::tuple{ ok_button, cancel_button, popup };
   }
}}

#endif
