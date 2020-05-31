/*=============================================================================
   Copyright (c) 2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_TAB_MAY_02_2020)
#define ELEMENTS_GALLERY_TAB_MAY_02_2020

#include <elements/element/gallery/button.hpp>
#include <elements/element/proxy.hpp>
#include <infra/support.hpp>

namespace cycfi { namespace elements
{
   void draw_tab(context const& ctx, bool state, bool hilite);

   template <bool state, typename Subject>
   class tab_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;
      using base_type::base_type;

      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;
      void                    draw(context const& ctx) override;
   };

   template <bool state, typename Subject>
   void tab_element<state, Subject>::draw(context const& ctx)
   {
      draw_tab(ctx, state, ctx.bounds.includes(ctx.view.cursor_pos()));
      base_type::draw(ctx);
   }

   template <bool state, typename Subject>
   bool tab_element<state, Subject>::cursor(context const& ctx, point /* p */, cursor_tracking /* status */)
   {
      ctx.view.refresh(ctx);
      return true;
   }

   template <bool state, typename Subject>
   bool tab_element<state, Subject>::wants_control() const
   {
      return true;
   }

   template <typename W1, typename W2>
   inline basic_choice tab(W1&& off, W2&& on)
   {
      return basic_choice(
         tab_element<false, remove_cvref_t<W1>>{ std::forward<W1>(off) }
       , tab_element<true, remove_cvref_t<W2>>{ std::forward<W2>(on) }
      );
   }
}}

#endif
