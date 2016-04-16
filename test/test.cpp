#include <photon/window.hpp>
#include <photon/app.hpp>
#include <photon/widget/panel.hpp>
#include <photon/widget/margin.hpp>
#include <photon/widget/align.hpp>
#include <photon/widget/size.hpp>
#include <photon/widget/tile.hpp>
#include <photon/widget/slider.hpp>
#include <photon/widget/layer.hpp>

int main()
{
   using namespace photon;

   auto& my_app = make_app<app>();

   {
      widget_ptr main_widget;

      {
         auto helem = top_margin(
            { 20 },
            hsize(64, panel())
         );

         auto rows = margin(
            { 20, 0, 20, 20 },
            vtile(
               halign(0.0,  helem),
               halign(0.2,  helem),
               halign(0.4,  helem),
               halign(0.6,  helem),
               halign(0.8,  helem),
               halign(1.0,  helem)
            )
         );

         //main_widget = rows;
      }

      {
         auto velem = left_margin(
            { 20 },
            vsize(64, panel())
         );

         auto columns = margin(
            { 0, 20, 20, 20 },
            htile(
               valign(0.0,  velem),
               valign(0.2,  velem),
               valign(0.4,  velem),
               valign(0.6,  velem),
               valign(0.8,  velem),
               valign(1.0,  velem)
            )
         );

         //main_widget = columns;
      }

      {
         auto sl = margin(
            { 20, 20, 20, 20 },
            layer(
                panel(),
                margin({ 20, 20, 20, 20 }, slider())
            )
         );

         main_widget = sl;
      }

      window main_window("Photon", { 1000, 600 }, colors::gray[30], my_app, main_widget);
      my_app.run();
   }

   return 0;
}
