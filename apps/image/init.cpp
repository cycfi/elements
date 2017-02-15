/*=======================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#include <photon/view.hpp>
#include <photon/widget.hpp>
#include <cstdlib>

namespace client
{
   using namespace photon;

   // Main window background color
   auto bkd_color = color{ 35, 35, 37, 255 };

   struct background : widget
   {
      void draw(context const& ctx)
      {
         auto&  cnv = ctx.canvas;
         cnv.fill_style(bkd_color);
         cnv.fill_rect(ctx.bounds);
      }
   };

   auto make_view_port()
   {
      auto  space = image{ "images/space.jpg" };
      return scroller(space);
   }

   init_view _init_view
   {
      [](photon::view& view_)
      {
         view_.content.push_back(share(background{}));
         view_.content.push_back(share(make_view_port()));
      }
   };
}
