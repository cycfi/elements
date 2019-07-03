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

std::string const text =
   "We are being called to explore the cosmos itself as an "
   "interface between will and energy. It is a sign of things "
   "to come. The dreamtime is approaching a tipping point."
   ;

// A Popup
void make_popup(view& view_)
{
   auto textbox = static_text_box{ text };
   auto ok_button = share(button("OK", 1.0, get_theme().indicator_color));

   auto popup = share(
      align_center_middle(
         fixed_size({ 400, 180 },
         layer(
            margin({ 20, 20, 20, 20 },
               vtile(
                  htile(
                     align_top(icon{ icons::attention, 2.5 }),
                     left_margin(20, std::move(textbox))
                  ),
                  align_right(hsize(100, hold(ok_button)))
               )
            ),
            panel{}
         )
      ))
   );

   view_.add(popup);

   ok_button->on_click =
      [&view_, p = weak(popup)](bool)
      {
         view_.io().post(
            [&view_, p]
            {
               auto popup = p.lock();
               if (popup)
                  view_.remove(popup);
            }
         );
      };
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

   view_.defer(1500ms,
      [&view_]()
      {
         make_popup(view_);
      }
   );

   _app.run();
   return 0;

}
