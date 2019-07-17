#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto bkd_color = rgba(62, 62, 62, 255);
auto background = box(bkd_color);

auto rbox = min_size({ 5, 5 },
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

auto make_vtile_aligns()
{
   auto _box = top_margin(
      { 10 },
      hsize(150, rbox)
   );

   return margin(
      { 10, 40, 10, 10 },
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

auto make_vtile_stretch()
{
   auto _box = top_margin(
      { 10 },
      rbox
   );

   return margin(
      { 10, 40, 10, 10 },
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

auto make_vtile_mixed()
{
   auto _box = top_margin(
      { 10 },
      rbox
   );

   auto _box2 = top_margin(
      { 10 },
      hsize(150, rbox)
   );

   return margin(
      { 10, 40, 10, 10 },
      hmin_size(150,
         vtile(
            halign(0.0, vsize(40.0, _box2)),
            vstretch(2.0, _box),
            vstretch(1.0, _box),
            vstretch(0.5, vmin_size(20, _box)),
            halign(1.0, vsize(40.0, _box2))
         )
      )
   );
}

auto make_htile_aligns()
{
   auto _box = left_margin(
      { 10 },
      vsize(150, rbox)
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

auto make_htile_stretch()
{
   auto _box = left_margin(
      { 10 },
      rbox
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

auto make_htile_mixed()
{
   auto _box = left_margin(
      { 10 },
      rbox
   );

   auto _box2 = left_margin(
      { 10 },
      vsize(150, rbox)
   );

   return margin(
      { 0, 50, 10, 10 },
      htile(
         valign(0.0, hsize(40.0, _box2)),
         hstretch(2.0, _box),
         hstretch(1.0, _box),
         hstretch(0.5, hmin_size(20, _box)),
         valign(1.0, hsize(40.0, _box2))
      )
   );
}

auto make_flow()
{
   static auto c = vector_composite<flowable_container>{};
   for (int i = 0; i < 50; ++i)
   {
      auto w = 10 + ((double(std::rand()) * 90) / RAND_MAX);
      auto _box = margin({ 5, 5, 5, 5 }, fixed_size({ float(w), 20 }, rbox));
      c.push_back(share(_box));
   }

   auto flow_pane = margin(
      { 0, 50, 10, 10 },
      align_top(flow(c))
   );

   return margin({ 10, 10, 10, 10 },
      group("VTile with Fixed-Sized, Aligned Elements", flow_pane, 0.9, false)
   );
}

auto make_aligns()
{
   return htile(
      margin({ 10, 10, 10, 10 },
         group("VTile with Fixed-Sized, Aligned Elements", make_vtile_aligns(), 0.9, false)
      ),
      margin({ 10, 10, 10, 10 },
         group("HTile with Fixed-Sized, Aligned Elements", make_htile_aligns(), 0.9, false)
      )
   );
}

auto make_percentages()
{
   return htile(
      margin({ 10, 10, 10, 10 },
         group("VTile with Stretchable Elements", make_vtile_stretch(), 0.9, false)
      ),
      margin({ 10, 10, 10, 10 },
         group("HTile with Stretchable Elements", make_htile_stretch(), 0.9, false)
      )
   );
}

auto make_mixed()
{
   return htile(
      margin({ 10, 10, 10, 10 },
         group("VTile Fixed-Sized and Stretchable Elements", make_vtile_mixed(), 0.9, false)
      ),
      margin({ 10, 10, 10, 10 },
         group("HTile Fixed-Sized and Stretchable Elements", make_htile_mixed(), 0.9, false)
      )
   );
}

template <typename MenuItem>
auto make_popup_menu(MenuItem& item1, MenuItem& item2, MenuItem& item3, MenuItem& item4)
{
   auto popup  = dropdown_menu("Layout", menu_position::bottom_left);

   auto menu =
      layer(
         vtile(link(item1), link(item2), link(item3), link(item4)),
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

   auto align_menu_item = menu_item("Fixed-Sized, Aligned Elements");
   auto percentages_menu_item = menu_item("Stretchable Elements");
   auto mixed_menu_item = menu_item("Fixed-Sized and Stretchable Elements");
   auto flow_menu_item = menu_item("Flow Elements");

   // Note: lower deck elements are at the top of the list (lower index)
   auto content = deck(
      make_flow(),
      make_mixed(),
      make_percentages(),
      make_aligns()
   );

   view view_(_win);

   align_menu_item.on_click = [&]()
   {
      content.select(0);
      view_.refresh(content);
   };

   percentages_menu_item.on_click = [&]()
   {
      content.select(1);
      view_.refresh(content);
   };

   mixed_menu_item.on_click = [&]()
   {
      content.select(2);
      view_.refresh(content);
   };

   flow_menu_item.on_click = [&]()
   {
      content.select(3);
      view_.refresh(content);
   };

   auto menu = make_popup_menu(
      align_menu_item,
      percentages_menu_item,
      mixed_menu_item,
      flow_menu_item
   );

   auto top = align_right(hsize(120, menu));
   auto main_pane = pane(top, link(content), false);
   auto main_element = margin({ 10, 10, 10, 10 }, main_pane);

   view_.content(
      {
         share(background),
         share(main_element)
      }
   );

   _app.run();
   return 0;
}
