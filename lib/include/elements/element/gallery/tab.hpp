/*=============================================================================
   Copyright (c) 2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_TAB_MAY_02_2020)
#define ELEMENTS_GALLERY_TAB_MAY_02_2020

#include <elements/element/gallery/button.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Tab
   ////////////////////////////////////////////////////////////////////////////
   struct tab_element : element, basic_receiver<int>
   {
                              tab_element(std::string text)
                               : _text(std::move(text))
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    wants_control() const override;

      std::string             _text;
   };

   inline auto tab(std::string text)
   {
      return choice(tab_element{ text });
   }
}}

#endif
