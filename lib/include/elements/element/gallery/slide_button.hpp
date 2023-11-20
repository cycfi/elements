/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERYSLIDE_BUTTON_NOVEMBER_10_2023)
#define ELEMENTS_GALLERYSLIDE_BUTTON_NOVEMBER_10_2023

#include <elements/element/gallery/button.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Slide Button
   ////////////////////////////////////////////////////////////////////////////
   class slide_button_styler : public element, public basic_receiver<button_state>
   {
   public:

      view_limits       limits(basic_context const& ctx) const override;
      void              draw(context const& ctx) override;
      bool              wants_control() const override;
   };

   inline auto slide_button()
   {
      return toggle_button(slide_button_styler{});
   }
}}

#endif
