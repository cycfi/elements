/*=======================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#include <photon.hpp>

extern "C"
{
    void blur_image_surface(cairo_surface_t *surface, int radius);
}

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

//         auto&  cnv = ctx.canvas;
//         cnv.fill_style(colors::black);
//         cnv.round_rect({ 100, 100, 250, 250 }, 30);
//
//         float blur = 20;
//         float radius = blur/2;
//
//         auto extent = cnv.fill_extent();
//         auto offsx = extent.left - radius;
//         auto offsy = extent.top - radius;
//         auto width = extent.width();
//         auto height = extent.height();
//
//         auto pm = pixmap{ { width+blur, height+blur } };
//         auto pm_ctx = pixmap_context{ pm };
//         auto pm_cnv = canvas{ *pm_ctx.context() };
//
//         cairo_surface_set_device_offset(pm._surface, -offsx, -offsy);
//         auto path = cairo_copy_path(&cnv.cairo_context());
//         cairo_append_path(&pm_cnv.cairo_context(), path);
//
//         pm_cnv.fill_style(colors::black.opacity(0.2));
//         pm_cnv.fill();
//
//         blur_image_surface(pm._surface, blur);
//
//         auto pms = pm.size();
//
//         auto& _context = cnv.cairo_context();
//         cairo_set_source_surface(&_context, pm._surface, 0, 0);
//         cairo_rectangle(&_context, 100, 100, 150, 150);
//         cairo_fill(&_context);
//         
//
//         cnv.line_width(0.2);
//
////         cairo_set_source_rgba(&_context, 1, 1, 1, 1);
////         cairo_rectangle(&_context, 100, 100, 150, 150);
////         cairo_stroke(&_context);
//         
//         
//         cairo_append_path(&_context, path);
//         cairo_path_destroy(path);
//
//         cnv.stroke_style(colors::white);
//         cnv.stroke();


// Case B
//         auto&  cnv = ctx.canvas;
//         cnv.fill_style(colors::black);
//         cnv.round_rect({ 40, 40, 220, 220 }, 30);
//
//         float blur = 20;
//         float radius = blur/2;
//
//         auto pm = pixmap{ { 200, 200 } };
//         auto pm_ctx = pixmap_context{ pm };
//         auto pm_cnv = canvas{ *pm_ctx.context() };
//
//
//         cairo_surface_set_device_offset(pm._surface, -30, -30);
//         auto path = cairo_copy_path(&cnv.cairo_context());
//         cairo_append_path(&pm_cnv.cairo_context(), path);
//
//         pm_cnv.fill_style(colors::black.opacity(0.2));
//         pm_cnv.fill();
//
//         blur_image_surface(pm._surface, blur);
//
//         auto& _context = cnv.cairo_context();
//         cairo_set_source_surface(&_context, pm._surface, 0, 0);
//         cairo_rectangle(&_context, 30, 30, 200, 200);
//         cairo_fill(&_context);
//         
//         cairo_append_path(&_context, path);
//         cairo_path_destroy(path);
//
////         cnv.stroke_style(colors::white);
////         cnv.line_width(0.2);
////         cnv.stroke();


// Case A
//         auto&  cnv = ctx.canvas;
//         cnv.fill_style(colors::black);
//         cnv.round_rect({ 20, 20, 200, 200 }, 30);
//
//         float blur = 20;
//         float radius = blur/2;
//
//         auto pm = pixmap{ { 200, 200 } };
//         auto pm_ctx = pixmap_context{ pm };
//         auto pm_cnv = canvas{ *pm_ctx.context() };
//
//
//         cairo_surface_set_device_offset(pm._surface, -10, -10);
//         auto path = cairo_copy_path(&cnv.cairo_context());
//         cairo_append_path(&pm_cnv.cairo_context(), path);
//
//         pm_cnv.fill_style(colors::black.opacity(0.2));
//         pm_cnv.fill();
//
//         blur_image_surface(pm._surface, blur);
//
//         auto& _context = cnv.cairo_context();
//         cairo_set_source_surface(&_context, pm._surface, 0, 0);
//         cairo_rectangle(&_context, 10, 10, 200, 200);
//         cairo_fill(&_context);
//
//         cairo_append_path(&_context, path);
//         cairo_path_destroy(path);

//         cnv.stroke_style(colors::white);
//         cnv.line_width(0.2);
//         cnv.stroke();


//         auto&  cnv = ctx.canvas;
//         cnv.fill_style(colors::black);
//         cnv.round_rect({ 10, 10, 190, 190 }, 20);
//
//         float blur = 20;
//
//
//         auto extent = cnv.fill_extent();
//         auto x = extent.left;
//         auto y = extent.top;
//         auto w = extent.width();
//         auto h = extent.height();
//
//         auto dw = w;
//         auto dh = h;
//
//
//
//         auto pm = pixmap{ { 250, 250 } };
//         auto pm_ctx = pixmap_context{ pm };
//         auto pm_cnv = canvas{ *pm_ctx.context() };
//
//         //cairo_surface_set_device_offset(pm._surface, -x, -y);
//
//         auto& _context = cnv.cairo_context();
//
//         auto path = cairo_copy_path(&_context);
//         cairo_append_path(&pm_cnv.cairo_context(), path);
//
//         pm_cnv.fill_style(colors::black.opacity(0.4));
//         pm_cnv.fill();
//
//         blur_image_surface(pm._surface, blur);
//
//
//
//         cairo_set_source_surface(&_context, pm._surface, 0, 0);
//         cairo_rectangle(&_context, 0, 0, 250, 250);
//         cairo_fill(&_context);







//         auto&  cnv = ctx.canvas;
//         cnv.fill_style(colors::black);
//         cnv.round_rect({ 10, 10, 190, 190 }, 20);
//
//         auto extent = cnv.fill_extent();
//         auto x = extent.left;
//         auto y = extent.top;
//         auto w = extent.width();
//         auto h = extent.height();
//
//
//
//         auto pm = pixmap{ { w, h } };
//         auto pm_ctx = pixmap_context{ pm };
//         auto pm_cnv = canvas{ *pm_ctx.context() };
//
//         cairo_surface_set_device_offset(pm._surface, -x, -y);
//
//
//         auto path = cairo_copy_path(&cnv.cairo_context());
//         cairo_append_path(&pm_cnv.cairo_context(), path);
//
//         pm_cnv.fill_style(colors::blue.opacity(0.4));
//         pm_cnv.fill();
//
//         blur_image_surface(pm._surface, 20);
//
//
//
//         auto& _context = cnv.cairo_context();
//         cairo_set_source_surface(&_context, pm._surface, x, y);
//         cairo_rectangle(&_context, 10, 10, 190, 190);
//         cairo_fill(&_context);


         //cairo_append_path(&cnv.cairo_context(), path);
         //cnv.fill();







         //auto&  cnv = ctx.canvas;
         //auto pm = pixmap{ { 200, 200 } };
         //auto pm_ctx = pixmap_context{ pm };
         //auto pm_cnv = canvas{ *pm_ctx.context() };
         //
         //
         //pm_cnv.fill_style(colors::black.opacity(0.2));
         //pm_cnv.fill_round_rect(rect{ 8, 8, 192, 192 }, 8);
         //
         //blur_image_surface(pm._surface, 20);
         //
         //cnv.draw(pm, {10, 10});
         //
         //cnv.fill_style(colors::black);
         //cnv.fill_round_rect({ 10, 10, 190, 190 }, 5);


//         auto&  cnv = ctx.canvas;
//         auto&  cr = cnv.cairo_context();
//
//         cairo_push_group_with_content(&cr, CAIRO_CONTENT_COLOR_ALPHA);
//
//         cnv.fill_style(colors::black.opacity(0.2));
//         cnv.fill_round_rect(rect{ 8, 8, 192, 192 }, 8);
//         blur_image_surface(cairo_get_group_target(&cr), 20);
//         cairo_pop_group_to_source(&cr);



//         auto&  cnv = ctx.canvas;
//         auto pm = pixmap{ { 200, 200 } };
//         auto pm_ctx = pixmap_context{ pm };
//         auto pm_cnv = canvas{ *pm_ctx.context() };
//
//         cnv.fill_style(colors::black);
//         cnv.round_rect({ 10, 10, 190, 190 }, 5);
//
//         auto path = cairo_copy_path(&cnv.cairo_context());
//         cairo_append_path(&pm_cnv.cairo_context(), path);
//
//         pm_cnv.fill_style(colors::black.opacity(0.2));
//         //pm_cnv.fill_round_rect(rect{ 8, 8, 192, 192 }, 5);
//         pm_cnv.fill();
//
//         blur_image_surface(pm._surface, 20);
//
//         cnv.draw(pm, {10, 10});
//
//
//         cairo_append_path(&cnv.cairo_context(), path);
//         cnv.fill();



         auto&  cnv = ctx.canvas;
         cnv.fill_style(colors::dark_sea_green.opacity(0.7));
         cnv.shadow_style({ 10, 10 }, 20, colors::black.opacity(0.2));
         cnv.fill_round_rect({ 30, 30, 250, 250 }, 20);
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
