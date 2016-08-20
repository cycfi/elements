/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_BASIC_APRIL_11_2016)
#define PHOTON_GUI_LIB_WIDGET_BASIC_APRIL_11_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Basic Widget
   //
   // The basic widget takes in a function that draws something
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename F>
   class basic_widget : public widget
   {
   public:

      basic_widget(F f)
       : f(f)
      {}

      virtual void
      draw(context const& ctx)
      {
         f(ctx);
      }

   private:

      F f;
   };

   template <typename F>
   inline basic_widget<F> basic(F f)
   {
      return { f };
   }
}

#endif