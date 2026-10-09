/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Heading levels: heading1 to heading5 draw in the theme's heading font at
   its size times the theme's heading_scale for the level; heading is
   heading5, the size headings had before the levels.
=============================================================================*/
#include "test_support.hpp"
#include <type_traits>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   struct restore_theme
   {
      ~restore_theme() { set_theme(theme{}); }
   };
}

TEST_CASE("heading levels scale the heading font", "[heading]")
{
   restore_theme guard;
   auto const& thm = get_theme();
   auto base = thm.heading_font._size;

   CHECK(heading1("a").get_font_size() == Approx(base * thm.heading_scale[0]));
   CHECK(heading2("a").get_font_size() == Approx(base * thm.heading_scale[1]));
   CHECK(heading3("a").get_font_size() == Approx(base * thm.heading_scale[2]));
   CHECK(heading4("a").get_font_size() == Approx(base * thm.heading_scale[3]));
   CHECK(heading5("a").get_font_size() == Approx(base * thm.heading_scale[4]));

   // Largest first.
   for (std::size_t i = 1; i != thm.heading_scale.size(); ++i)
      CHECK(thm.heading_scale[i] < thm.heading_scale[i - 1]);

   // heading is heading5, at the heading font's own size.
   CHECK(std::is_same_v<heading, heading5>);
   CHECK(thm.heading_scale[4] == 1.0f);

   // A minor-third scale: each level is 1.2 times the one below.
   for (std::size_t i = 0; i != 4; ++i)
      CHECK(thm.heading_scale[i] / thm.heading_scale[i + 1] == Approx(1.2).epsilon(0.001));
   CHECK(heading("a").get_font_size() == Approx(base));

   CHECK(heading2("a").class_name() == "heading2");

   // The smallest heading is no smaller than a label.
   CHECK(heading5("a").get_font_size() >= label("a").get_font_size());
}

TEST_CASE("heading levels follow the theme", "[heading]")
{
   restore_theme guard;
   theme t;
   t.heading_scale = {3.0f, 2.0f, 1.5f, 1.0f, 0.5f};
   set_theme(t);
   CHECK(heading1("a").get_font_size() == Approx(t.heading_font._size * 3.0f));
}

TEST_CASE("modifiers on a heading level", "[heading]")
{
   auto const& thm = get_theme();
   auto base = thm.heading_font._size;

   // relative_font_size scales the level's own size.
   CHECK(heading2("a").relative_font_size(0.5).get_font_size()
      == Approx(base * thm.heading_scale[1] * 0.5));

   // and a second size modifier still scales the level's, not the first's.
   auto h = heading2("a").font_size(40).relative_font_size(0.5);
   CHECK(h.get_font_size() == Approx(base * thm.heading_scale[1] * 0.5));
}

TEST_CASE("a heading takes its default alignment from the heading settings", "[heading]")
{
   restore_theme guard;
   theme t;
   t.heading_text_align = canvas::top | canvas::center;
   t.label_text_align = canvas::bottom | canvas::center;
   set_theme(t);

   CHECK(heading("a").get_default_text_align() == (canvas::top | canvas::center));
   CHECK(label("a").get_default_text_align() == (canvas::bottom | canvas::center));
   // A text_align without a vertical part keeps the style's default one.
   CHECK(heading("a").text_align(canvas::left).get_default_text_align()
      == (canvas::top | canvas::center));
}
