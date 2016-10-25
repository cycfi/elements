/*=======================================================================================
   Copyright (c) 2016 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]


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

=======================================================================================*/
#if !defined(PHOTON_GUI_LIB_KEY_APRIL_26_2016)
#define PHOTON_GUI_LIB_KEY_APRIL_26_2016

#include <cstdint>

namespace photon
{
   enum class key_action
   {
      unknown           = -1,
      release           = 0,
      press             = 1,
      repeat            = 2,
   };

   enum : uint16_t
   {
      mod_shift         = 0x0001,
      mod_control       = 0x0002,
      mod_alt           = 0x0004,
      mod_super         = 0x0008,
   };

   enum class key_code : int16_t
   {
      unknown           = -1,

      // Printable keys
      space             = 32,
      apostrophe        = 39,    // '
      comma             = 44,    // ,
      minus             = 45,    // -
      period            = 46,    // .
      slash             = 47,    // /
      _0                = 48,
      _1                = 49,
      _2                = 50,
      _3                = 51,
      _4                = 52,
      _5                = 53,
      _6                = 54,
      _7                = 55,
      _8                = 56,
      _9                = 57,
      semicolon         = 59,    // ;
      equal             = 61,    // =
      a                 = 65,
      b                 = 66,
      c                 = 67,
      d                 = 68,
      e                 = 69,
      f                 = 70,
      g                 = 71,
      h                 = 72,
      i                 = 73,
      j                 = 74,
      k                 = 75,
      l                 = 76,
      m                 = 77,
      n                 = 78,
      o                 = 79,
      p                 = 80,
      q                 = 81,
      r                 = 82,
      s                 = 83,
      t                 = 84,
      u                 = 85,
      v                 = 86,
      w                 = 87,
      x                 = 88,
      y                 = 89,
      z                 = 90,
      left_bracket      = 91,    // [
      backslash         = 92,    // \ (back-slash)
      right_bracket     = 93,    // ]
      grave_accent      = 96,    // `
      world_1           = 161,   // non-US #1
      world_2           = 162,   // non-US #2

      // Function keys
      escape            = 256,
      enter             = 257,
      tab               = 258,
      backspace         = 259,
      insert            = 260,
      _delete           = 261,
      right             = 262,
      left              = 263,
      down              = 264,
      up                = 265,
      page_up           = 266,
      page_down         = 267,
      home              = 268,
      end               = 269,
      caps_lock         = 280,
      scroll_lock       = 281,
      num_lock          = 282,
      print_screen      = 283,
      pause             = 284,
      f1                = 290,
      f2                = 291,
      f3                = 292,
      f4                = 293,
      f5                = 294,
      f6                = 295,
      f7                = 296,
      f8                = 297,
      f9                = 298,
      f10               = 299,
      f11               = 300,
      f12               = 301,
      f13               = 302,
      f14               = 303,
      f15               = 304,
      f16               = 305,
      f17               = 306,
      f18               = 307,
      f19               = 308,
      f20               = 309,
      f21               = 310,
      f22               = 311,
      f23               = 312,
      f24               = 313,
      f25               = 314,
      kp_0              = 320,
      kp_1              = 321,
      kp_2              = 322,
      kp_3              = 323,
      kp_4              = 324,
      kp_5              = 325,
      kp_6              = 326,
      kp_7              = 327,
      kp_8              = 328,
      kp_9              = 329,
      kp_decimal        = 330,
      kp_divide         = 331,
      kp_multiply       = 332,
      kp_subtract       = 333,
      kp_add            = 334,
      kp_enter          = 335,
      kp_equal          = 336,
      left_shift        = 340,
      left_control      = 341,
      left_alt          = 342,
      left_super        = 343,
      right_shift       = 344,
      right_control     = 345,
      right_alt         = 346,
      right_super       = 347,
      menu              = 348,

      last              = menu,
   };

   //////////////////////////////////////////////////////////////////////////////////////
   // Key info
   //////////////////////////////////////////////////////////////////////////////////////
   struct key_info
   {
      key_code          key;
      key_action        action;
      int               modifiers;
   };
}

#endif
