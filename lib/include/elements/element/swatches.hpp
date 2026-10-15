/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SWATCHES_OCTOBER_14_2026)
#define ELEMENTS_SWATCHES_OCTOBER_14_2026

namespace cycfi::elements
{
   /**
    * \class basic_swatches
    *
    * \brief
    *    A set of swatches to pick one from: colors, thumbnails, presets,
    *    the cells of an image grid. One is current, and one may be hot,
    *    under the pointer.
    *
    *    An element made of swatches draws a styler over each, a highlighter
    *    say, which finds the element with find_parent and asks, through
    *    this, which swatch it is drawing and in what state.
    */
   class basic_swatches
   {
   public:

      enum swatch_state { normal, hot, current };

      virtual                 ~basic_swatches() = default;

      // The swatch being drawn, a swatch's state, and the current swatch
      virtual int             drawing() const = 0;
      virtual swatch_state    state(int i) const = 0;
      virtual int             value() const = 0;
   };
}

#endif
