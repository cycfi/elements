/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MODAL_FEBRIARY_9_2024)
#define ELEMENTS_MODAL_FEBRIARY_9_2024

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Modal element hugs the UI and prevents any event from passing through
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class modal_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     modal_element(Subject subject);
      element*       hit_test(context const& ctx, point p, bool leaf, bool control) override;
      bool           click(context const& ctx, mouse_button btn) override;
      bool           key(context const& ctx, key_info k) override;
      bool           text(context const& ctx, text_info info) override;

      bool           wants_focus() const override { return true; }
      bool           wants_control() const override { return true; }
   };

   template <concepts::Element Subject>
   inline modal_element<remove_cvref_t<Subject>>
   modal(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline modal_element<Subject>::modal_element(Subject subject)
    : base_type(std::move(subject))
   {
   }

   template <concepts::Element Subject>
   inline element* modal_element<Subject>::hit_test(context const& ctx, point p, bool leaf, bool control)
   {
      if (auto e = this->subject().hit_test(ctx, p, leaf, control))
         return e;
      return this;
   }

   template <concepts::Element Subject>
   inline bool modal_element<Subject>::click(context const& ctx, mouse_button btn)
   {
      this->subject().click(ctx, btn);
      return true;
   }

   template <concepts::Element Subject>
   inline bool modal_element<Subject>::key(context const& ctx, key_info k)
   {
      base_type::key(ctx, k);
      return true;
   }

   template <concepts::Element Subject>
   inline bool modal_element<Subject>::text(context const& ctx, text_info info)
   {
      base_type::text(ctx, info);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   // An element that prevents any event from passing through. Add this as a
   // topmost layer in a view to lock the UI.
   ////////////////////////////////////////////////////////////////////////////
   inline auto ui_block(color color_ = {0.0, 0.0, 0.0, 0.5})
   {
      return modal(box_element{color_});
   }
}

#endif
