/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon.hpp>
#include <photon/support/draw_utils.hpp>

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

auto make_popup_menu()
{
   auto popup  = popup_button("Popup Button");

   auto menu =
      layer(
         vtile(
               menu_item("Once you stop learning, you start dying"),
               menu_item("Everything should be made as simple as possible, but not simpler"),
               menu_item("In the middle of difficulty lies opportunity"),
               menu_item_spacer(),
               menu_item("A person who never made a mistake never tried anything new"),
               menu_item("Anger dwells only in the bosom of fools")
         ),
         menu_background{}
      );

   popup.menu(menu);

   return popup;
}

auto make_buttons(view& view_)
{
   constexpr auto bred     = colors::red.opacity(0.4);
   constexpr auto bgreen   = colors::green.level(0.7).opacity(0.4);
   constexpr auto bblue    = colors::blue.opacity(0.4);
   constexpr auto brblue   = colors::royal_blue.opacity(0.4);

   auto mbutton            = button("Momentary Button");
   auto tbutton            = toggle_button("Toggle Button", bred);
   auto lbutton            = share(latching_button("Latching Button", bgreen));
   auto reset              = button("Clear Latch", icons::lock_open, bblue);
   auto note               = button(icons::cog, "Setup", brblue);

   reset.on_click =
      [lbutton, &view_](bool) mutable
      {
         lbutton->value(0);
         view_.refresh(*lbutton);
      };

   return
      margin({ 20, 0, 20, 20 },
         vtile(
            top_margin(20, make_popup_menu()),
            top_margin(20, mbutton),
            top_margin(20, tbutton),
            top_margin(20, link(lbutton)),
            top_margin(20, reset),
            top_margin(20, note)
         )
      );
}

auto make_controls(view& view_)
{
   return
      margin({ 20, 10, 20, 10 },
         htile(
            margin({ 20, 20, 20, 20 }, pane("Buttons", make_buttons(view_), 0.8f)),
            margin({ 20, 20, 20, 20 }, pane("22222", element(), 0.8f))
         )
      );
}

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win.host());

   view_.content(
      {
         share(background{}),
         share(make_controls(view_))
      }
   );

   _app.run();
   return 0;
}
