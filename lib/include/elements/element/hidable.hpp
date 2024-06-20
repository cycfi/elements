/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_HIDABLE_MAY_30_2020)
#define ELEMENTS_HIDABLE_MAY_30_2020

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Hidable
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class hidable_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              hidable_element(Subject subject);
      void                    draw(context const& ctx) override;
      bool                    wants_control() const override;
      bool                    wants_focus() const override;
      bool                    is_hidden = false;
   };

   template <concepts::Element Subject>
   inline hidable_element<Subject>::hidable_element(Subject subject)
    : base_type(std::move(subject))
   {}

   template <concepts::Element Subject>
   inline void hidable_element<Subject>::draw(context const& ctx)
   {
      if (!is_hidden)
         this->subject().draw(ctx);
   }

   template <concepts::Element Subject>
   inline bool hidable_element<Subject>::wants_control() const
   {
      if (is_hidden)
         return false;
      return this->subject().wants_control();
   }

   template <concepts::Element Subject>
   inline bool hidable_element<Subject>::wants_focus() const
   {
      if (is_hidden)
         return false;
      return this->subject().wants_focus();
   }

   template <concepts::Element Subject>
   inline hidable_element<remove_cvref_t<Subject>>
   hidable(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }
}

#endif
