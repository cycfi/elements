/*=======================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#include <photon.hpp>

namespace client
{
   using namespace photon;

   auto make_view_port()
   {
      return share(scroller(image{ "images/space.jpg" }));
   }

   init_view _init_view
   {
      [](photon::view& view_)
      {
         view_.content =
         {
            make_view_port()
         };
      }
   };
}
