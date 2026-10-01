/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_CURVE_EDITOR_OCTOBER_1_2026)
#define ELEMENTS_STYLE_CURVE_EDITOR_OCTOBER_1_2026

#include <elements/element/curve_editor.hpp>
#include <elements/support/theme.hpp>

namespace cycfi::elements
{
   /**
    * \struct curve_lines
    *
    * \brief
    *    The default line styler of a curve_editor: straight lines from
    *    point to point, filled down to the floor, and the floor itself.
    *    The colors default to the theme's indicator color.
    */
   struct curve_lines : element
   {
                              curve_lines();
                              curve_lines(color line, color fill);

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      color                   line_color;
      color                   fill_color;
      color                   floor_color;
      float                   line_width = 2.0f;
   };

   /**
    * \struct curve_handle
    *
    * \brief
    *    The default handle styler of a curve_editor: a ring at the point,
    *    drawn larger and filled while the cursor is over it or it is being
    *    dragged. A point that cannot be moved has none.
    */
   struct curve_handle : element
   {
                              curve_handle();
                              curve_handle(color ring, color interior);

      void                    draw(context const& ctx) override;

      color                   ring_color;
      color                   interior_color;
      float                   radius = 5.0f;
      float                   line_width = 2.0f;
   };

   /**
    * \brief
    *    A curve editor drawn by the default stylers
    */
   inline auto curve_editor(std::vector<curve_point> points)
   {
      return curve_editor(std::move(points), curve_lines{}, curve_handle{});
   }
}

#endif
