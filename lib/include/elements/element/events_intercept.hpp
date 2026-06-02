/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_EVENTS_INTERCEPT_JULY_4_2019)
#define ELEMENTS_EVENTS_INTERCEPT_JULY_4_2019

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Key Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   struct key_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                     key_intercept_element(Subject subject)
                      : base_type(std::move(subject))
                     {}

      bool           key(context const& ctx, key_info k) override;
      bool           wants_control() const override { return true; }
      bool           wants_focus() const override { return true; }

      using key_function = std::function<bool(key_info k)>;

      key_function   on_key = [](auto){ return false; };
   };

   template <concepts::Element Subject>
   inline key_intercept_element<remove_cvref_t<Subject>>
   key_intercept(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline bool key_intercept_element<Subject>::key(context const& ctx, key_info k)
   {
      if (on_key(k))
         return true;
      return this->subject().key(ctx, k);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Text Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   struct text_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                     text_intercept_element(Subject subject)
                      : base_type(std::move(subject))
                     {}

      bool           text(context const& ctx, text_info info) override;
      bool           wants_control() const override { return true; }
      bool           wants_focus() const override { return true; }

      using text_function = std::function<bool(text_info info)>;

      text_function   on_text = [](auto){ return false; };
   };

   template <concepts::Element Subject>
   inline text_intercept_element<remove_cvref_t<Subject>>
   text_intercept(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline bool text_intercept_element<Subject>::text(context const& ctx, text_info info)
   {
      if (on_text(info))
         return true;
      return this->subject().text(ctx, info);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Click Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   struct click_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                     click_intercept_element(Subject subject)
                      : base_type(std::move(subject))
                     {}

      bool           click(context const& ctx, mouse_button btn) override;
      bool           wants_control() const override { return true; }
      bool           wants_focus() const override { return true; }

      using click_function = std::function<bool(mouse_button btn)>;

      click_function on_click = [](auto){ return false; };
   };

   template <concepts::Element Subject>
   inline click_intercept_element<remove_cvref_t<Subject>>
   click_intercept(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline bool click_intercept_element<Subject>::click(context const& ctx, mouse_button btn)
   {
      if (on_click(btn))
         return true;
      return this->subject().click(ctx, btn);
   }
}

#endif
