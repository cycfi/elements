/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_GUI_LIB_WIDGET_POPUP_JUNE_4_2016)
#define CYCFI_ELEMENTS_GUI_LIB_WIDGET_POPUP_JUNE_4_2016

#include <elements/element/floating.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Popup
   ////////////////////////////////////////////////////////////////////////////
   class basic_popup_element : public floating_element
   {
   public:

      using click_function = std::function<void(context const& ctx, mouse_button btn)>;

                              basic_popup_element(rect bounds = {})
                               : floating_element(bounds)
                              {}

      virtual element*        hit_test(context const& ctx, point p);
      virtual element*        click(context const& ctx, mouse_button btn);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);

      void                    open(view& view_, click_function on_click = {});
      void                    close(view& view_);

   private:

      click_function          _on_click;
   };

   template <typename Subject>
   inline proxy<Subject, basic_popup_element>
   basic_popup(Subject&& subject, rect bounds = {})
   {
      return { std::forward<Subject>(subject), bounds };
   }
}}

#endif
