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

      using cursor_function = std::function<void(point p, cursor_tracking status)>;

                              basic_popup_element(rect bounds = {})
                               : floating_element(bounds)
                              {}

      bool                    wants_control() const override { return true; }
      element*                hit_test(context const& ctx, point p) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;

      bool                    is_open(view& view_) const;
      void                    open(view& view_);
      void                    close(view& view_);

      cursor_function         on_cursor = [](auto, auto){};
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, basic_popup_element>
   basic_popup(Subject&& subject, rect bounds = {})
   {
      return { std::forward<Subject>(subject), bounds };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Popup Menu
   ////////////////////////////////////////////////////////////////////////////
   class basic_popup_menu_element : public basic_popup_element
   {
   public:

      using basic_popup_element::basic_popup_element;
      using click_function = std::function<void()>;

      element*                hit_test(context const& ctx, point p) override;
      bool                    click(context const& ctx, mouse_button btn) override;
      click_function          on_click = [](){};
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, basic_popup_menu_element>
   basic_popup_menu(Subject&& subject, rect bounds = {})
   {
      return { std::forward<Subject>(subject), bounds };
   }
}}

#endif
