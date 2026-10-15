/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/elements/drag_and_drop.adoc
   and draws its figures. The figures land in the build's results
   directory; copy them to docs/modules/ROOT/images/elements/ to update the
   page.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using namespace std::chrono_literals;

namespace
{
   auto constexpr bkd_color = rgba(35, 35, 37, 255);

   void open(test_view& tv)
   {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
      headless::open(tv.view_);
#endif
   }

   void wait(view& v, std::chrono::milliseconds t = 1ms)
   {
      std::this_thread::sleep_for(t);
      v.poll();
   }

   mouse_button left(point p, bool down)
   {
      mouse_button b{};
      b.state = mouse_button::left;
      b.down = down;
      b.num_clicks = 1;
      b.pos = p;
      return b;
   }

   void click(view& v, point p)
   {
      v.click(left(p, true));
      v.click(left(p, false));
   }

   auto arranger(std::vector<std::string>& items)
   {
      auto cp = basic_vcell_composer(items.size(),
         [&](std::size_t i)
         {
            return share(draggable(align_left(label(items[i]))));
         });
      return share(drop_inserter(hold(share(vlist(cp, false))), {"text/uri-list"}));
   }
}

TEST_CASE("drag and drop page: rearranging", "[drag_and_drop_page]")
{
   SECTION("destroyed with its drag image step posted")
   {
      std::vector<std::string> items = {"a", "b", "c", "d"};
      test_view tv{extent{200, 200}};
      open(tv);
      {
         auto di = arranger(items);
         tv.view_.content(align_left_top(hold(di)), box(bkd_color));
         tv.view_.layout();
         tv.draw();
         click(tv.view_, {5, 5});                 // select a
         wait(tv.view_);
         tv.view_.click(left({5, 5}, true));       // press on it: posts
         tv.view_.content(box(bkd_color));         // the view lets go
      }
      wait(tv.view_, 300ms);
      CHECK(true);
   }
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   // The insertion point follows the cursor, which only the headless host
   // lets a test place.
   SECTION("selection after a move to its own place")
   {
      std::vector<std::string> items = {"a", "b", "c", "d", "e"};
      test_view tv{extent{200, 200}};
      open(tv);
      auto di = arranger(items);
      di->on_move = [&](std::size_t pos, std::vector<std::size_t> const& ix) { move_indices(items, pos, ix); };
      tv.view_.content(align_left_top(hold(di)), box(bkd_color));
      tv.view_.layout();
      tv.draw();
      auto s = find_subject<selection_list_element*>(di.get());
      REQUIRE(s);
      auto c = find_subject<composite_base*>(s);
      REQUIRE(c);
      context ctx{tv.view_, tv.cnv, c, rect{0, 0, 200, 200}};
      auto row = [&](std::size_t i) { return c->bounds_of(ctx, i); };

      // Select b and c, then drag them by c to the top half of b: back
      // where they were.
      s->set_selection({1, 2});
      s->update_selection(1, 2);
      auto from = center_point(row(2));
      auto to = point{from.x, row(1).top + 2};
      tv.view_.click(left(from, true));
      wait(tv.view_, 300ms);
      headless::cursor_pos(tv.view_, to);
      tv.view_.drag(left(to, true));
      wait(tv.view_);
      tv.draw();
      REQUIRE(di->insertion_pos() == 1);
      tv.view_.click(left(to, false));
      wait(tv.view_);
      tv.draw();

      CHECK(items == std::vector<std::string>{"a", "b", "c", "d", "e"});
      CHECK(s->get_selection() == std::vector<std::size_t>{1, 2});
      CHECK(s->get_select_start() == 1);
      CHECK(s->get_select_end() == 2);
      CHECK(di->insertion_pos() == -1);
   }
#endif
}

TEST_CASE("drag and drop page: drop boxes", "[drag_and_drop_page]")
{
   test_view tv{extent{200, 200}};
   open(tv);
   int dropped = 0;
   auto db = share(drop_box(box(colors::black), {"text/uri-list"}));
   db->on_drop = [&](drop_info const& info)
   {
      ++dropped;
      return contains_filepaths(info.data);
   };
   tv.view_.content(hold(db));
   tv.view_.layout();
   tv.draw();

   // The types it takes, for the host.
   CHECK(db->drop_types() == mime_types{"text/uri-list"});

   payload files{{"text/uri-list", std::string{"file:///tmp/a.jpg"}}};
   payload text{{"text/plain", std::string{"hello"}}};

   // A drag of a type it takes: it is tracking, and takes the drop.
   tv.view_.track_drop({files, {100, 100}}, cursor_tracking::entering);
   CHECK(db->is_tracking());
   CHECK(tv.view_.drop({files, {100, 100}}));
   CHECK(dropped == 1);
   CHECK(!db->is_tracking());

   // Of a type it does not: on_drop is not called.
   tv.view_.track_drop({text, {100, 100}}, cursor_tracking::entering);
   CHECK(!db->is_tracking());
   CHECK(!tv.view_.drop({text, {100, 100}}));
   CHECK(dropped == 1);
}

TEST_CASE("drag and drop page: inserter", "[drag_and_drop_page]")
{
   SECTION("delete erases the selection")
   {
      std::vector<std::string> items = {"a", "b", "c", "d"};
      test_view tv{extent{200, 200}};
      open(tv);
      auto di = arranger(items);
      std::vector<std::size_t> erased;
      di->on_erase = [&](std::vector<std::size_t> const& ix)
      {
         erased = ix;
         erase_indices(items, ix);
      };
      tv.view_.content(align_left_top(hold(di)), box(bkd_color));
      tv.view_.layout();
      tv.draw();

      // Click a, shift-click c: a to c are selected.
      std::vector<std::size_t> selected;
      di->on_select = [&](std::vector<std::size_t> const& ix, std::size_t) { selected = ix; };
      click(tv.view_, {5, 5});
      auto c_at = point{5, 5 + 2 * (find_subject<composite_base*>(di.get())->bounds_of(
         context{tv.view_, tv.cnv, di.get(), rect{0, 0, 200, 200}}, 0).height())};
      auto shift = left(c_at, true);
      shift.modifiers = mod_shift;
      tv.view_.click(shift);
      shift.down = false;
      tv.view_.click(shift);
      wait(tv.view_);
      CHECK(selected == std::vector<std::size_t>{0, 1, 2});

      CHECK(tv.view_.key(key_info{key_code::_delete, key_action::press, 0}));
      tv.draw();
      CHECK(erased == std::vector<std::size_t>{0, 1, 2});
      CHECK(items == std::vector<std::string>{"d"});
   }
}

#if !defined(ARTIST_RECORDING)
TEST_CASE("drag and drop page: figures", "[drag_and_drop_page]")
{
   {
      // examples/drop_file, with a file dragged over it.
      test_view tv{extent{480, 320}, 2};
      open(tv);
      auto image_    = share(image{std::string{EXAMPLES_PATH} + "drop_file/resources/space.jpg"});
      auto drop_box_ = share(drop_box(scroller(hold(image_)), {"text/uri-list"}));
      tv.view_.content(
         align_center_middle(label("Drop a picture here").font_size(20)),
         margin({20, 20, 20, 20}, hold(drop_box_)),
         box(bkd_color)
      );
      tv.view_.layout();
      tv.draw();
      payload files{{"text/uri-list", std::string{"file:///tmp/a.jpg"}}};
      tv.view_.track_drop({files, {240, 160}}, cursor_tracking::entering);
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + "drag_and_drop_box.png");
   }
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   {
      // examples/list_arranger, two items dragged.
      std::vector<std::string> items = {
         "/home/user/documents/quantum_energy_matrix_42.txt",
         "/var/www/html/interdimensional_portal_manifest.html",
         "/usr/bin/elixir_of_eternal_life.exe",
         "/mnt/data/enigmatic_astral_code_vortex.jpg",
         "/opt/software/ancient_relics_archeology.ini",
         "/tmp/temp/mystic_scroll_of_knowledge.tmp",
         "/home/user/documents/hyperdimensional_cosmic_key_99.txt",
         "/var/log/transcendental_being_encounter.log",
         "/usr/lib/quantum_realm_gateway.so",
         "/mnt/data/sacred_harmonic_resonance_music.mp3"
      };
      test_view tv{extent{480, 300}, 2};
      open(tv);
      auto di = arranger(items);
      tv.view_.content(
         margin({10, 10, 15, 10}, align_left_top(hold(di))),
         box(bkd_color)
      );
      tv.view_.layout();
      tv.draw();
      auto s = find_subject<selection_list_element*>(di.get());
      auto c = find_subject<composite_base*>(s);
      context ctx{tv.view_, tv.cnv, c, rect{10, 10, 470, 290}};
      auto row = [&](std::size_t i) { return c->bounds_of(ctx, i); };
      s->set_selection({1, 2});
      s->update_selection(1, 2);
      auto from = point{60, (row(2).top + row(2).bottom) / 2};
      auto to = point{140, row(9).bottom - 2};
      tv.view_.click(left(from, true));
      wait(tv.view_, 300ms);
      headless::cursor_pos(tv.view_, to);
      tv.view_.drag(left(to, true));
      wait(tv.view_);
      tv.view_.layout();
      tv.draw();
      tv.img.save_png(std::string{RESULTS_PATH} + "drag_and_drop_list.png");
      // Escape ends the drag with no move.
      auto before = items;
      tv.view_.key(key_info{key_code::escape, key_action::press, 0});
      wait(tv.view_);
      tv.view_.click(left(to, false));
      wait(tv.view_);
      tv.draw();
      CHECK(items == before);
      CHECK(di->insertion_pos() == -1);
   }
#endif
}
#endif
