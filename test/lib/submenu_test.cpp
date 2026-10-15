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
#include <random>
#include <string>
#include <thread>
#include <vector>

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

TEST_CASE("submenu: Left closes a submenu and keeps its item", "[submenu]")
{
   fixture f;
   auto& v = f.tv.view_;
   auto const base = f.layers();
   auto press = [&](key_code k)
   {
      v.key(key_info{k, key_action::press, 0});
      wait(v);
   };

   click(v, center(bounds_of(v, *f.file)));
   REQUIRE(f.layers() == base + 1);

   press(key_code::down);           // Open
   press(key_code::down);           // Recent
   INFO("open " << f.open_->is_selected() << " recent " << f.recent->is_selected());
   REQUIRE(f.recent->is_selected());

   press(key_code::right);          // Recent's submenu
   CHECK(f.layers() == base + 2);

   press(key_code::left);           // back to File's menu, on Recent
   CHECK(f.layers() == base + 1);
   CHECK(f.recent->is_selected());

   press(key_code::up);             // the keys still reach File's menu
   CHECK(f.open_->is_selected());
   CHECK(!f.recent->is_selected());

   press(key_code::left);           // the bar's previous menu: Help has
   CHECK(f.layers() == base + 1);   // none, so Edit's, wrapping around
   CHECK(f.edit->is_selected());
   CHECK(!f.file->is_selected());

   press(key_code::down);           // the keys reach Edit's menu
   CHECK(f.copy->is_selected());
}

TEST_CASE("submenu: Escape closes one menu, Enter picks", "[submenu]")
{
   fixture f;
   auto& v = f.tv.view_;
   auto const base = f.layers();
   auto press = [&](key_code k)
   {
      v.key(key_info{k, key_action::press, 0});
      wait(v);
   };

   click(v, center(bounds_of(v, *f.file)));
   press(key_code::down);           // Open
   press(key_code::down);           // Recent

   press(key_code::enter);          // Enter on Recent opens its submenu
   CHECK(f.layers() == base + 2);

   press(key_code::escape);         // closes the submenu alone
   CHECK(f.layers() == base + 1);
   CHECK(f.recent->is_selected());

   press(key_code::right);          // into the submenu again
   press(key_code::down);           // One
   press(key_code::enter);          // picks it, closing every menu
   CHECK(f.chose_one == 1);
   CHECK(f.layers() == base);
   CHECK(!f.file->is_selected());
}

namespace
{
   // A bar with menus two levels deep and a disabled item. File: New,
   // Disabled, Recent (Alpha, Deep (Beta, Gamma)), Save. Edit: Cut, Copy.
   // Help is a plain item.
   struct deep_fixture
   {
      deep_fixture()
       : tv{extent{500, 400}}
      {
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
         headless::open(tv.view_);
#endif
         for (auto* p : {&file, &edit, &help, &new_, &disabled, &recent,
            &alpha, &deep, &beta, &gamma, &save, &cut, &copy})
            *p = share(menu_item("Item"));
         bar_g    = {file, edit, help};
         file_g   = {new_, disabled, recent, save};
         recent_g = {alpha, deep};
         deep_g   = {beta, gamma};
         edit_g   = {cut, copy};
         disabled->is_enabled = []{ return false; };
         for (auto& i : {new_, save, alpha, beta, gamma, cut, copy, help})
            i->on_click = [this]{ ++picks; };

         auto menu = [](auto... items)
         {
            return hsize(140, layer(vtile(hold(items)...), panel{}));
         };
         deep->menu(menu(beta, gamma));
         recent->menu(menu(alpha, deep));
         file->menu(menu(new_, disabled, recent, save));
         edit->menu(menu(cut, copy));

         auto bar =
            htile(hmin(hold(file)), hmin(hold(edit)), hmin(hold(help)));
         tv.view_.content(align_left_top(bar), box(colors::black));
         tv.view_.layout();
         tv.draw();
         base = tv.view_.layers().size();
      }

      using group = std::vector<item_ptr>;

      int open() const
      {
         return int(tv.view_.layers().size()) - int(base);
      }

      static int selected(group const& g)
      {
         int n = 0;
         for (auto& i : g)
            n += i->is_selected();
         return n;
      }

      // The menu the keys go to: the deepest one open.
      group const* top() const
      {
         switch (open())
         {
            case 1:  return file->is_selected()? &file_g : &edit_g;
            case 2:  return &recent_g;
            case 3:  return &deep_g;
            default: return nullptr;
         }
      }

      mutable test_view tv;
      item_ptr file, edit, help, new_, disabled, recent, alpha, deep;
      item_ptr beta, gamma, save, cut, copy;
      group bar_g, file_g, recent_g, deep_g, edit_g;
      std::size_t base = 0;
      int picks = 0;
   };

   void settle(view& v)
   {
      for (int i = 0; i != 3; ++i)
         v.poll();
   }
}

TEST_CASE("submenu: keys open a submenu on its first item", "[submenu]")
{
   deep_fixture f;
   auto& v = f.tv.view_;
   auto press = [&](key_code k)
   {
      v.key(key_info{k, key_action::press, 0});
      settle(v);
   };

   click(v, center(bounds_of(v, *f.file)));
   press(key_code::down);           // New
   press(key_code::down);           // Recent: Disabled is skipped
   REQUIRE(f.recent->is_selected());

   press(key_code::right);
   CHECK(f.open() == 2);
   CHECK(f.alpha->is_selected());

   press(key_code::down);           // Deep
   press(key_code::enter);          // Enter opens it, on Beta
   CHECK(f.open() == 3);
   CHECK(f.beta->is_selected());

   press(key_code::escape);
   press(key_code::left);           // back to File's menu, on Recent
   CHECK(f.open() == 1);
   CHECK(f.recent->is_selected());

   press(key_code::right);          // Recent again: no stale selection
   CHECK(f.alpha->is_selected());
   CHECK(!f.deep->is_selected());
}

TEST_CASE("submenu: random keys keep the menus consistent", "[submenu]")
{
   key_code const keys[] = {
      key_code::up, key_code::down, key_code::left, key_code::right,
      key_code::enter, key_code::escape
   };
   char const* const names[] = {"Up", "Down", "Left", "Right", "Enter", "Esc"};

   for (unsigned seed : {1u, 2u, 3u, 4u})
   {
      deep_fixture f;
      auto& v = f.tv.view_;
      std::mt19937 rng{seed};
      std::string log;

      click(v, center(bounds_of(v, *f.file)));
      for (int step = 0; step != 400; ++step)
      {
         auto const ki = rng() % 6;
         auto const before_open = f.open();
         auto const* before_top = f.top();
         bool const leaf_pick = keys[ki] == key_code::enter && before_top
            && f.selected(*before_top) == 1
            && (*before_top == f.edit_g
               || (f.selected(f.file_g) == 1 && !f.recent->is_selected())
               || before_top == &f.deep_g
               || (before_top == &f.recent_g && f.alpha->is_selected()));
         auto const picks_before = f.picks;

         v.key(key_info{keys[ki], key_action::press, 0});
         settle(v);
         log += names[ki];
         log += ' ';

         INFO("seed " << seed << ", step " << step << ": " << log);

         // At most one item selected in each menu.
         REQUIRE(f.selected(f.bar_g) <= 1);
         REQUIRE(f.selected(f.file_g) <= 1);
         REQUIRE(f.selected(f.recent_g) <= 1);
         REQUIRE(f.selected(f.deep_g) <= 1);
         REQUIRE(f.selected(f.edit_g) <= 1);

         // The menus open are a chain from one bar title.
         auto const open = f.open();
         REQUIRE(open >= 0);
         REQUIRE(open <= 3);
         if (open > 0)
            REQUIRE((f.file->is_selected() || f.edit->is_selected()));
         if (open >= 2)
            REQUIRE((f.file->is_selected() && f.recent->is_selected()));
         if (open == 3)
            REQUIRE(f.deep->is_selected());

         // A submenu the keys open starts on its first item; a menu the
         // keys move to across the bar starts on none, as a click does.
         if (open > before_open && before_open > 0)
            REQUIRE(f.selected(*f.top()) == 1);
         else if (open == before_open && open == 1 && f.top() != before_top)
            REQUIRE(f.selected(*f.top()) == 0);

         // Enter on a plain item picks it once and closes every menu.
         if (leaf_pick)
         {
            REQUIRE(f.picks == picks_before + 1);
            REQUIRE(open == 0);
         }

         // The keys reach the menu on top: Down selects in it.
         if (open > 0 && step % 20 == 0)
         {
            v.key(key_info{key_code::down, key_action::press, 0});
            settle(v);
            log += "(Down) ";
            REQUIRE(f.selected(*f.top()) == 1);
         }

         // Closed, start again.
         if (open == 0 && step % 7 == 0)
         {
            click(v, center(bounds_of(v, *f.file)));
            log += "(File) ";
         }
      }

      // Escape, enough times, closes them all.
      for (int i = 0; i != 4; ++i)
      {
         v.key(key_info{key_code::escape, key_action::press, 0});
         settle(v);
      }
      CHECK(f.open() == 0);
   }
}
