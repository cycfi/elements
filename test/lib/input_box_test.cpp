/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The input box: it is made from a placeholder alone; undo and redo report
   through on_text; the theme's text limit holds for typing as for pasting;
   the placeholder is drawn in the box's own font; and a text box in a
   disabled element is dimmed at the theme's disabled_opacity.
=============================================================================*/
#include "test_support.hpp"
#include <cstdint>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   struct restore_theme
   {
      ~restore_theme() { set_theme(theme{}); }
   };

   struct input_view : test_view
   {
      input_view(std::shared_ptr<basic_input_box> in_)
       : test_view{extent{300, 60}, 1}
       , in{std::move(in_)}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         view_.content(hport(hold(in)), box(rgba(0, 0, 0, 255)));
         view_.layout();
         draw();
         in->begin_focus(element::from_top);
      }

      context ctx() { return {view_, cnv, in.get(), rect{0, 0, 300, 60}}; }

      void type(std::u32string const& s)
      {
         auto c = ctx();
         for (char32_t ch : s)
            in->text(c, text_info{ch, 0});
      }

      void press(key_code k, int mods = 0)
      {
         auto c = ctx();
         in->key(c, key_info{k, key_action::press, mods});
      }

      std::shared_ptr<basic_input_box> in;
   };

#if !defined(ARTIST_RECORDING)
   // The brightest red in the image, and the height of the rows holding
   // anything brighter than the background.
   int brightest(cycfi::artist::image const& img)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels());
      auto n = int(img.bitmap_size().x * img.bitmap_size().y);
      int hi = 0;
      for (int i = 0; i != n; ++i)
         hi = std::max<int>(hi, p[4 * i + 2]);
      return hi;
   }

   int ink_height(cycfi::artist::image const& img)
   {
      auto p = reinterpret_cast<std::uint8_t const*>(img.pixels());
      int w = int(img.bitmap_size().x), h = int(img.bitmap_size().y);
      int top = h, bottom = -1;
      for (int y = 0; y != h; ++y)
         for (int x = 0; x != w; ++x)
            if (p[4 * (y * w + x) + 2] > 40)
            {
               top = std::min(top, y);
               bottom = std::max(bottom, y);
            }
      return bottom - top;
   }
#endif
}

TEST_CASE("an input box is made from a placeholder alone", "[input_box]")
{
   basic_input_box a("Name");
   basic_input_box b;
   basic_input_box c("Name", basic_input_box::clip_left);
   CHECK(a.get_text().empty());
   CHECK(b.get_text().empty());
   CHECK(c.get_text().empty());
}

TEST_CASE("undo and redo report through on_text", "[input_box]")
{
   input_view iv{share(basic_input_box("Name"))};
   std::string last = "unset";
   iv.in->on_text = [&](std::string_view t) { last = t; };

   iv.type(U"abc");
   CHECK(last == "abc");
   iv.press(key_code::z, mod_action);
   CHECK(iv.in->get_utf8().empty());
   CHECK(last.empty());
   iv.press(key_code::z, mod_action | mod_shift);
   CHECK(iv.in->get_utf8() == "abc");
   CHECK(last == "abc");
}

TEST_CASE("the text limit holds for typing and pasting", "[input_box]")
{
   restore_theme guard;
   theme t;
   t.input_box_text_limit = 5;
   set_theme(t);

   input_view iv{share(basic_input_box("Name"))};
   iv.type(U"abcdefg");
   CHECK(iv.in->get_utf8() == "abcde");

   // Replacing a selection is not refused.
   iv.in->select_start(4);
   iv.in->select_end(5);
   iv.type(U"Z");
   CHECK(iv.in->get_utf8() == "abcdZ");

   // A paste fills only the room left, and stops at a line break.
   if (!clipboard_works())
      return;
   iv.in->select_start(0);
   iv.in->select_end(2);
   clipboard("12345\nmore");
   iv.press(key_code::v, mod_action);
   CHECK(iv.in->get_utf8() == "12cdZ");

   iv.in->select_all();
   iv.press(key_code::v, mod_action);
   CHECK(iv.in->get_utf8() == "12345");
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("the placeholder is drawn in the box's own font", "[input_box]")
{
   auto small = share(basic_input_box("Hg", get_theme().text_box_font.size(10)));
   auto large = share(basic_input_box("Hg", get_theme().text_box_font.size(30)));

   auto height = [](std::shared_ptr<basic_input_box> in)
   {
      test_view tv{extent{200, 60}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      tv.view_.content(align_left_top(hport(hold(in))), box(rgba(0, 0, 0, 255)));
      tv.view_.layout();
      tv.draw();
      return ink_height(tv.img);
   };
   CHECK(height(large) > 2 * height(small));
}

TEST_CASE("a text box in a disabled element is dimmed by the theme", "[input_box]")
{
   // Disabled through the tile it sits in, not by itself: drawn at the
   // theme's disabled_opacity, as when it is disabled itself.
   auto brightness = [](std::shared_ptr<element> e, bool own, bool tile)
   {
      auto row = share(htile(hport(hold(e))));
      if (own)
         e->enable(false);
      if (tile)
         row->enable(false);
      test_view tv{extent{200, 60}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      tv.view_.content(hold(row), box(rgba(0, 0, 0, 255)));
      tv.view_.layout();
      tv.draw();
      return brightest(tv.img);
   };
   auto make_input = []
   {
      auto in = share(basic_input_box("", get_theme().text_box_font.size(30)));
      in->set_text("WWWW");
      in->set_color(rgba(255, 255, 255, 255));
      return in;
   };
   auto make_text = []
   {
      auto tb = share(basic_text_box("WWWW", get_theme().text_box_font.size(30)));
      tb->set_color(rgba(255, 255, 255, 255));
      return tb;
   };
   auto dim = 255 * get_theme().disabled_opacity;

   CHECK(brightness(make_input(), false, false) > 240);
   // The margin allows for the gamma some backends apply to text, which
   // lifts a dimmed glyph's brightest pixel a little (Skia: 122 to 128).
   CHECK(brightness(make_input(), true, false) == Approx(dim).margin(16));
   CHECK(brightness(make_input(), false, true) == Approx(dim).margin(16));
   CHECK(brightness(make_text(), false, true) == Approx(dim).margin(16));
}
#endif
