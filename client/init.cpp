/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <infinity/application.hpp>

namespace client
{
   using namespace photon;

   // Main window background color
   auto bkd_color = color{ 35, 35, 37, 255 };

   struct background : widget
   {
      void draw(context const& ctx)
      {
         auto& cnv = ctx.canvas;
         cnv.fill_style(bkd_color);
         cnv.fill_rect(ctx.bounds);
      }
   };

   auto make_dial()
   {
      float scale = 1.0/4;

      auto spr =  sprite{ "assets/images/knob_sprites_white_128x128.png", 32, scale };
      auto rlines = image{ "assets/images/radial-lines.png", scale };

      auto di = dial(spr);
      auto lyr = layer(align_center(align_middle(di)), rlines);
      auto all = align_center(align_middle(caption(lyr, "Volume", 0.5)));

      return margin({ 50, 50, 50, 50 }, std::move(all));
   }

   auto vgzmo = margin(rect{20, 20, 20, 20}, halign(0.5, vgizmo{ "assets/images/slot.png", 1.0/4 }));

   void  init(photon::view& view_)
   {
      view_.maintain_aspect(true);
      view_.content.push_back(share(background{}));
      //view_.content.push_back(share(fit({ 150, 150 }, make_dial())));
      //view_.content.push_back(share(fit({ 150, 150 }, vgzmo)));

      view_.app = std::make_shared<infinity::application>(view_);
   }
}
