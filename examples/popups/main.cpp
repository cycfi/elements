/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon.hpp>

using namespace cycfi::photon;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);

struct background : element
{
   void draw(context const& ctx)
   {
      auto& cnv = ctx.canvas;
      cnv.fill_style(bkd_color);
      cnv.fill_rect(ctx.bounds);
   }
};

// A Popup
auto make_popup(view const& view_)
{
   return share(
      align_center_middle(
         fixed_size({ 400, 200 },
          pane("Popup", element())
      ))
   );
}

int main(int argc, const char* argv[])
{
   using namespace std::chrono_literals;

   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
         share(background{})
      }
   );

   auto popup = make_popup(view_);

   view_.defer(1500ms,
      [&view_, &popup]()
      {
         view_.add(popup);
      }
   );

   _app.run();
   return 0;

}
