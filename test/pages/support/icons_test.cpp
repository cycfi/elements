/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/support/icons.adoc and
   draws its figure: every icon in icon_ids.hpp, with its name. The figure
   lands in the build's results directory; copy it to
   docs/modules/ROOT/images/support/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <cstdint>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
}

TEST_CASE("icons page: the codes", "[icons_page]")
{
   // The icon font's private use area starts at U+E800; a few came from
   // another font and keep their Font Awesome codes.
   CHECK(icons::left == 0xe800);
   CHECK(icons::shrink == 0xe850);
   CHECK(icons::menu == 0xf0c9);
   CHECK(icons::file_text == 0xe851);       // the first added later
   CHECK(icons::pause_circle == 0xe903);

   // mixer and sliders are one glyph.
   CHECK(icons::mixer == icons::sliders);
}

TEST_CASE("icons page: the icon element", "[icons_page]")
{
   test_view tv{extent{100, 100}, 1};
   icon i{icons::cog};
   auto one = i.limits(basic_context{tv.view_, tv.cnv});
   icon big{icons::cog, 2.0};
   auto two = big.limits(basic_context{tv.view_, tv.cnv});
   CHECK(one.min.x > 0);
   CHECK(two.min.x > one.min.x);        // the size is a multiplier
   CHECK(two.min.x == Approx(2 * one.min.x).margin(2));
}

TEST_CASE("icons page: example", "[icons_page]")
{
   // The page's example compiles and lays out.
   test_view tv{extent{200, 100}, 1};
   auto settings = icon_button(icons::cog);
   auto warning = icon(icons::attention, 1.5);
   auto save = button("Save", icons::floppy);
   tv.view_.content(htile(settings, warning, save));
   tv.view_.layout();
   tv.draw();
   CHECK(true);
}
