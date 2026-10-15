/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/curve_editor.adoc
   and draws its figure. The figure lands in the build's results
   directory; copy it to docs/modules/ROOT/images/styles/ to update the
   page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/element/style/curve_editor.hpp>
#include <elements/element/style/caption.hpp>
#include <cmath>
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   std::vector<curve_point> envelope()
   {
      return {
         {{0.0f, 0.0f}}
       , {{0.2f, 1.0f}, true}
       , {{0.25f, 0.5f}, true}
       , {{0.3f, 0.5f}, true}
       , {{0.25f, 0.0f}, true}
      };
   }
}

TEST_CASE("curve editor styles page: defaults", "[curve_editor_style_page]")
{
   auto const& thm = get_theme();

   curve_lines lines;
   CHECK(lines.line_color == thm.indicator_color);
   CHECK(lines.fill_color == thm.indicator_color.opacity(0.15));
   CHECK(lines.floor_color == thm.ticks_color);
   CHECK(lines.line_width == 2.0f);
   CHECK(lines.min_size == point{120, 60});
   CHECK(!lines.shape);
   CHECK(lines.steps == 32);
   CHECK(lines.horizontal_guides == std::vector<float>{0.0f});
   CHECK(lines.vertical_guides.empty());

   curve_lines colored{colors::orange, colors::orange.opacity(0.2)};
   CHECK(colored.line_color == colors::orange);
   CHECK(colored.fill_color == colors::orange.opacity(0.2));

   curve_handle handle;
   CHECK(handle.ring_color == thm.indicator_color);
   CHECK(handle.interior_color == thm.panel_color.opacity(1.0));
   CHECK(handle.radius == 5.0f);
   CHECK(handle.line_width == 2.0f);

   // As large as it is given, and no smaller than min_size
   test_view tv{extent{200, 200}};
   basic_context bctx{tv.view_, tv.cnv};
   auto e = curve_editor(envelope());
   CHECK(e.limits(bctx).min == point{120, 60});
   CHECK(e.limits(bctx).max == point{full_extent, full_extent});
}

TEST_CASE("curve editor styles page: example", "[curve_editor_style_page]")
{
   curve_lines lines{colors::orange, colors::orange.opacity(0.2)};
   lines.shape =
      [](std::size_t /*segment*/, float t)
      {
         return 1 - std::pow(1 - t, 3);     // fast at first, then easing
      };
   lines.horizontal_guides = {0.0f, 0.5f};

   auto points = envelope();
   auto env = curve_editor(points, lines, curve_handle{colors::orange, colors::black});
   CHECK(env.size() == 5);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("curve editor styles page: figure", "[curve_editor_style_page]")
{
   auto plain = curve_editor(envelope());

   curve_lines bent_lines;
   bent_lines.shape =
      [](std::size_t /*segment*/, float t)
      {
         return 1 - std::pow(1 - t, 3);
      };
   auto bent = curve_editor(envelope(), bent_lines, curve_handle{});

   curve_lines lines{colors::orange, colors::orange.opacity(0.2)};
   lines.shape = bent_lines.shape;
   lines.horizontal_guides = {0.0f, 0.5f};
   lines.vertical_guides = {2, 3};
   auto colored = curve_editor(envelope(), lines, curve_handle{colors::orange, bkd_color});

   auto fig =
      margin({12, 12, 12, 20},
         htile_spaced(24,
            caption(fixed_size({200, 110}, plain), "The stock stylers"),
            caption(fixed_size({200, 110}, bent), "Bent segments"),
            caption(fixed_size({200, 110}, colored), "Colors and guides")
         )
      );

   point sz;
   {
      test_view m{extent{10, 10}};
      sz = fig.limits(basic_context{m.view_, m.cnv}).min;
   }
   test_view tv{extent{sz.x, sz.y}, 2};
   open(tv);
   tv.view_.content(fig, box(bkd_color));
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "curve_editor_styles.png");
}
#endif
