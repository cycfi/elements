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
#include <cctype>

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
   rect static_text_box_widget::limits(basic_context const& ctx) const
   {
      double text_box_font_size = ctx.theme().text_box_font_size;
      return { 50, text_box_font_size, full_extent, full_extent };
   }

   void static_text_box_widget::draw(context const& ctx)
   {
      ctx.theme().draw_text_box(ctx.bounds, text.c_str());
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect text_box_widget::limits(basic_context const& ctx) const
   {
      double text_box_font_size = ctx.theme().text_box_font_size;
      return { 50, text_box_font_size, full_extent, full_extent };
   }

   void text_box_widget::draw(context const& ctx)
   {
      char const* first = &text[0];
      theme::text_info info = {
         first,
         first + text.size(),
         (select_start == -1)? 0 : first+select_start,
         (select_end == -1)? 0 : first+select_end,
         ctx.window.is_focus()
      };
      ctx.theme().draw_edit_text_box(ctx.bounds, info);
   }

   widget* text_box_widget::click(context const& ctx, mouse_button btn)
   {
      if (!btn.is_pressed) // released? return early
         return this;

      char const* first = &text[0];
      theme::text_info info = {
         first,
         first + text.size()
      };
      if (char const* pos = ctx.theme().caret_position(ctx.bounds, info, ctx.cursor_pos()))
      {
         if (btn.num_clicks != 1)
         {
            char const* last = pos;
            char const* first = pos;

            if (btn.num_clicks == 2)
            {
               // $$$ TODO: Make this UTF8 friendly $$$
               auto wordch = [](char ch) { return std::isspace(ch) || std::ispunct(ch); };

               while (last < info.last && !wordch(*last))
                  last++;
               while (first > info.first && !wordch(*first))
                  first--;
            }
            else if (btn.num_clicks == 3)
            {
               auto parach = [](char ch) { return ch == '\n' || ch == '\r'; };

               while (last < info.last && !parach(*last))
                  last++;
               while (first > info.first && !parach(*first))
                  first--;
            }
            if (first != info.first)
                ++first;
            select_start = int(first-info.first);
            select_end = int(last-info.first);
         }
         else
         {
            select_start = select_end = int(pos-first);
         }
         ctx.window.draw();
      }
      return this;
   }

   void text_box_widget::drag(context const& ctx, mouse_button btn)
   {
      char const* first = &text[0];
      theme::text_info info = {
         first,
         first + text.size()
      };
      if (char const* pos = ctx.theme().caret_position(ctx.bounds, info, ctx.cursor_pos()))
      {
         select_end = int(pos-first);
         ctx.window.draw();
      }
   }

   bool text_box_widget::cursor(context const& ctx, point const& p)
   {
      if (ctx.bounds.includes(p))
      {
         ctx.window.set_cursor(cursor::ibeam);
         return true;
      }
      return false;
   }

   bool text_box_widget::is_control() const
   {
      return true;
   }
}
