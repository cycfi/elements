/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_SWATCHES_OCTOBER_14_2026)
#define ELEMENTS_STYLE_SWATCHES_OCTOBER_14_2026

#include <elements/element/element.hpp>
#include <elements/element/swatches.hpp>
#include <elements/support/theme.hpp>
#include <functional>
#include <string>
#include <vector>

namespace cycfi::elements
{
   /**
    * \struct swatch_highlighter
    *
    * \brief
    *    A styler for swatches (basic_swatches): the current swatch framed,
    *    and the hot one, under the pointer, tinted. The colors default to
    *    the theme's indicator color. Given a `label`, each swatch is
    *    labeled at its top left, in the theme's label font, as by its
    *    number.
    */
   struct swatch_highlighter : element
   {
                              swatch_highlighter();
                              swatch_highlighter(color hot, color current);

      using label_function = std::function<std::string(int swatch)>;

      void                    draw(context const& ctx) override;

      label_function          label;
      color                   hot_color;
      color                   current_color;
      float                   line_width = 2.0f;
      float                   corner_radius = 4.0f;
   };

   /**
    * \struct region_highlighter
    *
    * \brief
    *    A styler for swatches with parts (basic_swatches): regions of each
    *    swatch, each drawn by its state, a lit one framed and a dimmed one
    *    shaded, as a chart shows which of its boxes is selected and which
    *    are off.
    *
    *    A region is a rect in the swatch's own unit square, y downward.
    *    `regions[swatch]` lists a swatch's regions, and `state` says what
    *    each is now.
    */
   struct region_highlighter : element
   {
      enum region_state { normal, lit, dimmed };

      using regions_type = std::vector<std::vector<rect>>;
      using state_function =
         std::function<region_state(int swatch, int region)>;

                              region_highlighter(
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
}

#endif
