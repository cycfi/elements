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
   using ph::basic_context;
   using ph::color;
   using ph::view;

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

   void  init(view& v)
   {
      v.content.elements.push_back(new_(background{}));
      v.content.elements.push_back(new_(frets_));
   }
}
