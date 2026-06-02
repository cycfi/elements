/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_COLLAPSABLE_MAY_30_20206)
#define ELEMENTS_COLLAPSABLE_MAY_30_20206

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Vertical collapsable
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class vcollapsable_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              vcollapsable_element(Subject subject);
      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      bool                    wants_control() const override;
      bool                    wants_focus() const override;
      bool                    is_collapsed = false;
   };

   template <concepts::Element Subject>
   inline vcollapsable_element<Subject>::vcollapsable_element(Subject subject)
    : base_type(std::move(subject))
   {}

   template <concepts::Element Subject>
   inline view_limits
   vcollapsable_element<Subject>::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      return is_collapsed?
         view_limits{{e_limits.min.x, 0.0f}, {e_limits.max.x, 0.0f}} :
         e_limits;
   }

   template <concepts::Element Subject>
   inline void vcollapsable_element<Subject>::draw(context const& ctx)
   {
      if (!is_collapsed)
         this->subject().draw(ctx);
   }

   template <concepts::Element Subject>
   inline bool vcollapsable_element<Subject>::wants_control() const
   {
      if (is_collapsed)
         return false;
      return this->subject().wants_control();
   }

   template <concepts::Element Subject>
   inline bool vcollapsable_element<Subject>::wants_focus() const
   {
      if (is_collapsed)
         return false;
      return this->subject().wants_focus();
   }

   template <concepts::Element Subject>
   inline vcollapsable_element<remove_cvref_t<Subject>>
   vcollapsable(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }
}

#endif
