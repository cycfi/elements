/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/labels.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <cstdint>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   // The page's label style.
   struct note_styler : default_label_styler
   {
      using base_type = note_styler;

      float get_font_size() const override         { return 11; }
      float get_default_font_size() const override { return 11; }
      color get_font_color() const override        { return colors::gray[60]; }
      int   get_text_align() const override        { return canvas::left | canvas::middle; }
   };

   using note_label = label_styler_gen<basic_label_styler_base<note_styler>>;

#if !defined(ARTIST_RECORDING)
   // The leftmost and rightmost columns holding a pixel brighter than
   // the background, along row y.
   std::pair<int, int> ink_span(cycfi::artist::image const& img, int y0, int y1)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels());
      int w = int(img.bitmap_size().x);
      int lo = w, hi = -1;
      for (int y = y0; y != y1; ++y)
         for (int x = 0; x != w; ++x)
            if (p[4 * (y * w + x) + 2] > 120)
            {
               lo = std::min(lo, x);
               hi = std::max(hi, x);
            }
      return {lo, hi};
   }
#endif
}

TEST_CASE("labels page: the label's size is its text's", "[labels_page]")
{
   test_view tv{extent{200, 60}, 1};
   basic_context bctx{tv.view_, tv.cnv};

   auto l = label("Hello");
   auto f = get_theme().label_font;
   auto size = measure_text(tv.cnv, "Hello", f.size(f._size));
   auto lim = l.limits(bctx);
   CHECK(lim.min == point{size.x, size.y});
   CHECK(lim.max == lim.min);
}

TEST_CASE("labels page: label and heading take the theme's defaults", "[labels_page]")
{
   auto const& thm = get_theme();
   auto l = label("a");
   CHECK(l.get_font_size() == thm.label_font._size);
   CHECK(l.get_font_color() == thm.label_font_color);
   CHECK(l.get_text_align() == thm.label_text_align);

   auto h = heading("a");
   CHECK(h.get_font_size() == thm.heading_font._size);
   CHECK(h.get_font_color() == thm.heading_font_color);
   CHECK(h.get_text_align() == thm.heading_text_align);
   CHECK(h.get_text() == "a");
}

TEST_CASE("labels page: heading levels", "[labels_page]")
{
   auto const& thm = get_theme();
   auto base = thm.heading_font._size;
   float sizes[] = {
      heading1("a").get_font_size(), heading2("a").get_font_size(),
      heading3("a").get_font_size(), heading4("a").get_font_size(),
      heading5("a").get_font_size()
   };
   for (std::size_t i = 0; i != 5; ++i)
      CHECK(sizes[i] == Approx(base * thm.heading_scale[i]));
   for (std::size_t i = 1; i != 5; ++i)
      CHECK(sizes[i] < sizes[i - 1]);                // heading1 the largest
   CHECK(heading("a").get_font_size() == Approx(base)); // heading is heading5
   CHECK(sizes[4] >= label("a").get_font_size());     // no smaller than a label
   CHECK(heading1("a").get_font_color() == thm.heading_font_color);
}

TEST_CASE("labels page: the modifiers", "[labels_page]")
{
   auto const& thm = get_theme();
   auto l = label("a")
      .font(font_descr{"Open Sans"}.semi_bold())
      .font_size(18)
      .font_color(colors::gold)
      .text_align(canvas::right);

   // Each returns a new label; the earlier settings are kept.
   CHECK(l.get_font_size() == 18);
   CHECK(l.get_font_color() == colors::gold);
   CHECK(l.get_text_align() == canvas::right);
   CHECK(l.get_text() == "a");

   auto r = heading("b").relative_font_size(2);
   CHECK(r.get_font_size() == Approx(thm.heading_font._size * 2));
}

TEST_CASE("labels page: the text can be read and replaced", "[labels_page]")
{
   auto l = share(label("before"));
   l->set_text("after");
   CHECK(l->get_text() == "after");

   // Through the interface, from a proxy around the label.
   auto m = share(margin({4, 4, 4, 4}, hold(l)));
   auto* w = find_subject<text_writer_u8*>(m.get());
   REQUIRE(w);
   w->set_text("again");
   CHECK(l->get_text() == "again");
}

TEST_CASE("labels page: a live label", "[labels_page]")
{
   // The page's example: a readout that turns gold while clipping.
   test_view tv{extent{100, 40}, 1};
   auto& view_ = tv.view_;
   auto peak = share(label("0 dB").font_color(colors::gray[60]));
   bool clipping = true;

   peak->set_font_color(clipping ? colors::gold : colors::gray[60]);
   view_.refresh(*peak);
   CHECK(peak->get_font_color() == colors::gold);
}

TEST_CASE("labels page: disabled", "[labels_page]")
{
   auto l = label("a");
   CHECK(l.is_enabled());
   l.enable(false);
   CHECK(!l.is_enabled());
}

TEST_CASE("labels page: as_label", "[labels_page]")
{
   auto level = share(as_label<double>(
      [](double v) { return std::to_string(int(v * 100)) + "%"; },
      label("0%")));
   level->value(0.42);
   CHECK(level->value() == Approx(0.42));
   CHECK(level->actual_subject().get_text() == "42%");
}

TEST_CASE("labels page: a label style", "[labels_page]")
{
   auto c = note_label("Gain");
   CHECK(c.get_font_size() == 11);
   CHECK(c.get_text_align() == (canvas::left | canvas::middle));
   auto g = note_label("Gain").font_color(colors::gold);
   CHECK(g.get_font_color() == colors::gold);
   CHECK(g.get_font_size() == 11);                 // the style's, kept
   CHECK(note_label("x").relative_font_size(2).get_font_size() == 22);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("labels page: text_align places the text in a wider box", "[labels_page]")
{
   // A vtile is as wide as its widest child, and gives every child its
   // whole width: the short labels align against the long one.
   test_view tv{extent{300, 100}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   auto probe = share(vtile(
      label("A label long enough to set the width"),
      label("left").text_align(canvas::left),
      label("centre").text_align(canvas::center),
      label("right").text_align(canvas::right)
   ));
   tv.view_.content(align_left_top(hold(probe)), box(rgba(0, 0, 0, 255)));
   tv.view_.layout();
   tv.draw();

   basic_context bctx{tv.view_, tv.cnv};
   auto w = int(probe->limits(bctx).min.x);
   auto h = int(probe->at(0).limits(bctx).min.y);
   auto row = [&](int i) { return ink_span(tv.img, i * h + 2, (i + 1) * h - 2); };

   auto left = row(1);
   auto centre = row(2);
   auto right = row(3);
   CHECK(left.first < 3);
   CHECK(std::abs((centre.first + centre.second) / 2 - w / 2) < 4);
   CHECK(std::abs(right.second - w) < 4);
}
#endif

TEST_CASE("labels page: figure", "[labels_page]")
{
   auto caption = [](char const* text)
   {
      return label(text).font_size(12).font_color(rgba(150, 150, 150, 255));
   };
   auto row = [&](auto e, char const* text)
   {
      return htile(
         hsize(170, align_left_middle(caption(text))),
         align_left_middle(e));
   };

   test_view tv{extent{440, 210}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 12, 20, 12},
         vtile(
            vsize(30, row(label("The quick brown fox"), "label")),
            vsize(30, row(heading("The quick brown fox"), "heading")),
            vsize(34, row(label("The quick brown fox").font_size(22), ".font_size(22)")),
            vsize(30, row(label("The quick brown fox").font_color(colors::gold), ".font_color(colors::gold)")),
            vsize(30, row(label("The quick brown fox").font(font_descr{"Open Sans"}.italic()), ".font(f.italic())")),
            vsize(30, row(note_label("The quick brown fox"), "note_label"))
         )),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "elements_labels.png");
}

TEST_CASE("labels page: example figure", "[labels_page]")
{
   // The page's example rendered.
   auto title = label("Hello, Universe.")
      .font(font_descr{"Open Sans"}.semi_bold())
      .font_size(18)
      .font_color(colors::antique_white);

   auto lines = vtile(
      label("Elements: a C++ GUI library"),
      label("left").text_align(canvas::left),
      label("centre").text_align(canvas::center),
      label("right").text_align(canvas::right));

   // The converter as a function pointer (the leading +): MSVC cannot
   // instantiate hold() over a type named after a lambda.
   auto level = share(as_label<double>(
      +[](double v) { return std::to_string(int(v * 100)) + "%"; },
      label("0%")));
   level->value(0.42);

   test_view tv{extent{260, 165}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 12, 20, 12},
         vtile(
            align_left(title),
            margin_top(10, layer(margin({6, 4, 6, 4}, lines), frame{})),
            margin_top(10, align_left(hold(level)))
         )),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "labels_example.png");
}

TEST_CASE("labels page: readout figure", "[labels_page]")
{
   // The readout example, before and while clipping.
   auto quiet = share(label("0 dB").font_color(colors::gray[60]));
   auto peak = share(label("0 dB").font_color(colors::gray[60]));
   peak->set_font_color(colors::gold);

   auto caption = [](char const* text)
   {
      return align_center(label(text).font_size(12).font_color(rgba(150, 150, 150, 255)));
   };
   auto cell = [&](auto e, char const* text)
   {
      return hsize(90, vtile(vsize(30, align_center_middle(e)), caption(text)));
   };

   test_view tv{extent{200, 64}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({10, 8, 10, 8}, htile(cell(hold(quiet), "quiet"), cell(hold(peak), "clipping"))),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "labels_readout.png");
}

TEST_CASE("labels page: heading levels figure", "[labels_page]")
{
   auto caption = [](char const* text)
   {
      return label(text).font_size(12).font_color(rgba(150, 150, 150, 255));
   };
   auto row = [&](auto e, char const* text)
   {
      return htile(hsize(90, align_left_middle(caption(text))), align_left_middle(e));
   };

   test_view tv{extent{360, 186}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 10, 20, 10},
         vtile(
            vsize(46, row(heading1("Section title"), "heading1")),
            vsize(38, row(heading2("Section title"), "heading2")),
            vsize(32, row(heading3("Section title"), "heading3")),
            vsize(28, row(heading4("Section title"), "heading4")),
            vsize(26, row(heading5("Section title"), "heading5"))
         )),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "heading_levels.png");
}
