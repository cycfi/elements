/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_IMAGE_GRID_OCTOBER_1_2026)
#define ELEMENTS_STYLE_IMAGE_GRID_OCTOBER_1_2026

#include <elements/element/image_grid.hpp>
#include <elements/support/theme.hpp>
#include <functional>
#include <string>
#include <vector>

namespace cycfi::elements
{
   /**
    * \struct image_grid_highlight
    *
    * \brief
    *    The default cell styler of an image grid menu: the current cell
    *    framed, and the cell under the cursor tinted. The colors default to
    *    the theme's indicator color. Given a `label`, each cell is labeled
    *    at its top left, in the theme's label font, as by its number.
    */
   struct image_grid_highlight : element
   {
                              image_grid_highlight();
                              image_grid_highlight(color hot, color current);

      using label_function = std::function<std::string(int cell)>;

      void                    draw(context const& ctx) override;

      label_function          label;
      color                   hot_color;
      color                   current_color;
      float                   line_width = 2.0f;
      float                   corner_radius = 4.0f;
   };

   /**
    * \struct image_regions
    *
    * \brief
    *    A cell styler for a picture with parts: regions of each cell, each
    *    drawn by its state, a lit one framed and a dimmed one shaded, as a
    *    chart shows which of its boxes is selected and which are off.
    *
    *    A region is a rect in the cell's own unit square, y downward, as
    *    the image is. `regions[cell]` lists a cell's regions, and `state`
    *    says what each is now.
    */
   struct image_regions : element
   {
      enum region_state { normal, lit, dimmed };

      using regions_type = std::vector<std::vector<rect>>;
      using state_function =
         std::function<region_state(int cell, int region)>;

                              image_regions(
                                 regions_type regions
                               , state_function state);

      void                    draw(context const& ctx) override;

      regions_type            regions;
      state_function          state;
      color                   lit_color;
      color                   dim_color;
      float                   line_width = 2.0f;
      float                   corner_radius = 3.0f;
   };

   /**
    * \brief
    *    An image grid menu with the default highlight
    */
   template <typename Image>
   inline auto image_grid_menu(
      Image&& img, int columns, int rows, float scale = 1)
   {
      return image_grid_menu(
         std::forward<Image>(img), columns, rows, scale
       , image_grid_highlight{});
   }
}

#endif
