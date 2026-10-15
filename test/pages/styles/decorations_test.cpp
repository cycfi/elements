/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/decorations.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   template <typename E>
   auto sample(char const* name, E&& e)
   {
      return margin({10, 10, 10, 10},
         vtile(
            fixed_size({100, 60}, std::forward<E>(e)),
            margin_top(6, align_center(label(name).font_size(12)))
         )
      );
   }
}

TEST_CASE("decorations page: claims", "[decorations_page]")
{
   test_view tv{extent{200, 200}};
   basic_context bctx{tv.view_, tv.cnv};

   // The fills take any size; an icon has its own.
   auto lim = box(colors::red).limits(bctx);
   CHECK(lim.min == point{0, 0});
   CHECK(lim.max == point{full_extent, full_extent});
   auto ilim = icon{icons::attention, 2}.limits(bctx);
   CHECK(ilim.min == ilim.max);
   CHECK(ilim.min.x > 0);

   // The page's snippet.
   auto level = draw_value<float>(
      [](context const& ctx, float v)
      {
         auto r = ctx.bounds;
         r.top = r.bottom - r.height() * v;
         ctx.canvas.fill_style(colors::light_sea_green);
         ctx.canvas.fill_rect(r);
      }
   );
   level.value(0.5);
   CHECK(level.value() == 0.5f);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("decorations page: figures", "[decorations_page]")
{
   {
      test_view tv{extent{600, 200}, 2};
      open(tv);
      tv.view_.content(
         align_left_top(
            vtile(
               htile(
                  sample("box", box(colors::royal_blue)),
                  sample("rbox", rbox(colors::royal_blue, 8)),
                  sample("panel", panel{}),
                  sample("frame", frame{}),
                  sample("title_bar", title_bar{})
               ),
               htile(
                  sample("frame(color)", frame{colors::gold, 2, 6}),
                  sample("icon", align_center_middle(icon{icons::attention, 2})),
                  sample("draw", draw([](context const& ctx)
                     {
                        auto& c = ctx.canvas;
                        c.begin_path();
                        c.add_circle({center_point(ctx.bounds), 25});
                        c.fill_style(colors::light_sea_green);
                        c.fill();
                     }))
               )
            )
         ),
         box(bkd_color)
      );
      tv.view_.layout();
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + "decorations_elements.png");
   }
   {
      test_view tv{extent{600, 200}, 2};
      open(tv);
      tv.view_.content(
         align_left_top(
            vtile(
               htile(
                  sample("border", border{}),
                  sample("border_left", border_left{}),
                  sample("border_right", border_right{}),
                  sample("border_top", border_top{})
               ),
               htile(
                  sample("border_bottom", border_bottom{}),
                  sample("border_top_bottom", border_top_bottom{}),
                  sample("border_left_right", border_left_right{})
               )
            )
         ),
         box(bkd_color)
      );
      tv.view_.layout();
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + "decorations_borders.png");
   }
}
#endif
