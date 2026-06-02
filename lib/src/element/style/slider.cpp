/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/style/slider.hpp>
#include <elements/support/theme.hpp>
#include <cmath>

namespace cycfi::elements
{
   void draw_slider_marks(
      canvas& cnv, rect bounds, float size, std::size_t num_divs
    , std::size_t major_divs, color c)
   {
      auto w = bounds.width();
      auto h = bounds.height();
      bool vertical = w < h;
      float pos = vertical? bounds.top : bounds.left;
      float incr = (vertical? h : w) / num_divs;
      auto state = cnv.new_state();
      auto const& theme = get_theme();

      cnv.stroke_style(theme.ticks_color);
      for (std::size_t i = 0; i != num_divs+1; ++i)
      {
         float inset = 0;
         if (i % (num_divs / major_divs))
         {
            // Minor ticks
            inset = size / 6;
            cnv.line_width(theme.minor_ticks_width);
            cnv.stroke_style(c.level(theme.minor_ticks_level));
         }
         else
         {
            // Major ticks
            cnv.line_width(theme.major_ticks_width);
            cnv.stroke_style(c.level(theme.major_ticks_level));
         }

         if (vertical)
         {
            cnv.move_to({bounds.left + inset, pos});
            cnv.line_to({bounds.right - inset, pos});
         }
         else
         {
            cnv.move_to({pos, bounds.top + inset});
            cnv.line_to({pos, bounds.bottom - inset});
         }
         cnv.stroke();
         pos += incr;
      }
   }

   void draw_slider_marks_lin(
      canvas& cnv, rect bounds, float size, std::size_t major_divs
    , std::size_t minor_divs, color c)
   {
      auto w = bounds.width();
      auto h = bounds.height();
      auto state = cnv.new_state();
      auto const& theme = get_theme();

      float inset = size / 6;
      bool vertical = w < h;
      float xmin = vertical? bounds.top : bounds.left;
      float xmax = vertical? bounds.bottom : bounds.right;
      float major_step = (xmax-xmin)/major_divs;
      float minor_step = major_step/minor_divs;

      for (std::size_t i = 0; i != major_divs+1; ++i)
      {
         float pos = xmin + i*major_step;

         cnv.line_width(theme.major_ticks_width);
         cnv.stroke_style(c.level(theme.major_ticks_level));
         if (vertical)
         {
            cnv.move_to({bounds.left, pos});
            cnv.line_to({bounds.right, pos});
         }
         else
         {
            cnv.move_to({pos, bounds.top});
            cnv.line_to({pos, bounds.bottom});
         }
         cnv.stroke();

         if (i == major_divs) {break;}
         cnv.line_width(theme.minor_ticks_width);
         cnv.stroke_style(c.level(theme.minor_ticks_level));
         for (std::size_t j = 1; j != minor_divs; ++j)
         {
            float minor_pos = pos + j*minor_step;

            if (vertical)
            {
               cnv.move_to({bounds.left + inset, minor_pos});
               cnv.line_to({bounds.right - inset, minor_pos});
            }
            else
            {
               cnv.move_to({minor_pos, bounds.top + inset});
               cnv.line_to({minor_pos, bounds.bottom - inset});
            }
            cnv.stroke();
         }
      }
   }

   void draw_slider_marks_log(
      canvas& cnv, rect bounds, float size, std::size_t major_divs
    , std::size_t minor_divs, color c)
   {
      auto w = bounds.width();
      auto h = bounds.height();
      auto state = cnv.new_state();
      auto const& theme = get_theme();

      float inset = size / 6;
      bool vertical = w < h;
      float xmin = vertical? bounds.top : bounds.left;
      float xmax = vertical? bounds.bottom : bounds.right;
      float major_step = (xmax-xmin)/major_divs;

      std::vector<float> minor_offsets(minor_divs);
      for (std::size_t i = 1; i != minor_divs; ++i)
      {
         minor_offsets[i-1] = std::log10(i)*major_step;
      }

      for (std::size_t i = 0; i != major_divs+1; ++i)
      {
         float pos = xmin + i*major_step;

         cnv.line_width(theme.major_ticks_width);
         cnv.stroke_style(c.level(theme.major_ticks_level));
         if (vertical)
         {
            cnv.move_to({bounds.left, pos});
            cnv.line_to({bounds.right, pos});
         }
         else
         {
            cnv.move_to({pos, bounds.top});
            cnv.line_to({pos, bounds.bottom});
         }
         cnv.stroke();

         if (i == major_divs) {break;}
         cnv.line_width(theme.minor_ticks_width);
         cnv.stroke_style(c.level(theme.minor_ticks_level));
         for (std::size_t j = 1; j != minor_divs; ++j)
         {
            float minor_pos = pos + minor_offsets[j-1];
            if (vertical)
            {
               cnv.move_to({bounds.left + inset, minor_pos});
               cnv.line_to({bounds.right - inset, minor_pos});
            }
            else
            {
               cnv.move_to({minor_pos, bounds.top + inset});
               cnv.line_to({minor_pos, bounds.bottom - inset});
            }
            cnv.stroke();
         }
      }
   }

   void draw_slider_labels(
      canvas& cnv
    , rect bounds
    , float size
    , float font_size
    , std::string const labels[]
    , std::size_t num_labels
   )
   {
      bool reverse = size < 0;
      bool vertical = bounds.width() < bounds.height();
      auto w = bounds.width();
      auto h = bounds.height();
      float pos = vertical? bounds.top : bounds.left;
      float incr = (vertical? h : w) / (num_labels - 1);
      auto state = cnv.new_state();
      auto const& theme = get_theme();

      cnv.font(
         theme.label_font.size(theme.label_font._size * font_size)
      );

      cnv.text_align(cnv.middle | cnv.center);
      cnv.fill_style(theme.label_font_color);

      for (std::size_t i = 0; i != num_labels; ++i)
      {
         point where = vertical?
            point{reverse? bounds.left : bounds.right, pos} :
            point{pos, reverse? bounds.top : bounds.bottom}
            ;

         cnv.fill_text(labels[vertical? (num_labels-i)-1 : i].c_str(), where);
         pos += incr;
      }
   }
}
