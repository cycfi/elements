/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/view.hpp>
#include <elements/window.hpp>
#include <elements/support/context.hpp>

 namespace cycfi { namespace elements
 {
   std::string diplay(key_code k, int mod)
   {
      std::string r;
      char shifted = 0;

      if (mod & mod_shift)
      {
         switch (k)
         {
            case key_code::apostrophe:       shifted = '"'; break;
            case key_code::comma:            shifted = '<'; break;
            case key_code::minus:            shifted = '_'; break;
            case key_code::period:           shifted = '>'; break;
            case key_code::slash:            shifted = '?'; break;

            case key_code::_0:               shifted = ')'; break;
            case key_code::_1:               shifted = '!'; break;
            case key_code::_2:               shifted = '@'; break;
            case key_code::_3:               shifted = '#'; break;
            case key_code::_4:               shifted = '$'; break;
            case key_code::_5:               shifted = '%'; break;
            case key_code::_6:               shifted = '^'; break;
            case key_code::_7:               shifted = '&'; break;
            case key_code::_8:               shifted = '*'; break;
            case key_code::_9:               shifted = '('; break;

            case key_code::semicolon:        shifted = ':'; break;
            case key_code::equal:            shifted = '+'; break;

            case key_code::left_bracket:     shifted = '{'; break;
            case key_code::backslash:        shifted = '|'; break;;
            case key_code::right_bracket:    shifted = '}'; break;
            case key_code::grave_accent:     shifted = '~'; break;

            default:
               r += u8"⇧";
               break;
         }
      }

      if (mod & mod_alt)
         r += u8"⌥";
      if (mod & mod_alt)
         r += u8"⌃";
      if (mod & mod_super)
#if defined(__APPLE__)
         r += u8"⌘";
#elif defined(_WIN32)
         r += u8"⊞";
#elif defined(__linux__)
         r += u8"◇";
#else
         r += u8"�";
#endif

      if (shifted)
      {
         r += shifted;
      }
      else
      {
         switch (k)
         {
            default:                         r += u8"�"; break;

            case key_code::space:            r += ' '; break;
            case key_code::apostrophe:       r += '\''; break;
            case key_code::comma:            r += ','; break;
            case key_code::minus:            r += '-'; break;
            case key_code::period:           r += '.'; break;
            case key_code::slash:            r += '/'; break;

            case key_code::_0: case key_code::_1: case key_code::_2:
            case key_code::_3: case key_code::_4: case key_code::_5:
            case key_code::_6: case key_code::_7: case key_code::_8:
            case key_code::_9:
               r += '0' + (int(k) - int(key_code::_0));
               break;

            case key_code::semicolon:        r += ';'; break;
            case key_code::equal:            r += '='; break;

            case key_code::a: case key_code::b: case key_code::c:
            case key_code::d: case key_code::e: case key_code::f:
            case key_code::g: case key_code::h: case key_code::i:
            case key_code::j: case key_code::k: case key_code::l:
            case key_code::m: case key_code::n: case key_code::o:
            case key_code::p: case key_code::q: case key_code::r:
            case key_code::s: case key_code::t: case key_code::u:
            case key_code::v: case key_code::w: case key_code::x:
            case key_code::y: case key_code::z:
               r += 'A' + (int(k) - int(key_code::a));
               break;

            case key_code::left_bracket:     r += '['; break;
            case key_code::backslash:        r += '\\'; break;
            case key_code::right_bracket:    r += ']'; break;
            case key_code::grave_accent:     r += '`'; break;
            case key_code::escape:           r += u8"⎋"; break;
            case key_code::enter:            r += u8"⏎"; break;
            case key_code::tab:              r += u8"⇥"; break;
            case key_code::backspace:        r += u8"⌫"; break;
            case key_code::insert:           break;
            case key_code::_delete:          r += u8"⌫"; break;
            case key_code::right:            r += u8"→"; break;
            case key_code::left:             r += u8"←"; break;
            case key_code::down:             r += u8"↓"; break;
            case key_code::up:               r += u8"↑"; break;
            case key_code::page_up:          r += u8"⇞"; break;
            case key_code::page_down:        r += u8"⇟"; break;
            case key_code::home:             r += u8"⇱"; break;
            case key_code::end:              r += u8"⇲"; break;
            case key_code::caps_lock:        r += u8"⇪"; break;
            case key_code::scroll_lock:      r += u8"⤓"; break;
            case key_code::num_lock:         r += u8"⇭"; break;
            case key_code::print_screen:     r += u8"⎙"; break;
            case key_code::pause:            r += u8"⎉"; break;

            case key_code::f1: case key_code::f2: case key_code::f3:
            case key_code::f4: case key_code::f5: case key_code::f6:
            case key_code::f7: case key_code::f8: case key_code::f9:
            case key_code::f10: case key_code::f11: case key_code::f12:
            case key_code::f13: case key_code::f14: case key_code::f15:
            case key_code::f16: case key_code::f17: case key_code::f18:
            case key_code::f19: case key_code::f20: case key_code::f21:
            case key_code::f22: case key_code::f23: case key_code::f24:
            case key_code::f25:
               r += 'f' + std::to_string(1 + int(k) - int(key_code::f1));
               break;

            case key_code::kp_0: case key_code::kp_1: case key_code::kp_2:
            case key_code::kp_3: case key_code::kp_4: case key_code::kp_5:
            case key_code::kp_6: case key_code::kp_7: case key_code::kp_8:
            case key_code::kp_9:
               r += '0' + (int(k) - int(key_code::kp_0));
               break;

            case key_code::kp_decimal:       r += '.'; break;
            case key_code::kp_divide:        r += '/'; break;
            case key_code::kp_multiply:      r += '*'; break;
            case key_code::kp_subtract:      r += '-'; break;
            case key_code::kp_add:           r += '+'; break;
            case key_code::kp_enter:         r += u8"⌤"; break;
            case key_code::kp_equal:         r += '='; break;
            case key_code::left_shift:       r += u8"⇧"; break;
            case key_code::left_control:     r += u8"⇧"; break;
            case key_code::left_alt:         r += '='; break;
            case key_code::left_super:       r += '='; break;
            case key_code::right_shift:      r += '='; break;
            case key_code::right_control:    r += u8"⌃"; break;
            case key_code::right_alt:        r += u8"⌥"; break;
            case key_code::right_super:      r += u8"⌘"; break;
            case key_code::menu:             r += u8"☰"; break;
         }
      }

      return r;
   }

   view::view(extent size_)
    : base_view(size_)
    , _work(_io)
   {}

   view::view(host_view_handle h)
    : base_view(h)
    , _work(_io)
   {}

   view::view(window& win)
    : base_view(win.host())
    , _work(_io)
   {
      on_change_limits = [&win](view_limits limits_)
      {
         win.limits(limits_);
      };
   }

   view::~view()
   {
      _io.stop();
   }

   bool view::set_limits()
   {
      if (_content.empty())
         return false;

      auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
      auto context_ = cairo_create(surface_);
      canvas cnv{ *context_ };
      bool resized = false;

      // Update the limits and constrain the window size to the limits
      basic_context bctx{ *this, cnv };
      auto limits_ = _content.limits(bctx);
      if (limits_.min != _current_limits.min || limits_.max != _current_limits.max)
      {
         resized = true;
         _current_limits = limits_;
         if (on_change_limits)
            on_change_limits(limits_);
      }

      cairo_surface_destroy(surface_);
      cairo_destroy(context_);
      return resized;
   }

   void view::draw(cairo_t* context_, rect dirty_)
   {
      if (_content.empty())
         return;

      _dirty = dirty_;

      // Update the limits and constrain the window size to the limits
      if (set_limits())
      {
         refresh();
         return;
      }

      canvas cnv{ *context_ };
      auto size_ = size();
      rect subj_bounds = { 0, 0, size_.x, size_.y };
      context ctx{ *this, cnv, &_content, subj_bounds };

      // layout the subject only if the window bounds changes
      if (subj_bounds != _current_bounds)
      {
         _current_bounds = subj_bounds;
         _content.layout(ctx);
      }

      // draw the subject
      _content.draw(ctx);
   }

   namespace
   {
      template <typename F, typename This>
      void call(F f, This& self, rect _current_bounds)
      {
         auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
         auto context_ = cairo_create(surface_);
         canvas cnv{ *context_ };
         context ctx { self, cnv, &self.content(), _current_bounds };

         f(ctx, self.content());

         cairo_surface_destroy(surface_);
         cairo_destroy(context_);
      }
   }

   void view::layout()
   {
      if (_current_bounds.is_empty())
         return;

      call(
         [](auto const& ctx, auto& _content) { _content.layout(ctx); },
         *this, _current_bounds
      );

      refresh();
   }

   void view::layout(element &element)
   {
      if (_current_bounds.is_empty())
         return;

      call(
         [](auto const& ctx, auto& _content) { _content.layout(ctx); },
         *this, _current_bounds
      );

      refresh(element);
   }

   void view::refresh()
   {
      // Allow refresh to be called from another thread
      _io.post(
         [this]()
         {
            base_view::refresh();
         }
      );
   }

   void view::refresh(rect area)
   {
      // Allow refresh to be called from another thread
      _io.post(
         [this, area]()
         {
            base_view::refresh(area);
         }
      );
   }

   void view::refresh(element& element, int outward)
   {
      if (_current_bounds.is_empty())
         return;

      _io.post(
         [this, &element, outward]()
         {
            call(
               [&element, outward](auto const& ctx, auto& _content)
               {
                  _content.refresh(ctx, element, outward);
               },
               *this, _current_bounds
            );
         }
      );
   }

   void view::refresh(context const& ctx, int outward)
   {
      context const* ctx_ptr = &ctx;
      while (outward > 0 && ctx_ptr)
      {
         --outward;
         ctx_ptr = ctx_ptr->parent;
      }
      if (ctx_ptr)
      {
         auto tl = ctx.canvas.user_to_device(ctx_ptr->bounds.top_left());
         auto br = ctx.canvas.user_to_device(ctx_ptr->bounds.bottom_right());
         refresh({ tl.x, tl.y, br.x, br.y });
      }
   }

   void view::click(mouse_button btn)
   {
      _current_button = btn;
      if (_content.empty())
         return;

      call(
         [btn, this](auto const& ctx, auto& _content)
         {
            _content.click(ctx, btn);
            _is_focus = _content.focus();
         },
         *this, _current_bounds
      );
   }

   void view::drag(mouse_button btn)
   {
      _current_button = btn;
      if (_content.empty())
         return;

      call(
         [btn](auto const& ctx, auto& _content) { _content.drag(ctx, btn); },
         *this, _current_bounds
      );
   }

   void view::cursor(point p, cursor_tracking status)
   {
      if (_content.empty())
         return;

      call(
         [p, status](auto const& ctx, auto& _content)
         {
            if (!_content.cursor(ctx, p, status))
               set_cursor(cursor_type::arrow);
         },
         *this, _current_bounds
      );
   }

   void view::scroll(point dir, point p)
   {
      if (_content.empty())
         return;

      call(
         [dir, p](auto const& ctx, auto& _content) { _content.scroll(ctx, dir, p); },
         *this, _current_bounds
      );
   }

   void view::key(key_info const& k)
   {
      if (_content.empty())
         return;

      call(
         [k](auto const& ctx, auto& _content) { _content.key(ctx, k); },
         *this, _current_bounds
      );
   }

   void view::text(text_info const& info)
   {
      if (_content.empty())
         return;

      call(
         [info](auto const& ctx, auto& _content) { _content.text(ctx, info); },
         *this, _current_bounds
      );
   }

   void view::add_undo(undo_redo_task f)
   {
      _undo_stack.push(f);
      if (has_redo())
      {
         // clear the redo stack
         undo_stack_type empty{};
         _redo_stack.swap(empty);
      }
   }

   bool view::undo()
   {
      if (has_undo())
      {
         auto t = _undo_stack.top();
         _undo_stack.pop();
         _redo_stack.push(t);
         t.undo();  // execute undo function
         return true;
      }
      return false;
   }

   bool view::redo()
   {
      if (has_redo())
      {
         auto t = _redo_stack.top();
         _undo_stack.push(t);
         _redo_stack.pop();
         t.redo();  // execute redo function
         return true;
      }
      return false;
   }

   void view::focus(focus_request r)
   {
      if (_content.empty() || !_is_focus)
         return;

      _content.focus(r);
      refresh();
   }

   void view::content(layers_type&& layers)
   {
      _content = std::forward<layers_type>(layers);
      std::reverse(_content.begin(), _content.end());
      set_limits();
   }

   void view::poll()
   {
      _io.poll();
      if (_tracking_state != tracking::none)
      {
         using namespace std::chrono_literals;
         auto now = std::chrono::steady_clock::now();
         if ((now - _tracking_time) > 1s)
         {
            on_tracking(*_tracking_element, tracking::end_tracking);
            _tracking_time = now;
            _tracking_element = nullptr;
            _tracking_state = tracking::none;
         }
      }
   }

   void view::manage_on_tracking(element& e, tracking state)
   {
      if (_tracking_state == tracking::none &&
         state == tracking::while_tracking)
         state = tracking::begin_tracking;

      if (state == tracking::while_tracking &&
         _tracking_element && _tracking_element != &e)
         on_tracking(*_tracking_element, tracking::end_tracking);

      _tracking_element = &e;
      _tracking_state = state;
      _tracking_time = std::chrono::steady_clock::now();
      on_tracking(e, state);
   }
}}
