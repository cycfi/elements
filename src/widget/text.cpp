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
            _select_start = int(pos-first);
            if (ctx.window.key().modifiers != key_shift)
               _select_end = _select_start;
         }
         ctx.window.draw();
         _current_x = mp.x-ctx.bounds.left;
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

   namespace
   {
      void add_undo(
         context const& ctx
       , std::function<void()>& typing_state
       , std::function<void()> undo_f
       , std::function<void()> redo_f
      )
      {
         if (typing_state)
         {
            ctx.window.app().add_undo({ typing_state, undo_f });
            typing_state = {}; // reset
         }
         ctx.window.app().add_undo({ undo_f, redo_f });
      };
   }

   bool text_box_widget::text(context const& ctx, text_info const& info)
   {
      char text[8];
      codepoint_to_UTF8(info.codepoint, text);

      if (!_typing_state)
         _typing_state = capture_state();

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

      int start = std::min(_select_end, _select_start);
      int end = std::max(_select_end, _select_start);
      std::function<void()> undo_f = capture_state();

      switch (k.key)
      {
         case key_code::key_enter:
            {
               _text.replace(start, end-start, "\n");
               _select_start += 1;
               _select_end = _select_start;
               save_x = true;
               add_undo(ctx, _typing_state, undo_f, capture_state());
            }
            break;

         case key_code::key_backspace:
         case key_code::key_delete:
            delete_();
            save_x = true;
            add_undo(ctx, _typing_state, undo_f, capture_state());
            break;

         case key_code::key_left:
         case key_code::key_right:
            if (_select_start != -1 && _select_start > 0 && _select_start < _text.size())
               _select_start += (k.key == key_code::key_left) ? -1 : +1;
            caret_pos = true;
            save_x = true;
            break;

         case key_code::key_up:
         case key_code::key_down:
            if (_select_start != -1 && _select_start > 0 && _select_start < _text.size())
            {
               auto b = ctx.theme().glyph_bounds(ctx.bounds, info, &_text[_select_start]);
               auto y = (k.key == key_code::key_up) ? -b.lineh : +b.lineh;

               char const* cp = ctx.theme()
                  .caret_position(ctx.bounds, info, point{ ctx.bounds.left+_current_x, b.y+y });
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

         case key_code::key_a:
            if (k.modifiers & key_super)
            {
               _select_start = 0;
               _select_end = int(_text.size());
            }
            break;

         case key_code::key_x:
            if (k.modifiers & key_super)
            {
               cut(ctx.window, start, end);
               save_x = true;
               add_undo(ctx, _typing_state, undo_f, capture_state());
            }
            break;

         case key_code::key_c:
            if (k.modifiers & key_super)
               copy(ctx.window, start, end);
            break;

         case key_code::key_v:
            if (k.modifiers & key_super)
            {
               paste(ctx.window, start, end);
               save_x = true;
               add_undo(ctx, _typing_state, undo_f, capture_state());
            }
            break;

         case key_code::key_z:
            if (k.modifiers & key_super)
            {
               if (_typing_state)
               {
                  ctx.window.app().add_undo({ _typing_state, undo_f });
                  _typing_state = {}; // reset
               }

               if (k.modifiers & key_shift)
                  ctx.window.app().redo();
               else
                  ctx.window.app().undo();
            }
            break;

         default:
            break;
      }

      if (caret_pos)
      {
         if (!(k.modifiers & key_shift))
            _select_end = _select_start;
      }

      auto glyph_info = ctx.theme().glyph_bounds(ctx.bounds, info, &_text[_select_start]);

      if (!scrollable::find(ctx).scroll_into_view(glyph_info.bounds()))
         ctx.window.draw();

      if (save_x)
         _current_x = glyph_info.x-ctx.bounds.left;

      return true;
   }

   bool text_box_widget::is_control() const
   {
      return true;
   }

   void text_box_widget::delete_()
   {
      int start = std::min(_select_end, _select_start);
      if (start == _select_end)
      {
         if (start > 0)
            _text.erase(--start, 1);
      }
      else
      {
         int end = std::max(_select_end, _select_start);
         _text.erase(start, end-start);
      }
      _select_end = _select_start = start;
   }

   void text_box_widget::cut(window& w, int start, int end)
   {
      if (start != end)
      {
         w.clipboard(_text.substr(start, end-start));
         delete_();
      }
   }

   void text_box_widget::copy(window& w, int start, int end)
   {
      if (start != end)
         w.clipboard(_text.substr(start, end-start));
   }

   void text_box_widget::paste(window& w, int start, int end)
   {
      std::string ins = w.clipboard();
      _text.replace(start, end-start, ins);
      start += ins.size();
      _select_start = start;
      _select_end = end = start;
   }

   struct text_box_widget::state_saver
   {
      state_saver(text_box_widget* this_)
       : text(this_->_text)
       , save_text(this_->_text)
       , select_start(this_->_select_start)
       , save_select_start(this_->_select_start)
       , select_end(this_->_select_end)
       , save_select_end(this_->_select_end)
      {}

      void operator()()
      {
         text = save_text;
         select_start = save_select_start;
         select_end = save_select_end;
      }

      std::string&   text;
      int&           select_start;
      int&           select_end;

      std::string    save_text;
      int            save_select_start;
      int            save_select_end;
   };

   std::function<void()>
   text_box_widget::capture_state()
   {
      return state_saver(this);
   }
}
