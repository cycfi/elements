/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/text.hpp>
#include <photon/support/theme.hpp>
#include <photon/support/context.hpp>
#include <photon/support/detail/scratch_context.hpp>

namespace photon
{
   text_box::text_box(std::string const& text)
    : _text(text)
   {
   }

   text_box::~text_box()
   {
   }

   void text_box::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      glyphs g{ _text.data(), _text.data() + _text.size(), "Open Sans", 14 };
      
      cnv.stroke_style(colors::black);
      cnv.line_width(0.5);
      cnv.stroke_rect(ctx.bounds);

      auto  x = ctx.bounds.left;
      auto  y = ctx.bounds.top + 14;
      auto  w = ctx.bounds.width();

      std::vector<glyphs> lines;
      g.break_lines(w, lines);

      for (auto& line : lines)
      {
         line.draw({ x, y }, cnv);
         y += 20;
      }



      //auto  gw = g.width();

      //if (w > gw)
      //{
      //   g.draw({ x, y }, cnv);
      //}
      //else
      //{
      //   std::pair<glyphs, glyphs> partitions = g.break_line(w);
      //   while (partitions.first.size())
      //   {
      //      partitions.first.draw({ x, y }, cnv);
      //      y += 20;
      //      gw = partitions.second.width();
      //      if (w > gw)
      //      {
      //         partitions.second.draw({ x, y }, cnv);
      //         break;
      //      }
      //      partitions = partitions.second.break_line(w);
      //   }
      //}
   }

   void text_box::text(std::string const& text)
   {
      _text = text;
   }
}
