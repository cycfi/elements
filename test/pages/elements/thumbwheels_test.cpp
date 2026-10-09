/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/thumbwheels.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   mouse_button left_button(point p, bool down)
   {
      mouse_button btn{};
      btn.down = down;
      btn.state = mouse_button::left;
      btn.num_clicks = 1;
      btn.pos = p;
      return btn;
   }

   auto caption(char const* text)
   {
      return align_center(margin_top(4,
         label(text).font_size(12).font_color(rgba(150, 150, 150, 255))));
   }

   // A pad: a square with a dot at the point value, y up.
   auto pad()
   {
      return fixed_size({90, 90},
         draw_value<point>(
            [](context const& ctx, point val)
            {
               auto& cnv = ctx.canvas;
               auto b = ctx.bounds;
               cnv.add_round_rect(b.inset(1, 1), 4);
               cnv.stroke_style(rgba(90, 90, 95, 255));
               cnv.line_width(1);
               cnv.stroke();
               auto x = b.left + val.x * b.width();
               auto y = b.bottom - val.y * b.height();
               cnv.move_to({x, b.top + 4});
               cnv.line_to({x, b.bottom - 4});
               cnv.move_to({b.left + 4, y});
               cnv.line_to({b.right - 4, y});
               cnv.stroke_style(rgba(70, 70, 75, 255));
               cnv.stroke();
               cnv.add_circle({{x, y}, 6});
               cnv.fill_style(get_theme().indicator_color.level(1.5));
               cnv.fill();
            }));
   }

   auto hz_wheel()
   {
      return share(thumbwheel(
         as_label<double>(
            [](double v) { return std::to_string(int(20 + v * 1000)) + " Hz"; },
            heading("").font_size(24)),
         {0, 0.5f}));
   }
}

TEST_CASE("thumbwheels page: value, drag, wheel", "[thumbwheels_page]")
{
   // A thumbwheel over a draw_value<point> that records what it is given;
   // the value is a point, x right and y up, each 0.0 to 1.0.
   std::vector<point> seen;
   auto tw = share(thumbwheel(
      fixed_size({100, 40},
         draw_value<point>([&](context const&, point val) { seen.push_back(val); }))));
   tw->value({0.5f, 0.5f});

   test_view tv{extent{140, 80}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_center_middle(hold(tw)));
   tv.view_.layout();
   tv.draw();
   REQUIRE(!seen.empty());
   CHECK(seen.back() == point{0.5f, 0.5f});    // the subject is given the value

   std::vector<point> changes;
   tw->on_change = [&](point p) { changes.push_back(p); };
   tv.view_.click(left_button({70, 40}, true));
   tv.view_.drag(left_button({90, 20}, true));   // right 20, up 20
   CHECK(tw->value().x == Approx(0.6));
   CHECK(tw->value().y == Approx(0.6));
   tv.view_.click(left_button({90, 20}, false));
   CHECK(changes.size() == 1);

   tv.view_.scroll({0, 1}, {70, 40});            // 0.005 per unit
   CHECK(tw->value().y != Approx(0.6));
   CHECK(changes.size() == 2);

   tw->edit(tv.view_, {0.25f, 0.75f});
   CHECK(changes.back() == point{0.25f, 0.75f});

   changes.clear();
   tw->value({2, -1});                           // clamped, no callback
   CHECK(tw->value() == point{1, 0});
   CHECK(changes.empty());
}

TEST_CASE("thumbwheels page: a styler finds its thumbwheel", "[thumbwheels_page]")
{
   // A styler that is not a receiver reads the value from its control.
   struct reader : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{60, 30}, {60, 30}};
      }

      void draw(context const& ctx) override
      {
         if (auto* tw = find_parent<thumbwheel_base*>(ctx))
            seen = tw->value();
      }

      point seen = {-1, -1};
   };

   auto tw = share(thumbwheel(reader{}, {0.25f, 0.75f}));
   test_view tv{extent{100, 60}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_center_middle(hold(tw)));
   tv.view_.layout();
   tv.draw();
   CHECK(tw->actual_subject().seen == point{0.25f, 0.75f});
}

TEST_CASE("thumbwheels page: the example", "[thumbwheels_page]")
{
   test_view tv{extent{200, 60}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto hz = hz_wheel();
   double f = 0;
   hz->on_change = [&](point p) { f = 20 + p.y * 1000; };
   tv.view_.content(align_center_middle(hold(hz)));
   tv.view_.layout();
   tv.draw();                                    // the first call passes the value

   auto* text = find_subject<receiver<double>*>(hz.get());
   REQUIRE(text);
   CHECK(text->value() == Approx(0.5));         // the label is given y
   hz->edit(tv.view_, {0, 0.25f});
   CHECK(f == Approx(270));
   CHECK(text->value() == Approx(0.25));
}

TEST_CASE("thumbwheels page: figure", "[thumbwheels_page]")
{
   // A pad over draw_value<point>, and a thumbwheel over a list of items.
   auto xy = share(thumbwheel(pad(), {0.65f, 0.7f}));

   auto list_tw = share(vthumbwheel(20,
      [](std::size_t i)
      {
         return share(hsize(120, align_center(margin({10, 4, 10, 4},
            heading("Item " + std::to_string(i + 1)).font_size(18)))));
      }));
   list_tw->value({0, 0.15f});

   test_view tv{extent{320, 140}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 10, 20, 10},
         vtile(
            vsize(100, htile(
               hsize(140, align_center_middle(hold(xy))),
               hsize(140, align_center_middle(layer(hold(list_tw), frame{})))
            )),
            htile(
               hsize(140, caption("thumbwheel(pad)")),
               hsize(140, caption("vthumbwheel"))
            )
         )),
      box(bkd_color)
   );
   tv.view_.layout();

   // Let the list settle on its item before the picture is taken.
   for (int i = 0; i != 200; ++i)
   {
      tv.draw();
      tv.view_.poll();
   }
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "elements_thumbwheels.png");
}

TEST_CASE("thumbwheels page: example figure", "[thumbwheels_page]")
{
   // The page's example rendered.
   auto hz = hz_wheel();
   test_view tv{extent{200, 60}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_center_middle(hold(hz)), box(bkd_color));
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "thumbwheels_example.png");
}
