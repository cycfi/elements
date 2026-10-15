/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_IMAGE_GRID_OCTOBER_1_2026)
#define ELEMENTS_STYLE_IMAGE_GRID_OCTOBER_1_2026

#include <elements/element/image_grid.hpp>
#include <elements/element/style/swatches.hpp>

namespace cycfi::elements
{
   /**
    * \brief
    *    An image grid menu drawn by a swatch_highlighter
    */
   template <typename Image>
   inline auto image_grid_menu(
      Image&& img, int columns, int rows, float scale = 1)
   {
      return image_grid_menu(
         std::forward<Image>(img), columns, rows, scale
       , swatch_highlighter{});
   }
}

#endif
