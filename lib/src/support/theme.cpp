/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/theme.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   // The global theme
   theme global_theme::_theme;

   theme const& get_theme()
   {
      return global_theme::_theme;
   }

   void set_theme(theme const& thm)
   {
      global_theme::_theme = thm;
   }
}}
