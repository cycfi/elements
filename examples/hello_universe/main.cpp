#include <photon.hpp>

using namespace cycfi::photon;

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name(), { 20, 20, 660, 420 });
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win.host());

   view_.content(
      {
         share(scroller(image{ "space.jpg" }))
      }
   );

   _app.run();
   return 0;
}
