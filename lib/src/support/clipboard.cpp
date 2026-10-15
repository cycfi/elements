/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/base_view.hpp>
#include <algorithm>

namespace cycfi::elements
{
   // The macOS host asks its pasteboard instead: it lists a custom MIME
   // type under a name of its own making.
#if !defined(ELEMENTS_HOST_UI_LIBRARY_COCOA)
   bool clipboard_contains(std::string_view mime_type)
   {
      auto types = clipboard_types();
      return std::find(types.begin(), types.end(), mime_type) != types.end();
   }
#endif
}
