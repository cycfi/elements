/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_WIDGET_POPUP_JUNE_4_2016)
#define CYCFI_PHOTON_GUI_LIB_WIDGET_POPUP_JUNE_4_2016

#include <photon/element/floating.hpp>
#include <photon/view.hpp>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Popup
   ////////////////////////////////////////////////////////////////////////////
   class basic_popup_element : public floating_element
   {
   public:

      using click_function = std::function<void(context const& ctx, mouse_button btn)>;

                              basic_popup_element()
                               : floating_element({})
                              {}

      virtual element*        hit_test(context const& ctx, point p);
      virtual element*        click(context const& ctx, mouse_button btn);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);

      void                    open(context const& ctx, click_function on_click = {});
      void                    close(context const& ctx);

   private:

      click_function          _on_click;
   };

   template <typename Subject>
   inline proxy<typename std::decay<Subject>::type, basic_popup_element>
   basic_popup(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }
}}

#endif
