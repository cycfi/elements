/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SELECTABLE_OCTOBER_19_2019)
#define ELEMENTS_SELECTABLE_OCTOBER_19_2019

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // selectable base class for selectable elements
   // (e.g. menu items and radio buttons)
   ////////////////////////////////////////////////////////////////////////////
   class selectable
   {
   public:

      virtual                 ~selectable() = default;
      virtual bool            is_selected() const = 0;
      virtual void            select(bool state) = 0;
   };
}}

#endif