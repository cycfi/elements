#include <photon/window.hpp>
#include <photon/app.hpp>
#include <photon/widget/panel.hpp>
#include <photon/widget/margin.hpp>
#include <photon/widget/align.hpp>
#include <photon/widget/size.hpp>
#include <photon/widget/tile.hpp>

int main()
{
   using namespace photon;

   auto& my_app = make_app<app>();

   {
      auto elem = top_margin(
         { 20 },
         hsize(64, panel())
      );

      auto rows = margin(
         { 20, 0, 20, 20 },
         vtile(
            halign(0.0,  elem),
            halign(0.1,  elem),
            halign(0.2,  elem),
            halign(0.3,  elem),
            halign(0.4,  elem),
            halign(0.5,  elem),
            halign(0.6,  elem),
            halign(0.7,  elem),
            halign(0.8,  elem),
            halign(0.9,  elem),
            halign(1.0,  elem)
         )
      );

      //auto columns = htile();

      auto main_widget = rows;

      window main_window("Photon", { 1000, 600 }, colors::gray[30], my_app, main_widget);
      my_app.run();
   }

   return 0;
}
