/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon.hpp>
#include <photon/support/draw_utils.hpp>

using namespace photon;

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
      return margin({ 10, 10, 10, 10 }, layer(margin({ 10, 5, 10, 5 }, el), frame{}));
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
            margin({ 10, 10, 10, 10 }, group("Icons", std::move(icons)))
         )
      );
}

auto make_basic_text2()
{
   auto text_box = margin({ 10, 50, 10, 10 }, static_text_box{ text });

   return
      margin(
         { 10, 10, 10, 10 },
         group("Text Box", std::move(text_box))
      );
}

// Our dial
auto make_dial(char const* label)
{
   return dial(
      "knob_sprites_white_128x128.png", 128
      , "radial-lines.png"
      , label
      , 0.5
   );
}

auto make_controls()
{
   auto dial_info = make_dial("Volume");
   return align_center_middle(dial_info.first);
}

auto make_controls2()
{
   auto d = dial(radial_marks<15>(basic_knob<100>()), 0.5);
   return align_center_middle(d);
}

auto make_controls3()
{
   auto s = slider(basic_thumb<25>(), slider_marks<35>(basic_track<5>()), 0.5);
   return align_middle(xside_margin({ 120, 120 }, std::move(s)));
}

auto make_controls4()
{
//   auto s = slider(basic_thumb<25>(), basic_track<5, true>(), 0.5);
   auto s = slider(basic_thumb<25>(), slider_marks<35>(basic_track<5, true>()), 0.5);
   return align_center(yside_margin({ 50, 50 }, std::move(s)));
}


//auto make_controls()
//{
//   auto sl = margin(
//      { 20, 20, 20, 20 },
//      group("Text Box",
//         htile(
//            xside_margin(10, slider{}),
//            xside_margin(10, slider{}),
//            xside_margin(10, slider{}),
//            xside_margin(10, slider{}),
//            xside_margin(10, slider{}),
//            xside_margin(10, slider{}),
//            xside_margin(10,
//               vtile(
//                  yside_margin(10, knob{}),
//                  yside_margin(10, knob{}),
//                  yside_margin(10, knob{})
//               )
//            )
//         )
//      )
//   );
//
//   //main_widget = new_(std::move(sl));
//}

struct testing : element
{
   void draw(context const& ctx)
   {
   
   
    // Dial Knob
//      auto&  cnv = ctx.canvas;
//      circle c{200, 200, 32};
//      draw_knob(cnv, c, colors::black);
//      draw_indicator(cnv, c, 0.5, color{ 0, 127, 255, 200 }.level(2.0));
////      draw_indicator(cnv, c, 0.5, color{ 0, 127, 255, 200 }.level(2.0), true);

      // Shadow...
      // auto&  cnv = ctx.canvas;
      // cnv.fill_style(colors::white.opacity(0.7));
      // cnv.shadow_style({ 2, 4 }, 20, colors::black.opacity(0.4));
      // cnv.fill_round_rect({ 130, 130, 350, 350 }, 50);

      //auto const&    theme_ = get_theme();
      //auto&          canvas_ = ctx.canvas;
      //auto           state = canvas_.new_state();
      //
      //canvas_.shadow_style({ 2, 4 }, 20, colors::black.opacity(0.2));
      //
      //canvas_.line_width(0.5);
      //canvas_.fill_style(theme_.heading_font_color);
      //canvas_.font(theme_.heading_font, 72, theme_.heading_style);
      //canvas_.text_align(canvas_.middle | canvas_.center);
      //
      //float cx = ctx.bounds.left + (ctx.bounds.width() / 2);
      //float cy = ctx.bounds.top + (ctx.bounds.height() / 2);
      //
      //canvas_.fill_text(point{ cx, cy }, "Hello World");
   }
};

int main(int argc, const char* argv[])
{
   auto next_btn = button("Next");

   auto content = deck(
      make_basic_text(),
      make_basic_text2(),
      make_controls(),
      testing{},
      make_controls2(),
      make_controls3(),
      make_controls4()
   );

   int select = 0;

   app my_app{
      [&](view& view_)
      {
         next_btn.on_click = [&](bool)
         {
            if (++select == content.size())
               select = 0;
            content.select(select);
            view_.refresh(content);
         };

         auto top = htile(
            hspan(1.0, align_left(right_margin(8, label("Basics")))),
            hspan(0.1, link(next_btn))
         );
         auto main_pane = pane(top, link(content), false);
         auto main_element = margin({ 20, 20, 20, 20 }, main_pane);

         view_.content =
         {
            share(background{}),
            share(main_element)
         };
      }
   };
   return my_app.main(argc, argv);
}
