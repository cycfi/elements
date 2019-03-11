#include <photon.hpp>

using namespace cycfi::photon;

// Main window background color
auto constexpr bkd_color = color{ 35, 35, 37, 255 };

struct background : element
{
   void draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      cnv.fill_style(bkd_color);
      cnv.fill_rect(ctx.bounds);
   }
};

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name(), { 50, 50, 1024, 768 });

   auto s = _win.size();
   auto p = _win.position();
   _win.on_close = [&_app]() { _app.stop(); };

   view my_view(_win.host());
   my_view.content(
      { share(background{}) }
   );

   _app.run();
   return 0;
}
