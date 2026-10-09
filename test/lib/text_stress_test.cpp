/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Stress tests for text entry. Seeded random sessions drive a text box and
   an input box through the view, as a user would: typing, the arrow keys,
   Home and End, deletion, the clipboard, Enter, the mouse, undo and redo.

   After every step the selection must be in range and the UTF-8 and UTF-32
   texts must agree. In the model sessions, every edit is checked against a
   model computed from the selection the edit was made at. In the history
   sessions, undoing every step must give back the starting text, and
   redoing as many steps the final one.

   The number of steps per session is TEXT_STRESS_STEPS, 1500 by default.
=============================================================================*/
#include "test_support.hpp"
#include <infra/utf8_utils.hpp>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using cycfi::to_utf8;
using cycfi::to_utf32;

namespace
{
   int steps()
   {
      if (auto const* s = std::getenv("TEXT_STRESS_STEPS"))
         return std::max(1, std::atoi(s));
      return 1500;
   }

   struct restore_theme
   {
      ~restore_theme() { set_theme(theme{}); }
   };

   // What a user types: ASCII, accents and a combining mark, CJK, Hebrew
   // and an emoji outside the basic plane.
   char32_t const typed[] = {
      U'a', U'b', U'Z', U' ', U' ', U'.', U',', U'0', U'é', U'́',
      U'中', U'ש', U'\U0001F600', U'-', U'w'
   };

   // What a clipboard holds: plain text, line breaks, an emoji, a long run,
   // the empty string, and, in the headless host's own clipboard, invalid
   // or truncated UTF-8. A system clipboard holds text only.
   std::vector<std::string> const clips = {
      "x", "hello world", "two\nlines", "crlf\r\nend", "\xF0\x9F\x98\x80",
      std::string(300, 'q') + " tail", "", "\n", "  ",
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      "\xff\xfe broken", "cut \xF0\x9F"
#endif
   };

   int const word_mods[] = {mod_alt, mod_control};

   template <typename Box>
   struct session
   {
      session(std::shared_ptr<Box> box_, unsigned seed)
       : box{std::move(box_)}
       , rng{seed}
      {}

      int pick(int n) { return std::uniform_int_distribution<int>{0, n - 1}(rng); }
      bool chance(int percent) { return pick(100) < percent; }

      // The selection is in range, and both ends are set or neither is.
      void check_invariants()
      {
         auto size = int(box->get_text().size());
         auto s = box->select_start();
         auto e = box->select_end();
         INFO("step " << step << " after " << last << ": size " << size << " select " << s << ", " << e);
         REQUIRE(s >= -1);
         REQUIRE(e >= -1);
         REQUIRE(s <= size);
         REQUIRE(e <= size);
         REQUIRE((s == -1) == (e == -1));
         REQUIRE(to_utf32(box->get_utf8()) == std::u32string{box->get_text()});
      }

      std::shared_ptr<Box> box;
      std::mt19937 rng;
      int step = 0;
      std::string last;
   };

   mouse_button mouse(point p, bool down, int clicks, int mods)
   {
      mouse_button btn{};
      btn.down = down;
      btn.state = mouse_button::left;
      btn.num_clicks = clicks;
      btn.pos = p;
      btn.modifiers = mods;
      return btn;
   }

   // One random step that may move the caret or the selection but never
   // changes the text: arrows, Home and End, select all, copy, the mouse.
   template <typename S>
   void navigate(S& s, view& view_, point size)
   {
      static key_code const keys[] = {
         key_code::left, key_code::right, key_code::up, key_code::down,
         key_code::home, key_code::end
      };
      int const mods[] = {
         0, mod_shift, word_mods[s.pick(2)], mod_shift | word_mods[s.pick(2)],
         mod_action, mod_action | mod_shift
      };

      switch (s.pick(5))
      {
         case 0: case 1:
         {
            auto k = s.pick(6);
            auto m = mods[s.pick(6)];
            s.last = "key " + std::to_string(k) + " mods " + std::to_string(m);
            view_.key(key_info{keys[k], key_action::press, m});
            break;
         }
         case 2:
            s.last = "select all or copy";
            view_.key(key_info{s.chance(50)? key_code::a : key_code::c, key_action::press, mod_action});
            break;
         case 3:
         {
            // A click, a double or a triple click, maybe extended with Shift,
            // maybe dragged.
            point p{float(s.pick(int(size.x))), float(s.pick(int(size.y)))};
            int clicks = 1 + s.pick(3);
            int m = s.chance(20)? mod_shift : 0;
            s.last = "click " + std::to_string(clicks) + " mods " + std::to_string(m);
            view_.click(mouse(p, true, clicks, m));
            if (s.chance(40))
            {
               point q{float(s.pick(int(size.x))), float(s.pick(int(size.y)))};
               view_.drag(mouse(q, true, clicks, m));
               p = q;
            }
            view_.click(mouse(p, false, clicks, m));
            break;
         }
         default:
            // Setting the selection from code, sometimes out of range.
            s.last = "select from code";
            s.box->select_start(s.pick(int(s.box->get_text().size()) + 4) - 2);
            s.box->select_end(s.pick(int(s.box->get_text().size()) + 4) - 2);
            if ((s.box->select_start() == -1) != (s.box->select_end() == -1))
               s.box->select_none();
            break;
      }
   }

   // The model's edit, from the selection [a, b) the edit is made at.
   std::u32string replaced(std::u32string const& t, int a, int b, std::u32string_view with)
   {
      return t.substr(0, a) + std::u32string{with} + t.substr(b);
   }

   template <typename Box>
   void put_in_view(test_view& tv, std::shared_ptr<Box> e);

   // An editable text box in a scroller narrower and shorter than its text.
   template <>
   void put_in_view<basic_text_box>(test_view& tv, std::shared_ptr<basic_text_box> e)
   {
      tv.view_.content(
         margin({10, 10, 10, 10}, scroller(hold(e), no_hscroll)),
         box(rgba(0, 0, 0, 255))
      );
   }

   // An input box, framed, in a tile with a text box below it, so the
   // focus has somewhere to go.
   template <>
   void put_in_view<basic_input_box>(test_view& tv, std::shared_ptr<basic_input_box> e)
   {
      tv.view_.content(
         margin({10, 10, 10, 10},
            vtile(
               input_box(hold(e)),
               basic_text_box("below")
            )),
         box(rgba(0, 0, 0, 255))
      );
   }

   // Click the box to give it the focus.
   template <typename Box>
   void focus(test_view& tv, std::shared_ptr<Box> const& box)
   {
      for (int i = 0; i != 2 && !box->is_focus(); ++i)
      {
         tv.view_.click(mouse({20, 20}, true, 1, 0));
         tv.view_.click(mouse({20, 20}, false, 1, 0));
      }
   }

   // A model session: random edits, each checked against the model.
   template <typename Box>
   void model_session(unsigned seed, std::string const& initial)
   {
      std::shared_ptr<Box> box;
      if constexpr (std::is_same_v<Box, basic_input_box>)
      {
         box = share(basic_input_box("Type here"));
         box->set_text(initial);
      }
      else
      {
         box = share(basic_text_box(initial));
      }

      bool const input = std::is_same_v<Box, basic_input_box>;
      test_view tv{extent{240, 140}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      put_in_view(tv, box);
      tv.view_.layout();
      tv.draw();

      session<Box> s{box, seed};
      std::u32string model{box->get_text()};
      auto limit = get_theme().input_box_text_limit;
      auto const n = steps();
      auto size = point{240, 140};

      for (s.step = 0; s.step != n; ++s.step)
      {
         if (!box->is_focus() || s.chance(2))
            focus(tv, box);

         auto a0 = box->select_start();
         auto b0 = box->select_end();
         auto a = std::min(a0, b0);
         auto b = std::max(a0, b0);
         bool const live = box->is_focus() && a0 != -1;
         auto expect = model;
         auto op = s.pick(10);
         s.last = "op " + std::to_string(op);

         switch (op)
         {
            case 0: case 1: case 2:       // typing
            {
               char32_t c = typed[s.pick(int(std::size(typed)))];
               tv.view_.text(text_info{uint32_t(c), 0});
               if (live && !(input && a == b && model.size() >= limit))
                  expect = replaced(model, a, b, std::u32string(1, c));
               break;
            }
            case 3:                        // backspace or delete
            {
               bool forward = s.chance(50);
               tv.view_.key(key_info{forward? key_code::_delete : key_code::backspace, key_action::press, 0});
               if (live)
               {
                  if (a != b)
                     expect = replaced(model, a, b, U"");
                  else if (forward && a < int(model.size()))
                     expect = replaced(model, a, a + 1, U"");
                  else if (!forward && a > 0)
                     expect = replaced(model, a - 1, a, U"");
               }
               break;
            }
            case 4:                        // cut
               tv.view_.key(key_info{key_code::x, key_action::press, mod_action});
               if (live && a != b)
                  expect = replaced(model, a, b, U"");
               break;
            case 5:                        // paste
            {
               if (s.chance(40))
                  clipboard(clips[s.pick(int(clips.size()))]);
               auto clip = to_utf32(clipboard());
               tv.view_.key(key_info{key_code::v, key_action::press, mod_action});
               if (live)
               {
                  if (input)
                  {
                     if (!clip.empty())
                     {
                        auto nl = clip.find_first_of(U"\r\n");
                        if (nl != std::u32string::npos)
                           clip.resize(nl);
                        auto kept = model.size() - (b - a);
                        auto room = (kept < limit)? limit - kept : 0;
                        if (clip.size() > room)
                           clip.resize(room);
                        expect = replaced(model, a, b, clip);
                     }
                  }
                  else
                  {
                     expect = replaced(model, a, b, clip);
                  }
               }
               break;
            }
            case 6:                        // Enter
               if (!input || s.chance(10))
                  tv.view_.key(key_info{key_code::enter, key_action::press, 0});
               if (live && !input)
                  expect = replaced(model, a, b, U"\n");
               break;
            default:
               navigate(s, tv.view_, size);
               break;
         }

         if (s.chance(15))
            tv.view_.poll();
         if (s.chance(20))
         {
            tv.view_.layout();
            tv.draw();
         }

         s.check_invariants();
         INFO("seed " << seed << " step " << s.step << " op " << op
            << " select " << a0 << ", " << b0
            << " before \"" << to_utf8(model) << "\"");
         REQUIRE(to_utf8(box->get_text()) == to_utf8(expect));
         model = expect;
      }
   }

   // A history session: random edits and navigation mixed with undo and
   // redo, then undo to the start and redo to the end.
   template <typename Box>
   void history_session(unsigned seed, std::string const& initial)
   {
      std::shared_ptr<Box> box;
      if constexpr (std::is_same_v<Box, basic_input_box>)
      {
         box = share(basic_input_box("Type here"));
         box->set_text(initial);
      }
      else
      {
         box = share(basic_text_box(initial));
      }

      test_view tv{extent{240, 140}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      put_in_view(tv, box);
      tv.view_.layout();
      tv.draw();

      session<Box> s{box, seed};
      auto const n = steps();
      auto size = point{240, 140};
      auto const start = to_utf8(box->get_text());

      for (s.step = 0; s.step != n; ++s.step)
      {
         if (!box->is_focus() || s.chance(2))
            focus(tv, box);

         switch (s.pick(12))
         {
            case 0: case 1: case 2:
               tv.view_.text(text_info{uint32_t(typed[s.pick(int(std::size(typed)))]), 0});
               break;
            case 3:
               tv.view_.key(key_info{s.chance(50)? key_code::_delete : key_code::backspace, key_action::press, 0});
               break;
            case 4:
               tv.view_.key(key_info{key_code::x, key_action::press, mod_action});
               break;
            case 5:
               if (s.chance(40))
                  clipboard(clips[s.pick(int(clips.size()))]);
               tv.view_.key(key_info{key_code::v, key_action::press, mod_action});
               break;
            case 6:
               if (!std::is_same_v<Box, basic_input_box>)
                  tv.view_.key(key_info{key_code::enter, key_action::press, 0});
               break;
            case 7:
               tv.view_.key(key_info{key_code::z, key_action::press, mod_action});
               break;
            case 8:
               tv.view_.key(key_info{key_code::z, key_action::press, mod_action | mod_shift});
               break;
            default:
               navigate(s, tv.view_, size);
               break;
         }

         if (s.chance(15))
            tv.view_.poll();
         if (s.chance(20))
         {
            tv.view_.layout();
            tv.draw();
         }
         s.check_invariants();
      }

      // Undo to the start, then redo as many steps, back to the end. (The
      // redo stack may already hold steps undone in the session; those are
      // past the end.)
      auto const end = to_utf8(box->get_text());
      int undone = 0;
      while (tv.view_.undo())
      {
         ++undone;
         s.check_invariants();
      }
      INFO("seed " << seed);
      CHECK(to_utf8(box->get_text()) == start);

      for (int i = 0; i != undone; ++i)
      {
         REQUIRE(tv.view_.redo());
         s.check_invariants();
      }
      CHECK(to_utf8(box->get_text()) == end);
      tv.view_.layout();
      tv.draw();
   }
}

TEST_CASE("text box: random edits match the model", "[text_stress]")
{
   for (unsigned seed : {1u, 2u, 3u})
      model_session<basic_text_box>(seed, "The quick brown fox\njumps over\n\nthe lazy dog.");
   model_session<basic_text_box>(4u, "");
}

TEST_CASE("input box: random edits match the model", "[text_stress]")
{
   for (unsigned seed : {11u, 12u, 13u})
      model_session<basic_input_box>(seed, "Ada Lovelace");
   model_session<basic_input_box>(14u, "");
}

TEST_CASE("input box: random edits at a small text limit", "[text_stress]")
{
   restore_theme guard;
   theme t;
   t.input_box_text_limit = 12;
   set_theme(t);
   for (unsigned seed : {21u, 22u})
      model_session<basic_input_box>(seed, "abc");
}

TEST_CASE("text box: undo and redo across a random session", "[text_stress]")
{
   for (unsigned seed : {31u, 32u, 33u})
      history_session<basic_text_box>(seed, "Some text to edit,\nover two lines.");
}

TEST_CASE("input box: undo and redo across a random session", "[text_stress]")
{
   for (unsigned seed : {41u, 42u})
      history_session<basic_input_box>(seed, "start");
}

TEST_CASE("two text boxes share the view's undo history", "[text_stress]")
{
   // Edits in two boxes, the focus moving between them, typing left in
   // progress in one while the other is edited. Undoing every step must
   // give back both starting texts, and redoing every step both final ones.
   for (unsigned seed : {51u, 52u, 53u})
   {
      auto top = share(basic_text_box("top text"));
      auto bottom = share(basic_text_box("bottom text"));
      test_view tv{extent{240, 140}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
      tv.view_.content(
         margin({10, 10, 10, 10}, vtile(vsize(50, hold(top)), vsize(50, hold(bottom)))),
         box(rgba(0, 0, 0, 255))
      );
      tv.view_.layout();
      tv.draw();

      std::mt19937 rng{seed};
      auto pick = [&](int n) { return std::uniform_int_distribution<int>{0, n - 1}(rng); };
      auto click_on = [&](float y)
      {
         tv.view_.click(mouse({20, y}, true, 1, 0));
         tv.view_.click(mouse({20, y}, false, 1, 0));
      };

      for (int step = 0; step != steps() / 5; ++step)
      {
         switch (pick(8))
         {
            case 0: click_on(20); break;
            case 1: click_on(75); break;
            case 2: case 3: case 4:
               tv.view_.text(text_info{uint32_t(typed[pick(int(std::size(typed)))]), 0});
               break;
            case 5:
               tv.view_.key(key_info{key_code::backspace, key_action::press, 0});
               break;
            case 6:
               tv.view_.key(key_info{key_code::z, key_action::press, mod_action});
               break;
            default:
               tv.view_.key(key_info{key_code::z, key_action::press, mod_action | mod_shift});
               break;
         }
      }

      auto const top_end = top->get_utf8();
      auto const bottom_end = bottom->get_utf8();

      int undone = 0;
      while (tv.view_.undo())
         ++undone;
      INFO("seed " << seed);
      CHECK(top->get_utf8() == "top text");
      CHECK(bottom->get_utf8() == "bottom text");

      for (int i = 0; i != undone; ++i)
         REQUIRE(tv.view_.redo());
      CHECK(top->get_utf8() == top_end);
      CHECK(bottom->get_utf8() == bottom_end);
   }
}

TEST_CASE("text box: long text", "[text_stress]")
{
   // A text of many paragraphs and one very long line, edited at the
   // start, the middle and the end.
   std::string text;
   for (int i = 0; i != 400; ++i)
      text += "Line " + std::to_string(i) + " of a long text.\n";
   text += std::string(5000, 'x');

   auto box = share(basic_text_box(text));
   test_view tv{extent{240, 140}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   put_in_view(tv, box);
   tv.view_.layout();
   tv.draw();
   focus(tv, box);
   REQUIRE(box->is_focus());
   bool const clip = clipboard_works();     // the cut is pasted back

   auto size = int(box->get_text().size());
   for (int pos : {0, size / 2, size})
   {
      box->select_start(pos);
      box->select_end(pos);
      tv.view_.text(text_info{U'#', 0});
      tv.view_.key(key_info{key_code::down, key_action::press, 0});
      tv.view_.key(key_info{key_code::end, key_action::press, mod_shift});
      tv.view_.key(key_info{key_code::x, key_action::press, mod_action});
      tv.view_.key(key_info{key_code::v, key_action::press, mod_action});
      tv.view_.layout();
      tv.draw();
   }
   if (clip)
      CHECK(int(box->get_text().size()) == size + 3);

   box->select_all();
   tv.view_.key(key_info{key_code::backspace, key_action::press, 0});
   CHECK(box->get_text().empty());
   tv.view_.layout();
   tv.draw();
}

TEST_CASE("text box: characters outside Unicode", "[text_stress]")
{
   // A host may hand over a UTF-16 surrogate half, as Windows does for a
   // character outside the basic plane, or a value past U+10FFFF. Neither
   // is a character; the box must stay sound.
   auto box = share(basic_text_box("ab"));
   test_view tv{extent{240, 140}, 1};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   put_in_view(tv, box);
   tv.view_.layout();
   tv.draw();
   focus(tv, box);
   REQUIRE(box->is_focus());

   for (uint32_t cp : {0xD83Du, 0xDE00u, 0x110000u, 0xFFFFFFFFu, 0xDE00u, 0xD83Du})
   {
      INFO("codepoint " << cp);
      CHECK_NOTHROW(tv.view_.text(text_info{cp, 0}));
      CHECK(to_utf32(box->get_utf8()) == std::u32string{box->get_text()});
   }
   tv.view_.layout();
   tv.draw();

   // The halves of a pair, one after the other, are one character; halves
   // without a partner, and values past U+10FFFF, are dropped.
   std::u32string t{box->get_text()};
   CHECK(t.size() == 3);
   CHECK(t.find(U'\U0001F600') != std::u32string::npos);
}
