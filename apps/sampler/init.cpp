/*=======================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#include <photon.hpp>

namespace client
{
   using namespace photon;

   // Main window background color
   auto bkd_color = color{ 62, 62, 62, 255 };

   struct background : widget
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

   auto make_view_port()
   {
      auto  space = image{ "images/space.jpg" };
      return scroller(space);
   }

   struct testing : widget
   {
      void draw(context const& ctx)
      {
         auto&  cnv = ctx.canvas;
         cnv.fill_style(colors::black.opacity(0.7));
         cnv.shadow_style({ 2, 4 }, 20, colors::black.opacity(0.4));
         cnv.fill_round_rect({ 30, 30, 250, 250 }, 5);
         
//          auto const&    theme_ = get_theme();
//          auto&          canvas_ = ctx.canvas;
//          auto           state = canvas_.new_state();
//
//          canvas_.shadow_style({ 2, 4 }, 20, colors::black.opacity(0.2));
//
//          canvas_.line_width(0.5);
//          canvas_.stroke_style(theme_.heading_font_color);
//          canvas_.font(theme_.heading_font, 72, theme_.heading_style);
//          canvas_.text_align(canvas_.middle | canvas_.center);
//
//          float cx = ctx.bounds.left + (ctx.bounds.width() / 2);
//          float cy = ctx.bounds.top + (ctx.bounds.height() / 2);
//
//          canvas_.stroke_text(point{ cx, cy }, "Hello World");
      }
   };

   init_view _init_view
   {
      [](photon::view& view_)
      {
         auto main_pane = pane("Menu", make_htile_main(), 1.0, false);
         auto main_widget = margin({ 20, 20, 20, 20 }, main_pane);
         auto space_dark = image{ "images/space_dark.jpg" };

         view_.content =
         {
            //share(space_dark),
            share(background{}),
            share(testing{}),
            //share(main_widget)
         };
      }
   };
}
