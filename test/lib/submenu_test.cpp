/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Submenus: a menu item with a menu of its own. In a row of items, a menu
   bar, the menu opens below the item; in a menu, to its right. While one is
   open, pointing at another item beside it switches to that item's menu.
   Choosing an item closes every menu it was reached through.
=============================================================================*/
#include "test_support.hpp"
#include <chrono>
#include <memory>
#include <thread>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
using namespace std::chrono_literals;

namespace
{
   void wait(view& v)
   {
      for (int i = 0; i != 3; ++i)
      {
         std::this_thread::sleep_for(1ms);
         v.poll();
      }
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
      wait(v);
   }

   void point_at(view& v, point p)
   {
      v.cursor(p, cursor_tracking::hovering);
      wait(v);
   }

   rect bounds_of(view& v, element& e)
   {
      rect r;
      v.in_context_do(e, [&](context const& ctx) { r = ctx.bounds; });
      return r;
   }

   point center(rect r)
   {
      return {(r.left + r.right) / 2, (r.top + r.bottom) / 2};
   }

   using item_ptr = std::shared_ptr<basic_menu_item_element>;

   // A bar of File, Edit and Help. File has Open (with a shortcut) and
   // Recent, whose submenu has One; Edit has Copy; Help is a plain item.
   struct fixture
   {
      fixture()
       : tv{extent{400, 300}}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(tv.view_);
#endif
         file = share(menu_item("File"));
         edit = share(menu_item("Edit"));
         help = share(menu_item("Help"));
         auto const sko = shortcut_key{key_code::o, mod_action};
         open_ = share(menu_item("Open", sko));
         recent = share(menu_item("Recent"));
         one = share(menu_item("One"));
         copy = share(menu_item("Copy"));

         open_->on_click = [this]{ ++opened; };
         one->on_click = [this]{ ++chose_one; };
         help->on_click = [this]{ ++helped; };

         auto file_menu = vtile(hold(open_), hold(recent));
         recent->menu(hsize(120, layer(vtile(hold(one)), panel{})));
         file->menu(hsize(150, layer(file_menu, panel{})));
         edit->menu(hsize(150, layer(vtile(hold(copy)), panel{})));

         auto bar =
            htile(hmin(hold(file)), hmin(hold(edit)), hmin(hold(help)));
         tv.view_.content(align_left_top(bar), box(colors::black));
         tv.view_.layout();
         tv.draw();
      }

      std::size_t layers() { return tv.view_.layers().size(); }
      rect top_layer()
      {
         auto top = std::dynamic_pointer_cast<basic_popup_element>(
            tv.view_.layers().back());
         return top? top->bounds() : rect{};
      }

      test_view tv;
      item_ptr file, edit, help, open_, recent, one, copy;
      int opened = 0, chose_one = 0, helped = 0;
   };

#if defined(__APPLE__)
   constexpr int action_mods = mod_action | mod_command;
#else
   constexpr int action_mods = mod_action | mod_control;
#endif
}

TEST_CASE("submenu: a bar item opens its menu below it", "[submenu]")
{
   fixture f;
   auto const base = f.layers();
   auto const fb = bounds_of(f.tv.view_, *f.file);

   click(f.tv.view_, center(fb));
   REQUIRE(f.layers() == base + 1);
   CHECK(f.file->is_selected());
   auto const m = f.top_layer();
   CHECK(m.top == fb.bottom);
   CHECK(m.left == fb.left);

   // Pressed again, the title closes its menu.
   click(f.tv.view_, center(fb));
   CHECK(f.layers() == base);
   CHECK(!f.file->is_selected());
}

TEST_CASE("submenu: pointing at another title switches menus", "[submenu]")
{
   fixture f;
   auto const base = f.layers();
   auto const fb = bounds_of(f.tv.view_, *f.file);
   auto const eb = bounds_of(f.tv.view_, *f.edit);

   click(f.tv.view_, center(fb));
   REQUIRE(f.layers() == base + 1);

   point_at(f.tv.view_, center(eb));
   CHECK(f.layers() == base + 1);
   CHECK(f.edit->is_selected());
   CHECK(!f.file->is_selected());
   CHECK(f.top_layer().left == eb.left);

   // A plain item in the bar does not take the open menu's place.
   point_at(f.tv.view_, center(bounds_of(f.tv.view_, *f.help)));
   CHECK(f.layers() == base + 1);
   CHECK(f.edit->is_selected());
}

TEST_CASE("submenu: in a menu, a submenu opens to the right", "[submenu]")
{
   fixture f;
   auto const base = f.layers();
   click(f.tv.view_, center(bounds_of(f.tv.view_, *f.file)));
   REQUIRE(f.layers() == base + 1);

   auto const rb = bounds_of(f.tv.view_, *f.recent);
   point_at(f.tv.view_, center(rb));
   REQUIRE(f.layers() == base + 2);
   CHECK(f.recent->is_selected());
   auto const sub = f.top_layer();
   CHECK(sub.left == rb.right);
   CHECK(sub.top == rb.top);

   // Back on a plain item of the menu, the submenu closes.
   point_at(f.tv.view_, center(bounds_of(f.tv.view_, *f.open_)));
   CHECK(f.layers() == base + 1);
   CHECK(!f.recent->is_selected());
}

TEST_CASE("submenu: choosing an item closes every menu", "[submenu]")
{
   fixture f;
   auto const base = f.layers();
   click(f.tv.view_, center(bounds_of(f.tv.view_, *f.file)));
   point_at(f.tv.view_, center(bounds_of(f.tv.view_, *f.recent)));
   REQUIRE(f.layers() == base + 2);

   click(f.tv.view_, center(bounds_of(f.tv.view_, *f.one)));
   CHECK(f.chose_one == 1);
   CHECK(f.layers() == base);
   CHECK(!f.file->is_selected());
   CHECK(!f.recent->is_selected());
}

TEST_CASE("submenu: a plain bar item acts once per click", "[submenu]")
{
   fixture f;
   click(f.tv.view_, center(bounds_of(f.tv.view_, *f.help)));
   CHECK(f.helped == 1);
}

TEST_CASE("submenu: shortcuts work with the menus closed", "[submenu]")
{
   fixture f;
   auto const k = key_info{key_code::o, key_action::press, action_mods};
   CHECK(f.tv.view_.key(k));
   CHECK(f.opened == 1);
}

TEST_CASE("submenu: Left and Right move across the bar", "[submenu]")
{
   fixture f;
   auto const base = f.layers();
   click(f.tv.view_, center(bounds_of(f.tv.view_, *f.file)));
   REQUIRE(f.layers() == base + 1);

   // Into the menu, then to the next menu in the bar, and back, wrapping.
   f.tv.view_.key(key_info{key_code::down, key_action::press, 0});
   f.tv.view_.key(key_info{key_code::right, key_action::press, 0});
   wait(f.tv.view_);
   CHECK(f.edit->is_selected());
   CHECK(!f.file->is_selected());
   CHECK(f.layers() == base + 1);

   f.tv.view_.key(key_info{key_code::down, key_action::press, 0});
   f.tv.view_.key(key_info{key_code::right, key_action::press, 0});
   wait(f.tv.view_);
   CHECK(f.file->is_selected());
   CHECK(f.layers() == base + 1);
}
