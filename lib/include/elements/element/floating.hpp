/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FLOATING_JUNE_9_2016)
#define ELEMENTS_FLOATING_JUNE_9_2016

#include <elements/element/proxy.hpp>
#include <infra/support.hpp>

namespace cycfi::elements
{
   /**
    * \class floating_element
    *
    * \brief
    *    A proxy that allows a given subject to be treated as a floating
    *    element with explicit bounds.
    */
   class floating_element : public proxy_base
   {
   public:
                              floating_element(rect bounds)
                               : _bounds(bounds)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      rect                    bounds() const;
      void                    bounds(rect bounds_);

   private:

      rect                    _bounds;
   };

   /**
    * \brief
    *    Creates a floating element.
    *
    * \tparam Subject
    *    The element type which must satisfy the `Element` concept.
    *
    * \param bounds
    *    A `rect` specifying the initial bounds of the `floating_element`.
    *
    * \param subject
    *    The `Subject` that needs to be treated as floating.
    *
    * \returns
    *    A `proxy` object that wraps the `Subject` and treats it as a
    *    `floating_element`.
    */
   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, floating_element>
   floating(rect bounds, Subject&& subject)
   {
      return {std::forward<Subject>(subject), bounds};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   namespace inlines{}

   /**
    * \brief
    *    Get the bounds of the `floating_element`.
    *
    * \returns
    *    The bounds of the `floating_element`.
    */
   inline rect floating_element::bounds() const
   {
      return _bounds;
   }

   /**
    * \brief
    *    Set the bounds of the floating element.
    *
    * \param bounds_
    *    A `rect` specifying the new bounds of the `floating_element`.
    */
   inline void floating_element::bounds(rect bounds_)
   {
      _bounds = bounds_;
   }
}

#endif
