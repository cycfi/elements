/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <photon/support.hpp>
#include <photon/widget.hpp>

#include <elf/frets.hpp>
#include <elf/pickup.hpp>

namespace client
{
   namespace ph = photon;

   using ph::widget;
   using ph::point;
   using ph::rect;
   using ph::context;
   using ph::canvas;
   using ph::basic_context;
   using ph::color;
   using ph::view;
   using ph::layer;
   using ph::pixmap;
   using ph::image;
   using ph::sprite;
   using ph::gizmo;
   using ph::vgizmo;
   using ph::dial;
   using ph::basic;
   using ph::slider;
   using ph::codepoint_to_utf8;

   namespace colors = ph::colors;
   namespace icons = ph::icons;

   // Main window background color
   auto bkd_color = color{ 35, 35, 37, 255 };

   struct background : widget
   {
      rect limits(basic_context const& ctx) const
      {
         // Main window limits
         return { 640, 480, 640*2, 480*2 };
      }

      void draw(context const& ctx)
      {
         auto cnv = ctx.canvas();
         cnv.fill_style(bkd_color);
         cnv.fill_rect(ctx.bounds);
      }
   };

   struct test : widget
   {
      void draw(context const& ctx)
      {
//         auto cnv = ctx.canvas();
//         auto* cr = &cnv.cairo_context();
//
//         int              w, h;
//         cairo_surface_t *image;
//
//         image = cairo_image_surface_create_from_png ("assets/images/test.png");
//         w = cairo_image_surface_get_width (image);
//         h = cairo_image_surface_get_height (image);
//
//         cairo_translate (cr, 128.0, 128.0);
//         //cairo_rotate (cr, 45* M_PI/180);
//         cairo_scale  (cr, 256.0/w, 256.0/h);
//         cairo_translate (cr, -0.5*w, -0.5*h);
//
//         cairo_set_source_surface (cr, image, 0, 0);
//         cairo_paint (cr);
//         cairo_surface_destroy (image);
      }
   };

   struct drawings : widget
   {
      void draw(context const& ctx)
      {
         auto cnv = ctx.canvas();
         cnv.stroke_style(colors::gray[10]);
         cnv.line_width(0.5);

         {
            cnv.fill_style(colors::dim_gray);
            auto r = rect{ 5, 25, 160, 160 };
            cnv.fill_rect(r);
            cnv.stroke_rect(r);
         }

         {
            auto r = rect{ 50, 5, 100, 80 };
            auto grad = canvas::linear_gradient{ { 50, 5 }, { 50, 50 } };
            grad.add_color_stop({0, colors::dark_turquoise });
            grad.add_color_stop({1, colors::midnight_blue });
            cnv.fill_style(grad);
            cnv.fill_round_rect(r, 5);
            cnv.stroke_round_rect(r, 5);
         }

         {
            auto r = rect{ 80, 30, 130, 110 };
            auto cp = center_point(r);
            auto grad = canvas::radial_gradient{ cp, 5, cp, 50 };
            grad.add_color_stop({0, colors::light_cyan });
            grad.add_color_stop({1, colors::gold.opacity(0.3) });
            cnv.fill_style(grad);
            cnv.fill_round_rect(r, 10);
            cnv.stroke_round_rect(r, 10);
         }

         {
            cnv.fill_style(colors::dim_gray);
            cnv.font("Open Sans", 36, cnv.bold | cnv.italic);
            cnv.fill_text({ 200, 30 }, "Hello, World!");
            cnv.stroke_text({ 200, 80 }, "Hello, World!");

            auto grad = canvas::linear_gradient{ { 200, 50 }, { 400, 50 } };
            grad.add_color_stop({0, colors::dark_turquoise });
            grad.add_color_stop({1, colors::midnight_blue });
            cnv.fill_style(grad);
            cnv.fill_text({ 200, 130 }, "Hello, World!");
         }

         auto texty = [&cnv](float x, float y, char const* text)
         {
            cnv.stroke_style(colors::magenta.opacity(0.5));
            cnv.line_width(0.5);
            cnv.move_to({ x-100, y });
            cnv.line_to({ x+100, y });
            cnv.move_to({ x, y-50 });
            cnv.line_to({ x, y+50 });
            cnv.stroke();

            cnv.stroke_style(colors::dim_gray);
            cnv.fill_style(colors::dim_gray);
            cnv.font("Open Sans", 24);
            cnv.fill_text({ x, y }, text);
         };

         {
            cnv.text_align(cnv.left);
            texty(100, 250, "Lefty");

            cnv.text_align(cnv.right);
            texty(250, 250, "Righty");

            cnv.text_align(cnv.center);
            texty(350, 250, "Centery");

            cnv.text_align(cnv.left | cnv.top);
            texty(450, 250, "Topy");

            cnv.text_align(cnv.left | cnv.middle);
            texty(550, 250, "Middley");

            cnv.text_align(cnv.left | cnv.bottom);
            texty(650, 250, "Bottomy");
         }

         {
            cnv.font("photon_basic", 48);
            rect b = { 50, 320, 120, 390 };

            cnv.stroke_rect(b);
            draw_icon(cnv, b, icons::power);

            b = b.move(80, 0);
            cnv.stroke_rect(b);
            draw_icon(cnv, b, icons::wrench);

            b = b.move(80, 0);
            cnv.stroke_rect(b);
            draw_icon(cnv, b, icons::cog);
         }
      }
   };

   auto gtr =
      layer(
         elf::pickup{ 0.42, elf::pickup::double_, 0 },
         elf::pickup{ 0.28, elf::pickup::single, 0 },
         elf::pickup{ 0.13, elf::pickup::double_, 0.2 },
         elf::frets{}
      );
   auto vpups = align_middle(align_center(gtr));

   struct my_image : widget
   {
      pixmap pm = pixmap{ "assets/images/space.jpg" };

      void draw(context const& ctx)
      {
         auto cnv = ctx.canvas();

         {
            cnv.draw(pm, ctx.bounds);
            cnv.draw(pm, ctx.bounds.inset(ctx.bounds.width()/4, ctx.bounds.height()/4));

//            float v = 50;
//
//            cnv.draw(pm, { 0, 0, 100, 100 }, { v, v, v+100, v+400 });
//
//            cnv.line_width(0.5);
//            cnv.stroke_rect({ 0, 0, v, v });
         }
      }
   };
   
   auto sldr_knob = image{ "assets/images/slider-white.png", 1.0/4 };
   auto sldr_knob_middle = halign(0.5, valign(0.5, sldr_knob));


   auto img = image{ "assets/images/space.jpg" };
   auto spr =  sprite{ "assets/images/knob_sprites_150x150_darker.png", 50, 1.0/3 };
   auto spr_middle = halign(0.5, valign(0.5, spr));
   auto gzmo = margin(rect{20, 20, 20, 20}, gizmo{ "assets/images/button.png", 1.0/4 });
   auto vgzmo = margin(rect{20, 20, 20, 20}, halign(0.5, vgizmo{ "assets/images/slot.png", 1.0/4 }));

   auto make_dial()
   {
      float scale = 1.0/4;

      auto spr =  sprite{ "assets/images/knob_sprites_white_128x128.png", 32, scale };
      auto rlines = image{ "assets/images/radial-lines.png", scale };


//      auto di = dial(spr);
//      
//      auto lyr = layer(
//        align_center(align_middle(di)),
//        align_center(align_middle(rlines))
//      );
//      
//      auto all = caption(lyr, "Volume");

      auto di = dial(spr);
      auto lyr = layer(align_center(align_middle(di)), rlines);
      auto all = align_center(align_middle(caption(lyr, "Volume", 0.5)));

      return margin({ 50, 50, 50, 50 }, std::move(all));
   }

   auto make_slider()
   {
      struct box : widget
      {
         void draw(context const& ctx)
         {
            auto c = ctx.canvas();
            c.line_width(1);
            c.stroke_rect(ctx.bounds.inset(1, 1));
         }
      };

      struct filled_box : widget
      {
         void draw(context const& ctx)
         {
            auto c = ctx.canvas();
            c.fill_style(colors::white);
            c.fill_rect(ctx.bounds.inset(1, 1));
            c.line_width(1);
            c.stroke_rect(ctx.bounds.inset(1, 1));
         }
      };

      auto vind = fixed_size({ 40, 20 }, filled_box{});
      auto vslot = hsize(10, box{});
      auto vsldr = slider{ new_(std::move(vind)), new_(std::move(vslot)), 0.3 };

      auto hind = fixed_size({ 20, 40 }, filled_box{});
      auto hslot = vsize(10, box{});
      auto hsldr = slider{ new_(std::move(hind)), new_(std::move(hslot)), 0.3 };

      return
         vtile(
            margin({ 50, 50, 50, 50 }, halign(0.5, std::move(vsldr))),
            margin({ 50, 0, 50, 50 }, valign(0.5, std::move(hsldr)))
         );
   }
   
   auto make_slider2()
   {
      auto vslot = yside_margin({5, 5}, vgizmo{ "assets/images/slot.png", 1.0/4 });
      auto vind = left_margin(4, image{ "assets/images/slide-switch.png", 1.0/4 });
      auto vsldr = slider{ new_(std::move(vind)), new_(std::move(vslot)) };
      return margin({ 50, 50, 50, 50 }, halign(0.5, std::move(vsldr)));
   }

   void  init(view& v)
   {

      v.content.elements.push_back(new_(background{}));
      
      //v.content.elements.push_back(new_(make_slider2()));

      
      //v.content.elements.push_back(new_(sldr_knob_middle));

      //v.content.elements.push_back(new_(test{}));


      //v.content.elements.push_back(new_(my_image{}));

      //v.content.elements.push_back(new_(gzmo));

      v.content.elements.push_back(new_(make_dial()));
      
      //v.content.elements.push_back(new_(make_slider()));

      //v.content.elements.push_back(new_(vgzmo));
      //v.content.elements.push_back(new_(spr_middle));
      //v.content.elements.push_back(new_(img));
      //v.content.elements.push_back(new_(drawings{}));
      
      //v.content.elements.push_back(new_(vpups));
   }
}
