/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_ACTIVATOR_OCTOBER_6_2023)
#define ELEMENTS_ACTIVATOR_OCTOBER_6_2023

#include <elements/element/element.hpp>

namespace cycfi { namespace elements
{
   struct activator : element
   {
      void     enable(bool state_ = true) override;
      bool     is_enabled() const override;
      bool     _is_enabled = true;
   };

   inline void activator::enable(bool state)
   {
      _is_enabled = state;
   }

   inline bool activator::is_enabled() const
   {
      return _is_enabled;
   }
}}

#endif