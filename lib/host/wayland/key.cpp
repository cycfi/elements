/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>
#include <xkbcommon/xkbcommon-keysyms.h>

namespace cycfi::elements
{
   // xkbcommon keysym → key_code. xkb keysyms share X11 keysym values, so this
   // mirrors the X11 host's table with XKB_KEY_ names.
   key_code translate_key(unsigned key)
   {
      switch (key)
      {
         default:    return key_code::unknown;

         case XKB_KEY_0:  return key_code:: _0;
         case XKB_KEY_1:  return key_code:: _1;
         case XKB_KEY_2:  return key_code:: _2;
         case XKB_KEY_3:  return key_code:: _3;
         case XKB_KEY_4:  return key_code:: _4;
         case XKB_KEY_5:  return key_code:: _5;
         case XKB_KEY_6:  return key_code:: _6;
         case XKB_KEY_7:  return key_code:: _7;
         case XKB_KEY_8:  return key_code:: _8;
         case XKB_KEY_9:  return key_code:: _9;

         case XKB_KEY_a:  return key_code::a;
         case XKB_KEY_b:  return key_code::b;
         case XKB_KEY_c:  return key_code::c;
         case XKB_KEY_d:  return key_code::d;
         case XKB_KEY_e:  return key_code::e;
         case XKB_KEY_f:  return key_code::f;
         case XKB_KEY_g:  return key_code::g;
         case XKB_KEY_h:  return key_code::h;
         case XKB_KEY_i:  return key_code::i;
         case XKB_KEY_j:  return key_code::j;
         case XKB_KEY_k:  return key_code::k;
         case XKB_KEY_l:  return key_code::l;
         case XKB_KEY_m:  return key_code::m;
         case XKB_KEY_n:  return key_code::n;
         case XKB_KEY_o:  return key_code::o;
         case XKB_KEY_p:  return key_code::p;
         case XKB_KEY_q:  return key_code::q;
         case XKB_KEY_r:  return key_code::r;
         case XKB_KEY_s:  return key_code::s;
         case XKB_KEY_t:  return key_code::t;
         case XKB_KEY_u:  return key_code::u;
         case XKB_KEY_v:  return key_code::v;
         case XKB_KEY_w:  return key_code::w;
         case XKB_KEY_x:  return key_code::x;
         case XKB_KEY_y:  return key_code::y;
         case XKB_KEY_z:  return key_code::z;

         case XKB_KEY_apostrophe:   return key_code::apostrophe;
         case XKB_KEY_backslash:    return key_code::backslash;
         case XKB_KEY_comma:        return key_code::comma;
         case XKB_KEY_equal:        return key_code::equal;
         case XKB_KEY_grave:        return key_code::grave_accent;
         case XKB_KEY_bracketleft:  return key_code::left_bracket;
         case XKB_KEY_minus:        return key_code::minus;
         case XKB_KEY_period:       return key_code::period;
         case XKB_KEY_bracketright: return key_code::right_bracket;
         case XKB_KEY_semicolon:    return key_code::semicolon;
         case XKB_KEY_slash:        return key_code::slash;

         case XKB_KEY_BackSpace:    return key_code::backspace;
         case XKB_KEY_Delete:       return key_code::_delete;
         case XKB_KEY_End:          return key_code::end;
         case XKB_KEY_Return:
         case XKB_KEY_ISO_Enter:    return key_code::enter;
         case XKB_KEY_Escape:       return key_code::escape;
         case XKB_KEY_Home:         return key_code::home;
         case XKB_KEY_Insert:       return key_code::insert;
         case XKB_KEY_Menu:         return key_code::menu;
         case XKB_KEY_Page_Down:    return key_code::page_down;
         case XKB_KEY_Page_Up:      return key_code::page_up;
         case XKB_KEY_Pause:        return key_code::pause;
         case XKB_KEY_space:        return key_code::space;
         case XKB_KEY_Tab:
         case XKB_KEY_ISO_Left_Tab: return key_code::tab;
         case XKB_KEY_Caps_Lock:    return key_code::caps_lock;
         case XKB_KEY_Num_Lock:     return key_code::num_lock;
         case XKB_KEY_Scroll_Lock:  return key_code::scroll_lock;
         case XKB_KEY_Print:        return key_code::print_screen;

         case XKB_KEY_F1:   return key_code::f1;
         case XKB_KEY_F2:   return key_code::f2;
         case XKB_KEY_F3:   return key_code::f3;
         case XKB_KEY_F4:   return key_code::f4;
         case XKB_KEY_F5:   return key_code::f5;
         case XKB_KEY_F6:   return key_code::f6;
         case XKB_KEY_F7:   return key_code::f7;
         case XKB_KEY_F8:   return key_code::f8;
         case XKB_KEY_F9:   return key_code::f9;
         case XKB_KEY_F10:  return key_code::f10;
         case XKB_KEY_F11:  return key_code::f11;
         case XKB_KEY_F12:  return key_code::f12;
         case XKB_KEY_F13:  return key_code::f13;
         case XKB_KEY_F14:  return key_code::f14;
         case XKB_KEY_F15:  return key_code::f15;
         case XKB_KEY_F16:  return key_code::f16;
         case XKB_KEY_F17:  return key_code::f17;
         case XKB_KEY_F18:  return key_code::f18;
         case XKB_KEY_F19:  return key_code::f19;
         case XKB_KEY_F20:  return key_code::f20;
         case XKB_KEY_F21:  return key_code::f21;
         case XKB_KEY_F22:  return key_code::f22;
         case XKB_KEY_F23:  return key_code::f23;
         case XKB_KEY_F24:  return key_code::f24;

         case XKB_KEY_Alt_L:     return key_code::left_alt;
         case XKB_KEY_Control_L: return key_code::left_control;
         case XKB_KEY_Shift_L:   return key_code::left_shift;
         case XKB_KEY_Super_L:   return key_code::left_super;
         case XKB_KEY_Alt_R:     return key_code::right_alt;
         case XKB_KEY_Control_R: return key_code::right_control;
         case XKB_KEY_Shift_R:   return key_code::right_shift;
         case XKB_KEY_Super_R:   return key_code::right_super;
         case XKB_KEY_Down:      return key_code::down;
         case XKB_KEY_Left:      return key_code::left;
         case XKB_KEY_Right:     return key_code::right;
         case XKB_KEY_Up:        return key_code::up;

         case XKB_KEY_KP_0:  return key_code::kp_0;
         case XKB_KEY_KP_1:  return key_code::kp_1;
         case XKB_KEY_KP_2:  return key_code::kp_2;
         case XKB_KEY_KP_3:  return key_code::kp_3;
         case XKB_KEY_KP_4:  return key_code::kp_4;
         case XKB_KEY_KP_5:  return key_code::kp_5;
         case XKB_KEY_KP_6:  return key_code::kp_6;
         case XKB_KEY_KP_7:  return key_code::kp_7;
         case XKB_KEY_KP_8:  return key_code::kp_8;
         case XKB_KEY_KP_9:  return key_code::kp_9;

         case XKB_KEY_KP_Add:       return key_code::kp_add;
         case XKB_KEY_KP_Decimal:   return key_code::kp_decimal;
         case XKB_KEY_KP_Divide:    return key_code::kp_divide;
         case XKB_KEY_KP_Enter:     return key_code::kp_enter;
         case XKB_KEY_KP_Equal:     return key_code::kp_equal;
         case XKB_KEY_KP_Multiply:  return key_code::kp_multiply;
         case XKB_KEY_KP_Subtract:  return key_code::kp_subtract;
      }
   }
}
