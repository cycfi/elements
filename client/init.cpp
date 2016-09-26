/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <infinity/application.hpp>
#include <cstdlib>

namespace client
{
   using namespace photon;

   // Main window background color
   auto bkd_color = color{ 35, 35, 37, 255 };

   struct background : widget
   {
      void draw(context const& ctx)
      {
         auto&  cnv = ctx.canvas;
         cnv.fill_style(bkd_color);
         cnv.fill_rect(ctx.bounds);
      }
   };

   auto make_button()
   {
      auto btn = button("Hello!!!");
      return margin({ 100, 100, 100, 100 }, btn);
      //return align_center_middle(fixed_size({ 40, 120 }, btn));
   }

   struct box : widget
   {
      void draw(context const& ctx)
      {
         auto& c = ctx.canvas;
         c.fill_style(colors::gold.opacity(0.8));
         c.fill_round_rect(ctx.bounds, 4);
      }
   };

   auto make_vtile()
   {
      auto _box = top_margin(
         { 20 },
         hsize(100, box{})
      );

      return margin(
         { 20, 0, 20, 20 },
         vtile(
            halign(0.0, _box),
            halign(0.2, _box),
            halign(0.4, _box),
            halign(0.6, _box),
            halign(0.8, _box),
            halign(1.0, _box)
         )
      );
   }

   auto make_dropdown_menu()
   {
      auto dropdown  = popup_button("Dropdown");

      auto menu =
         layer(
            yside_margin({ 5, 5 },
               vtile(
                  menu_item("Perfection is achieved,"),
                  menu_item("not when there is nothing more to add,"),
                  menu_item("but when there is nothing left to take away."),
                  menu_item_spacer(),
                  menu_item("We are what we repeatedly do;"),
                  menu_item("excellence, then,"),
                  menu_item("is not an act but a habit.")
               )
            ),
            menu_background{}
         );

      dropdown.menu(menu);

      return dropdown;
   }

   auto make_buttons(photon::view& view_)
   {
      auto bred      = colors::red.opacity(0.4);
      auto bgreen    = colors::green.level(0.7).opacity(0.4);
      auto bblue     = colors::blue.opacity(0.4);
      auto brblue    = colors::royal_blue.opacity(0.4);

      auto mbutton   = button("Momentary Button");
      auto tbutton   = toggle_button("Toggle Button", bred);
      auto lbutton   = ref(latching_button("Latching Button", bgreen));
      auto reset     = button("Clear Latch", icons::lock_open, bblue);
      auto note      = button(icons::cog, "Setup", brblue);

      reset.on_click =
         [lbutton, &view_](bool) mutable
         {
            lbutton.get().value(0);
            view_.refresh();
         };

      return
         margin({ 20, 20, 20, 20 },
             group("Buttons",
                margin({ 20, 30, 20, 20 },
                   vtile(
                      top_margin(20, make_dropdown_menu()),
                      top_margin(20, mbutton),
                      top_margin(20, tbutton),
                      top_margin(20, lbutton),
                      top_margin(20, reset),
                      top_margin(20, note)
                   )
                ),
             0.9, false)
         );
   }

   std::string const text =
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
      "life-force. Only a seeker of the galaxy may engender this wellspring of hope. "
      "Yes, it is possible to eliminate the things that can destroy us, but not "
      "without wellbeing on our side. Where there is delusion, faith cannot thrive. "
      "You may be ruled by desire without realizing it. Do not let it eliminate "
      "the growth of your journey.\n\n"

      "    Я могу есть стекло, оно мне не вредит.\n"
      "    —New-Age Bullshit Generator"
      ;

   auto make_flow()
   {
      static auto c = vector_composite<flowable_container>{};
      for (int i = 0; i < 100; ++i)
      {
         auto w = 10 + ((double(std::rand()) * 90) / RAND_MAX);
         auto _box = margin({ 5, 5, 5, 5 }, fixed_size({ float(w), 20 }, box{}));
         c.push_back(share(_box));
      }
      return align_top(flow(c));
   }

   auto make_text()
   {
      return margin({ 20, 20, 20, 20 }, text_box{text});
   }

   void  init(photon::view& view_)
   {
      //view_.content.push_back(share(background{}));
      view_.content.push_back(share(make_text()));
      //view_.content.push_back(share(make_flow()));
      //view_.content.push_back(share(make_buttons(view_)));

      //view_.maintain_aspect(true);
      //view_.app = std::make_shared<infinity::application>(view_);
   }
}
