/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elemental/support/theme.hpp>
#include <elemental/view.hpp>

namespace cycfi { namespace elemental
{
   // The global theme
   theme _theme;

   theme const& get_theme()
   {
      return _theme;
   }

   void set_theme(theme const& thm)
   {
      _theme = thm;
   }
}}
