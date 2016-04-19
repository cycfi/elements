#include <photon/window.hpp>
#include <photon/app.hpp>
#include <photon/widget/panel.hpp>
#include <photon/widget/margin.hpp>
#include <photon/widget/align.hpp>
#include <photon/widget/size.hpp>
#include <photon/widget/tile.hpp>
#include <photon/widget/slider.hpp>
#include <photon/widget/layer.hpp>
#include <photon/widget/text.hpp>
#include <photon/widget/icon.hpp>

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
                margin({ 20, 20, 20, 20 }, slider()),
                panel()
            )
         );

         //main_widget = sl;
      }

      {
         auto fr = [](widget_ptr el) -> widget_ptr
         {
            return margin({ 10, 10, 10, 10 }, layer(margin({ 10, 5, 10, 5 }, el), frame()));
         };

         auto eh = [=](char const* txt) -> widget_ptr
         {
            return fr(halign(0.5,  heading(txt)));
         };

         auto el = [=](double align, char const* txt) -> widget_ptr
         {
            return fr(halign(align,  label(txt)));
         };

         auto txt = margin(
            { 20, 20, 20, 20 },
            layer(
                margin(
                    { 20, 20, 20, 20 },
                    vtile(
                        eh("Photon UI"),
                        el(1.0, "Hello, World. I am Photon."),
                        el(1.0, "A cross-platform, fine-grained, highly modular C++ GUI library."),
                        el(0.0, "I used to be called Pica in the mid 90s."),
                        el(0.5, "Now, Joel rewrote my code using modern C++14.")
                    )
                ),
                panel()
            )
         );

         //main_widget = txt;
      }

      {
         char const* text =
            "We are in the midst of an intergalatic condensing of beauty that will "
            "clear a path toward the planet itself. The quantum leap of rebirth is "
            "now happening worldwide. It is time to take healing to the next level. "
            "Soon there will be a deepening of chi the likes of which the infinite "
            "has never seen. The universe is approaching a tipping point. This "
            "vision quest never ends. Imagine a condensing of what could be. "
            "We can no longer afford to live with stagnation. Suffering is born "
            "in the gap where stardust has been excluded. You must take a stand "
            "against discontinuity.\n\n"
          
            "Without complexity, one cannot dream. Stagnation is the antithesis of "
            "life-force. Only a seeker of the galaxy may engender this wellspring of hope."
            "Yes, it is possible to eliminate the things that can destroy us, but not "
            "without wellbeing on our side. Where there is delusion, faith cannot thrive. "
            "You may be ruled by desire without realizing it. Do not let it eliminate "
            "the growth of your journey.\n\n"
          
            "--New-Age Bullshit Generator"
            ;

         auto txbx = margin(
            { 20, 20, 20, 20 },
            layer(
                margin({ 20, 20, 20, 20 }, text_box(text)),
                panel()
            )
         );

         //main_widget = txbx;
      }
      
      {
         auto icn = margin(
            { 20, 20, 20, 20 },
            icon<0x1F50D, 300>()
         );
         
         main_widget = icn;
      }

      window main_window("Photon", { 1000, 600 }, colors::gray[30], my_app, main_widget);
      my_app.run();
   }

   return 0;
}
