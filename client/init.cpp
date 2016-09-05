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

   auto img = image{ "assets/images/space.jpg" };
   auto spr =  sprite<150, 150>{ "assets/images/knob_sprites_150x150_darker.png" };
   auto spr_middle = halign(0.5, valign(0.5, spr));
   auto gzmo = margin(rect{20, 20, 20, 20}, gizmo{ "assets/images/button.png", 0.25 });

   void  init(view& v)
   {
      v.content.elements.push_back(new_(my_image{}));

      //v.content.elements.push_back(new_(background{}));
      //v.content.elements.push_back(new_(gzmo));

      //v.content.elements.push_back(new_(spr_middle));
      //v.content.elements.push_back(new_(img));
      //v.content.elements.push_back(new_(drawings{}));
      //v.content.elements.push_back(new_(vpups));
   }
}
