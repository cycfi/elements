/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/images.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/support/error_handler.hpp>
#include <cstdint>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   std::string const space = std::string{EXAMPLES_PATH} + "hello_universe/resources/space.jpg";
   std::string const knobs = std::string{EXAMPLES_PATH} + "sprite_sliders_and_knobs/resources/knob_sprites_white_128x128.png";

   // A solid image, w by h, in one colour.
   image_ptr solid(float w, float h, color c)
   {
      auto img = std::make_shared<cycfi::artist::image>(w, h);
      {
         offscreen_image off{*img};
         canvas cnv{off.context()};
         cnv.fill_style(c);
         cnv.fill_rect({0, 0, w, h});
      }
      return img;
   }

   // An image of eight cells, four by two, each numbered on a colour.
   image_ptr numbered_cells(float cw, float ch)
   {
      color const colors[] = {
         rgba(200, 80, 70, 255), rgba(220, 150, 60, 255), rgba(210, 200, 80, 255),
         rgba(110, 180, 90, 255), rgba(70, 170, 170, 255), rgba(80, 120, 200, 255),
         rgba(130, 100, 190, 255), rgba(190, 100, 160, 255)
      };
      auto img = std::make_shared<cycfi::artist::image>(cw * 4, ch * 2);
      {
         offscreen_image off{*img};
         canvas cnv{off.context()};
         cnv.font(font_descr{"Roboto", ch * 0.45f}.bold());
         cnv.text_align(canvas::center | canvas::middle);
         for (int i = 0; i != 8; ++i)
         {
            float x = (i % 4) * cw, y = (i / 4) * ch;
            cnv.fill_style(colors[i]);
            cnv.fill_rect({x, y, x + cw, y + ch});
            cnv.fill_style(rgba(255, 255, 255, 230));
            cnv.fill_text(std::to_string(i + 1), {x + cw / 2, y + ch / 2});
         }
      }
      return img;
   }

   auto caption(char const* text)
   {
      return align_center(margin_top(6,
         label(text).font_size(12).font_color(rgba(150, 150, 150, 255))));
   }

#if !defined(ARTIST_RECORDING)
   // The bounding box of the pixels brighter than the background.
   rect ink(cycfi::artist::image const& img)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels());
      int w = int(img.bitmap_size().x), h = int(img.bitmap_size().y);
      int l = w, t = h, r = -1, b = -1;
      for (int y = 0; y != h; ++y)
         for (int x = 0; x != w; ++x)
            if (p[4 * (y * w + x) + 2] > 100)
            {
               l = std::min(l, x); r = std::max(r, x);
               t = std::min(t, y); b = std::max(b, y);
            }
      return {float(l), float(t), float(r + 1), float(b + 1)};
   }
#endif

   template <typename E>
   cycfi::artist::image const& drawn(test_view& tv, E e)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      tv.view_.content(std::move(e), box(rgba(0, 0, 0, 255)));
      tv.view_.layout();
      tv.draw();
      return tv.img;
   }
}

TEST_CASE("images page: an image at its size, scaled", "[images_page]")
{
   test_view tv{extent{100, 100}, 1};
   basic_context bctx{tv.view_, tv.cnv};

   image a{space};
   CHECK(a.size() == point{1920, 1080});
   CHECK(a.limits(bctx).min == point{1920, 1080});
   CHECK(a.limits(bctx).max == point{1920, 1080});

   image b{space, 0.1f};
   CHECK(b.scale() == Approx(0.1f));
   CHECK(b.size().x == Approx(192));
   CHECK(b.limits(bctx).min.y == Approx(108));
   CHECK(b.limits(bctx).max == b.limits(bctx).min);
}

TEST_CASE("images page: fit and fill take any size", "[images_page]")
{
   test_view tv{extent{100, 100}, 1};
   basic_context bctx{tv.view_, tv.cnv};
   image f{space, image::fit};
   image g{space, image::fill};
   CHECK(f.limits(bctx).min == point{0, 0});
   CHECK(f.limits(bctx).max == point{full_extent, full_extent});
   CHECK(g.limits(bctx).max == point{full_extent, full_extent});
   CHECK(f.size() == point{-1, -1});              // no size of its own
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("images page: fit keeps the proportions, fill does not", "[images_page]")
{
   // A 100 by 50 image in a 200 by 200 view.
   {
      test_view tv{extent{200, 200}, 1};
      auto r = ink(drawn(tv, image{solid(100, 50, rgba(255, 255, 255, 255)), image::fit}));
      CHECK(r.width() == Approx(200).margin(1));
      CHECK(r.height() == Approx(100).margin(1));
      CHECK(r.top == Approx(50).margin(1));      // centered
   }
   {
      test_view tv{extent{200, 200}, 1};
      auto r = ink(drawn(tv, image{solid(100, 50, rgba(255, 255, 255, 255)), image::fill}));
      CHECK(r.width() == Approx(200).margin(1));
      CHECK(r.height() == Approx(200).margin(1));
   }
}
#endif

TEST_CASE("images page: replacing the image", "[images_page]")
{
   image a{solid(10, 10, rgba(255, 0, 0, 255))};
   auto b = solid(20, 30, rgba(0, 255, 0, 255));
   a.set_image(b);
   CHECK(a.get_image() == b);
   CHECK(a.size() == point{20, 30});
}

TEST_CASE("images page: a missing file is reported, then thrown", "[images_page]")
{
   auto& eh = error_handler::get();
   auto saved = eh.on_resource_error;
   error_id seen{};
   eh.on_resource_error = [&](error_id id, std::string_view) { seen = id; };
   CHECK_THROWS(image{"no/such/file.png"});
   CHECK(seen == error_id::image_load_failed);
   eh.on_resource_error = saved;
}

TEST_CASE("images page: sprite", "[images_page]")
{
   test_view tv{extent{100, 100}, 1};
   basic_context bctx{tv.view_, tv.cnv};

   // 100 frames of 128 by 128, one under the other, drawn at half size:
   // the height given is a frame's height as drawn.
   sprite s{knobs, 64, 0.5f};
   CHECK(s.num_frames() == 100);
   CHECK(s.size() == point{64, 64});
   CHECK(s.limits(bctx).min == point{64, 64});
   CHECK(s.limits(bctx).max == point{64, 64});

   CHECK(s.index() == 0);
   s.index(42);
   CHECK(s.index() == 42);
   s.index(100);                                  // past the last: ignored
   CHECK(s.index() == 42);

   context ctx{tv.view_, tv.cnv, &s, rect{0, 0, 64, 64}};
   CHECK(s.source_rect(ctx) == rect{0, 128 * 42, 128, 128 * 43});
}

TEST_CASE("images page: SpriteSubject", "[images_page]")
{
   CHECK(concepts::SpriteSubject<sprite>);
   CHECK(concepts::SpriteSubject<proxy<sprite>>);
   CHECK(!concepts::SpriteSubject<image>);
}

TEST_CASE("images page: image grid", "[images_page]")
{
   auto img = numbered_cells(40, 30);
   auto menu = share(image_grid_menu(img, 4, 2, 1.0f));
   auto cell = share(image_grid_cell(img, 4, 2, image_grid_highlight{}));
   CHECK(menu->columns() == 4);
   CHECK(menu->rows() == 2);
   CHECK(menu->size() == 8);

   int picked = -1;
   menu->on_change = [&](int i) { picked = i; cell->value(i); };

   test_view tv{extent{200, 100}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_left_top(hold(menu)));
   tv.view_.layout();
   tv.draw();

   basic_context bctx{tv.view_, tv.cnv};
   CHECK(menu->limits(bctx).min == point{160, 60});

   // A press and a release on the same cell pick it.
   mouse_button btn{};
   btn.state = mouse_button::left;
   btn.num_clicks = 1;
   btn.pos = {100, 45};                           // column 2, row 1: cell 6
   btn.down = true;
   tv.view_.click(btn);
   btn.down = false;
   tv.view_.click(btn);
   CHECK(menu->value() == 6);
   CHECK(picked == 6);
   CHECK(cell->value() == 6);

   // Set from code: clamped, and no callback.
   picked = -1;
   menu->value(99);
   CHECK(menu->value() == 7);
   CHECK(picked == -1);
}

TEST_CASE("images page: figure", "[images_page]")
{
   // One picture three ways: at a scale, fitted and filled, each in a box
   // of 160 by 110 framed to show its bounds.
   auto boxed = [](auto e)
   {
      return layer(fixed_size({160, 110}, std::move(e)), frame{});
   };

   test_view tv{extent{560, 160}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 14, 20, 10},
         htile(
            vtile(align_center(layer(align_center_middle(image{space, 0.06f}), fixed_size({160, 110}, element{}), frame{})),
               caption("image(path, 0.06)")),
            margin_left(20, vtile(align_center(boxed(image{space, image::fit})), caption("image(path, image::fit)"))),
            margin_left(20, vtile(align_center(boxed(image{space, image::fill})), caption("image(path, image::fill)")))
         )),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "elements_images.png");
}

TEST_CASE("images page: sprite figure", "[images_page]")
{
   // Five frames of the knob strip, at half size.
   auto frame_of = [](std::size_t i)
   {
      sprite s{knobs, 64, 0.5f};
      s.index(i);
      return vtile(align_center(s), caption(("index " + std::to_string(i)).c_str()));
   };

   test_view tv{extent{440, 120}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 14, 20, 10},
         htile(frame_of(0), frame_of(25), frame_of(50), frame_of(75), frame_of(99))),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "elements_sprite.png");
}

TEST_CASE("images page: image grid figure", "[images_page]")
{
   // The menu with cell 6 current and the pointer over cell 3, and the
   // cell showing the current one.
   auto img = numbered_cells(80, 60);             // drawn at half scale, for 2x
   auto menu = share(image_grid_menu(img, 4, 2, 0.5f));
   auto cell = share(image_grid_cell(img, 4, 2, element{}));
   menu->value(5);
   cell->value(5);

   test_view tv{extent{340, 120}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 14, 20, 10},
         htile(
            vtile(align_center(hold(menu)), caption("image_grid_menu")),
            margin_left(30, vtile(align_center(fixed_size({60, 60}, hold(cell))), caption("image_grid_cell")))
         )),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();

   // The pointer over cell 3.
   tv.view_.cursor({20 + 80 + 20, 14 + 15}, cursor_tracking::hovering);
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "elements_image_grid.png");
}

TEST_CASE("images page: example", "[images_page]")
{
   // The page's example, with the example resources' paths.
   test_view tv{extent{240, 150}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto& view_ = tv.view_;

   auto knob = share(sprite{knobs, 64, 0.5});

   auto show = [knob, &view_](double v)
   {
      knob->index(std::size_t(v * (knob->num_frames() - 1)));
      view_.refresh(*knob);
   };

   view_.content(
      align_center_middle(hold(knob)),
      image{space, image::fill}
   );
   show(0.7);
   CHECK(knob->index() == 69);

   view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "images_example.png");
}
