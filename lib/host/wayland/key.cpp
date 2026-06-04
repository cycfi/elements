/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>

namespace cycfi::elements
{
   // xkbcommon keysym → key_code. Phase W1 stub; the full table (shared with
   // the X11 host's XK_ values, which xkb keysyms equal) lands with input in W2.
   key_code translate_key(unsigned /*keysym*/)
   {
      return key_code::unknown;
   }
}
