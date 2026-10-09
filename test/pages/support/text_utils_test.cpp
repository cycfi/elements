/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/support/text_utils.adoc
   and draws its figure. The figure lands in the build's results
   directory; copy it to docs/modules/ROOT/images/support/ to update the
   page.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);
}

TEST_CASE("text_utils page: figure", "[text_utils_page]")
{
   test_view tv{extent{300, 60}, 2};
   tv.cnv.fill_style(bkd_color);
   tv.cnv.add_rect({0, 0, 300, 60});
   tv.cnv.fill();

   // The page's example: icons in the theme's icon color, then a colored
   // one, each centered in its cell.
   draw_icon(tv.cnv, {10, 10, 60, 50}, icons::ok, 16);
   draw_icon(tv.cnv, {70, 10, 120, 50}, icons::cancel, 16);
   draw_icon(tv.cnv, {130, 10, 180, 50}, icons::cog, 24);
   draw_icon(tv.cnv, {190, 10, 240, 50}, icons::volume_up, 24);
   draw_icon(tv.cnv, {250, 10, 300, 50}, icons::attention, 24, colors::gold);

   tv.img.save_png(std::string{RESULTS_PATH} + "text_utils_icons.png");
}

TEST_CASE("text_utils page: measure_text", "[text_utils_page]")
{
   test_view tv{extent{100, 100}, 1};
   auto font_ = get_theme().label_font;

   // Width grows with the text, height with the font and not the text.
   auto a = measure_text(tv.cnv, "Hello", font_);
   auto b = measure_text(tv.cnv, "Hello, world", font_);
   CHECK(a.x > 0);
   CHECK(b.x > a.x);
   CHECK(b.y == Approx(a.y));

   // The height is the font's ascent, descent and leading together.
   tv.cnv.font(font_);
   auto m = tv.cnv.measure_text("Hello");
   CHECK(a.y == Approx(m.ascent + m.descent + m.leading));

   auto big = measure_text(tv.cnv, "Hello", font_.size(28));
   CHECK(big.y > a.y);
   CHECK(big.x > a.x);
}

TEST_CASE("text_utils page: measure_icon", "[text_utils_page]")
{
   test_view tv{extent{100, 100}, 1};
   auto s16 = measure_icon(tv.cnv, icons::ok, 16);
   auto s32 = measure_icon(tv.cnv, icons::ok, 32);
   CHECK(s16.x > 0);
   CHECK(s32.x > s16.x);
   CHECK(s32.y > s16.y);
}

TEST_CASE("text_utils page: _as_char", "[text_utils_page]")
{
   // A UTF-8 literal as char const*, usable where a std::string is built.
   std::string s = u8"⌘"_as_char;
   CHECK(s.size() == 3);           // U+2318 is three bytes in UTF-8
   CHECK(s == "\xe2\x8c\x98");
}

TEST_CASE("text_utils page: text readers and writers", "[text_utils_page]")
{
   // The page's example: a label's text, found inside a proxy and changed
   // through the interfaces, without knowing which styler holds it.
   auto lbl = share(margin({10, 10, 10, 10}, label("Before").font_size(18)));
   auto reader = find_subject<text_reader_u8*>(lbl.get());
   auto writer = find_subject<text_writer_u8*>(lbl.get());
   REQUIRE(reader);
   REQUIRE(writer);
   CHECK(reader->get_text() == "Before");
   writer->set_text("After");
   CHECK(reader->get_text() == "After");

   // Text boxes hold UTF-32.
   test_view tv{extent{200, 100}, 1};
   auto box = share(margin({10, 10, 10, 10}, basic_text_box("abc")));
   auto r32 = find_subject<text_reader_u32*>(box.get());
   REQUIRE(r32);
   CHECK(r32->get_text() == U"abc");
}
