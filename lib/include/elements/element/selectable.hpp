/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SELECTABLE_OCTOBER_19_2019)
#define ELEMENTS_SELECTABLE_OCTOBER_19_2019

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // tracker tracks the mouse movement.
   ////////////////////////////////////////////////////////////////////////////
   class selectable
   {
   public:

      virtual                 ~selectable() = default;
      bool                    is_selected() const;
      void                    select(bool state);

   private:

      bool                    _selected = false;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline bool selectable::is_selected() const
   {
      return _selected;
   }

   inline void selectable::select(bool state)
   {
      _selected = state;
   }
}}

#endif