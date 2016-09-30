/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/text.hpp>
#include <photon/widget/port.hpp>
#include <photon/support/theme.hpp>
#include <photon/support/text_utils.hpp>
#include <photon/support/context.hpp>
#include <photon/view.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Static Text Box
   ////////////////////////////////////////////////////////////////////////////////////////////////
   static_text_box::static_text_box(
      std::string const& text
    , char const* face
    , float size
    , color color_
    , int style
   )
    : _text(text)
    , _layout(_text.data(), _text.data() + _text.size(), face, size, style)
    , _color(color_)
   {}

   widget_limits static_text_box::limits(basic_context const& ctx) const
   {
      auto  size = _layout.metrics();
      auto  line_height = size.ascent + size.descent + size.leading;
      return {
         { 200, line_height },
         { full_extent, full_extent }
      };
   }

   void static_text_box::layout(context const& ctx)
   {
      _rows.clear();
      _current_width =ctx.bounds.width();
      _layout.break_lines(_current_width, _rows);
   }

   void static_text_box::draw(context const& ctx)
   {
      auto  metrics = _layout.metrics();
      auto  line_height = metrics.ascent + metrics.descent + metrics.leading;
      auto& cnv = ctx.canvas;
      auto  x = ctx.bounds.left;
      auto  y = ctx.bounds.top + metrics.ascent;

      cnv.rect(ctx.bounds);
      cnv.clip();
      cnv.fill_style(_color);
      for (auto& row : _rows)
      {
         row.draw({ x, y }, cnv);
         y += line_height;
         if (y > ctx.bounds.bottom + metrics.ascent)
            break;
      }
   }

   void static_text_box::text(std::string const& text)
   {
      _text = text;
      _rows.clear();
      _layout.text(_text.data(), _text.data() + _text.size());
      _layout.break_lines(_current_width, _rows);
   }

   void static_text_box::value(std::string val)
   {
      text(val);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Editable Text Box
   ////////////////////////////////////////////////////////////////////////////////////////////////
   basic_text_box::basic_text_box(
      std::string const& text
    , char const* face
    , float size
    , color color_
    , int style
   )
    : static_text_box(text, face, size, color_, style)
    , _select_start(100) // (-1)
    , _select_end(400) // (-1)
    , _current_x(0)
    , _is_focus(false)
   {}

   void basic_text_box::draw(context const& ctx)
   {
      draw_selection(ctx);
      static_text_box::draw(ctx);
   }

   widget* basic_text_box::click(context const& ctx, mouse_button btn)
   {
      if (!btn.down) // released? return early
         return this;

      if (_text.empty())
      {
         _select_start = _select_end = 0;
         scroll_into_view(ctx, false);
         return this;
      }

      char const*   _first = _text.data();
      char const*   _last = _first + _text.size();

      auto mp = btn.pos;
      if (char const* pos = caret_position(ctx, mp))
      {
         if (btn.num_clicks != 1)
         {
            char const* last = pos;
            char const* first = pos;

            if (btn.num_clicks == 2)
            {
               // $$$ TODO: Make this UTF8 friendly $$$
               auto wordch = [](char ch) { return std::isspace(ch) || std::ispunct(ch); };

               while (last < _last && !wordch(*last))
                  last++;
               while (first > _first && !wordch(*first))
                  first--;
            }
            else if (btn.num_clicks == 3)
            {
               auto parach = [](char ch) { return ch == '\n' || ch == '\r'; };

               while (last < _last && !parach(*last))
                  last++;
               while (first > _first && !parach(*first))
                  first--;
            }
            if (first != _first)
                ++first;
            _select_start = int(first - _first);
            _select_end = int(last - _first);
         }
         else
         {
            _select_start = int(pos - _first);
            if (btn.modifiers != mod_shift)
               _select_end = _select_start;
         }
         scroll_into_view(ctx, false);
         _current_x = mp.x-ctx.bounds.left;
      }
      return this;
   }

   void basic_text_box::drag(context const& ctx, mouse_button btn)
   {
      char const* first = &_text[0];
      if (char const* pos = caret_position(ctx, btn.pos))
      {
         _select_end = int(pos-first);
         ctx.view.refresh(ctx);
      }
   }

   bool basic_text_box::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (ctx.bounds.includes(p))
      {
         //ctx.window.set_cursor(cursor::ibeam);
         return true;
      }
      return false;
   }

//   namespace
//   {
//      void add_undo(
//         context const& ctx
//       , std::function<void()>& typing_state
//       , std::function<void()> undo_f
//       , std::function<void()> redo_f
//      )
//      {
//         if (typing_state)
//         {
//            ctx.window.app().add_undo({ typing_state, undo_f });
//            typing_state = {}; // reset
//         }
//         ctx.window.app().add_undo({ undo_f, redo_f });
//      };
//   }

   bool basic_text_box::text(context const& ctx, text_info info_)
   {
      if (_select_start == -1)
         return false;

      std::string text = codepoint_to_utf8(info_.codepoint);

      if (!_typing_state)
         _typing_state = capture_state();

      if (_select_start == _select_end)
         _text.insert(_select_start, text);
      else
         _text.replace(_select_start, _select_end-_select_start, text);
      _select_end = ++_select_start;

      update_text();
      layout(ctx);

      scroll_into_view(ctx, true);
      return true;
   }

   bool basic_text_box::key(context const& ctx, key_info k)
   {
      if (_select_start == -1
         || k.action == key_action::release
         || k.action == key_action::unknown
         )
         return false;

      bool move_caret = false;
      bool save_x = false;

      int start = std::min(_select_end, _select_start);
      int end = std::max(_select_end, _select_start);
      std::function<void()> undo_f = capture_state();

      auto up_down = [this, &ctx, &k, &move_caret]()
      {
         bool up = k.key == key_code::up;
         glyph_metrics info;
         if (_select_end != -1 && _select_start != _select_end)
         {
            int pos = up ?
               std::min(_select_start, _select_end) :
               std::max(_select_start, _select_end)
               ;
            info = glyph_info(ctx, &_text[pos]);
         }
         else
         {
            info = glyph_info(ctx, &_text[_select_start]);
         }
         if (info.str)
         {
            auto y = up ? -info.line_height : +info.line_height;
            auto pos = point{ ctx.bounds.left + _current_x, info.pos.y + y };
            char const* cp = caret_position(ctx, pos);
            if (cp)
               _select_start = int(cp - &_text[0]);
            else
               _select_start = up ? 0 : int(_text.size());
            move_caret = true;
         }
      };

      switch (k.key)
      {
         case key_code::enter:
            {
               _text.replace(start, end-start, "\n");
               _select_start += 1;
               _select_end = _select_start;
               save_x = true;
               //add_undo(ctx, _typing_state, undo_f, capture_state());
            }
            break;

         case key_code::backspace:
         case key_code::_delete:
            delete_();
            save_x = true;
            //add_undo(ctx, _typing_state, undo_f, capture_state());
            break;

         case key_code::left:
            if (_select_end != -1 && _select_start != _select_end)
            {
               if (k.modifiers & mod_shift)
                  --_select_end;
               else
                  _select_start = _select_end = std::min(_select_start, _select_end);
            }
            else if (_select_start != -1 && _select_start > 0)
            {
               --_select_start;
            }
            move_caret = true;
            save_x = true;
            break;

         case key_code::right:
            if (_select_end != -1 && _select_start != _select_end)
            {
               if (k.modifiers & mod_shift)
                  ++_select_end;
               else
                  _select_start = _select_end = std::max(_select_start, _select_end);
            }
            else if (_select_start != -1 && _select_start < _text.size())
            {
               ++_select_start;
            }
            move_caret = true;
            save_x = true;
            break;

         case key_code::up:
            if (_select_start != -1)
               up_down();
            break;

         case key_code::down:
            if (_select_start != -1)
               up_down();
            break;

         case key_code::home:
            _select_start = 0;
            move_caret = true;
            save_x = true;
            break;

         case key_code::end:
            _select_start = int(_text.size());
            move_caret = true;
            save_x = true;
            break;

         case key_code::a:
            if (k.modifiers & mod_super)
            {
               _select_start = 0;
               _select_end = int(_text.size());
            }
            break;

         case key_code::x:
            if (k.modifiers & mod_super)
            {
               cut(ctx.view, start, end);
               save_x = true;
               //add_undo(ctx, _typing_state, undo_f, capture_state());
            }
            break;

         case key_code::c:
            if (k.modifiers & mod_super)
               copy(ctx.view, start, end);
            break;

         case key_code::v:
            if (k.modifiers & mod_super)
            {
               paste(ctx.view, start, end);
               save_x = true;
               //add_undo(ctx, _typing_state, undo_f, capture_state());
            }
            break;

         case key_code::z:
            if (k.modifiers & mod_super)
            {
               if (_typing_state)
               {
                  //ctx.window.app().add_undo({ _typing_state, undo_f });
                  _typing_state = {}; // reset
               }

               if (k.modifiers & mod_shift)
                  ;//ctx.window.app().redo();
               else
                  ;//ctx.window.app().undo();
            }
            break;

         default:
            break;
      }

      if (move_caret)
      {
         clamp(_select_start, 0, int(_text.size()));
         clamp(_select_end, 0, int(_text.size()));
         if (!(k.modifiers & mod_shift))
            _select_end = _select_start;
      }
      else
      {
         update_text();
         layout(ctx);
      }

      scroll_into_view(ctx, save_x);
      return true;
   }

   bool basic_text_box::is_control() const
   {
      return true;
   }

   void basic_text_box::draw_selection(context const& ctx)
   {
      auto& canvas = ctx.canvas;
      auto const& theme = get_theme();
      auto  start_info = glyph_info(ctx, _text.data() + _select_start);
      if (_select_start == _select_end)
      {
         auto width = theme.text_box_caret_width;
         rect& caret = start_info.bounds;
         canvas.line_width(width);
         canvas.stroke_style(theme.text_box_caret_color);
         canvas.move_to({ caret.left + (width/2), caret.top });
         canvas.line_to({ caret.left + (width/2), caret.bottom });
         canvas.stroke();
      }
      else
      {
         rect& r1 = start_info.bounds;
         r1.right = ctx.bounds.right;

         auto  end_info = glyph_info(ctx, _text.data() + _select_end);
         rect& r2 = end_info.bounds;
         r2.right = r2.left;
         r2.left = ctx.bounds.left;

         canvas.fill_style(theme.text_box_hilite_color);
         if (r1.top == r2.top)
         {
            canvas.fill_rect({ r1.left, r1.top, r2.right, r1.bottom });
         }
         else
         {
            canvas.begin_path();
            canvas.move_to(r1.top_left());
            canvas.line_to(r1.top_right());
            canvas.line_to({ r1.right, r2.top });
            canvas.line_to(r2.top_right());
            canvas.line_to(r2.bottom_right());
            canvas.line_to(r2.bottom_left());
            canvas.line_to({ r2.left, r1.bottom });
            canvas.line_to(r1.bottom_left());
            canvas.close_path();
            canvas.fill();
         }
      }
   }

   char const* basic_text_box::caret_position(context const& ctx, point p)
   {
      auto  x = ctx.bounds.left;
      auto  y = ctx.bounds.top;
      auto  metrics = _layout.metrics();
      auto  line_height = metrics.ascent + metrics.descent + metrics.leading;

      char const* found = nullptr;
      for (auto& row : _rows)
      {
         // Check if p is within this row
         if ((p.y >= y) && (p.y < y + line_height))
         {
            // Check if we are at the very start of the row
            if (p.x == x)
            {
               found = row.begin();
               break;
            }

            // Get the actual coordinates of the glyph
            row.for_each(
               [p, x, &found](char const* utf8, unsigned codepoint, float left, float right)
               {
                  if ((p.x >= (x + left)) && (p.x < (x + right)))
                  {
                     found = utf8;
                     return false;
                  }
                  return true;
               }
            );
            // Assume it's at the end of the row if we haven't found a hit
            if (!found)
                found = row.end();
            break;
         }
         y += line_height;
      }
      return found;
   }

   basic_text_box::glyph_metrics basic_text_box::glyph_info(context const& ctx, char const* s)
   {
      auto  metrics = _layout.metrics();
      auto  x = ctx.bounds.left;
      auto  y = ctx.bounds.top + metrics.ascent;
      auto  descent = metrics.descent;
      auto  ascent = metrics.ascent;
      auto  leading = metrics.leading;
      auto  line_height = ascent + descent + leading;

      glyph_metrics info;
      info.str = nullptr;
      info.line_height = line_height;

      // Check if s is at the very end
      if (s == _text.data() + _text.size())
      {
         auto const& last_row = _rows.back();
         auto        rightmost = x + last_row.width();
         auto        bottom_y = y + (line_height * (_rows.size() - 1));

         info.pos = { rightmost, bottom_y };
         info.bounds = { rightmost, bottom_y - ascent, ctx.bounds.right, bottom_y + descent };
         info.str = s;
         return info;
      }

      glyphs*  prev_row = nullptr;
      for (auto& row : _rows)
      {
         // Check if s is within this row
         if (s >= row.begin() && s < row.end())
         {
            // Get the actual coordinates of the glyph
            row.for_each(
               [s, &info, x, y, ascent, descent](char const* utf8, unsigned codepoint, float left, float right)
               {
                  if (utf8 >= s)
                  {
                     info.pos = { x + left, y };
                     info.bounds = { x + left, y - ascent, x + right, y + descent };
                     info.str = utf8;
                     return false;
                  }
                  return true;
               }
            );
            break;
         }
         // This handles the case where s is in between the start of the
         // current row and the end of the previous.
         else if (s < row.begin() && prev_row)
         {
            auto  rightmost = x + prev_row->width();
            auto  prev_y = y - line_height;
            info.pos = { rightmost, prev_y };
            info.bounds = { rightmost, prev_y - ascent, ctx.bounds.right, prev_y + descent };
            info.str = s;
            break;
         }
         y += line_height;
         prev_row = &row;
      }

      return info;
   }

   void basic_text_box::delete_()
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

   void basic_text_box::cut(view& w, int start, int end)
   {
      if (start != end)
      {
         //w.clipboard(_text.substr(start, end-start));
         delete_();
      }
   }

   void basic_text_box::copy(view& w, int start, int end)
   {
      if (start != end)
         ;//w.clipboard(_text.substr(start, end-start));
   }

   void basic_text_box::paste(view& w, int start, int end)
   {
      std::string ins; // = w.clipboard();
      _text.replace(start, end-start, ins);
      start += ins.size();
      _select_start = start;
      _select_end = end = start;
   }

   struct basic_text_box::state_saver
   {
      state_saver(basic_text_box* this_)
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
   basic_text_box::capture_state()
   {
      return state_saver(this);
   }

   void basic_text_box::scroll_into_view(context const& ctx, bool save_x)
   {
      auto info = glyph_info(ctx, &_text[_select_start]);
      if (info.str)
      {
         if (!scrollable::find(ctx).scroll_into_view(info.bounds))
            ctx.view.refresh(ctx);

         if (save_x)
            _current_x = info.pos.x - ctx.bounds.left;
      }
   }

   bool basic_text_box::focus(focus_request r)
   {
      switch (r) {

         case focus_request::wants_focus:
            return true;

         case focus_request::begin_focus:
            _is_focus = true;
            return true;

         case focus_request::end_focus:
            _is_focus = false;
            return true;
      }
      return false;
   }

   void basic_text_box::select_start(int pos)
   {
      if (pos == -1 || (pos >= 0 && pos < _text.size()))
         _select_start = pos;
   }

   void basic_text_box::select_end(int pos)
   {
      if (pos == -1 || (pos >= 0 && pos < _text.size()))
         _select_end = pos;
   }

   void basic_text_box::select_all()
   {
      _select_start = 0;
      _select_end = int(_text.size());
   }

   void basic_text_box::select_none()
   {
      _select_start = _select_end = -1;
   }

   void basic_text_box::update_text()
   {
      _rows.clear();
      _layout.text(_text.data(), _text.data() + _text.size());
      _layout.break_lines(_current_width, _rows);
   }
}
