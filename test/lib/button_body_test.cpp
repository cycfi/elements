/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   A button whose face is not text: content layered over a button_body,
   which takes the button's whole size, whatever the content's, or a
   button_face, one look while off and another while on.
=============================================================================*/
#include "test_support.hpp"

namespace cycfi::elements::test
{
   namespace
   {
      // Records the bounds it was drawn with
      struct bounds_probe : element
      {
         view_limits limits(basic_context const&) const override
         {
            return full_limits;
         }

         void draw(context const& ctx) override
         {
            *seen = ctx.bounds;
         }

         std::shared_ptr<rect> seen = std::make_shared<rect>();
      };
   }
}

using namespace cycfi::elements;
using namespace cycfi::elements::test;

TEST_CASE("button_body: the face of a button, as large as the button")
{
   test_view tv{{200, 120}};
   bounds_probe content;
   auto b = share(latching_button(layer(
      content
    , button_body{colors::red}
   )));
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_left_top(fixed_size({80, 60}, hold(b))));
   tv.view_.layout();
   tv.draw();

   rect button;
   tv.view_.in_context_do(*b, [&](context const& ctx)
   {
      button = ctx.bounds;
      basic_context bctx{ctx.view, ctx.canvas};
      auto const l = button_body{}.limits(bctx);
      CHECK(l.min.y == Approx(0));
      CHECK(l.max.y >= full_extent);
   });
   CHECK(button.height() == Approx(60));
   CHECK(content.seen->height() == Approx(60));
}

TEST_CASE("button_face: one look while off, the other while on")
{
   test_view tv{{200, 120}};
   bounds_probe off;
   bounds_probe on;
   auto b = share(toggle_button(button_face(off, on)));
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_left_top(fixed_size({40, 30}, hold(b))));
   tv.view_.layout();

   b->value(false);
   tv.draw();
   CHECK(off.seen->width() == Approx(40));
   CHECK(on.seen->width() == Approx(0));

   *off.seen = {};
   b->value(true);
   tv.draw();
   CHECK(on.seen->width() == Approx(40));
   CHECK(off.seen->width() == Approx(0));
}

TEST_CASE("frame: a given color, width and corner radius")
{
   test_view tv{{100, 60}};
   auto f = share(frame{colors::red, 2.0f, 4.0f});
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(hold(f));
   tv.view_.layout();
   tv.draw();               // a custom frame draws, as the theme's does
   SUCCEED();
}
