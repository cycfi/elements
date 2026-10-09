/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles.adoc and draws its
   figure. The figure lands in the build's results directory; copy it to
   docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>

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

   // A styler that reads its button's state, as the page's example does.
   struct lamp : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{40, 40}, {40, 40}};
      }

      void draw(context const& ctx) override
      {
         auto btn = find_parent<basic_button*>(ctx);
         if (!btn)
            return;
         ++draws;
         on = btn->value();
         lit = btn->hilite();
         enabled = ctx.enabled;
      }

      int   draws = 0;
      bool  on = false;
      bool  lit = false;
      bool  enabled = false;
   };

   // The page's example.
   struct lamp_styler : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{24, 24}, {24, 24}};
      }

      void draw(context const& ctx) override
      {
         auto btn = find_parent<basic_button*>(ctx);
         if (!btn)
            return;

         auto& cnv = ctx.canvas;
         cnv.add_circle({center_point(ctx.bounds), 10});
         cnv.fill_style(
            !ctx.enabled ? colors::gray[40] :
            btn->value() ? colors::gold :
            colors::gray[20]);
         cnv.fill();
      }
   };

   // A thumb that records where the slider puts it.
   struct marker : element
   {
      view_limits limits(basic_context const&) const override
      {
         return {{10, 10}, {10, 10}};
      }

      void draw(context const& ctx) override
      {
         where = ctx.bounds;
      }

      rect where;
   };
}

TEST_CASE("styles page: a styler reads its control", "[styles_page]")
{
   auto b = share(toggle_button(lamp{}));
   test_view tv{extent{100, 100}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_center_middle(hold(b)));
   tv.view_.layout();
   tv.draw();

   auto& s = b->actual_subject();
   REQUIRE(s.draws > 0);
   CHECK(!s.on);
   CHECK(s.enabled);

   // The styler holds no state; the button does, and the styler reads it.
   tv.view_.click(left_button({50, 50}, true));
   tv.view_.click(left_button({50, 50}, false));
   tv.draw();
   CHECK(b->value());
   CHECK(s.on);

   b->value(false);
   tv.draw();
   CHECK(!s.on);
}

TEST_CASE("styles page: ctx.enabled covers the enclosing elements", "[styles_page]")
{
   auto b = share(toggle_button(lamp{}));
   auto row = share(htile(hold(b)));
   test_view tv{extent{100, 100}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_center_middle(hold(row)));
   tv.view_.layout();
   auto& s = b->actual_subject();

   b->enable(false);                          // the button itself
   tv.draw();
   CHECK(!s.enabled);

   b->enable(true);
   row->enable(false);                        // the tile it sits in
   tv.draw();
   CHECK(b->is_enabled());
   CHECK(!s.enabled);
}

TEST_CASE("styles page: a slider places its thumb", "[styles_page]")
{
   auto sl = share(slider(marker{}, basic_track<5, false>(), 0.0));
   test_view tv{extent{120, 40}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(margin({10, 10, 10, 10}, hold(sl)));
   tv.view_.layout();

   auto* thumb = dynamic_cast<marker*>(&sl->thumb());
   REQUIRE(thumb);
   tv.draw();
   auto left = thumb->where.left;
   sl->value(1.0);
   tv.draw();
   CHECK(thumb->where.left > left);           // moved by the slider
   CHECK(thumb->where.width() == Approx(10)); // at the thumb's own size
}

TEST_CASE("styles page: a thumbwheel passes its value", "[styles_page]")
{
   double seen = -1;
   auto tw = share(thumbwheel(
      fixed_size({60, 30},
         draw_value<double>([&](context const&, double v) { seen = v; })),
      {0.2f, 0.6f}));
   test_view tv{extent{100, 60}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_center_middle(hold(tw)));
   tv.view_.layout();
   tv.draw();
   CHECK(seen == Approx(0.6));                // a receiver<double> gets y
}

TEST_CASE("styles page: figure", "[styles_page]")
{
   // One behavior, four looks: each is toggle_button over another styler,
   // off in the first row and on in the second.
   auto small = [](char const* text, color c = rgba(150, 150, 150, 255))
   {
      return label(text).font_size(12).font_color(c);
   };

   auto row = [&](bool on)
   {
      auto a = share(toggle_button("Power"));
      auto b = share(check_box("Enabled"));
      auto c = share(slide_switch());
      auto d = share(toggle_icon_button(icons::power, 1.2));
      a->value(on); b->value(on); c->value(on); d->value(on);
      auto cell = [](auto e) { return hsize(130, vsize(44, align_center_middle(e))); };
      return htile(
         hsize(40, align_left_middle(small(on ? "on" : "off"))),
         cell(hold(a)), cell(hold(b)), cell(hold(c)), cell(hold(d)));
   };

   auto caption = [&](char const* text)
   {
      return hsize(130, align_center(small(text)));
   };

   test_view tv{extent{600, 130}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 10, 20, 10},
         vtile(
            row(false),
            row(true),
            margin_top(6, htile(
               hsize(40, element{}),
               caption("default_button_styler"),
               caption("check_box_styler"),
               caption("slide_switch_styler"),
               caption("icon_button_styler")))
         )),
      box(bkd_color)
   );
   tv.view_.layout();

   // The slide switch slides its knob a step per draw; let it settle.
   for (int i = 0; i != 30; ++i)
   {
      tv.draw();
      tv.view_.poll();
   }
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "styles_stylers.png");
}

TEST_CASE("styles page: a styler outside its control draws nothing", "[styles_page]")
{
   auto l = share(lamp{});
   test_view tv{extent{60, 60}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_center_middle(hold(l)));
   tv.view_.layout();
   tv.draw();
   CHECK(l->draws == 0);
}

TEST_CASE("styles page: example figure", "[styles_page]")
{
   // The example's lamp: off, on, and disabled.
   auto off = share(toggle_button(lamp_styler{}));
   auto on = share(toggle_button(lamp_styler{}));
   auto dis = share(toggle_button(lamp_styler{}));
   on->value(true);
   dis->enable(false);

   auto cell = [](auto e, char const* text)
   {
      return hsize(60, vtile(
         vsize(34, align_center_middle(e)),
         align_center(label(text).font_size(12).font_color(rgba(150, 150, 150, 255)))));
   };
   test_view tv{extent{200, 70}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({10, 8, 10, 8}, htile(cell(hold(off), "off"), cell(hold(on), "on"), cell(hold(dis), "disabled"))),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "styles_example.png");
}
