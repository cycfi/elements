/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>
#include <X11/keysym.h>

namespace cycfi::elements
{
   // X11 KeySym → key_code. Phase 1 stub; the full mapping table lands with
   // input handling in Phase 2.
   key_code translate_key(unsigned /*keysym*/)
   {
      return key_code::unknown;
   }
}
