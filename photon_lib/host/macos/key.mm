/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)

   Key mapping ported to C++ from GLFW3

   Copyright (c) 2009-2016 Camilla Berglund <elmindreda@glfw.org>

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would
      be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not
      be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.

=============================================================================*/
#include <photon/host.hpp>
#import <Cocoa/Cocoa.h>

namespace cycfi { namespace photon
{
   key_code translate_key(unsigned int key)
   {
      switch (key)
      {
         default:    return key_code::unknown;

         case 0x1D:  return key_code::_0;
         case 0x12:  return key_code::_1;
         case 0x13:  return key_code::_2;
         case 0x14:  return key_code::_3;
         case 0x15:  return key_code::_4;
         case 0x17:  return key_code::_5;
         case 0x16:  return key_code::_6;
         case 0x1A:  return key_code::_7;
         case 0x1C:  return key_code::_8;
         case 0x19:  return key_code::_9;

         case 0x00:  return key_code::a;
         case 0x0B:  return key_code::b;
         case 0x08:  return key_code::c;
         case 0x02:  return key_code::d;
         case 0x0E:  return key_code::e;
         case 0x03:  return key_code::f;
         case 0x05:  return key_code::g;
         case 0x04:  return key_code::h;
         case 0x22:  return key_code::i;
         case 0x26:  return key_code::j;
         case 0x28:  return key_code::k;
         case 0x25:  return key_code::l;
         case 0x2E:  return key_code::m;
         case 0x2D:  return key_code::n;
         case 0x1F:  return key_code::o;
         case 0x23:  return key_code::p;
         case 0x0C:  return key_code::q;
         case 0x0F:  return key_code::r;
         case 0x01:  return key_code::s;
         case 0x11:  return key_code::t;
         case 0x20:  return key_code::u;
         case 0x09:  return key_code::v;
         case 0x0D:  return key_code::w;
         case 0x07:  return key_code::x;
         case 0x10:  return key_code::y;
         case 0x06:  return key_code::z;

         case 0x27:  return key_code::apostrophe;
         case 0x2A:  return key_code::backslash;
         case 0x2B:  return key_code::comma;
         case 0x18:  return key_code::equal;
         case 0x32:  return key_code::grave_accent;
         case 0x21:  return key_code::left_bracket;
         case 0x1B:  return key_code::minus;
         case 0x2F:  return key_code::period;
         case 0x1E:  return key_code::right_bracket;
         case 0x29:  return key_code::semicolon;
         case 0x2C:  return key_code::slash;
         case 0x0A:  return key_code::world_1;

         case 0x33:  return key_code::backspace;
         case 0x39:  return key_code::caps_lock;
         case 0x75:  return key_code::_delete;
         case 0x7D:  return key_code::down;
         case 0x77:  return key_code::end;
         case 0x24:  return key_code::enter;
         case 0x35:  return key_code::escape;

         case 0x7A:  return key_code::f1;
         case 0x78:  return key_code::f2;
         case 0x63:  return key_code::f3;
         case 0x76:  return key_code::f4;
         case 0x60:  return key_code::f5;
         case 0x61:  return key_code::f6;
         case 0x62:  return key_code::f7;
         case 0x64:  return key_code::f8;
         case 0x65:  return key_code::f9;
         case 0x6D:  return key_code::f10;
         case 0x67:  return key_code::f11;
         case 0x6F:  return key_code::f12;
         case 0x69:  return key_code::f13;
         case 0x6B:  return key_code::f14;
         case 0x71:  return key_code::f15;
         case 0x6A:  return key_code::f16;
         case 0x40:  return key_code::f17;
         case 0x4F:  return key_code::f18;
         case 0x50:  return key_code::f19;
         case 0x5A:  return key_code::f20;

         case 0x73:  return key_code::home;
         case 0x72:  return key_code::insert;
         case 0x7B:  return key_code::left;
         case 0x3A:  return key_code::left_alt;
         case 0x3B:  return key_code::left_control;
         case 0x38:  return key_code::left_shift;
         case 0x37:  return key_code::left_super;
         case 0x6E:  return key_code::menu;
         case 0x47:  return key_code::num_lock;
         case 0x79:  return key_code::page_down;
         case 0x74:  return key_code::page_up;
         case 0x7C:  return key_code::right;
         case 0x3D:  return key_code::right_alt;
         case 0x3E:  return key_code::right_control;
         case 0x3C:  return key_code::right_shift;
         case 0x36:  return key_code::right_super;
         case 0x31:  return key_code::space;
         case 0x30:  return key_code::tab;
         case 0x7E:  return key_code::up;

         case 0x52:  return key_code::kp_0;
         case 0x53:  return key_code::kp_1;
         case 0x54:  return key_code::kp_2;
         case 0x55:  return key_code::kp_3;
         case 0x56:  return key_code::kp_4;
         case 0x57:  return key_code::kp_5;
         case 0x58:  return key_code::kp_6;
         case 0x59:  return key_code::kp_7;
         case 0x5B:  return key_code::kp_8;
         case 0x5C:  return key_code::kp_9;

         case 0x45:  return key_code::kp_add;
         case 0x41:  return key_code::kp_decimal;
         case 0x4B:  return key_code::kp_divide;
         case 0x4C:  return key_code::kp_enter;
         case 0x51:  return key_code::kp_equal;
         case 0x43:  return key_code::kp_multiply;
         case 0x4E:  return key_code::kp_subtract;
      }
   }

   int translate_flags(NSUInteger flags)
   {
      int mods = 0;

      if (flags & NSEventModifierFlagShift)
         mods |= mod_shift;
      if (flags & NSEventModifierFlagControl)
         mods |= mod_control;
      if (flags & NSEventModifierFlagOption)
         mods |= mod_alt;
      if (flags & NSEventModifierFlagCommand)
         mods |= mod_super;

      return mods;
   }

   NSUInteger translate_key_to_modifier_flag(key_code key)
   {
      switch (key)
      {
         case key_code::left_shift:
         case key_code::right_shift:
            return NSEventModifierFlagShift;

         case key_code::left_control:
         case key_code::right_control:
            return NSEventModifierFlagControl;

         case key_code::left_alt:
         case key_code::right_alt:
            return NSEventModifierFlagOption;

         case key_code::left_super:
         case key_code::right_super:
            return NSEventModifierFlagCommand;

         default:
            return 0;
      }
   }
}}
