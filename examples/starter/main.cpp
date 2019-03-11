#include <photon.hpp>

using namespace cycfi::photon;

class my_view : base_view
{
public:

   using base_view::base_view;

   void draw(cairo_t* ctx, rect area) override
   {
   }
};

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name(), { 50, 50, 1024, 768 });

   auto s = _win.size();
   auto p = _win.position();
   _win.on_close = [&_app]() { _app.stop(); };

   my_view view(_win.host());

   _app.run();
   return 0;
}
