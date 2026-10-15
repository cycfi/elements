/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/swatches.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/element/style/image_grid.hpp>
#include <elements/element/style/caption.hpp>
#include <cstdint>
#include <string>
#include <type_traits>
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

   // Eight swatches, 4 by 2, each cw by ch
   image_ptr swatches(float cw, float ch)
   {
      color const colors[] = {
         rgba(200, 80, 70, 255), rgba(220, 150, 60, 255), rgba(210, 200, 80, 255),
         rgba(110, 180, 90, 255), rgba(70, 170, 170, 255), rgba(80, 120, 200, 255),
         rgba(130, 100, 190, 255), rgba(190, 100, 160, 255)
      };
      auto img = std::make_shared<cycfi::artist::image>(cw * 4, ch * 2);
      offscreen_image off{*img};
      canvas cnv{off.context()};
      for (int i = 0; i != 8; ++i)
      {
         float x = (i % 4) * cw, y = (i / 4) * ch;
         cnv.fill_style(colors[i].level(0.7));
         cnv.fill_rect({x, y, x + cw, y + ch});
      }
      return img;
   }

   // Two charts, side by side, of three boxes each: a stack, and a row
   // over one. The boxes, in each cell's unit square, are the regions.
   region_highlighter::regions_type const chart_boxes = {
      {{0.35f, 0.1f, 0.65f, 0.3f}, {0.35f, 0.4f, 0.65f, 0.6f}, {0.35f, 0.7f, 0.65f, 0.9f}}
    , {{0.1f, 0.15f, 0.4f, 0.35f}, {0.6f, 0.15f, 0.9f, 0.35f}, {0.35f, 0.65f, 0.65f, 0.85f}}
   };

   image_ptr charts(float cw, float ch)
   {
      auto img = std::make_shared<cycfi::artist::image>(cw * 2, ch);
      offscreen_image off{*img};
      canvas cnv{off.context()};
      cnv.fill_style(rgba(50, 50, 54, 255));
      cnv.fill_rect({0, 0, cw * 2, ch});
      cnv.font(font_descr{"Roboto", ch * 0.12f}.bold());
      cnv.text_align(canvas::center | canvas::middle);
      for (int c = 0; c != 2; ++c)
      {
         int n = 1;
         for (auto const& u : chart_boxes[c])
         {
            rect r{c * cw + u.left * cw, u.top * ch, c * cw + u.right * cw, u.bottom * ch};
            cnv.fill_style(rgba(90, 90, 96, 255));
            cnv.fill_rect(r);
            cnv.fill_style(colors::white);
            cnv.fill_text(std::to_string(n++), center_point(r));
         }
      }
      return img;
   }

   int red_at(cycfi::artist::image const& img, int x, int y)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels())
         + 4 * (y * int(img.bitmap_size().x) + x);
      return p[2];
   }
}

TEST_CASE("swatches styles page: defaults", "[swatches_style_page]")
{
   auto const& thm = get_theme();

   swatch_highlighter h;
   CHECK(h.hot_color == thm.indicator_color.opacity(0.25));
   CHECK(h.current_color == thm.indicator_color);
   CHECK(h.line_width == 2.0f);
   CHECK(h.corner_radius == 4.0f);
   CHECK(!h.label);

   swatch_highlighter c{colors::orange.opacity(0.3), colors::orange};
   CHECK(c.hot_color == colors::orange.opacity(0.3));
   CHECK(c.current_color == colors::orange);

   region_highlighter r{chart_boxes, [](int, int) { return region_highlighter::normal; }};
   CHECK(r.regions.size() == 2);
   CHECK(r.lit_color == thm.indicator_hilite_color);
   CHECK(r.dim_color == thm.panel_color.opacity(0.7));
   CHECK(r.line_width == 2.0f);
   CHECK(r.corner_radius == 3.0f);

   // A menu without a styler is drawn by an swatch_highlighter
   auto menu = image_grid_menu(swatches(40, 30), 4, 2);
   CHECK(std::is_same_v<decltype(menu), swatch_grid_t<image>>);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("swatches styles page: regions", "[swatches_style_page]")
{
   // The first box lit, the second dimmed, the third as drawn
   auto cell = share(image_grid_cell(charts(100, 100), 2, 1,
      region_highlighter{chart_boxes, [](int, int i)
      {
         return i == 0? region_highlighter::lit
              : i == 1? region_highlighter::dimmed : region_highlighter::normal;
      }}));

   test_view tv{extent{100, 100}};
   open(tv);
   tv.view_.content(hold(cell), box(colors::black));
   tv.view_.layout();
   tv.draw();

   // The lit box framed in the indicator's highlight, at its top edge
   auto lit = get_theme().indicator_hilite_color;
   auto edge = red_at(tv.img, 50, 10);
   CHECK(std::abs(edge - int(lit.red * 255)) < 40);

   // The dimmed box darker than the one left as drawn
   CHECK(red_at(tv.img, 40, 50) < red_at(tv.img, 40, 80));
}
#endif

TEST_CASE("swatches styles page: example", "[swatches_style_page]")
{
   image_ptr swatches = ::swatches(80, 60);

   static char const* names[] = {"Ruby", "Amber", "Gold", "Moss", "Teal", "Sky", "Iris", "Rose"};

   swatch_highlighter look;
   look.label = [](int cell) { return std::string{names[cell]}; };
   auto palette = image_grid_menu(swatches, 4, 2, 1.0f, look);
   CHECK(palette.size() == 8);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("swatches styles page: figure", "[swatches_style_page]")
{
   static char const* names[] = {"Ruby", "Amber", "Gold", "Moss", "Teal", "Sky", "Iris", "Rose"};
   swatch_highlighter look;
   look.label = [](int cell) { return std::string{names[cell]}; };
   auto menu = share(image_grid_menu(swatches(80, 60), 4, 2, 1.0f, look));
   menu->value(5);

   auto chart = share(image_grid_cell(charts(120, 120), 2, 1,
      region_highlighter{chart_boxes, [](int, int i)
      {
         return i == 0? region_highlighter::lit
              : i == 1? region_highlighter::dimmed : region_highlighter::normal;
      }}));

   // A swatch grid of drawn content: five boxes in a row
   auto chip = [](color c) { return margin({3, 3, 3, 3}, box(c)); };
   auto colors_ = share(swatch_grid(5, 1,
      fixed_size({180, 36}, htile(
         chip(rgba(200, 80, 70, 255)), chip(rgba(220, 150, 60, 255))
       , chip(rgba(110, 180, 90, 255)), chip(rgba(70, 170, 170, 255))
       , chip(rgba(80, 120, 200, 255))))
    , swatch_highlighter{}));
   colors_->value(3);

   auto fig =
      margin({20, 20, 20, 20},
         htile_spaced(30,
            caption(align_center(hold(menu)), "An image grid, labeled"),
            caption(align_center(hold(colors_)), "A palette of colors"),
            caption(align_center(fixed_size({120, 120}, hold(chart))), "A chart, its parts marked")
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

   // The pointer over Amber: tinted
   rect mb;
   tv.view_.in_context_do(*menu, [&](context const& ctx) { mb = ctx.bounds; });
   tv.view_.cursor({mb.left + 80 + 40, mb.top + 30}, cursor_tracking::hovering);
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "swatches.png");
}
#endif
