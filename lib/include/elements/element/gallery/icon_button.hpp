/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_ICON_BUTTON_JUNE_5_2016)
#define ELEMENTS_GALLERY_ICON_BUTTON_JUNE_5_2016

#include <elements/element/element.hpp>
#include <elements/element/gallery/button.hpp>
#include <elements/support/theme.hpp>

namespace cycfi { namespace elements
{
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
    , bool enabled
   );

   void draw_plain_icon_button(
      context const& ctx
    , uint32_t code
    , float size
    , bool hilite
    , bool enabled
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

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    enable(bool state_ = true) override;
      bool                    is_enabled() const override;

      uint32_t                _code;
      float                   _size;
      color                   _body_color;
      bool                    _is_enabled = true;
   };

   template <bool hilite>
   struct plain_icon_button_element : element
   {
                              plain_icon_button_element(
                                 uint32_t code
                               , float size
                              )
                               : _code(code)
                               , _size(size)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    enable(bool state = true) override;
      bool                    is_enabled() const override;

      uint32_t                _code;
      float                   _size;
      bool                    _is_enabled = true;
   };

   template <bool state, bool hilite>
   inline view_limits icon_button_element<state, hilite>::limits(basic_context const& /* ctx */) const
   {
      auto  size = _size * get_theme().icon_font_size * 1.8f;
      return {{size, size}, {size, size}};
   }

   template <bool state, bool hilite>
   inline void icon_button_element<state, hilite>::draw(context const& ctx)
   {
      draw_icon_button(
         ctx, _code, _size * get_theme().icon_font_size, _body_color, state, hilite, is_enabled()
      );
   }

   template <bool state, bool hilite>
   inline void icon_button_element<state, hilite>::enable(bool state_)
   {
      _is_enabled = state_;
   }

   template <bool state, bool hilite>
   inline bool icon_button_element<state, hilite>::is_enabled() const
   {
      return _is_enabled;
   }

   template <bool hilite>
   inline view_limits plain_icon_button_element<hilite>::limits(basic_context const& /* ctx */) const
   {
      auto  size = _size * get_theme().icon_font_size * 1.8f;
      return {{size, size}, {size, size}};
   }

   template <bool hilite>
   inline void plain_icon_button_element<hilite>::draw(context const& ctx)
   {
      draw_plain_icon_button(
         ctx, _code, _size * get_theme().icon_font_size, hilite, is_enabled()
      );
   }

   template <bool hilite>
   void plain_icon_button_element<hilite>::enable(bool state)
   {
      _is_enabled = state;
   }

   template <bool hilite>
   bool plain_icon_button_element<hilite>::is_enabled() const
   {
      return _is_enabled;
   }

   inline basic_toggle_button<> toggle_icon_button(
      uint32_t code
    , float size
    , color body_color = get_theme().default_button_color
   )
   {
      return {
         icon_button_element<false>{code, size, body_color}
       , icon_button_element<true>{code, size, body_color}
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
         icon_button_element<true, true>{code1, size, body_color}
       , icon_button_element<true, true>{code2, size, body_color}
      };
   }

   inline basic_toggle_button<> plain_toggle_icon_button(
      uint32_t code1
    , uint32_t code2
    , float size
   )
   {
      return {
         plain_icon_button_element<true>{code1, size}
       , plain_icon_button_element<true>{code2, size}
      };
   }

   inline layered_button icon_button(
      uint32_t code
    , float size
    , color body_color = get_theme().default_button_color
   )
   {
      return {
         icon_button_element<false>{code, size, body_color}
       , icon_button_element<true>{code, size, body_color}
      };
   }

   inline layered_button plain_icon_button(uint32_t code, float size)
   {
      return {
         plain_icon_button_element<false>{code, size}
       , plain_icon_button_element<true>{code, size}
      };
   }
}}

#endif
