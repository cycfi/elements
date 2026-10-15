/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/styles/text_entry.adoc and
   draws its figure. The figure lands in the build's results directory;
   copy it to docs/modules/ROOT/images/styles/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <elements/element/style/text_entry.hpp>
#include <elements/element/style/caption.hpp>
#include <memory>
#include <string>
#include <type_traits>

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

   float line_height(font_descr fd)
   {
      auto m = font{fd}.metrics();
      return m.ascent + m.descent + m.leading;
   }

   bool near(float a, float b)
   {
      return std::abs(a - b) < 0.01f;
   }
}

TEST_CASE("text entry page: factories", "[text_entry_page]")
{
   test_view tv{extent{300, 300}};
   basic_context bctx{tv.view_, tv.cnv};
   auto const& thm = get_theme();

   // Each returns the framed box and the basic_input_box in it.
   auto [box, in] = input_box("Your name");
   CHECK(std::is_same_v<decltype(in), std::shared_ptr<basic_input_box>>);
   CHECK(in->get_text().empty());

   // One line of the font, padded by 0.3 of the font size above and below.
   auto fs = thm.text_box_font._size;
   CHECK(near(box.limits(bctx).min.y, line_height(thm.text_box_font) + 0.6f * fs));

   // `size` scales the font's own size.
   auto big = input_box("Your name", 1.5).first;
   CHECK(near(big.limits(bctx).min.y,
      line_height(thm.text_box_font.size(fs * 1.5f)) + 0.6f * fs * 1.5f));

   auto f = font_descr{"Open Sans"}.size(20);
   auto custom = input_box("Your name", f, 1.5).first;
   CHECK(near(custom.limits(bctx).min.y, line_height(f.size(30)) + 0.6f * 30));

   // No placeholder.
   auto [plain, plain_in] = input_box();
   CHECK(plain_in->get_text().empty());

   // Any input box of your own, with a padding of 5 unless given.
   auto mine = share(basic_input_box("Your name"));
   auto framed = input_box(hold(mine));
   CHECK(near(framed.limits(bctx).min.y, line_height(thm.text_box_font) + 10));
   auto framed2 = input_box(hold(mine), {8, 8, 8, 8});
   CHECK(near(framed2.limits(bctx).min.y, line_height(thm.text_box_font) + 16));
}

namespace
{
   std::string title;
   void update_title(std::string_view text) { title = text; }
}

TEST_CASE("text entry page: example", "[text_entry_page]")
{
   auto [name_box, name] = input_box("Your name");
   name->on_text =
      [](std::string_view text)
      {
         update_title(text);
      };

   auto entry = left_caption(name_box, "Name");

   test_view tv{extent{300, 300}};
   basic_context bctx{tv.view_, tv.cnv};
   CHECK(entry.limits(bctx).min.x > 0);
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("text entry page: figure", "[text_entry_page]")
{
   auto [empty_box, empty_in] = input_box("Your name");
   auto [text_box, text_in] = input_box("Your name", 1.4);
   text_in->set_text("Ada Lovelace");

   std::string const long_text = "The quick brown fox jumps over the lazy dog";
   auto [right_box, right_in] = input_box("", input_clip_action::clip_right);
   right_in->set_text(long_text);
   auto [left_box, left_in] = input_box("", input_clip_action::clip_left);
   left_in->set_text(long_text);

   auto cap = [](auto e, char const* text)
   {
      return margin_top(14,
         htile(
            hsize(100, align_middle(align_left(label(text)))),
            hsize(240, e)
         ));
   };

   auto fig =
      margin({20, 6, 20, 20},
         vtile(
            cap(empty_box, "Placeholder"),
            cap(text_box, "Size 1.4"),
            cap(right_box, "clip_right"),
            cap(left_box, "clip_left")
         )
      );

   point sz;
   {
      test_view m{extent{10, 10}};
      sz = fig.limits(basic_context{m.view_, m.cnv}).min;
   }
   test_view tv{extent{sz.x, sz.y}, 2};
   open(tv);
   tv.view_.content(fig, box(bkd_color));
   tv.view_.layout();
   tv.draw();
   tv.draw();     // The clip actions take effect from the second frame
   tv.img.save_png(std::string{RESULTS_PATH} + "text_entry.png");
}
#endif
