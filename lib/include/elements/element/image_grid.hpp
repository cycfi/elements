/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_IMAGE_GRID_OCTOBER_1_2026)
#define ELEMENTS_IMAGE_GRID_OCTOBER_1_2026

#include <elements/element/image.hpp>
#include <elements/element/swatch_grid.hpp>
#include <utility>

namespace cycfi::elements
{
   /**
    * \brief
    *    Make an image grid menu: a swatch grid of an image, its columns and
    *    rows, the scale it is drawn at, and the highlighter drawn over each
    *    cell. `img` is an image_ptr or a path.
    */
   template <concepts::Element Highlighter, typename Image>
   inline auto image_grid_menu(
      Image&& img, int columns, int rows, float scale, Highlighter&& h)
   {
      return swatch_grid(
         columns, rows
       , image{std::forward<Image>(img), scale}
       , std::forward<Highlighter>(h));
   }

   /**
    * \brief
    *    Make an image grid cell: a swatch cell of an image, its columns and
    *    rows, and the highlighter drawn over the cell.
    */
   template <concepts::Element Highlighter, typename Image>
   inline auto image_grid_cell(
      Image&& img, int columns, int rows, Highlighter&& h)
   {
      return swatch_cell(
         columns, rows
       , image{std::forward<Image>(img)}
       , std::forward<Highlighter>(h));
   }
}

#endif
