/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/text.hpp>
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Headings
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect heading_widget::limits(basic_context const& ctx) const
   {
      point s = ctx.theme().measure_heading(text.c_str());
      return { s.x, s.y, s.x, s.y };
   }

   void heading_widget::draw(context const& ctx)
   {
      ctx.theme().draw_heading(ctx.bounds, text.c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Labels
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect label_widget::limits(basic_context const& ctx) const
   {
      point s = ctx.theme().measure_label(text.c_str());
      return { s.x, s.y, s.x, s.y };
   }

   void label_widget::draw(context const& ctx)
   {
      ctx.theme().draw_label(ctx.bounds, text.c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Text Boxes
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect text_box_widget::limits(basic_context const& ctx) const
   {
      double text_box_font_size = ctx.theme().text_box_font_size;
      return { 50, text_box_font_size, full_extent, full_extent };
   }

   void text_box_widget::draw(context const& ctx)
   {
      ctx.theme().draw_text_box(ctx.bounds, text.c_str());
   }
}
