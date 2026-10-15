/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/swatches.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/element/swatch_grid.hpp>
#include <elements/element/style/swatches.hpp>
#include <elements/element/style/misc.hpp>
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

   // Swatches of one's own: a row of colors, drawn by code, a styler over
   // each. Nothing of it is an image.
   class basic_palette : public proxy_base, public basic_swatches
   {
   public:

      basic_palette(std::vector<color> colors, float size)
       : _colors(std::move(colors)), _size(size)
      {}

      int drawing() const override { return _drawing; }
      int value() const override { return _value; }
      void value(int i) { _value = i; }
      void hot_swatch(int i) { _hot = i; }

      swatch_state state(int i) const override
      {
         return i == _hot? swatch_state::hot
            : i == _value? swatch_state::current : swatch_state::normal;
      }

      view_limits limits(basic_context const&) const override
      {
         point s{_size * _colors.size(), _size};
         return {s, s};
      }

      void draw(context const& ctx) override
      {
         for (int i = 0; i != int(_colors.size()); ++i)
         {
            rect b{ctx.bounds.left + i * _size, ctx.bounds.top
             , ctx.bounds.left + (i + 1) * _size, ctx.bounds.top + _size};
            ctx.canvas.fill_style(_colors[i]);
            ctx.canvas.fill_rect(b.inset(3, 3));
            _drawing = i;
            context sctx{ctx, &subject(), b};
            subject().draw(sctx);
         }
      }

   private:

      std::vector<color> _colors;
      float _size;
      int _drawing = 0;
      int _value = 0;
      int _hot = -1;
   };

   template <concepts::Element Styler>
   auto palette(std::vector<color> colors, float size, Styler&& styler)
   {
      return proxy<std::remove_cvref_t<Styler>, basic_palette>{
         std::forward<Styler>(styler), std::move(colors), size};
   }

   int red_at(cycfi::artist::image const& img, int x, int y)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels())
         + 4 * (y * int(img.bitmap_size().x) + x);
      return p[2];
   }

   auto chips()
   {
      auto chip = [](color c) { return margin({3, 3, 3, 3}, box(c)); };
      return fixed_size({180, 36}, htile(
         chip(rgba(200, 80, 70, 255)), chip(rgba(220, 150, 60, 255))
       , chip(rgba(110, 180, 90, 255)), chip(rgba(70, 170, 170, 255))
       , chip(rgba(80, 120, 200, 255))));
   }
}

namespace
{
   int used = -1;
   void use_color(int i) { used = i; }
}

TEST_CASE("swatches page: example", "[swatches_page]")
{
   auto chip = [](color c) { return margin({3, 3, 3, 3}, box(c)); };

   auto colors_ = share(swatch_grid(5, 1,
      fixed_size({180, 36}, htile(
         chip(rgba(200, 80, 70, 255)), chip(rgba(220, 150, 60, 255))
       , chip(rgba(110, 180, 90, 255)), chip(rgba(70, 170, 170, 255))
       , chip(rgba(80, 120, 200, 255))))
    , swatch_highlighter{}));
   colors_->value(3);
   colors_->on_change = [](int i) { use_color(i); };

   CHECK(colors_->columns() == 5);
   CHECK(colors_->rows() == 1);
   CHECK(colors_->size() == 5);
   CHECK(colors_->value() == 3);
   colors_->value(9);
   CHECK(colors_->value() == 4);                  // clamped, no callback
   CHECK(used == -1);

   test_view tv{extent{200, 60}};
   open(tv);
   tv.view_.content(align_left_top(hold(colors_)), box(colors::black));
   tv.view_.layout();
   tv.draw();

   basic_context bctx{tv.view_, tv.cnv};
   CHECK(colors_->limits(bctx).min == point{180, 36});    // the content's

   // A press and a release on the second picks it
   mouse_button btn{};
   btn.state = mouse_button::left;
   btn.num_clicks = 1;
   btn.pos = {36 + 18, 18};
   btn.down = true;
   tv.view_.click(btn);
   btn.down = false;
   tv.view_.click(btn);
   CHECK(colors_->value() == 1);
   CHECK(used == 1);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("swatches page: any element made of swatches", "[swatches_page]")
{
   auto p = share(palette(
      {colors::black, colors::black, colors::black}, 40, swatch_highlighter{}));
   p->value(1);

   test_view tv{extent{120, 40}};
   open(tv);
   tv.view_.content(hold(p), box(colors::black));
   tv.view_.layout();
   tv.draw();

   // The current swatch, 1, outlined in the indicator color; the others
   // are not
   auto ind = get_theme().indicator_color;
   auto outline = [&](int x) { return red_at(tv.img, x, 3); };
   CHECK(std::abs(outline(60) - int(ind.red * ind.alpha * 255)) < 40);
   CHECK(outline(20) < 20);
   CHECK(outline(100) < 20);
}
#endif

TEST_CASE("swatches page: a swatch cell shows the current cell alone", "[swatches_page]")
{
   auto cell = swatch_cell(5, 1, chips(), element{});
   test_view tv{extent{100, 100}};
   basic_context bctx{tv.view_, tv.cnv};
   CHECK(cell.limits(bctx).min == point{0, 0});        // no size of its own
   CHECK(cell.state(0) == basic_swatches::current);
   cell.value(2);
   CHECK(cell.state(2) == basic_swatches::current);
   CHECK(cell.state(0) == basic_swatches::normal);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("swatches page: figure", "[swatches_page]")
{
   auto grid = share(swatch_grid(5, 1, chips(), swatch_highlighter{}));
   auto cell = share(swatch_cell(5, 1, chips(), swatch_highlighter{}));
   grid->value(3);
   cell->value(3);

   auto fig =
      margin({20, 20, 20, 20},
         htile_spaced(30,
            caption(align_center(hold(grid)), "swatch_grid"),
            caption(align_center(fixed_size({60, 60}, hold(cell))), "swatch_cell")
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

   // The pointer over the second
   rect gb;
   tv.view_.in_context_do(*grid, [&](context const& ctx) { gb = ctx.bounds; });
   tv.view_.cursor({gb.left + 36 + 18, gb.top + 18}, cursor_tracking::hovering);
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "elements_swatches.png");
}
#endif
