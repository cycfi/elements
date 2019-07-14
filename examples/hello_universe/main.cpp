#include <elements.hpp>

using namespace cycfi::elements;

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
         share(scroller(image{ "space.jpg" }))
      }
   );

   _app.run();
   return 0;
}
