/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>
#include <gdk/gdkkeysyms.h>

namespace cycfi { namespace elements
{
   key_code translate_key(unsigned key)
   {
      constexpr auto print_screen_key = GDK_KEY_3270_PrintScreen;

      switch (key)
      {
         default:    return key_code::unknown;

         case GDK_KEY_0:  return key_code:: _0;
         case GDK_KEY_1:  return key_code:: _1;
         case GDK_KEY_2:  return key_code:: _2;
         case GDK_KEY_3:  return key_code:: _3;
         case GDK_KEY_4:  return key_code:: _4;
         case GDK_KEY_5:  return key_code:: _5;
         case GDK_KEY_6:  return key_code:: _6;
         case GDK_KEY_7:  return key_code:: _7;
         case GDK_KEY_8:  return key_code:: _8;
         case GDK_KEY_9:  return key_code:: _9;

         case GDK_KEY_a:  return key_code::a;
         case GDK_KEY_b:  return key_code::b;
         case GDK_KEY_c:  return key_code::c;
         case GDK_KEY_d:  return key_code::d;
         case GDK_KEY_e:  return key_code::e;
         case GDK_KEY_f:  return key_code::f;
         case GDK_KEY_g:  return key_code::g;
         case GDK_KEY_h:  return key_code::h;
         case GDK_KEY_i:  return key_code::i;
         case GDK_KEY_j:  return key_code::j;
         case GDK_KEY_k:  return key_code::k;
         case GDK_KEY_l:  return key_code::l;
         case GDK_KEY_m:  return key_code::m;
         case GDK_KEY_n:  return key_code::n;
         case GDK_KEY_o:  return key_code::o;
         case GDK_KEY_p:  return key_code::p;
         case GDK_KEY_q:  return key_code::q;
         case GDK_KEY_r:  return key_code::r;
         case GDK_KEY_s:  return key_code::s;
         case GDK_KEY_t:  return key_code::t;
         case GDK_KEY_u:  return key_code::u;
         case GDK_KEY_v:  return key_code::v;
         case GDK_KEY_w:  return key_code::w;
         case GDK_KEY_x:  return key_code::x;
         case GDK_KEY_y:  return key_code::y;
         case GDK_KEY_z:  return key_code::z;

         case GDK_KEY_apostrophe:   return key_code::apostrophe;
         case GDK_KEY_backslash:    return key_code::backslash;
         case GDK_KEY_comma:        return key_code::comma;
         case GDK_KEY_equal:        return key_code::equal;
         case GDK_KEY_grave:        return key_code::grave_accent;
         case GDK_KEY_bracketleft:  return key_code::left_bracket;
         case GDK_KEY_minus:        return key_code::minus;
         case GDK_KEY_period:       return key_code::period;
         case GDK_KEY_bracketright: return key_code::right_bracket;
         case GDK_KEY_semicolon:    return key_code::semicolon;
         case GDK_KEY_slash:        return key_code::slash;

         case GDK_KEY_BackSpace:    return key_code::backspace;
         case GDK_KEY_Delete:       return key_code::_delete;
         case GDK_KEY_End:          return key_code::end;
         case GDK_KEY_Return:
         case GDK_KEY_ISO_Enter:    return key_code::enter;
         case GDK_KEY_Escape:       return key_code::escape;
         case GDK_KEY_Home:         return key_code::home;
         case GDK_KEY_Insert:       return key_code::insert;
         case GDK_KEY_Menu:         return key_code::menu;
         case GDK_KEY_Page_Down:    return key_code::page_down;
         case GDK_KEY_Page_Up:      return key_code::page_up;
         case GDK_KEY_Pause:        return key_code::pause;
         case GDK_KEY_space:        return key_code::space;
         case GDK_KEY_Tab:          return key_code::tab;
         case GDK_KEY_Caps_Lock:    return key_code::caps_lock;
         case GDK_KEY_Num_Lock:     return key_code::num_lock;
         case GDK_KEY_Scroll_Lock:  return key_code::scroll_lock;

         case GDK_KEY_F1:     return key_code::f1;
         case GDK_KEY_F2:     return key_code::f2;
         case GDK_KEY_F3:     return key_code::f3;
         case GDK_KEY_F4:     return key_code::f4;
         case GDK_KEY_F5:     return key_code::f5;
         case GDK_KEY_F6:     return key_code::f6;
         case GDK_KEY_F7:     return key_code::f7;
         case GDK_KEY_F8:     return key_code::f8;
         case GDK_KEY_F9:     return key_code::f9;
         case GDK_KEY_F10:    return key_code::f10;
         case GDK_KEY_F11:    return key_code::f11;
         case GDK_KEY_F12:    return key_code::f12;
         case GDK_KEY_F13:    return key_code::f13;
         case GDK_KEY_F14:    return key_code::f14;
         case GDK_KEY_F15:    return key_code::f15;
         case GDK_KEY_F16:    return key_code::f16;
         case GDK_KEY_F17:    return key_code::f17;
         case GDK_KEY_F18:    return key_code::f18;
         case GDK_KEY_F19:    return key_code::f19;
         case GDK_KEY_F20:    return key_code::f20;
         case GDK_KEY_F21:    return key_code::f21;
         case GDK_KEY_F22:    return key_code::f22;
         case GDK_KEY_F23:    return key_code::f23;
         case GDK_KEY_F24:    return key_code::f24;

         case GDK_KEY_Alt_L:     return key_code::left_alt;
         case GDK_KEY_Control_L: return key_code::left_control;
         case GDK_KEY_Shift_L:   return key_code::left_shift;
         case GDK_KEY_Super_L:   return key_code::left_super;
         case print_screen_key:  return key_code::print_screen;
         case GDK_KEY_Alt_R:     return key_code::right_alt;
         case GDK_KEY_Control_R: return key_code::right_control;
         case GDK_KEY_Shift_R:   return key_code::right_shift;
         case GDK_KEY_Super_R:   return key_code::right_super;
         case GDK_KEY_Down:      return key_code::down;
         case GDK_KEY_Left:      return key_code::left;
         case GDK_KEY_Right:     return key_code::right;
         case GDK_KEY_Up:        return key_code::up;

         case GDK_KEY_KP_0:  return key_code::kp_0;
         case GDK_KEY_KP_1:  return key_code::kp_1;
         case GDK_KEY_KP_2:  return key_code::kp_2;
         case GDK_KEY_KP_3:  return key_code::kp_3;
         case GDK_KEY_KP_4:  return key_code::kp_4;
         case GDK_KEY_KP_5:  return key_code::kp_5;
         case GDK_KEY_KP_6:  return key_code::kp_6;
         case GDK_KEY_KP_7:  return key_code::kp_7;
         case GDK_KEY_KP_8:  return key_code::kp_8;
         case GDK_KEY_KP_9:  return key_code::kp_9;
         case GDK_KEY_KP_Add:       return key_code::kp_add;
         case GDK_KEY_KP_Decimal:   return key_code::kp_decimal;
         case GDK_KEY_KP_Divide:    return key_code::kp_divide;
         case GDK_KEY_KP_Enter:     return key_code::kp_enter;
         case GDK_KEY_KP_Equal:     return key_code::kp_equal;
         case GDK_KEY_KP_Multiply:  return key_code::kp_multiply;
         case GDK_KEY_KP_Subtract:  return key_code::kp_subtract;
      }
   }
}}
