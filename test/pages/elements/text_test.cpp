/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/text.adoc and
   draws its figures. The figures land in the build's results directory;
   copy them to docs/modules/ROOT/images/elements/ to update the page.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   float line_height(font const& f)
   {
      auto m = f.metrics();
      return m.ascent + m.descent + m.leading;
   }

   // A view holding one element at the top left, and a context for it.
   template <typename E>
   struct text_view : test_view
   {
      text_view(std::shared_ptr<E> e_, extent size = {300, 120})
       : test_view{size, 1}
       , e{std::move(e_)}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(view_);
#endif
         view_.content(hold(e));
         view_.layout();
         draw();
      }

      context ctx() { return {view_, cnv, e.get(), rect{0, 0, view_.size().x, view_.size().y}}; }

      void type(std::u32string const& s)
      {
         auto c = ctx();
         for (char32_t ch : s)
            e->text(c, text_info{ch, 0});
      }

      bool press(key_code k, int mods = 0)
      {
         auto c = ctx();
         return e->key(c, key_info{k, key_action::press, mods});
      }

      std::shared_ptr<E> e;
   };

   mouse_button left_click(point p, int clicks)
   {
      mouse_button btn{};
      btn.down = true;
      btn.state = mouse_button::left;
      btn.num_clicks = clicks;
      btn.pos = p;
      return btn;
   }
}

TEST_CASE("text page: static text box", "[text_page]")
{
   auto const& thm = get_theme();
   auto st = share(static_text_box("One two three"));
   test_view tv{extent{300, 120}, 1};
   basic_context bctx{tv.view_, tv.cnv};

   // Before layout: at least 200 wide and one line high, and no maximum.
   auto lim = st->limits(bctx);
   CHECK(lim.min.x == 200);
   CHECK(lim.min.y == Approx(line_height(st->get_font())));
   CHECK(lim.max.x == full_extent);

   CHECK(st->get_utf8() == "One two three");
   CHECK(st->get_color() == thm.text_box_font_color);

   st->set_text("Alpha");
   CHECK(st->get_utf8() == "Alpha");
   st->insert(5, " Beta");
   CHECK(st->get_utf8() == "Alpha Beta");
   st->replace(0, 5, "Gamma");
   CHECK(st->get_utf8() == "Gamma Beta");
   st->erase(5, 5);
   CHECK(st->get_utf8() == "Gamma");

   // As a receiver of a UTF-32 string.
   receiver<std::u32string_view>& r = *st;
   r.value(U"Delta");
   CHECK(st->get_utf8() == "Delta");
}

TEST_CASE("text page: a static text box wraps to its width", "[text_page]")
{
   auto st = share(static_text_box(
      "The text wraps at the width the box is given, and the box is as "
      "high as the lines it takes."));
   test_view tv{extent{220, 200}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(align_left_top(hold(st)));
   tv.view_.layout();
   tv.draw();

   auto lh = line_height(st->get_font());
   CHECK(st->current_size().x == 220);
   CHECK(st->current_size().y > lh * 1.5);       // more than one line

   basic_context bctx{tv.view_, tv.cnv};
   CHECK(st->limits(bctx).min.y == Approx(st->current_size().y));
}

TEST_CASE("text page: selection and typing", "[text_page]")
{
   text_view<basic_text_box> tv{share(basic_text_box("Hello"))};
   auto& tb = *tv.e;

   CHECK(tb.select_start() == -1);               // no selection, no caret
   tb.begin_focus(element::from_top);
   CHECK(tb.is_focus());
   CHECK(tb.select_start() == 0);                // focus puts the caret at the start
   CHECK(tb.select_end() == 0);

   tb.end();
   tv.type(U" World");
   CHECK(tb.get_utf8() == "Hello World");

   // Typing replaces the selection.
   tb.select_start(6);
   tb.select_end(11);
   tv.type(U"There");
   CHECK(tb.get_utf8() == "Hello There");

   // Shift-Left extends the selection backwards: start after end.
   tb.end();
   tv.press(key_code::left, mod_shift);
   tv.press(key_code::left, mod_shift);
   CHECK(tb.select_start() == 11);
   CHECK(tb.select_end() == 9);

   tv.press(key_code::backspace);
   CHECK(tb.get_utf8() == "Hello The");

   tb.select_all();
   CHECK(tb.select_start() == 0);
   CHECK(tb.select_end() == 9);
   tb.select_none();
   CHECK(tb.select_start() == -1);

   tb.home();
   CHECK(tb.select_end() == 0);
   tb.end(true);                                  // with Shift: select to the end
   CHECK(tb.select_start() == 0);
   CHECK(tb.select_end() == 9);
}

TEST_CASE("text page: clipboard and undo", "[text_page]")
{
   text_view<basic_text_box> tv{share(basic_text_box("cut here"))};
   auto& tb = *tv.e;
   tb.begin_focus(element::from_top);

   tb.select_start(0);
   tb.select_end(4);
   tv.press(key_code::c, mod_action);
   CHECK(clipboard() == "cut ");

   tv.press(key_code::x, mod_action);
   CHECK(tb.get_utf8() == "here");

   tb.end();
   tv.press(key_code::v, mod_action);
   CHECK(tb.get_utf8() == "herecut ");

   // Undo and redo through the view.
   tv.press(key_code::z, mod_action);
   CHECK(tb.get_utf8() == "here");
   tv.press(key_code::z, mod_action | mod_shift);
   CHECK(tb.get_utf8() == "herecut ");
}

TEST_CASE("text page: double click selects a word", "[text_page]")
{
   text_view<basic_text_box> tv{share(basic_text_box("alpha beta gamma"))};
   auto& tb = *tv.e;
   tb.begin_focus(element::from_top);

   // A point inside "beta": the caret position after "alpha be".
   auto c = tv.ctx();
   auto x = tb.get_layout().caret_point(8).x;
   auto lh = line_height(tb.get_font());
   tb.click(c, left_click({x, lh / 2}, 2));
   CHECK(tb.select_start() == 6);
   CHECK(tb.select_end() == 10);                 // "beta"
}

TEST_CASE("text page: read only and disabled", "[text_page]")
{
   text_view<basic_text_box> tv{share(basic_text_box("fixed").read_only())};
   auto& tb = *tv.e;
   tb.begin_focus(element::from_top);
   CHECK(!tb.editable());

   tv.type(U"x");
   CHECK(tb.get_utf8() == "fixed");               // typing is ignored
   tb.select_all();
   tv.press(key_code::c, mod_action);
   CHECK(clipboard() == "fixed");                // copying is not

   tb.read_only(false);
   CHECK(tb.editable());
   tb.enable(false);
   CHECK(!tb.editable());
   CHECK(!tb.is_enabled());
}

TEST_CASE("text page: input box", "[text_page]")
{
   auto ib = share(basic_input_box("Name"));
   text_view<basic_input_box> tv{ib};
   auto& in = *tv.e;

   // One line high, and as wide as it is given.
   basic_context bctx{tv.view_, tv.cnv};
   auto lim = in.limits(bctx);
   CHECK(lim.min.y == Approx(line_height(in.get_font())));
   CHECK(lim.max.y == lim.min.y);
   CHECK(lim.min.x == full_extent);

   std::vector<std::string> texts;
   in.on_text = [&](std::string_view t) { texts.emplace_back(t); };
   std::string entered;
   in.on_enter = [&](std::string_view t) { entered = t; return true; };
   bool escaped = false;
   in.on_escape = [&]() { escaped = true; };

   in.begin_focus(element::from_top);
   tv.type(U"Ann");
   CHECK(texts == std::vector<std::string>{"A", "An", "Ann"});

   CHECK(tv.press(key_code::enter));
   CHECK(entered == "Ann");

   CHECK(tv.press(key_code::escape));
   CHECK(escaped);

   // Tab, Up and Down are left to the enclosing elements.
   CHECK(!tv.press(key_code::tab));
   CHECK(!tv.press(key_code::up));

   // Paste stops at the first newline.
   clipboard("first\nsecond");
   in.select_all();
   tv.press(key_code::v, mod_action);
   CHECK(in.get_utf8() == "first");
   CHECK(texts.back() == "first");
}

TEST_CASE("text page: on_end_focus can keep the focus", "[text_page]")
{
   auto ib = share(basic_input_box());
   text_view<basic_input_box> tv{ib};
   auto& in = *tv.e;
   in.on_end_focus = [](std::string_view t) { return !t.empty(); };

   in.begin_focus(element::from_top);
   CHECK(!in.end_focus());                        // empty: refused
   tv.type(U"ok");
   CHECK(in.end_focus());
}

TEST_CASE("text page: more editing", "[text_page]")
{
   text_view<basic_text_box> tv{share(basic_text_box("first\nsecond"))};
   auto& tb = *tv.e;
   tb.begin_focus(element::from_top);

   // A position past the text is ignored.
   tb.select_start(99);
   CHECK(tb.select_start() == 0);

   // Down moves a line, keeping the horizontal position the caret was
   // last placed at.
   tb.home();
   tv.press(key_code::right);
   tv.press(key_code::right);
   CHECK(tb.select_end() == 2);
   tv.press(key_code::down);
   CHECK(tb.select_end() > 6);
   CHECK(tb.select_end() < 10);

   // Typing is undone as a run.
   tb.end();
   tv.type(U"xyz");
   CHECK(tb.get_utf8() == "first\nsecondxyz");
   tv.press(key_code::z, mod_action);
   CHECK(tb.get_utf8() == "first\nsecond");

   // Enter starts a new paragraph.
   tb.end();
   tv.press(key_code::enter);
   CHECK(tb.get_utf8() == "first\nsecond\n");
}

TEST_CASE("text page: cut reports through on_text", "[text_page]")
{
   auto ib = share(basic_input_box());
   text_view<basic_input_box> tv{ib};
   auto& in = *tv.e;
   in.begin_focus(element::from_top);
   tv.type(U"abc");

   std::string last;
   in.on_text = [&](std::string_view t) { last = t; };
   in.select_all();
   tv.press(key_code::x, mod_action);
   CHECK(in.get_utf8().empty());
   CHECK(last.empty());
   CHECK(clipboard() == "abc");
}

TEST_CASE("text page: on_enter decides the focus", "[text_page]")
{
   // In a tile, so the focus has somewhere to go.
   auto [box, in] = input_box("Your name");
   auto other = share(basic_text_box("other"));
   test_view tv{extent{300, 120}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(vtile(box, hold(other)));
   tv.view_.layout();
   tv.draw();

   in->on_enter = [](std::string_view text) { return !text.empty(); };

   mouse_button btn = left_click({20, 10}, 1);
   tv.view_.click(btn);
   btn.down = false;
   tv.view_.click(btn);
   REQUIRE(in->is_focus());

   tv.view_.key(key_info{key_code::enter, key_action::press, 0});
   CHECK(in->is_focus());                        // empty: the focus stays

   tv.view_.text(text_info{U'A', 0});
   tv.view_.key(key_info{key_code::enter, key_action::press, 0});
   CHECK(!in->is_focus());                       // accepted: the focus goes
}

namespace
{
   void greet(std::string_view) {}
}

TEST_CASE("text page: example figure", "[text_page]")
{
   // The page's example, rendered.
   auto notes = share(basic_text_box("Notes: the take at 2:14 is the keeper."));
   auto notes_pane = scroller(margin({5, 5, 5, 5}, hold(notes)), no_hscroll);

   auto [name_box, name] = input_box("Your name");
   name->on_enter =
      [](std::string_view text)
      {
         if (text.empty())
            return false;     // keep the focus until a name is entered
         greet(text);
         return true;
      };

   test_view tv{extent{320, 130}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({14, 14, 14, 14},
         vtile(
            vsize(60, layer(notes_pane, frame{})),
            margin_top(12, name_box)
         )),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "text_example.png");
}

TEST_CASE("text page: figure", "[text_page]")
{
   auto caption = [](char const* text)
   {
      return margin_top(6, align_left(
         label(text).font_size(12).font_color(rgba(150, 150, 150, 255))));
   };

   auto st = share(static_text_box(
      "A static text box shows text in a font and colour, wrapped to the "
      "width it is given."));

   auto tb = share(basic_text_box(
      "An editable text box: the selection is drawn behind the text, and "
      "the caret blinks while it has the focus."));
   tb->begin_focus(element::from_top);
   tb->select_start(4);
   tb->select_end(25);

   auto empty_in = input_box("Your name");
   auto full_in = input_box("Your name");
   full_in.second->set_text("Ada Lovelace");

   test_view tv{extent{440, 226}, 2};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      margin({20, 14, 20, 14},
         vtile(
            vstretch(0, hold(st)),
            caption("static_text_box"),
            margin_top(14, vstretch(0, hold(tb))),
            caption("basic_text_box, with a selection"),
            margin_top(14, htile(
               empty_in.first,
               margin_left(12, full_in.first)
            )),
            caption("basic_input_box in an input_box: the placeholder, and text"),
            empty()
         )),
      box(bkd_color)
   );
   tv.view_.layout();
   tv.draw();
   tv.img.save_png(std::string{RESULTS_PATH} + "elements_text.png");
}
