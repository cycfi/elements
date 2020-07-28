/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using namespace std::chrono_literals;

float position = 0.0;
constexpr float incr = 0.001;
constexpr auto fps = 1000ms / 60;

void animate(view& view_, vport_element& port)
{
   position += incr;
   if (position < 1.0)
      view_.post(fps, [&]() { animate(view_, port); });
   else
      position = 1.0;
   port.valign(position);
   view_.refresh();
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Animation", "com.cycfi.simple_animation");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   auto port = share(vport(image{ "moving.png" }));
   view_.content(port);

   view_.post(fps, [&]() { animate(view_, *port); });

   _app.run();
   return 0;
}
