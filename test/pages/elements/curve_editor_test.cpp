/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/curve_editor.adoc
   and draws its figures. The figures land in the build's results
   directory; copy them to docs/modules/ROOT/images/elements/ to update
   the page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/element/style/curve_editor.hpp>
#include <elements/element/style/caption.hpp>
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);
   constexpr float eps = 1e-3f;

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   // The envelope of the page's example
   std::vector<curve_point> adsr_points()
   {
      return {
         {{0.0f, 0.0f}}             // the start
       , {{0.2f, 1.0f}, true}       // attack: 0.2 after the start
       , {{0.2f, 0.6f}, true}       // decay
       , {{0.3f, 0.6f}, true}       // sustain
       , {{0.2f, 0.0f}, true}       // release
      };
   }
}

TEST_CASE("curve editor page: points", "[curve_editor_page]")
{
   auto e = curve_editor(adsr_points());
   REQUIRE(e.size() == 5);

   // x as the point holds it, an offset; position summed
   CHECK(e.x(2) == Approx(0.2f).margin(eps));
   CHECK(e.position(2).x == Approx(0.4f).margin(eps));
   CHECK(e.position(4).x == Approx(0.9f).margin(eps));
   CHECK(e.positions().back().x == Approx(0.9f).margin(eps));

   // Setting an offset moves the points after it
   e.x(1, 0.3f);
   CHECK(e.position(4).x == Approx(1.0f).margin(eps));
   e.y(3, 0.5f);
   CHECK(e.y(3) == Approx(0.5f).margin(eps));

   // A node per point, with its callbacks
   CHECK(e[1].spec.offset_x);
   CHECK(e.node_at(1).get() == &e[1]);

   // Inserted and erased
   CHECK(e.insert(5, curve_point{{1.0f, 0.0f}}));
   CHECK(e.size() == 6);
   CHECK(e.erase(5));
   CHECK(!e.erase(5));
   CHECK(!e.insert(7, curve_point{}));

   // Every point movable unless `movable` says otherwise
   CHECK(e.is_movable(0));
   e.movable = [](std::size_t i) { return i != 0; };
   CHECK(!e.is_movable(0));
   CHECK(e.is_movable(1));
}

TEST_CASE("curve editor page: the unit square on the screen", "[curve_editor_page]")
{
   test_view tv{extent{216, 116}};
   open(tv);
   auto e = share(curve_editor({curve_point{{0.5f, 0.5f}}}));
   tv.view_.content(hold(e));
   tv.view_.layout();
   tv.draw();

   rect plot;
   tv.view_.in_context_do(*e, [&](context const& ctx) { plot = e->plot(ctx); });
   CHECK(plot == rect{8, 8, 208, 108});            // inset by 8

   // y up: (0, 0) at the bottom left
   CHECK(e->to_screen({0, 0}, plot) == point{8, 108});
   CHECK(e->to_screen({1, 1}, plot) == point{208, 8});
   auto back = e->from_screen({58, 83}, plot);
   CHECK(back.x == Approx(0.25f).margin(eps));
   CHECK(back.y == Approx(0.25f).margin(eps));

   // fit_width: the farthest point reaches the right edge
   e->fit_width = true;
   CHECK(e->to_screen({0.5f, 0}, plot).x == Approx(208).margin(eps));
}

TEST_CASE("curve editor page: the pieces of a constraint", "[curve_editor_page]")
{
   basic_curve_editor::points_type pts = {{0.2f, 0.5f}, {0.5f, 0.5f}, {0.8f, 0.5f}};
   CHECK(in_unit_square({1.5f, -0.5f}) == point{1, 0});
   CHECK(between_neighbors(pts, 1, {0.9f, 0.5f}).x == Approx(0.8f));
   CHECK(between_neighbors(pts, 1, {0.1f, 0.5f}).x == Approx(0.2f));
   CHECK(default_constraint(pts, 1, {0.9f, 1.5f}) == point{0.8f, 1.0f});
}

TEST_CASE("curve editor page: example", "[curve_editor_page]")
{
   test_view tv{extent{216, 116}, 1};
   auto& view_ = tv.view_;

   value_model<float> attack = 0.2f;

   auto env = share(curve_editor({
      {{0.0f, 0.0f}}             // the start
    , {{0.2f, 1.0f}, true}       // attack: 0.2 after the start
    , {{0.2f, 0.6f}, true}       // decay
    , {{0.3f, 0.6f}, true}       // sustain
    , {{0.2f, 0.0f}, true}       // release
   }));
   env->movable = [](std::size_t i) { return i != 0; };

   model_binder binder{[&view_](element& e) { view_.refresh(e); }};
   binder.bind(attack, x_of(env, 1));

   attack = 0.25f;
   CHECK(env->x(1) == Approx(0.25f).margin(eps));
   CHECK(env->position(4).x == Approx(0.95f).margin(eps));
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("curve editor page: figure", "[curve_editor_page]")
{
   auto envelope = share(curve_editor(adsr_points()));
   envelope->movable = [](std::size_t i) { return i != 0; };

   auto transfer = share(curve_editor({
      curve_point{{0.0f, 0.0f}}
    , curve_point{{0.45f, 0.75f}}
    , curve_point{{1.0f, 1.0f}}
   }));
   transfer->movable = [](std::size_t i) { return i == 1; };

   auto pad_lines = curve_lines{};
   pad_lines.fill_color = colors::black.opacity(0);
   pad_lines.horizontal_guides = {0.65f};
   pad_lines.vertical_guides = {0};
   auto pad = share(curve_editor(
      {curve_point{{0.35f, 0.65f}}}, pad_lines, curve_handle{}));

   auto fig =
      margin({20, 20, 20, 20},
         htile_spaced(24,
            caption(fixed_size({220, 130}, hold(envelope)), "An envelope"),
            caption(fixed_size({130, 130}, hold(transfer)), "A transfer curve"),
            caption(fixed_size({130, 130}, hold(pad)), "An XY pad")
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

   // The pointer over the envelope's decay point: drawn larger, filled
   rect plot;
   tv.view_.in_context_do(*envelope, [&](context const& ctx) { plot = envelope->plot(ctx); });
   tv.view_.cursor(envelope->to_screen(envelope->position(2), plot), cursor_tracking::hovering);
   CHECK(envelope->state(2) == basic_curve_editor::hot);
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "curve_editor.png");
}
#endif
