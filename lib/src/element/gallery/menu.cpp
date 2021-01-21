/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/menu.hpp>

namespace cycfi { namespace elements
{
   basic_menu
   button_menu(std::string text, menu_position pos, color body_color)
   {
      auto icon =
         (pos == menu_position::bottom_right || pos == menu_position::bottom_left)?
         icons::down_dir : icons::up_dir
         ;
      auto menu = make_button<basic_menu>(std::move(text), icon, 1.0, body_color);
      menu.position(pos);
      return menu;
   }

   basic_menu
   button_menu(menu_position pos, color body_color)
   {
      auto icon =
         (pos == menu_position::bottom_right || pos == menu_position::bottom_left)?
         icons::down_dir : icons::up_dir
         ;
      auto menu = make_button<basic_menu>(icon, 1.0, body_color);
      menu.position(pos);
      return menu;
   }

   basic_menu icon_menu(uint32_t code, float size, menu_position pos)
   {
      auto menu = text_button<basic_menu>(code, size, /*no_frame*/ true);
      menu.position(pos);
      return menu;
   }

   view_limits menu_item_spacer_element::limits(basic_context const& /* ctx */) const
   {
      auto height = get_theme().label_font_size;
      return { { 0, height }, { full_extent, height } };
   }

   void menu_item_spacer_element::draw(context const& ctx)
   {
      auto& canvas_ = ctx.canvas;
      float y = ctx.bounds.top + ctx.bounds.height() / 2;

      canvas_.begin_path();
      canvas_.move_to({ ctx.bounds.left, y });
      canvas_.line_to({ ctx.bounds.right, y });
      canvas_.stroke_style(get_theme().frame_color.opacity(0.25));
      canvas_.line_width(1);
      canvas_.stroke();
   }

   std::pair<basic_menu, std::shared_ptr<basic_label>>
   selection_menu(std::string init)
   {
      auto btn_text = share(label(std::move(init)).relative_font_size(1.0));

      auto menu_btn = text_button<basic_menu>(
         margin(
            get_theme().button_margin,
            htile(
               align_left(hold(btn_text)),
               align_right(left_margin(12, icon(icons::down_dir, 1.0)))
            )
         )
      );

      menu_btn.position(menu_position::bottom_right);
      return { std::move(menu_btn), btn_text };
   }

   std::pair<basic_menu, std::shared_ptr<basic_label>>
   selection_menu(
      std::function<void(string_view item)> on_select
    , menu_selector const& items
    , float text_align
   )
   {
      auto r = selection_menu(items.size()? std::string(items[0]) : "");

      if (items.size())
      {
         vtile_composite list;
         for (std::size_t i = 0; i != items.size(); ++i)
         {
            auto e = share(menu_item(std::string(items[i]), text_align));
            auto label = find_subject<text_reader*>(e.get());
            if (label)
            {
               e->on_click = [btn_text = r.second, on_select, label]()
               {
                  auto text = label->get_text();
                  btn_text->set_text(text);
                  on_select(text);
               };
            }
            list.push_back(e);
         }

         auto menu = layer(list, panel{});
         r.first.menu(menu);
      }

      return r;
   }

   std::pair<std::string, std::string>
   diplay_shortcut(key_code k, int mod)
   {
      std::string mod_, key_;
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
#if defined(__APPLE__)
               mod_ += u8"⇧";
#else
               mod_ += "Shift+";
#endif
               break;
         }
      }

#if defined(__APPLE__)
      if (mod & mod_alt)
         mod_ += u8"⌥";
      if (mod & mod_control)
         mod_ += u8"⌃";
      if ((mod & mod_command) || (mod & mod_action))
         mod_ += u8"⌘";
#else
      if ((mod & mod_control) || (mod & mod_action))
         mod_ = "Ctrl+" + mod_;
      if (mod & mod_alt)
         mod_ += "Alt+";
      if (mod & mod_super)
#if defined(_WIN32)
         mod_ += u8"⊞";
#else
         mod_ += u8"◇";
#endif
#endif

      if (shifted)
      {
         key_ += shifted;
      }
      else
      {
         switch (k)
         {
            default:                         key_ += u8"�"; break;

            case key_code::space:            key_ += ' '; break;
            case key_code::apostrophe:       key_ += '\''; break;
            case key_code::comma:            key_ += ','; break;
            case key_code::minus:            key_ += '-'; break;
            case key_code::period:           key_ += '.'; break;
            case key_code::slash:            key_ += '/'; break;

            case key_code::_0: case key_code::_1: case key_code::_2:
            case key_code::_3: case key_code::_4: case key_code::_5:
            case key_code::_6: case key_code::_7: case key_code::_8:
            case key_code::_9:
               key_ += '0' + (int(k) - int(key_code::_0));
               break;

            case key_code::semicolon:        key_ += ';'; break;
            case key_code::equal:            key_ += '='; break;

            case key_code::a: case key_code::b: case key_code::c:
            case key_code::d: case key_code::e: case key_code::f:
            case key_code::g: case key_code::h: case key_code::i:
            case key_code::j: case key_code::k: case key_code::l:
            case key_code::m: case key_code::n: case key_code::o:
            case key_code::p: case key_code::q: case key_code::r:
            case key_code::s: case key_code::t: case key_code::u:
            case key_code::v: case key_code::w: case key_code::x:
            case key_code::y: case key_code::z:
               key_ += 'A' + (int(k) - int(key_code::a));
               break;

            case key_code::left_bracket:     key_ += '['; break;
            case key_code::backslash:        key_ += '\\'; break;
            case key_code::right_bracket:    key_ += ']'; break;
            case key_code::grave_accent:     key_ += '`'; break;
            case key_code::escape:           key_ += u8"⎋"; break;
            case key_code::enter:            key_ += u8"⏎"; break;
            case key_code::tab:              key_ += u8"⇥"; break;
            case key_code::backspace:        key_ += u8"⌫"; break;
            case key_code::insert:           break;
            case key_code::_delete:          key_ += u8"⌫"; break;
            case key_code::right:            key_ += u8"→"; break;
            case key_code::left:             key_ += u8"←"; break;
            case key_code::down:             key_ += u8"↓"; break;
            case key_code::up:               key_ += u8"↑"; break;
            case key_code::page_up:          key_ += u8"⇞"; break;
            case key_code::page_down:        key_ += u8"⇟"; break;
            case key_code::home:             key_ += u8"⇱"; break;
            case key_code::end:              key_ += u8"⇲"; break;
            case key_code::caps_lock:        key_ += u8"⇪"; break;
            case key_code::scroll_lock:      key_ += u8"⤓"; break;
            case key_code::num_lock:         key_ += u8"⇭"; break;
            case key_code::print_screen:     key_ += u8"⎙"; break;
            case key_code::pause:            key_ += u8"⎉"; break;

            case key_code::f1: case key_code::f2: case key_code::f3:
            case key_code::f4: case key_code::f5: case key_code::f6:
            case key_code::f7: case key_code::f8: case key_code::f9:
            case key_code::f10: case key_code::f11: case key_code::f12:
            case key_code::f13: case key_code::f14: case key_code::f15:
            case key_code::f16: case key_code::f17: case key_code::f18:
            case key_code::f19: case key_code::f20: case key_code::f21:
            case key_code::f22: case key_code::f23: case key_code::f24:
            case key_code::f25:
               key_ += 'f' + std::to_string(1 + int(k) - int(key_code::f1));
               break;

            case key_code::kp_0: case key_code::kp_1: case key_code::kp_2:
            case key_code::kp_3: case key_code::kp_4: case key_code::kp_5:
            case key_code::kp_6: case key_code::kp_7: case key_code::kp_8:
            case key_code::kp_9:
               key_ += '0' + (int(k) - int(key_code::kp_0));
               break;

            case key_code::kp_decimal:       key_ += '.'; break;
            case key_code::kp_divide:        key_ += '/'; break;
            case key_code::kp_multiply:      key_ += '*'; break;
            case key_code::kp_subtract:      key_ += '-'; break;
            case key_code::kp_add:           key_ += '+'; break;
            case key_code::kp_enter:         key_ += u8"⌤"; break;
            case key_code::kp_equal:         key_ += '='; break;
            case key_code::menu:             key_ += u8"☰"; break;
         }
      }

      return { mod_, key_ };
   }
}}
