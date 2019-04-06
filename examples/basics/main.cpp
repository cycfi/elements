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

auto box = basic(
   [](context const& ctx)
   {
      auto& c = ctx.canvas;

      c.begin_path();
      c.round_rect(ctx.bounds, 4);
      c.fill_style(colors::gold.opacity(0.8));
      c.fill();
   }
);

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
   "life-force. Only a seeker of the galaxy may engender this wellspring of hope."
   "Yes, it is possible to eliminate the things that can destroy us, but not "
   "without wellbeing on our side. Where there is delusion, faith cannot thrive. "
   "You may be ruled by desire without realizing it. Do not let it eliminate "
   "the growth of your journey.\n\n"

   "--New-Age Bullshit Generator"
;

auto make_basic_text()
{
   auto fr = [](auto el)
   {
      return margin(
         { 10, 10, 10, 10 },
         layer(
            margin({ 10, 5, 10, 5 }, el),
            frame{}
         )
      );
   };

   auto eh = [=](char const* txt)
   {
      return fr(halign(0.5, heading{ txt }));
   };

   auto el = [=](double align, char const* txt)
   {
      return fr(halign(align, label{ txt }));
   };

   auto icons =
      margin({ 10, 50, 10, 10 },
         htile(
            align_center(icon{ icons::docs,           24 }),
            align_center(icon{ icons::right,          24 }),
            align_center(icon{ icons::trash,          24 }),
            align_center(icon{ icons::block,          24 }),
            align_center(icon{ icons::cw,             24 }),
            align_center(icon{ icons::attention,      24 })
         )
      );

   return
      margin(
         { 10, 10, 10, 10 },
         vtile(
            eh("Photon UI"),
            el(1.0, "Hello, Universe. I am Photon."),
            el(1.0, "A cross-platform, fine-grained, highly modular C++ GUI library."),
            el(0.0, "Based on a GUI framework written in the mid 90s named Pica."),
            el(0.5, "Now, Joel rewrote my code using modern C++14."),
            margin({ 10, 10, 10, 10 }, group("Icons", std::move(icons))),
            empty()
         )
      );
}

auto make_basic_text2()
{
   static auto textbox = vport(static_text_box{ text });
   return margin(
         { 10, 10, 10, 10 },
         link(textbox)
      );
}

auto make_elements()
{
   return
      margin({ 20, 10, 20, 10 },
         htile(
            margin({ 20, 20, 20, 20 }, pane("Static Text", make_basic_text(), 0.8f)),
            margin({ 20, 20, 20, 20 }, pane("Text Box", make_basic_text2(), 0.8f))
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
         share(make_elements())
      }
   );

   _app.run();
   return 0;
}
