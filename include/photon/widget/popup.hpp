/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_POPUP_JUNE_4_2016)
#define PHOTON_GUI_LIB_WIDGET_POPUP_JUNE_4_2016

#include <photon/widget/floating.hpp>
#include <photon/widget/button.hpp>
#include <photon/view.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Popup
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_popup_widget : public floating_widget
   {
   public:
                              basic_popup_widget(rect bounds)
                               : floating_widget(bounds)
                               , _button(0)
                              {}

      virtual widget*         hit_test(context const& ctx, point p);
      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);

      void                    remove(context const& ctx);
      void                    install(context const& ctx, basic_button& button_);
      void                    layout_from_button(context const& ctx);
      void                    click_from_button(context const& ctx, mouse_button btn);

   private:

      basic_button*           _button;
   };

   template <typename Subject>
   inline proxy<typename std::decay<Subject>::type, basic_popup_widget>
   basic_popup(rect bounds, Subject&& subject)
   {
      return { std::forward<Subject>(subject), bounds };
   }
}

#endif
