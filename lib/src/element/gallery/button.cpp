/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/button.hpp>
#include <utility>

namespace cycfi { namespace elements
{
   void draw_button_base(context const& ctx, rect bounds, color color_, float corner_radius)
   {
      draw_button(ctx.canvas, bounds, color_, corner_radius);
   }

   void basic_button_body::draw(context const& ctx)
   {
      draw_button_base(ctx, ctx.bounds, body_color, corner_radius);
   }

   layered_button
   button(
      std::string text
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(std::move(text), size, body_color);
   }

   layered_button
   button(
      std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(icon_code, size, body_color);
   }

   layered_button
   button(
      std::uint32_t icon_code
    , std::string text
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(icon_code, std::move(text), size, body_color);
   }

   layered_button
   button(
      std::string text
    , std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<layered_button>(std::move(text), icon_code, size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::string text
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(std::move(text), size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(icon_code, size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::uint32_t icon_code
    , std::string text
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(icon_code, std::move(text), size, body_color);
   }

   basic_toggle_button<>
   toggle_button(
      std::string text
    , std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_toggle_button<>>(std::move(text), icon_code, size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::string text
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(std::move(text), size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(icon_code, size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::uint32_t icon_code
    , std::string text
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(icon_code, std::move(text), size, body_color);
   }

   basic_latching_button<>
   latching_button(
      std::string text
    , std::uint32_t icon_code
    , float size
    , color body_color
   )
   {
      return make_button<basic_latching_button<>>(std::move(text), icon_code, size, body_color);
   }
}}
