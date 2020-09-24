/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto box = draw(
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
   auto fr = [](auto&& el, float top = 10)
   {
      return margin(
         { 10, top, 10, 10 },
         layer(
            margin({ 10, 5, 10, 5 }, std::move(el)),
            frame{}
         )
      );
   };

   auto eh = [=](char const* txt)
   {
      return fr(halign(0.5, heading{ txt }), 0);
   };

   auto el = [=](auto const& label_)
   {
      return fr(halign(0.5, label_));
   };

   auto icons =
      margin({ 10, 0, 10, 10 },
         htile(
            align_center(icon{ icons::docs }),
            align_center(icon{ icons::right }),
            align_center(icon{ icons::trash }),
            align_center(icon{ icons::block }),
            align_center(icon{ icons::cw }),
            align_center(icon{ icons::attention }),
            align_center(icon{ icons::menu }),
            align_center(icon{ icons::lightbulb }),
            align_center(icon{ icons::sliders }),
            align_center(icon{ icons::exchange })
         )
      );

   static float const grid[] = { 0.32, 1.0 };

   auto my_label = [=](auto text)
   {
      return right_margin(10, label(text).text_align(canvas::right));
   };

   auto my_input = [=](auto caption, auto input)
   {
      return bottom_margin(10, hgrid(grid, my_label(caption), input));
   };

   // This is an example on how to add an on_text callback:
   auto in = input_box("Show me the money");
   in.second->on_text =
      [input = in.second.get()](std::string_view text)
      {
         if (text == "$1000000")
         {
            input->set_text("Thank You!!!");
            input->select_all();
         }
      };

   auto text_input =
      pane("Text Input",
         margin({ 10, 5, 10, 5 },
            vtile(
               my_input("Gimme Some", in.first),
               my_input("Gimme Some More", input_box("Show me more").first),
               my_input("Cute Text Boxes",
                  htile(
                     input_box(0.7).first,
                     left_margin(10, input_box(0.7).first),
                     left_margin(10, input_box(0.7).first)
                  )
               )
            )
         ))
      ;

   auto labels =
      top_margin(20, pane("Labels",
         vtile(
            el(label("Hello, Universe. This is Elements.")
               .font(font_descr{ "Open Sans" }.semi_bold())
               .font_color(colors::antique_white)
               .font_size(18)
            ),
            el(
               vtile(
                  label("A cross-platform,")
                     .text_align(canvas::center),
                  label("fine-grained,")
                     .text_align(canvas::left),
                  label("highly modular C++ GUI library.")
                     .text_align(canvas::right),
                  label("Based on a GUI framework written in the mid 90s named Pica."),
                  label("Now, Joel rewrote my code using modern C++17.")
               )
            )
         )))
      ;

   return
      margin(
         { 10, 0, 10, 10 },
         vtile(
            text_input,
            labels,
            top_margin(20, pane("Icons", std::move(icons))),
            empty()
         )
      );
}

auto make_basic_text2()
{
   auto textbox = share(vport(static_text_box{ text }));
   return hmin_size(350, margin(
         { 10, 0, 10, 10 },
         hold(textbox)
      ));
}

auto make_elements()
{
   return
      max_size({ 1280, 640 },
         margin({ 20, 10, 20, 10 },
            htile(
               margin({ 20, 20, 20, 20 }, make_basic_text()),
               margin({ 20, 20, 20, 20 },
                  pane("Text Box", make_basic_text2())
               )
            )
         )
      );
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Text and Icons", "com.cycfi.text-and-icons");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_elements(),
      background
   );

   _app.run();
   return 0;
}
