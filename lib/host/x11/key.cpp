/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman
   Copyright (c) 2022 George Hilliard

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>

#include <X11/Xlib.h>
#include <X11/keysym.h>

namespace cycfi { namespace elements
{
   key_code translate_key(KeySym key)
   {
      switch (key)
      {
         default:    return key_code::unknown;

         case XK_0:  return key_code:: _0;
         case XK_1:  return key_code:: _1;
         case XK_2:  return key_code:: _2;
         case XK_3:  return key_code:: _3;
         case XK_4:  return key_code:: _4;
         case XK_5:  return key_code:: _5;
         case XK_6:  return key_code:: _6;
         case XK_7:  return key_code:: _7;
         case XK_8:  return key_code:: _8;
         case XK_9:  return key_code:: _9;

         case XK_a:  return key_code::a;
         case XK_b:  return key_code::b;
         case XK_c:  return key_code::c;
         case XK_d:  return key_code::d;
         case XK_e:  return key_code::e;
         case XK_f:  return key_code::f;
         case XK_g:  return key_code::g;
         case XK_h:  return key_code::h;
         case XK_i:  return key_code::i;
         case XK_j:  return key_code::j;
         case XK_k:  return key_code::k;
         case XK_l:  return key_code::l;
         case XK_m:  return key_code::m;
         case XK_n:  return key_code::n;
         case XK_o:  return key_code::o;
         case XK_p:  return key_code::p;
         case XK_q:  return key_code::q;
         case XK_r:  return key_code::r;
         case XK_s:  return key_code::s;
         case XK_t:  return key_code::t;
         case XK_u:  return key_code::u;
         case XK_v:  return key_code::v;
         case XK_w:  return key_code::w;
         case XK_x:  return key_code::x;
         case XK_y:  return key_code::y;
         case XK_z:  return key_code::z;

         case XK_apostrophe:   return key_code::apostrophe;
         case XK_backslash:    return key_code::backslash;
         case XK_comma:        return key_code::comma;
         case XK_equal:        return key_code::equal;
         case XK_grave:        return key_code::grave_accent;
         case XK_bracketleft:  return key_code::left_bracket;
         case XK_minus:        return key_code::minus;
         case XK_period:       return key_code::period;
         case XK_bracketright: return key_code::right_bracket;
         case XK_semicolon:    return key_code::semicolon;
         case XK_slash:        return key_code::slash;

         case XK_BackSpace:    return key_code::backspace;
         case XK_Delete:       return key_code::_delete;
         case XK_End:          return key_code::end;

         case XK_Return:
         case XK_ISO_Enter:    return key_code::enter;

         case XK_Escape:       return key_code::escape;
         case XK_Home:         return key_code::home;
         case XK_Insert:       return key_code::insert;
         case XK_Menu:         return key_code::menu;
         case XK_Page_Down:    return key_code::page_down;
         case XK_Page_Up:      return key_code::page_up;
         case XK_Pause:        return key_code::pause;
         case XK_space:        return key_code::space;

         case XK_Tab:
         case XK_KP_Tab:
         case XK_ISO_Left_Tab:
                               return key_code::tab;

         case XK_Caps_Lock:    return key_code::caps_lock;
         case XK_Num_Lock:     return key_code::num_lock;
         case XK_Scroll_Lock:  return key_code::scroll_lock;

         case XK_F1:     return key_code::f1;
         case XK_F2:     return key_code::f2;
         case XK_F3:     return key_code::f3;
         case XK_F4:     return key_code::f4;
         case XK_F5:     return key_code::f5;
         case XK_F6:     return key_code::f6;
         case XK_F7:     return key_code::f7;
         case XK_F8:     return key_code::f8;
         case XK_F9:     return key_code::f9;
         case XK_F10:    return key_code::f10;
         case XK_F11:    return key_code::f11;
         case XK_F12:    return key_code::f12;
         case XK_F13:    return key_code::f13;
         case XK_F14:    return key_code::f14;
         case XK_F15:    return key_code::f15;
         case XK_F16:    return key_code::f16;
         case XK_F17:    return key_code::f17;
         case XK_F18:    return key_code::f18;
         case XK_F19:    return key_code::f19;
         case XK_F20:    return key_code::f20;
         case XK_F21:    return key_code::f21;
         case XK_F22:    return key_code::f22;
         case XK_F23:    return key_code::f23;
         case XK_F24:    return key_code::f24;

         case XK_Alt_L:     return key_code::left_alt;
         case XK_Control_L: return key_code::left_control;
         case XK_Shift_L:   return key_code::left_shift;
         case XK_Super_L:   return key_code::left_super;
         case XK_Print:     return key_code::print_screen;
         case XK_Alt_R:     return key_code::right_alt;
         case XK_Control_R: return key_code::right_control;
         case XK_Shift_R:   return key_code::right_shift;
         case XK_Super_R:   return key_code::right_super;
         case XK_Down:      return key_code::down;
         case XK_Left:      return key_code::left;
         case XK_Right:     return key_code::right;
         case XK_Up:        return key_code::up;

         case XK_KP_0:  return key_code::kp_0;
         case XK_KP_1:  return key_code::kp_1;
         case XK_KP_2:  return key_code::kp_2;
         case XK_KP_3:  return key_code::kp_3;
         case XK_KP_4:  return key_code::kp_4;
         case XK_KP_5:  return key_code::kp_5;
         case XK_KP_6:  return key_code::kp_6;
         case XK_KP_7:  return key_code::kp_7;
         case XK_KP_8:  return key_code::kp_8;
         case XK_KP_9:  return key_code::kp_9;

         case XK_KP_Add:       return key_code::kp_add;
         case XK_KP_Decimal:   return key_code::kp_decimal;
         case XK_KP_Divide:    return key_code::kp_divide;
         case XK_KP_Enter:     return key_code::kp_enter;
         case XK_KP_Equal:     return key_code::kp_equal;
         case XK_KP_Multiply:  return key_code::kp_multiply;
         case XK_KP_Subtract:  return key_code::kp_subtract;
      }
   }
}}

