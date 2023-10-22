/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/text.hpp>
#include <elements/element/port.hpp>
#include <elements/support/theme.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>
#include <infra/utf8_utils.hpp>
#include <utility>

namespace cycfi { namespace elements
{
   using namespace std::chrono_literals;
   using text_layout = artist::text_layout;

   ////////////////////////////////////////////////////////////////////////////
   // Static Text Box
   ////////////////////////////////////////////////////////////////////////////
   static_text_box::static_text_box(
      std::string_view text
    , font_descr font_
    , color color_
   )
    : _font{font_}
    , _layout{font_, text}
    , _color{color_}
   {}

   view_limits static_text_box::limits(basic_context const& /* ctx */) const
   {
      auto  m = _font.metrics();
      auto  min_line_height = m.ascent + m.descent + m.leading;
      float line_height =
         (_current_size.y == -1) ?
         min_line_height :
         std::max(_current_size.y, min_line_height)
         ;

      return {
         {200, line_height},
         {full_extent, full_extent}
      };
   }

   void static_text_box::layout(context const& ctx)
   {
      auto  new_x = ctx.bounds.width();
      _layout.flow(new_x);

      auto  m = _font.metrics();
      auto  new_y = _layout.num_lines() * (m.ascent + m.descent + m.leading);

      // Refresh the union of the old and new bounds if the size has changed
      if (_current_size.x != new_x || _current_size.y != new_y)
      {
         if (_current_size.x != -1 && _current_size.y != -1)
            ctx.view.refresh(union_(ctx.bounds, rect(ctx.bounds.top_left(), extent{_current_size})));
         else
            ctx.view.refresh(ctx.bounds);
      }

      _current_size.x = new_x;
      _current_size.y = new_y;
   }

   void static_text_box::draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      auto  state = cnv.new_state();
      auto  m = _font.metrics();
      auto  p = point{ctx.bounds.left, ctx.bounds.top + m.ascent};

      cnv.add_rect(ctx.bounds);
      cnv.clip();
      _layout.draw(cnv, p, _color);
   }

   void static_text_box::set_text(std::u32string_view text)
   {
      _layout.text(text);
      _layout.flow(_current_size.x);
   }

   void static_text_box::set_text(std::string_view text_)
   {
      set_text(to_utf32(text_));
   }

   void static_text_box::value(std::u32string_view val)
   {
      set_text(val);
   }

   std::size_t static_text_box::insert(std::size_t pos, std::string_view text)
   {
      std::u32string s{get_text().data(), get_text().size()};
      auto utf32 = to_utf32(text);
      auto size = utf32.size();
      s.insert(pos, std::move(utf32));
      set_text(s);
      return size;
   }

   std::size_t static_text_box::replace(std::size_t pos, std::size_t len, std::string_view text)
   {
      std::u32string s{get_text().data(), get_text().size()};
      auto utf32 = to_utf32(text);
      auto size = utf32.size();
      s.replace(pos, len, std::move(utf32));
      set_text(s);
      return size;
   }

   void static_text_box::erase(std::size_t pos, std::size_t len)
   {
      std::u32string s{get_text().data(), get_text().size()};
      s.erase(pos, len);
      set_text(s);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Editable Text Box
   ////////////////////////////////////////////////////////////////////////////
   basic_text_box::basic_text_box(std::string_view text, font_descr font_)
    : static_text_box(text, font_)
    , _select_start(-1)
    , _select_end(-1)
    , _current_x(0)
    , _is_focus(false)
    , _show_caret(true)
    , _caret_started(false)
   {}

   struct basic_text_box::state_saver : std::enable_shared_from_this<basic_text_box::state_saver>
   {
      state_saver(basic_text_box* this_)
       : _this(this_)
       , save_text(this_->get_text())
       , save_select_start(this_->_select_start)
       , save_select_end(this_->_select_end)
      {}

      void restore()
      {
         if (_this)
         {
            _this->set_text(save_text);
            _this->select_start(save_select_start);
            _this->select_end(save_select_end);
         }
      }

      basic_text_box*   _this;
      std::u32string    save_text;
      int               save_select_start;
      int               save_select_end;
   };

   basic_text_box::~basic_text_box()
   {
      for (auto& ss : _state_savers)
         ss.get()->_this = nullptr;
   }

   void basic_text_box::draw(context const& ctx)
   {
      draw_selection(ctx);
      static_text_box::draw(ctx);
      draw_caret(ctx);
   }

   bool basic_text_box::click(context const& ctx, mouse_button btn)
   {
      if (btn.state != mouse_button::left)
         return false;

      _show_caret = true;

      if (!btn.down) // released? return early
         return true;

      auto _text = get_text();
      if (_text.empty())
      {
         _select_start = _select_end = 0;
         scroll_into_view(ctx, false);
         return true;
      }

      char32_t const* begin = _text.data();
      int _size = _text.size();

      if (char32_t const* pos = caret_position(ctx, btn.pos))
      {
         if (btn.num_clicks != 1)
         {
            int end = pos - begin;
            int start = pos - begin;

            auto fixup = [&]()
            {
               if (start > 0)
                  ++start;
               _select_start = start;
               _select_end = end;
            };

            if (btn.num_clicks == 2)
            {
               while (end < _size && !word_break(end))
                  ++end;
               while (start >= 0 && !word_break(start))
                  --start;
               fixup();
            }
            else if (btn.num_clicks == 3)
            {
               while (end < _size && !line_break(end))
                  end++;
               while (start >= 0 && !line_break(start))
                  start--;
               fixup();
            }
         }
         else
         {
            auto hit = int(pos - begin);
            if ((btn.modifiers == mod_shift) && (_select_start != -1))
            {
               if (hit < _select_start)
                  _select_start = hit;
               else
                  _select_end = hit;
            }
            else
            {
               _select_end = _select_start = hit;
            }
         }
         _current_x = btn.pos.x-ctx.bounds.left;
         ctx.view.refresh(ctx);
      }
      return true;
   }

   void basic_text_box::drag(context const& ctx, mouse_button btn)
   {
      char32_t const* first = &get_text()[0];
      if (char32_t const* pos = caret_position(ctx, btn.pos))
      {
         _select_end = int(pos-first);
         ctx.view.refresh(ctx);
         scroll_into_view(ctx, true);
      }
   }

   bool basic_text_box::cursor(context const& ctx, point p, cursor_tracking /* status */)
   {
      if (ctx.bounds.includes(p))
      {
         set_cursor(cursor_type::ibeam);
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
            ctx.view.add_undo({typing_state, undo_f});
            typing_state = {}; // reset
         }
         ctx.view.add_undo({undo_f, redo_f});
      }
   }

   bool basic_text_box::text(context const& ctx, text_info info_)
   {
      _show_caret = true;

      if (_select_start == -1)
         return false;

      if (_select_start > _select_end)
         std::swap(_select_end, _select_start);

      std::string text = codepoint_to_utf8(info_.codepoint);

      if (!_typing_state)
         _typing_state = capture_state();

      bool do_replace = false;
      if (_select_start == _select_end)
      {
         insert(_select_start, text);
      }
      else
      {
         replace(_select_start, _select_end-_select_start, text);
         do_replace = true;
      }

      layout(ctx);

      if (do_replace)
      {
         _select_end = _select_start;
         scroll_into_view(ctx, true);
         _select_end = _select_start += 1;
      }
      else
      {
         _select_end = _select_start += 1;
         scroll_into_view(ctx, true);
      }
      return true;
   }

   void basic_text_box::set_text(std::u32string_view text_)
   {
      static_text_box::set_text(text_);
      _select_start = std::min<int>(_select_start, text_.size());
      _select_end = std::min<int>(_select_end, text_.size());
   }

   bool basic_text_box::key(context const& ctx, key_info k)
   {
      _show_caret = true;

      if (_select_start == -1
         || k.action == key_action::release
         || k.action == key_action::unknown
         )
         return false;

      bool move_caret = false;
      bool save_x = false;
      bool handled = false;

      int start = std::min(_select_end, _select_start);
      int end = std::max(_select_end, _select_start);
      std::function<void()> undo_f = capture_state();
      auto _text = get_text();

      auto up_down = [this, &ctx, k, &move_caret, &_text]()
      {
         bool up = k.key == key_code::up;
         caret_metrics info;
         info = caret_info(ctx, &_text[_select_end]);
         if (info.str)
         {
            auto y = up ? -info.line_height : +info.line_height;
            auto pos = point{ctx.bounds.left + _current_x, info.pos.y + y};
            char32_t const* cp = caret_position(ctx, pos);
            if (cp)
               _select_end = int(cp - &_text[0]);
            else
               _select_end = up ? 0 : int(_text.size());
            move_caret = true;
         }
      };

      auto next_char = [this, &_text]()
      {
         if (_text.size() > 1 && _select_end < static_cast<int>(_text.size()))
            ++_select_end;
      };

      auto prev_char = [this]()
      {
         if (_select_end > 0)
            --_select_end;
      };

      auto next_word = [this, &_text]()
      {
         if (_select_end < static_cast<int>(_text.size()))
         {
            int pos = _select_end;
            int size = _text.size();
            while (pos != size && word_break(pos))
               ++pos;
            while (pos != size && !word_break(pos))
               ++pos;
            _select_end = pos;
         }
      };

      auto prev_word = [this]()
      {
         if (_select_end > 0)
         {
            int pos = _select_end-1;
            while (pos != 0 && word_break(pos))
               --pos;
            while (pos != 0 && !word_break(pos))
               --pos;
            if (pos != 0)
               ++pos;
            _select_end = pos;
         }
      };

      if (k.action == key_action::press || k.action == key_action::repeat)
      {
         switch (k.key)
         {
            case key_code::enter:
               {
                  replace(start, end-start, "\n");
                  _select_start += 1;
                  _select_end = _select_start;
                  save_x = true;
                  add_undo(ctx, _typing_state, undo_f, capture_state());
                  handled = true;
               }
               break;

            case key_code::backspace:
            case key_code::_delete:
               {
                  delete_(k.key == key_code::_delete);
                  save_x = true;
                  add_undo(ctx, _typing_state, undo_f, capture_state());
                  handled = true;
               }
               break;

            case key_code::left:
               if (_select_end != -1)
               {
                  if (k.modifiers & mod_alt)
                     prev_word();
                  else
                     prev_char();
                  if (!(k.modifiers & mod_shift))
                     _select_start = _select_end = std::min(_select_start, _select_end);
               }
               move_caret = true;
               save_x = true;
               handled = true;
               break;

            case key_code::right:
               if (_select_end != -1)
               {
                  if (k.modifiers & mod_alt)
                     next_word();
                  else
                     next_char();
                  if (!(k.modifiers & mod_shift))
                     _select_start = _select_end = std::max(_select_start, _select_end);
               }
               move_caret = true;
               save_x = true;
               handled = true;
               break;

            case key_code::up:
            case key_code::down:
               if (_select_start != -1)
                  up_down();
               handled = true;
               break;

            case key_code::a:
               if (k.modifiers & mod_action)
               {
                  _select_start = 0;
                  _select_end = int(_text.size());
                  handled = true;
               }
               break;

            case key_code::x:
               if (k.modifiers & mod_action)
               {
                  cut(ctx.view, start, end);
                  save_x = true;
                  add_undo(ctx, _typing_state, undo_f, capture_state());
                  handled = true;
               }
               break;

            case key_code::c:
               if (k.modifiers & mod_action)
               {
                  copy(ctx.view, start, end);
                  handled = true;
               }
               break;

            case key_code::v:
               if (k.modifiers & mod_action)
               {
                  paste(ctx.view, start, end);
                  save_x = true;
                  add_undo(ctx, _typing_state, undo_f, capture_state());
                  handled = true;
               }
               break;

            case key_code::z:
               if (k.modifiers & mod_action)
               {
                  if (_typing_state)
                  {
                     ctx.view.add_undo({_typing_state, undo_f});
                     _typing_state = {}; // reset
                  }

                  if (k.modifiers & mod_shift)
                     ctx.view.redo();
                  else
                     ctx.view.undo();
                  handled = true;
               }
               break;

            default:
               break;
         }
      }

      if (move_caret)
      {
         clamp(_select_start, 0, int(_text.size()));
         clamp(_select_end, 0, int(_text.size()));
         if (!(k.modifiers & mod_shift))
            _select_start = _select_end;
      }
      else if (handled)
      {
         layout(ctx);
         auto bounds = ctx.bounds;
         auto size = current_size();
         bounds.width(size.x);
         bounds.height(size.y);
         ctx.view.refresh(bounds);
      }

      if (handled)
         scroll_into_view(ctx, save_x);
      return handled;
   }

   bool basic_text_box::wants_control() const
   {
      return true;
   }

   void basic_text_box::draw_caret(context const& ctx)
   {
      if (_select_start == -1)
         return;

      auto& canvas = ctx.canvas;
      auto const& theme = get_theme();
      rect caret_bounds;
      bool has_caret = false;
      auto _text = get_text();

      // Handle the case where text is empty
      if (_is_focus && _text.empty())
      {
         auto  m = get_font().metrics();
         auto  line_height = m.ascent + m.descent + m.leading;
         auto  width = theme.text_box_caret_width;
         auto  left = ctx.bounds.left;
         auto  top = ctx.bounds.top;

         if (_show_caret)
         {
            canvas.line_width(width);
            canvas.stroke_style(theme.text_box_caret_color);
            canvas.move_to({left + (width/2), top});
            canvas.line_to({left + (width/2), top + line_height});
            canvas.stroke();
         }

         has_caret = true;
         caret_bounds = rect{left, top, left+width, top + line_height};
      }
      // Draw the caret
      else if (_is_focus && (_select_start != -1) && (_select_start == _select_end))
      {
         auto  start_info = caret_info(ctx, _text.data() + _select_start);
         auto width = theme.text_box_caret_width;
         rect& caret = start_info.caret;

         if (_show_caret)
         {
            canvas.line_width(width);
            canvas.stroke_style(theme.text_box_caret_color);
            canvas.move_to({caret.left + (width/2), caret.top});
            canvas.line_to({caret.left + (width/2), caret.bottom});
            canvas.stroke();
         }

         has_caret = true;
         caret_bounds = rect{caret.left, caret.top, caret.left+width, caret.bottom};
      }

      if (_is_focus && has_caret && !_caret_started)
      {
         auto tl = ctx.canvas.user_to_device(caret_bounds.top_left());
         auto br = ctx.canvas.user_to_device(caret_bounds.bottom_right());
         caret_bounds = {tl.x, tl.y, br.x, br.y};

         _caret_started = true;
         ctx.view.post(500ms,
            [this, &_view = ctx.view, caret_bounds]()
            {
               _show_caret = !_show_caret;
               _view.refresh(caret_bounds);
               _caret_started = false;
            }
         );
      }
   }

   void basic_text_box::draw_selection(context const& ctx)
   {
      if (_select_start == -1)
         return;

      auto& canvas = ctx.canvas;
      auto const& theme = get_theme();
      auto _text = get_text();

      if (!_text.empty())
      {
         auto  start_info = caret_info(ctx, _text.data() + _select_start);
         rect& r1 = start_info.caret;
         r1.right = ctx.bounds.right;

         auto  end_info = caret_info(ctx, _text.data() + _select_end);
         rect& r2 = end_info.caret;
         r2.right = r2.left;
         r2.left = ctx.bounds.left;

         auto color = theme.text_box_hilite_color;
         if (!_is_focus)
            color = color.opacity(0.15);
         canvas.fill_style(color);
         if (r1.top == r2.top)
         {
            canvas.fill_rect({r1.left, r1.top, r2.right, r1.bottom});
         }
         else
         {
            canvas.begin_path();
            canvas.move_to(r1.top_left());
            canvas.line_to(r1.top_right());
            canvas.line_to({r1.right, r2.top});
            canvas.line_to(r2.top_right());
            canvas.line_to(r2.bottom_right());
            canvas.line_to(r2.bottom_left());
            canvas.line_to({r2.left, r1.bottom});
            canvas.line_to(r1.bottom_left());
            canvas.close_path();
            canvas.fill();
         }
      }
   }

   char32_t const* basic_text_box::caret_position(context const& ctx, point p)
   {
      auto  m = get_font().metrics();
      auto  x = ctx.bounds.left;
      auto  y = ctx.bounds.top + m.ascent;

      auto  index = get_layout().caret_index(p.x-x, p.y-y); // relative to top-left
      if (index != get_layout().npos)
         return get_text().data() + index;
      return nullptr;
   }

   basic_text_box::caret_metrics basic_text_box::caret_info(context const& ctx, char32_t const* s)
   {
      auto  m = get_font().metrics();
      auto  x = ctx.bounds.left;
      auto  y = ctx.bounds.top + m.ascent;
      auto  pos = get_layout().caret_point(s - &get_text()[0]);

      pos.x += x;
      pos.y += y;
      caret_metrics info;
      info.str = s;
      info.pos = pos;
      info.caret = {pos.x, pos.y - (m.leading + m.ascent), pos.x + 1, pos.y + m.descent};
      info.line_height = m.leading + m.ascent + m.descent;
      return info;
   }

   void basic_text_box::delete_(bool forward)
   {
      auto  start = std::min(_select_end, _select_start);

      if (start != -1)
      {
         auto end = std::max(_select_end, _select_start);
         if (start == end)
         {
            if (forward)
               erase(start, 1);
            else if (start > 0)
               erase(--start, 1);
         }
         else
         {
            erase(start, end-start);
         }
         _select_end = _select_start = start;
      }
   }

   void basic_text_box::cut(view& /* v */, int start, int end)
   {
      if (start != -1 && start != end)
      {
         auto  end_ = std::max(start, end);
         auto  start_ = std::min(start, end);
         clipboard(to_utf8(get_text().substr(start, end_-start_)));
         delete_(false);
      }
   }

   void basic_text_box::copy(view& /* v */, int start, int end)
   {
      if (start != -1 && start != end)
      {
         auto  end_ = std::max(start, end);
         auto  start_ = std::min(start, end);
         clipboard(to_utf8(get_text().substr(start, end_-start_)));
      }
   }

   void basic_text_box::paste(view& /* v */, int start, int end)
   {
      if (start != -1)
      {
         auto  end_ = std::max(start, end);
         auto  start_ = std::min(start, end);
         std::string ins = clipboard();
         start += replace(start, end_-start_, ins);
         _select_end = _select_start = start;
      }
   }

   std::function<void()>
   basic_text_box::capture_state()
   {
      // garbage collect
      for (auto i = _state_savers.begin(); i != _state_savers.end(); )
      {
         if (i->use_count() == 1)
            i = _state_savers.erase(i);
         else
            ++i;
      }

      auto saver_ptr = std::make_shared<state_saver>(this);
      _state_savers.insert(saver_ptr);
      return
         [saver_ptr]()
         {
            if (saver_ptr.use_count() > 1)
               saver_ptr->restore();
         };
   }

   void basic_text_box::scroll_into_view(context const& ctx, bool save_x)
   {
      if (get_text().empty())
      {
         auto caret = rect{
            ctx.bounds.left-1,
            ctx.bounds.top,
            ctx.bounds.left+1,
            ctx.bounds.bottom
         };
         scrollable::find(ctx).scroll_into_view(caret);
         ctx.view.refresh(ctx);
         if (save_x)
            _current_x = 0;
         return;
      }

      if (_select_end == -1)
         return;

      basic_text_box::caret_metrics info;
      if (_select_end >= int(get_text().size()))
         info = caret_info(ctx, U"");
      else
         info = caret_info(ctx, &get_text()[_select_end]);

      if (info.str)
      {
         auto caret = rect{
            info.caret.left-1,
            info.caret.top,
            info.caret.left+1,
            info.caret.bottom
         };
         if (!scrollable::find(ctx).scroll_into_view(caret))
            ctx.view.refresh(ctx);

         if (save_x)
            _current_x = info.pos.x - ctx.bounds.left;
      }
   }

   bool basic_text_box::wants_focus() const
   {
      return true;
   }

   void basic_text_box::begin_focus(focus_request /*req*/)
   {
     _is_focus = true;
      if (_select_start == -1)
         _select_start = _select_end = 0;
   }

   void basic_text_box::end_focus()
   {
      _is_focus = false;
   }

   void basic_text_box::select_start(int pos)
   {
      if (pos == -1 || (pos >= 0 && pos <= static_cast<int>(get_text().size())))
         _select_start = pos;
   }

   void basic_text_box::select_end(int pos)
   {
      if (pos == -1 || (pos >= 0 && pos <= static_cast<int>(get_text().size())))
         _select_end = pos;
   }

   void basic_text_box::select_all()
   {
      _select_start = 0;
      _select_end = static_cast<int>(get_text().size());
   }

   void basic_text_box::select_none()
   {
      _select_start = _select_end = -1;
   }

   bool basic_text_box::word_break(int index) const
   {
      return get_layout().word_break(index) == text_layout::allow_break || line_break(index);
   }

   bool basic_text_box::line_break(int index) const
   {
      return index == 0 || get_layout().line_break(index) == text_layout::must_break;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Input Text Box
   ////////////////////////////////////////////////////////////////////////////
   view_limits basic_input_box::limits(basic_context const& /* ctx */) const
   {
      auto  m = get_font().metrics();
      auto  line_height = m.ascent + m.descent + m.leading;
      return {{32, line_height}, {full_extent, line_height}};
   }

   void basic_input_box::draw(context const& ctx)
   {
      if (get_text().empty())
      {
         if (!_placeholder.empty())
         {
            draw_selection(ctx);

            auto& canvas = ctx.canvas;
            auto& theme = get_theme();
            auto  m = get_font().metrics();

            canvas.text_align(canvas::left);
            canvas.font(theme.text_box_font);
            canvas.fill_style(theme.inactive_font_color);
            canvas.fill_text(
               _placeholder.c_str()
             , {ctx.bounds.left, ctx.bounds.top + m.ascent}
            );
         }
         draw_caret(ctx);
      }
      else
      {
         basic_text_box::draw(ctx);
      }
   }

   bool basic_input_box::text(context const& ctx, text_info info)
   {
      bool r = basic_text_box::text(ctx, info);
      if (on_text)
         on_text(to_utf8(get_text()));
      return r;
   }

   bool basic_input_box::key(context const& ctx, key_info k)
   {
      if (k.action == key_action::press || k.action == key_action::repeat)
      {
         switch (k.key)
         {
            case key_code::enter:
               if (on_enter)
                  on_enter(to_utf8(get_text()));
               ctx.view.refresh(ctx);
               ctx.view.end_focus();
               return true;

            case key_code::up:
            case key_code::down:
            case key_code::tab:
               return false;

            case key_code::home:
               {
                  select_start(0);
                  select_end(0);
                  scroll_into_view(ctx, false);
                  return true;
               }

            case key_code::end:
               {
                  int end = static_cast<int>(get_text().size());
                  select_start(end);
                  select_end(end);
                  scroll_into_view(ctx, false);
                  return true;
               }

            default:
               break;
         }
      }
      return basic_text_box::key(ctx, k);
   }

   void basic_input_box::paste(view& /* v */, int start, int end)
   {
      if (start != -1)
      {
         auto  end_ = std::max(start, end);
         auto  start_ = std::min(start, end);

         std::string clip = clipboard();
         if (clip.empty())
            return;

         std::string ins;

         // Copy clip ito ins, stop when a newline is found.
         // Also, limit ins to input_box_text_limit characters.
         char const* p = &clip[0];
         char const* last = p + clip.size();

         auto const max_chars = get_theme().input_box_text_limit;
         for (std::size_t i = 0; (i < max_chars) && (p != last); ++p, ++i)
         {
            if (is_newline(uint8_t(*p)))
               break;
            ins += *p;
         }

         start_ += replace(start_, end_-start_, ins);
         select_start(start_);
         select_end(start_);

         if (on_text)
            on_text(to_utf8(get_text()));
      }
   }

   void basic_input_box::delete_(bool forward)
   {
      basic_text_box::delete_(forward);
      if (on_text)
         on_text(to_utf8(get_text()));
   }

   bool basic_input_box::click(context const& ctx, mouse_button btn)
   {
      if (btn.state != mouse_button::left)
         return false;

      if (_first_focus && select_start() != select_end())
      {
         _first_focus = false;
         return true;
      }
      _first_focus = false;

      return basic_text_box::click(ctx, btn);
   }

   void basic_input_box::begin_focus(focus_request req)
   {
      _first_focus = true;
      basic_text_box::begin_focus(req);
   }

   void basic_input_box::end_focus()
   {
      _first_focus = false;
      basic_text_box::end_focus();
   }
}}
