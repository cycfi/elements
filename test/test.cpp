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
#include <photon/widget/button.hpp>
#include <photon/widget/image.hpp>
#include <photon/widget/port.hpp>

int main()
{
   using namespace photon;

   auto& my_app = make_app<app>();

   {
      widget_ptr main_widget;
      
      auto box = basic(
         [](context const& ctx)
         {
            auto& c = ctx.canvas();

            c.begin_path();
            c.round_rect(ctx.bounds, 4);
            c.fill_color(colors::gold);
            c.fill();
         }
      );
      
      {
         auto _box = top_margin(
            { 20 },
            hsize(64, box)
         );
      
         auto rows = margin(
            { 20, 0, 20, 20 },
            vtile(
               halign(0.0,  _box),
               halign(0.2,  _box),
               halign(0.4,  _box),
               halign(0.6,  _box),
               halign(0.8,  _box),
               halign(1.0,  _box)
            )
         );

         //main_widget = new_(std::move(rows));
      }

      {
         auto _box = left_margin(
            { 20 },
            vsize(64, box)
         );

         auto columns = margin(
            { 0, 20, 20, 20 },
            htile(
               valign(0.0,  _box),
               valign(0.2,  _box),
               valign(0.4,  _box),
               valign(0.6,  _box),
               valign(0.8,  _box),
               valign(1.0,  _box)
            )
         );

         // main_widget = new_(std::move(columns));
      }

      {
         auto sl = margin(
            { 20, 20, 20, 20 },
            layer(
               margin({ 20, 20, 20, 20 }, slider()),
               panel{}
            )
         );

         // main_widget = new_(std::move(sl));
      }

      {
         auto fr = [](auto el)
         {
            return margin({ 10, 10, 10, 10 }, layer(margin({ 10, 5, 10, 5 }, el), frame{}));
         };

         auto eh = [=](char const* txt)
         {
            return fr(halign(0.5,  heading{ txt }));
         };

         auto el = [=](double align, char const* txt)
         {
            return fr(halign(align,  label{ txt }));
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

         // main_widget = new_(std::move(txt));
      }

      std::string text =
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

      std::string text2 =
         "To traverse the quest is to become one with it.\n\n"

         "You and I are adventurers of the quantum cycle. The goal of expanding wave "
         "functions is to plant the seeds of non-locality rather than pain. "
         "The complexity of the present time seems to demand a redefining of our "
         "bodies if we are going to survive. "
         "We are at a crossroads of will and greed. Humankind has nothing to lose. "
         "Our conversations with other storytellers have led to an evolving of "
         "hyper-sentient consciousness. "
         "If you have never experienced this flow on a cosmic scale, it can be "
         "difficult to self-actualize. Although you may not realize it, you are "
         "ancient. Have you found your vision quest?\n\n"

         "Imagine a deepening of what could be. We are being called to explore the "
         "galaxy itself as an interface between nature and transformation. This "
         "circuit never ends. Entity, look within and recreate yourself. "
         "Eons from now, we warriors will exist like never before as we are reborn "
         "by the universe. We must change ourselves and empower others. The "
         "wellspring of sharing is now happening worldwide. "
         "You will soon be awakened by a power deep within yourself - a power "
         "that is ethereal, dynamic. Astral projection may be the solution to "
         "what's holding you back from an ecstatic oasis of divinity. As you "
         "reflect, you will enter into infinite freedom that transcends understanding.\n\n"
         ;

      {
         auto txbx = margin(
            { 20, 20, 20, 20 },
            layer(
               margin({ 20, 20, 20, 20 }, static_text_box{ text }),
               panel{}
            )
         );

         // main_widget = new_(std::move(txbx));
      }

      {
         auto icn = margin(
            { 20, 20, 20, 20 },
            htile(
               icon<0x1F50D, 48>{}
             , icon<0x2716, 48>{}
             , icon<0xE75E, 48>{}
             , icon<0x2713, 48>{}
             , icon<0xE740, 48>{}
             , icon<0xE729, 48>{}
            )
         );

         // main_widget = new_(std::move(icn));
      }


      {
         auto btn = margin(
            { 20, 20, 20, 20 },
            layer(
               margin({ 100, 20, 100, 20 }, button(color{ 0, 0, 0, 0 }, "Button")),
               panel()
            )
         );

         // main_widget = new_(std::move(btn));
      }

      {
         auto txbx = margin(
            { 20, 20, 20, 20 },
            layer(
               margin(
                  { 20, 20, 20, 20 },
                  basic_text_box(text, 800)
               ),
               panel{}
            )
         );

         // main_widget = new_(txbx);
      }
      
      {
         auto img =
            size(
               point{ 1920, 1080 }
             , image("./assets/images/space.jpg")
            );


         auto p =
               margin(
                  { 20, 20, 20, 20 },
                  layer(
                     margin(
                        { 20, 20, 20, 20 },
                        port(img)
                     ),
                     panel{}
                  )
               )
            ;

         //main_widget = new_(p);
      }

      {
         auto img =
            size(
               point{ 1920, 1080 }
             , image("./assets/images/space.jpg")
            );


         auto p =
               margin(
                  { 20, 20, 20, 20 },
                  layer(
                     margin(
                        { 20, 20, 20, 20 },
                        scroller(img)
                     ),
                     panel()
                  )
               )
            ;

         //main_widget = new_(p);
      }

      {
         auto txbx = margin(
            { 20, 20, 20, 20 },
            layer(
               scroller(
                  margin(
                     { 20, 20, 20, 20 },
                     basic_text_box(text2+text, 800)
                  )
               ),
               panel()
            )
         );

         //main_widget = new_(txbx);
      }

      {
         auto ixbx = margin(
            { 20, 20, 20, 20 },
            input_box("Placeholder")
         );

         auto frm = margin(
            { 20, 20, 20, 20 },
            layer(
               ixbx,
               panel()
            )
         );

         main_widget = new_(frm);
      }

      window main_window("Photon", { 1000, 600 }, colors::gray[30], my_app, main_widget);
      my_app.run();
   }

   return 0;
}
