/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MODAL_FEBRIARY_9_2024)
#define ELEMENTS_MODAL_FEBRIARY_9_2024

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/style/misc.hpp>

namespace cycfi::elements
{
   /**
    * \class modal_element
    *
    * \brief
    *    A template class for creating modal UI elements that block
    *    interaction with underlying UI elements.
    *
    *    Wrap around any UI element, making it modal. This means the wrapped
    *    element becomes the sole focus of interaction, preventing any events
    *    from reaching other elements beneath it. It's particularly useful
    *    for implementing dialog boxes, overlays, and other interactive
    *    components that require user attention before allowing interaction
    *    with other parts of the UI.
    *
    * \tparam Subject
    *    The type of the UI element to be wrapped.
    */
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

   /**
    * \brief
    *    Create a modal_element with the specified subject.
    *
    *    This function wraps a given UI element in a modal_element, making it
    *    the focal point for user interaction within its context. It
    *    effectively blocks interaction with other UI elements beneath it.
    *
    * \tparam Subject
    *    The type of the UI element to be wrapped. Must conform to the
    *    `concepts::Element` concept.
    *
    * \param subject
    *    The UI element to be wrapped by the modal_element.
    *
    * \return
    *    A modal_element wrapping the specified subject.
    */
   template <concepts::Element Subject>
   inline modal_element<remove_cvref_t<Subject>>
   modal(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Constructor that initializes the modal_element with a given subject.
    *
    * \param subject
    *    The UI element to be wrapped by this modal_element.
    */
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

   /**
    * \brief
    *    Creates a modal_element that acts as a UI blocker that prevents any
    *    event from passing through. Add this as a topmost layer in a view to
    *    lock the UI.
    *
    *    This function creates a modal_element with a semi-transparent
    *    overlay, effectively locking the UI and preventing any interaction
    *    with underlying UI elements. It's useful for creating overlays that
    *    require user interaction before proceeding.
    *
    * \param color_
    *    The color of the overlay, including opacity. Defaults to a
    *    semi-transparent black.
    *
    * \return
    *    A modal_element that acts as a UI blocker.
    */
   inline auto ui_block(color color_ = {0.0, 0.0, 0.0, 0.5})
   {
      return modal(box_element{color_});
   }
}

#endif
