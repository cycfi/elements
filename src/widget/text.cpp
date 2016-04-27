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
#include <cmath>

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
      char const* first = &_text[0];
      theme::text_info info = {
         first,
         first + _text.size()
      };

      rect bounds = { 0, 0, _width, full_extent };
      double height = ctx.theme().edit_text_box_height(bounds, info);
      return { _width, height, _width, height };
   }

   void text_box_widget::draw(context const& ctx)
   {
      char const* first = &_text[0];
      theme::text_draw_info info = {
         first,
         first + _text.size(),
         (_select_start == -1)? 0 : first+_select_start,
         (_select_end == -1)? 0 : first+_select_end,
         ctx.window.is_focus()
      };
      ctx.theme().draw_edit_text_box(ctx.bounds, info);
   }

   widget* text_box_widget::click(context const& ctx, mouse_button btn)
   {
      if (!btn.is_pressed) // released? return early
         return this;

      char const* first = &_text[0];
      theme::text_info info = {
         first,
         first + _text.size()
      };

      auto mp = ctx.cursor_pos();
      if (char const* pos = ctx.theme().caret_position(ctx.bounds, info, mp))
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
            _select_start = int(first-info.first);
            _select_end = int(last-info.first);
         }
         else
         {
            _select_start = _select_end = int(pos-first);
         }
         ctx.window.draw();
         _current_x = mp.x;
      }
      return this;
   }

   void text_box_widget::drag(context const& ctx, mouse_button btn)
   {
      char const* first = &_text[0];
      theme::text_info info = {
         first,
         first + _text.size()
      };
      if (char const* pos = ctx.theme().caret_position(ctx.bounds, info, ctx.cursor_pos()))
      {
         _select_end = int(pos-first);
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

   bool text_box_widget::text(context const& ctx, text_info const& info)
   {
      char text[8];
      codepoint_to_UTF8(info.codepoint, text);

      if (_select_start == _select_end)
         _text.insert(_select_start, text);
      else
         _text.replace(_select_start, _select_end-_select_start, text);
      _select_end = ++_select_start;
      ctx.window.draw();
      return true;
   }

   bool text_box_widget::key(context const& ctx, key_info const& k)
   {
      if (k.action == key_action::release || k.action == key_action::unknown)
         return false;

      bool caret_pos = false;
      bool save_x = false;

      char const* first = &_text[0];
      theme::text_info info = {
         first,
         first + _text.size()
      };

      switch (k.key)
      {
         case key_code::key_backspace:
         case key_code::key_delete:
            if (_select_start == _select_end)
            {
               if (_select_start > 0)
                  _text.erase(--_select_start, 1);
            }
            else
            {
               _text.erase(_select_start, _select_end-_select_start);
            }
            _select_end = _select_start;
            save_x = true;
            break;

         case key_code::key_right:
            if (_select_start != -1 && _select_start < _text.size())
               ++_select_start;
            caret_pos = true;
            save_x = true;
            break;

         case key_code::key_left:
            if (_select_start != -1 && _select_start > 0)
               --_select_start;
            caret_pos = true;
            save_x = true;
            break;

         case key_code::key_down:
            if (_select_start != -1 && _select_start < _text.size())
            {
               auto b = ctx.theme().glyph_bounds(ctx.bounds, info, &_text[_select_start]);
               char const* cp = ctx.theme().caret_position(ctx.bounds, info, point{ _current_x, b.y+b.lineh });
               if (cp)
               {
                  _select_start = int(cp - &_text[0]);
                  caret_pos = true;
               }
            }
            break;

         case key_code::key_up:
            if (_select_start != -1 && _select_start > 0)
            {
               auto b = ctx.theme().glyph_bounds(ctx.bounds, info, &_text[_select_start]);
               char const* cp = ctx.theme().caret_position(ctx.bounds, info, point{ _current_x, b.y-b.lineh });
               if (cp)
               {
                  _select_start = int(cp - &_text[0]);
                  caret_pos = true;
               }
            }
            break;

         case key_code::key_home:
            _select_start = 0;
            caret_pos = true;
            save_x = true;
            break;

         case key_code::key_end:
            _select_start = int(_text.size());
            caret_pos = true;
            save_x = true;
            break;

         default:
            break;
      }

      if (caret_pos)
      {
         if (!(k.modifiers & key_shift))
            _select_end = _select_start;
      }

      ctx.window.draw();

      if (save_x)
      {
         auto b = ctx.theme().glyph_bounds(ctx.bounds, info, &_text[_select_start]);
         _current_x = b.x;
      }

      return true;
   }

   bool text_box_widget::is_control() const
   {
      return true;
   }
}
