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
      auto  space = image{ "images/space.jpg" };
      return scroller(space);
   }

   init_view _init_view
   {
      [](photon::view& view_)
      {
         view_.content =
         {
            share(make_view_port())
         };
      }
   };
}
