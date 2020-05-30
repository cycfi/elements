/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_POPUP_JUNE_4_2016)
#define ELEMENTS_POPUP_JUNE_4_2016

#include <elements/element/floating.hpp>
#include <elements/view.hpp>
#include <infra/support.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Popup
   ////////////////////////////////////////////////////////////////////////////
   class basic_popup_element : public floating_element
   {
   public:

      using click_function = std::function<void(view& view_)>;

                              basic_popup_element(rect bounds = {})
                               : floating_element(bounds)
                              {}

      element*                hit_test(context const& ctx, point p) override;
      element*                click(context const& ctx, mouse_button btn) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;

      void                    open(view& view_, click_function on_click = {});
      void                    close(view& view_);

   private:

      click_function          _on_click;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, basic_popup_element>
   basic_popup(Subject&& subject, rect bounds = {})
   {
      return { std::forward<Subject>(subject), bounds };
   }
}}

#endif
