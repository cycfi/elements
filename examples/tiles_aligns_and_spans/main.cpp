#include <elemental.hpp>

using namespace cycfi::elemental;

// Main window background color
auto bkd_color = rgba(62, 62, 62, 255);

struct background : element
{
   void draw(context const& ctx)
   {
      auto&  cnv = ctx.canvas;
      cnv.fill_style(bkd_color);
      cnv.fill_rect(ctx.bounds);
   }
};

auto box = min_size({ 5, 5 },
   basic(
      [](context const& ctx)
      {
         auto& c = ctx.canvas;

         c.begin_path();
         c.round_rect(ctx.bounds, 4);
         c.fill_style(colors::gold.opacity(0.8));
         c.fill();
      }
   )
);

auto make_vtile()
{
   auto _box = top_margin(
      { 10 },
      hsize(150, box)
   );

   return margin(
      { 10, 50, 10, 10 },
      hmin_size(150,
         vtile(
            halign(0.0, _box),
            halign(0.2, _box),
            halign(0.4, _box),
            halign(0.6, _box),
            halign(0.8, _box),
            halign(1.0, _box)
         )
      )
   );
}

auto make_vtile2()
{
   auto _box = top_margin(
      { 10 },
      box
   );

   return margin(
      { 10, 50, 10, 10 },
      hmin_size(150,
         vtile(
            vstretch(1.0, _box),
            vstretch(0.5, _box),
            vstretch(0.5, _box),
            vstretch(0.5, _box),
            vstretch(2.0, _box)
         )
      )
   );
}

auto make_vtile3()
{
   auto _box = top_margin(
      { 10 },
      box
   );

   return margin(
      { 10, 50, 10, 10 },
      hmin_size(150,
         vtile(
            vsize(40.0, _box),
            vstretch(2.0, _box),
            vstretch(1.0, _box),
            vstretch(0.5, vmin_size(20, _box)),
            vsize(40.0, _box)
         )
      )
   );
}

auto make_vtile_main()
{
   return htile(
      margin({ 10, 10, 10, 10 }, group("Aligns", make_vtile(), 0.9, false)),
      margin({ 10, 10, 10, 10 }, group("Percentages", make_vtile2(), 0.9, false)),
      margin({ 10, 10, 10, 10 }, group("Mixed", make_vtile3(), 0.9, false))
   );
}

auto make_htile()
{
   auto _box = left_margin(
      { 10 },
      vsize(100, box)
   );

   return margin(
      { 0, 50, 10, 10 },
      htile(
         valign(0.0, _box),
         valign(0.2, _box),
         valign(0.4, _box),
         valign(0.6, _box),
         valign(0.8, _box),
         valign(1.0, _box)
      )
   );
}

auto make_htile2()
{
   auto _box = left_margin(
      { 10 },
      box
   );

   return margin(
      { 0, 50, 10, 10 },
      htile(
         hstretch(1.0, _box),
         hstretch(0.5, _box),
         hstretch(0.5, _box),
         hstretch(0.5, _box),
         hstretch(2.0, _box)
      )
   );
}

auto make_htile3()
{
   auto _box = left_margin(
      { 10 },
      box
   );

   return margin(
      { 0, 50, 10, 10 },
      htile(
         hsize(40.0, _box),
         hstretch(2.0, _box),
         hstretch(1.0, _box),
         hstretch(0.5, hmin_size(20, _box)),
         hsize(40.0, _box)
      )
   );
}

auto make_htile_main()
{
   return htile(
      margin({ 10, 10, 10, 10 }, group("Aligns", make_htile(), 0.9, false)),
      margin({ 10, 10, 10, 10 }, group("Stretch", make_htile2(), 0.9, false)),
      margin({ 10, 10, 10, 10 }, group("Mixed", make_htile3(), 0.9, false))
   );
}

template <typename MenuItem>
auto make_popup_menu(MenuItem& hmenu, MenuItem& vmenu)
{
   auto popup  = dropdown_menu("Orientation");

   auto menu =
      layer(
         vtile(link(hmenu), link(vmenu)),
         menu_background{}
      );

   popup.menu(menu);

   return popup;
}

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name(), window::standard, { 50, 50, 1024, 768 });

   auto s = _win.size();
   auto p = _win.position();
   _win.on_close = [&_app]() { _app.stop(); };

   auto hmenu = menu_item("Horizontal");
   auto vmenu = menu_item("Vertical");

   auto content = deck(
      make_vtile_main(),   // vertical tiles aligns and stretch
      make_htile_main()    // horizontal tiles aligns and stretch
   );

   view view_(_win);

   hmenu.on_click = [&]()
   {
      content.select(1);
      view_.refresh(content);
   };

   vmenu.on_click = [&]()
   {
      content.select(0);
      view_.refresh(content);
   };

   auto top = align_right(hsize(120, make_popup_menu(hmenu, vmenu)));
   auto main_pane = pane(top, link(content), false);
   auto main_element = margin({ 10, 10, 10, 10 }, main_pane);

   view_.content(
      {
         share(background{}),
         share(main_element)
      }
   );

   _app.run();
   return 0;
}
