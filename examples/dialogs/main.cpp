/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using namespace std::chrono_literals;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto make_message()
{
   char const* msg = "Patience... Wait for it...\n\n"
      "The nexus is overflowing with supercharged waveforms. "
      "Awareness is a constant. "
      ;

   return message_box0(msg, icons::hand, { 400, 150 });
}

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);
   auto msg_box = make_message();

   view_.content(
      {
         msg_box,
         share(background)
      }
   );

   _app.run();
   return 0;

}
