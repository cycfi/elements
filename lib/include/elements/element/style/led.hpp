/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_LED_OCTOBER_22_2026)
#define ELEMENTS_STYLE_LED_OCTOBER_22_2026

#include <elements/element/element.hpp>
#include <elements/support/context.hpp>
#include <elements/support/receiver.hpp>
#include <elements/support/theme.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // LED
   //
   // A round lamp that shows a bool: on, it is lit in its color and glows;
   // off, it is the dim color, by default the on color at a quarter. A
   // small highlight dot sits up and to its left, either way. The
   // lamp is `size` across, centered in a square twice that, the room
   // its glow takes, so the glow is redrawn with it.
   ////////////////////////////////////////////////////////////////////////////
   class led_element : public element, public receiver<bool>
   {
   public:
                              led_element(
                                 color on_color
                               , float size
                               , color off_color
                              )
                               : _on_color{on_color}
                               , _off_color{off_color}
                               , _size{size}
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      std::string             class_name() const override { return "led"; }

      bool                    value() const override { return _on; }
      void                    value(bool on) override { _on = on; }

      color                   on_color() const { return _on_color; }
      void                    on_color(color c) { _on_color = c; }
      color                   off_color() const { return _off_color; }
      void                    off_color(color c) { _off_color = c; }

   private:

      color                   _on_color;
      color                   _off_color;
      float                   _size;
      bool                    _on = false;
   };

   inline auto led(
      color on_color = get_theme().indicator_hilite_color
    , float size = 12
   )
   {
      return led_element{on_color, size, on_color.level(0.25)};
   }

   inline auto led(color on_color, float size, color off_color)
   {
      return led_element{on_color, size, off_color};
   }

   inline view_limits led_element::limits(basic_context const&) const
   {
      auto room = 2 * _size;
      return {{room, room}, {room, room}};
   }

   inline void led_element::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      auto c = center_point(ctx.bounds);
      auto r = _size / 2;
      auto state = cnv.new_state();

      // The glow: the lit color, a little wider than the lamp, blurred so
      // it has faded at the edge of its room
      if (_on)
      {
         auto glow = cnv.new_state();
         cnv.blur_style(r / 2);
         cnv.fill_style(_on_color.opacity(0.8));
         cnv.begin_path();
         cnv.add_circle(c.x, c.y, r * 1.25f);
         cnv.fill();
      }

      // The lamp
      cnv.fill_style(_on ? _on_color : _off_color);
      cnv.begin_path();
      cnv.add_circle(c.x, c.y, r);
      cnv.fill();

      // A small highlight dot up and to the left, soft at its edge
      auto dot_r = r / 3.5f;
      auto dot = point{c.x - r * 0.38f, c.y - r * 0.38f};
      auto gr = canvas::radial_gradient{dot, 0, dot, dot_r};
      gr.add_color_stop({0.0f, colors::white.opacity(_on ? 0.85f : 0.3f)});
      gr.add_color_stop({0.5f, colors::white.opacity(_on ? 0.6f : 0.2f)});
      gr.add_color_stop({1.0f, colors::white.opacity(0)});
      cnv.fill_style(gr);
      cnv.begin_path();
      cnv.add_circle(dot.x, dot.y, dot_r);
      cnv.fill();

      // A dark rim
      cnv.stroke_style(colors::black.opacity(0.5));
      cnv.line_width(1);
      cnv.begin_path();
      cnv.add_circle(c.x, c.y, r);
      cnv.stroke();
   }
}

#endif
