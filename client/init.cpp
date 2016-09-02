/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <photon/support.hpp>
#include <photon/widget.hpp>

#include <elf/frets.hpp>

namespace client
{
   namespace ph = photon;

   using ph::widget;
   using ph::rect;
   using ph::context;
   using ph::canvas;
   using ph::basic_context;
   using ph::color;
   using ph::view;
   
   namespace colors = ph::colors;

   auto frets_ = align_middle(align_center(elf::frets{}));

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
         
         
      }
   };

   void  init(view& v)
   {
      v.content.elements.push_back(new_(drawings{}));
      //v.content.elements.push_back(new_(frets_));
   }
}
