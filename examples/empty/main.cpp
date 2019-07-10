#include <elemental.hpp>

using namespace cycfi::elemental;

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
                           // Add more content layers here. The order
                           // specifies the layering. The lowest layer
                           // is at the top of this list.

         share(element())  // Replace element() with your main element.
      }
   );

   _app.run();
   return 0;
}
