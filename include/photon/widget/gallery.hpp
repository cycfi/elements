/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016)
#define PHOTON_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016

#include <photon/support/theme.hpp>
#include <photon/support/text_utils.hpp>
#include <photon/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Frames
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class frame : public widget
   {
   public:

      virtual void   draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Headings
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class heading : public widget
   {
   public:

                              heading(std::string const& text, float size_ = 1.0)
                               : _text(text)
                               , _size(size_)
                              {}

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

      std::string             text() const                     { return _text; }
      void                    text(std::string const& text)    { _text = text; }

      using widget::text;

   private:

      std::string       _text;
      float             _size;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Labels
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class label : public widget
   {
   public:

                              label(std::string const& text, float size_ = 1.0)
                               : _text(text)
                               , _size(size_)
                              {}

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

      std::string             text() const                     { return _text; }
      void                    text(std::string const& text)    { _text = text; }

      using widget::text;

   private:

      std::string       _text;
      float             _size;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Grid Lines
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class vgrid_lines : public widget
   {
   public:

                              vgrid_lines(float major_divisions, float minor_divisions)
                               : _major_divisions(major_divisions)
                               , _minor_divisions(minor_divisions)
                              {}

      virtual void            draw(context const& ctx);

   private:

      float                   _major_divisions;
      float                   _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Groups
   ////////////////////////////////////////////////////////////////////////////////////////////////
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

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Captions
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   inline auto caption(Content&& content, std::string const& title, float size = 1.0)
   {
      return
         vtile(
            std::forward<Content>(content),
            align_center(top_margin(5.0, label(title, size)))
         );
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Icons
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class icon : public widget
   {
   public:
                              icon(std::uint32_t code_, float size_ = -1);

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

   private:

      std::uint32_t           _code;
      float                   _size;
   };

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

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

   private:

      std::string             _text;
   };

   template <bool state>
   widget_limits check_box_widget<state>::limits(basic_context const& ctx) const
   {
      auto& thm = get_theme();
      auto  size = measure_text(ctx.canvas, _text.c_str(), thm.label_font, thm.label_font_size);
      return { { size.x, size.y }, { size.x, size.y } };
   }

   template <bool state>
   void check_box_widget<state>::draw(context const& ctx)
   {
      draw_check_box(ctx, _text, state, ctx.bounds.includes(ctx.view.cursor_pos()));
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

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

      uint32_t                _code;
      float                   _size;
   };

   template <bool state>
   widget_limits icon_button_widget<state>::limits(basic_context const& ctx) const
   {
      auto  size = _size * 1.8f;
      return { { size, size }, { size, size } };
   }

   template <bool state>
   void icon_button_widget<state>::draw(context const& ctx)
   {
      draw_icon_button(ctx, _code, _size, state, ctx.bounds.includes(ctx.view.cursor_pos()));
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
   // Popup Button
   ////////////////////////////////////////////////////////////////////////////////////////////////
   basic_popup_button
   popup_button(
      std::string const& text
    , color body_color = default_button_color
   );

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Menu Background
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class menu_background : public widget
   {
   public:

      virtual void            draw(context const& ctx);
   };

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

      virtual widget_limits limits(basic_context const& ctx) const;
      virtual void          draw(context const& ctx);
   };

   inline auto menu_item_spacer()
   {
      return menu_item_spacer_widget{};
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Text Entry
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class input_panel : public widget
   {
   public:

      virtual void draw(context const& ctx);
   };

   template <typename InputBox, typename Panel>
   inline auto input_box(
      InputBox&& text_input
    , Panel&& panel
    , rect pad  = rect{ 5, 5, 5, 4 }
   )
   {
      return layer(
         margin(
            pad,
            scroller(
               hsize(4096, std::move(text_input)),
               no_scrollbars | no_vscroll
            )
         ),
         std::move(panel)
      );
   }
   
   template <typename InputBox>
   inline auto input_box(
      InputBox&& text_input
    , rect pad  = rect{ 5, 5, 5, 4 }
   )
   {
      return input_box(std::move(text_input), input_panel{}, pad);
   }

   inline auto input_box(
      std::string const& placeholder
    , rect pad  = rect{ 5, 5, 5, 4 }
   )
   {
      return input_box(
         basic_input_box{ placeholder }
       , pad
      );
   }

   inline auto input_box(
      char const* placeholder
    , rect pad  = rect{ 5, 5, 5, 4 }
   )
   {
      return input_box(
         basic_input_box{ placeholder }
       , pad
      );
   }
}

#endif