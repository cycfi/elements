/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   A label's setters change a live label in place, each for a setting the
   label was built with. The alignment setter is set_text_align: named
   text_align, it was hidden by the modifier of the same name, and a call
   made a discarded copy instead.
=============================================================================*/
#include "test_support.hpp"

using namespace cycfi::elements;
using namespace cycfi::elements::test;

TEST_CASE("label setters change the label in place", "[label]")
{
   auto l = share(label("x")
      .font(font_descr{"Open Sans"})
      .font_size(12)
      .font_color(colors::gray[60])
      .text_align(canvas::left));

   l->set_text_align(canvas::right);
   CHECK(l->get_text_align() == canvas::right);

   l->set_font_size(20);
   CHECK(l->get_font_size() == 20);

   l->set_relative_font_size(2);
   CHECK(l->get_font_size() == Approx(2 * get_theme().label_font._size));

   l->set_font_color(colors::gold);
   CHECK(l->get_font_color() == colors::gold);
}
