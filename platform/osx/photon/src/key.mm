/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Distributed under the The MIT License (https://opensource.org/licenses/MIT)

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

=================================================================================================*/
#include <photon/support/key.hpp>
#include <array>
#include <algorithm>
#import <PhotonView.hpp>

namespace photon
{
   struct keys_impl
   {
      keys_impl()
      {
         std::fill(_keys.begin(), _keys.end(), key_code::unknown);

         _keys[0x1D] = key_code::_0;
         _keys[0x12] = key_code::_1;
         _keys[0x13] = key_code::_2;
         _keys[0x14] = key_code::_3;
         _keys[0x15] = key_code::_4;
         _keys[0x17] = key_code::_5;
         _keys[0x16] = key_code::_6;
         _keys[0x1A] = key_code::_7;
         _keys[0x1C] = key_code::_8;
         _keys[0x19] = key_code::_9;
         _keys[0x00] = key_code::a;
         _keys[0x0B] = key_code::b;
         _keys[0x08] = key_code::c;
         _keys[0x02] = key_code::d;
         _keys[0x0E] = key_code::e;
         _keys[0x03] = key_code::f;
         _keys[0x05] = key_code::g;
         _keys[0x04] = key_code::h;
         _keys[0x22] = key_code::i;
         _keys[0x26] = key_code::j;
         _keys[0x28] = key_code::k;
         _keys[0x25] = key_code::l;
         _keys[0x2E] = key_code::m;
         _keys[0x2D] = key_code::n;
         _keys[0x1F] = key_code::o;
         _keys[0x23] = key_code::p;
         _keys[0x0C] = key_code::q;
         _keys[0x0F] = key_code::r;
         _keys[0x01] = key_code::s;
         _keys[0x11] = key_code::t;
         _keys[0x20] = key_code::u;
         _keys[0x09] = key_code::v;
         _keys[0x0D] = key_code::w;
         _keys[0x07] = key_code::x;
         _keys[0x10] = key_code::y;
         _keys[0x06] = key_code::z;

         _keys[0x27] = key_code::apostrophe;
         _keys[0x2A] = key_code::backslash;
         _keys[0x2B] = key_code::comma;
         _keys[0x18] = key_code::equal;
         _keys[0x32] = key_code::grave_accent;
         _keys[0x21] = key_code::left_bracket;
         _keys[0x1B] = key_code::minus;
         _keys[0x2F] = key_code::period;
         _keys[0x1E] = key_code::right_bracket;
         _keys[0x29] = key_code::semicolon;
         _keys[0x2C] = key_code::slash;
         _keys[0x0A] = key_code::world_1;

         _keys[0x33] = key_code::backspace;
         _keys[0x39] = key_code::caps_lock;
         _keys[0x75] = key_code::_delete;
         _keys[0x7D] = key_code::down;
         _keys[0x77] = key_code::end;
         _keys[0x24] = key_code::enter;
         _keys[0x35] = key_code::escape;
         _keys[0x7A] = key_code::f1;
         _keys[0x78] = key_code::f2;
         _keys[0x63] = key_code::f3;
         _keys[0x76] = key_code::f4;
         _keys[0x60] = key_code::f5;
         _keys[0x61] = key_code::f6;
         _keys[0x62] = key_code::f7;
         _keys[0x64] = key_code::f8;
         _keys[0x65] = key_code::f9;
         _keys[0x6D] = key_code::f10;
         _keys[0x67] = key_code::f11;
         _keys[0x6F] = key_code::f12;
         _keys[0x69] = key_code::f13;
         _keys[0x6B] = key_code::f14;
         _keys[0x71] = key_code::f15;
         _keys[0x6A] = key_code::f16;
         _keys[0x40] = key_code::f17;
         _keys[0x4F] = key_code::f18;
         _keys[0x50] = key_code::f19;
         _keys[0x5A] = key_code::f20;
         _keys[0x73] = key_code::home;
         _keys[0x72] = key_code::insert;
         _keys[0x7B] = key_code::left;
         _keys[0x3A] = key_code::left_alt;
         _keys[0x3B] = key_code::left_control;
         _keys[0x38] = key_code::left_shift;
         _keys[0x37] = key_code::left_super;
         _keys[0x6E] = key_code::menu;
         _keys[0x47] = key_code::num_lock;
         _keys[0x79] = key_code::page_down;
         _keys[0x74] = key_code::page_up;
         _keys[0x7C] = key_code::right;
         _keys[0x3D] = key_code::right_alt;
         _keys[0x3E] = key_code::right_control;
         _keys[0x3C] = key_code::right_shift;
         _keys[0x36] = key_code::right_super;
         _keys[0x31] = key_code::space;
         _keys[0x30] = key_code::tab;
         _keys[0x7E] = key_code::up;

         _keys[0x52] = key_code::kp_0;
         _keys[0x53] = key_code::kp_1;
         _keys[0x54] = key_code::kp_2;
         _keys[0x55] = key_code::kp_3;
         _keys[0x56] = key_code::kp_4;
         _keys[0x57] = key_code::kp_5;
         _keys[0x58] = key_code::kp_6;
         _keys[0x59] = key_code::kp_7;
         _keys[0x5B] = key_code::kp_8;
         _keys[0x5C] = key_code::kp_9;
         _keys[0x45] = key_code::kp_add;
         _keys[0x41] = key_code::kp_decimal;
         _keys[0x4B] = key_code::kp_divide;
         _keys[0x4C] = key_code::kp_enter;
         _keys[0x51] = key_code::kp_equal;
         _keys[0x43] = key_code::kp_multiply;
         _keys[0x4E] = key_code::kp_subtract;
      }

      std::array<key_code, 256> _keys;
   };

   std::array<key_code, 256> const& keys()
   {
      static keys_impl _impl;
      return _impl._keys;
   }

   key_code translate_key(unsigned int key)
   {
       if (key >= 256)
           return key_code::unknown;

       return keys()[key];
   }

   int translate_flags(NSUInteger flags)
   {
      int mods = 0;

      if (flags & NSShiftKeyMask)
         mods |= mod_shift;
      if (flags & NSControlKeyMask)
         mods |= mod_control;
      if (flags & NSAlternateKeyMask)
         mods |= mod_alt;
      if (flags & NSCommandKeyMask)
         mods |= mod_super;

      return mods;
   }

   NSUInteger translate_key_to_modifier_flag(key_code key)
   {
      switch (key)
      {
         case key_code::left_shift:
         case key_code::right_shift:
            return NSShiftKeyMask;

         case key_code::left_control:
         case key_code::right_control:
            return NSControlKeyMask;

         case key_code::left_alt:
         case key_code::right_alt:
            return NSAlternateKeyMask;

         case key_code::left_super:
         case key_code::right_super:
            return NSCommandKeyMask;

         default:
            return 0;
      }
   }
}
