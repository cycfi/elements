/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon.hpp>

using namespace cycfi::photon;

// Main window background color
auto bkd_color = color{ 62, 62, 62, 255 };

struct background : element
{
   void draw(context const& ctx)
   {
      auto&  cnv = ctx.canvas;
      cnv.fill_style(bkd_color);
      cnv.fill_rect(ctx.bounds);
   }
};

auto box = basic(
   [](context const& ctx)
   {
      auto& c = ctx.canvas;

      c.begin_path();
      c.round_rect(ctx.bounds, 4);
      c.fill_style(colors::gold.opacity(0.8));
      c.fill();
   }
);

auto make_vtile()
{
   auto _box = top_margin(
      { 20 },
      hsize(150, box)
   );

   return margin(
      { 20, 50, 20, 20 },
      vtile(
         halign(0.0, _box),
         halign(0.2, _box),
         halign(0.4, _box),
         halign(0.6, _box),
         halign(0.8, _box),
         halign(1.0, _box)
      )
   );
}

auto make_vtile2()
{
   auto _box = top_margin(
      { 20 },
      box
   );

   return margin(
      { 20, 50, 20, 20 },
      vtile(
         vspan(1.0, _box),
         vspan(0.5, _box),
         vspan(0.5, _box),
         vspan(0.5, _box),
         vspan(2.0, _box)
      )
   );
}

auto make_vtile_main()
{
   return htile(
      margin({ 10, 10, 10, 10 }, group("Aligns", make_vtile(), 0.9, false)),
      margin({ 10, 10, 10, 10 }, group("Percentages", make_vtile2(), 0.9, false))
   );
}

auto make_htile()
{
   auto _box = left_margin(
      { 20 },
      vsize(100, box)
   );

   return margin(
      { 0, 50, 20, 20 },
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
      { 20 },
      box
   );

   return margin(
      { 0, 50, 20, 20 },
      htile(
         hspan(1.0, _box),
         hspan(0.5, _box),
         hspan(0.5, _box),
         hspan(0.5, _box),
         hspan(2.0, _box)
      )
   );
}

auto make_htile_main()
{
   return htile(
      margin({ 10, 10, 10, 10 }, group("Aligns", make_htile(), 0.9, false)),
      margin({ 10, 10, 10, 10 }, group("Spans", make_htile2(), 0.9, false))
   );
}

int main(int argc, const char* argv[])
{
   auto right_btn = button(icons::right);
   auto up_btn = button(icons::up);

   auto content = deck(
      make_vtile_main(),   // vertical tiles aligns and spans
      make_htile_main()    // horizontal tiles aligns and spans
   );

   app my_app{
      [&](view& view_)
      {
         right_btn.on_click = [&](bool)
         {
            content.select(1);
            view_.refresh(content);
         };

         up_btn.on_click = [&](bool)
         {
            content.select(0);
            view_.refresh(content);
         };

         auto top = htile(
            hspan(1.0, align_left(right_margin(8, label("Tiles Aligns and Spans")))),
            hspan(0.1, link(right_btn)),
            hspan(0.1, link(up_btn))
         );

         auto main_pane = pane(top, link(content), false);
         auto main_element = margin({ 20, 20, 20, 20 }, main_pane);

         view_.content =
         {
            share(background{}),
            share(main_element)
         };
      }
   };
   return my_app.main(argc, argv);
}
