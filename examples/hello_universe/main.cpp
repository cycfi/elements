/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon.hpp>

using namespace photon;

auto make_view()
{
   return share(scroller(image{ "space.jpg" }));
}

int main(int argc, const char* argv[])
{
   app my_app{
      [](view& view_)
      {
         view_.content =
         {
            make_view()
         };
      }
   };
   return my_app.main(argc, argv);
}
