/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <algorithm>
#include <random>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

constexpr auto bred     = colors::red.opacity(0.4);
constexpr auto bgreen   = colors::green.level(0.7).opacity(0.4);
constexpr auto bblue    = colors::blue.opacity(0.4);
constexpr auto brblue   = colors::royal_blue.opacity(0.4);
constexpr auto pgold    = colors::gold.opacity(0.8);

///////////////////////////////////////////////////////////////////////////////
// Now we have custom buttons. It uses the same button logic as with standard
// element buttons, but you supply the code for drawing the button.
//
// Here's how to make a custom button.
///////////////////////////////////////////////////////////////////////////////
struct my_custom_button : button_styler_base
{
   view_limits             limits(basic_context const& ctx) const override;
   void                    draw(context const& ctx) override;
};

view_limits my_custom_button::limits(basic_context const& ctx) const
{
   // Returns the min and max limits. The example below sets the
   // vertical size to exactly 40, while the horizontal size has a
   // minimum of 30 and fully stretchable to `full_extent`.
   return {{200, 30}, {full_extent, 30}};
}

void my_custom_button::draw(context const& ctx)
{
   auto& cnv = ctx.canvas;    // The canvas
   auto bounds = ctx.bounds;  // The bounding rectangle

   // This is the state of the button. Adjust the rendering based on these if
   // necessary. For this simple example, we wiill deal only with `value` and
   // `hilite`.

   auto state = value();
   bool value = state.value;     // button is on or off
   bool hilite = state.hilite;   // cursor is hovering over the button
   bool enabled = ctx.enabled;   // button is enabled or disabled

   bounds = bounds.inset(1, 1);
   if (value)
      bounds = bounds.move(1, 1);   // Simulate click

   // Render the button using the library
   cnv.fill_style(colors::dark_slate_blue);
   cnv.fill_round_rect(bounds, 8);
   cnv.line_width(1);
   cnv.stroke_style(
      hilite?
         colors::antique_white.opacity(0.5) :
         colors::antique_white.opacity(0.3)
      );
   cnv.stroke_round_rect(bounds, 8);

   cnv.font(font_descr{"Roboto", 14.0}.italic());
   cnv.fill_style(hilite? pgold.level(1.2) : pgold);
   cnv.text_align(cnv.center | cnv.middle);
   cnv.fill_text("My Custom Button", center_point(bounds));
}

auto make_custom_button()
{
   return momentary_button(my_custom_button());
}

auto make_buttons(view& view_)
{
   auto mbutton         = button("Momentary Button");
   auto tbutton         = toggle_button("Toggle Button", 1.0, bred);
   auto lbutton         = share(latching_button("Latching Button", 1.0, bgreen));
   auto reset           = share(button("Clear Latch", icons::lock_open, 1.0, bblue));
   auto note            = button(icons::cog, "Setup", 1.0, brblue);
   auto prog_bar        = share(progress_bar(rbox(colors::black), rbox(pgold)));
   auto prog_advance    = icon_button(icons::plus);
   auto disabled_button = button("Disabled Button");

   // This is the new way of making buttons that is consistent with the label
   // element interface. The old way will still be available, but this new
   // interface gives better clarity and flexibility at the expense of
   // verbosity.

   auto left            =  momentary_button(
                              button_styler{"Left"}
                                 .align_left()
                                 .icon(icons::left_circled)
                                 .icon_left()
                                 .body_color(bred)
                           );
   auto center          =  momentary_button(
                              button_styler{"Center"}
                                 .body_color(bblue)
                           );
   auto right           =  momentary_button(
                              button_styler{"Right"}
                                 .align_right()
                                 .icon(icons::right_circled)
                                 .body_color(bgreen)
                           );

   // The corner radii can be individually controlled with the 
   // `corner_radius`, `rounded`, and `rounded_corner` methods.
   auto left_rounded    =  momentary_button(
                              button_styler{"Rounded Left"}
                                 .align_left()
                                 .icon(icons::left_circled)
                                 .icon_left()
                                 .body_color(bred)
                                 .rounded_left(10)
                           );

   auto center_square   =  momentary_button(
                              button_styler{"Square Center"}
                                 .body_color(bblue)
                                 .corner_radius(0)
                           );

   auto right_rounded   =  momentary_button(
                              button_styler{"Rounded Right"}
                                 .align_right()
                                 .icon(icons::right_circled)
                                 .body_color(bgreen)
                                 .corner_radius(0, 10, 10, 0)
                           );

   auto slide_btn1      = slide_switch();
   auto slide_btn2      = slide_switch();

   slide_btn1.value(true);
   slide_btn2.enable(false); // Disable

   // Now we have custom buttons
   auto custom          = make_custom_button();

   disabled_button.enable(false); // Disable this
   reset->enable(false); // Disable initially

   lbutton->on_click =
      [reset = get(reset), &view_](bool) mutable
      {
         if (auto p = reset.lock())
         {
            (*p)->set_icon(icons::lock);
            p->enable(true);
            view_.refresh(*p);
         }
      };

   reset->on_click =
      [lbutton = get(lbutton), reset = get(reset), &view_](bool) mutable
      {
         if (auto p = lbutton.lock())
         {
            p->value(0);
            view_.refresh(*p);
         }

         if (auto p = reset.lock())
         {
            (*p)->set_icon(icons::lock_open);
            p->enable(false);
            view_.refresh(*p);
         }
      };

   prog_advance.on_click =
      [prog_bar, &view_](bool) mutable
      {
         auto val = prog_bar->value();
         if (val > 0.9)
            prog_bar->value(0.0);
         else
            prog_bar->value(val + 0.125);
         view_.refresh(*prog_bar);
      };

   return
      margin({20, 0, 20, 20},
         vtile(
            margin_top(20, mbutton),
            margin_top(20, tbutton),
            margin_top(20, hold(lbutton)),
            margin_top(20, hold(reset)),
            margin_top(20, note),
            margin_top(20, htile(
               margin_right(3, valign(0.5, prog_advance)),
               vsize(27, hold(prog_bar))
            )),
            margin_top(20, disabled_button),
            margin_top(20,
               htile(
                  label("Enabled"),
                  hspace(10),
                  align_left(slide_btn1),
                  hmargin(10, label("Slide Buttons")),
                  align_right(slide_btn2),
                  hspace(10),
                  label("Disabled")
               )
            ),
            margin_top(20, htile(left, center, right)),
            margin_top(20, htile(left_rounded, center_square, right_rounded)),
            margin_top(20, custom)
         )
      );
}

auto make_controls(view& view_)
{
   auto  check_box1 = check_box("Reionizing electrons");
   auto  check_box2 = check_box("The Nexus Meridian Unfolding");
   auto  check_box3 = check_box("Serenity Dreamscape Exploration");
   auto  check_box4 = check_box("Forever Disabled");

   check_box1.value(true);
   check_box2.value(true);
   check_box3.value(true);
   check_box4.enable(false); // Disable this one

   auto  check_boxes =
         group("Check boxes",
            margin({10, 10, 20, 20},
               margin_top(25,
                  vtile(
                     margin_top(10, align_left(check_box1)),
                     margin_top(10, align_left(check_box2)),
                     margin_top(10, align_left(check_box3)),
                     margin_top(10, align_left(check_box4))
                  )
               )
            )
         );

   auto  radio_button1 = radio_button("Eons from now");
   auto  radio_button2 = radio_button("Ultra-sentient particles");
   auto  radio_button3 = radio_button("The stratosphere is electrified");
   auto  radio_button4 = radio_button("No, no, not me");

   radio_button1.select(true);
   radio_button4.enable(false); // Disable this one

   auto  radio_buttons =
         group("Radio Buttons",
            margin({10, 10, 20, 20},
               margin_top(25,
                  vtile(
                     margin_top(10, align_left(radio_button1)),
                     margin_top(10, align_left(radio_button2)),
                     margin_top(10, align_left(radio_button3)),
                     margin_top(10, align_left(radio_button4))
                  )
               )
            )
         );

   auto indicator_color = get_theme().indicator_color;

   auto disabled_icon_button = icon_button(icons::block, 1.2);
   disabled_icon_button.enable(false);

   auto  icon_buttons =
         group("Icon Buttons",
            margin({10, 10, 20, 10},
               vtile(
                  margin_top(35,
                     htile(
                        align_center(toggle_icon_button(icons::power, 1.2, indicator_color)),
                        align_center(icon_button(icons::magnifying_glass, 1.2)),
                        align_center(icon_button(icons::left_circled, 1.2)),
                        align_center(toggle_icon_button(icons::left, icons::right, 1.2)),
                        align_center(disabled_icon_button)
                     )
                  )
               )
            )
         );

   float const button_scale = 1.0/4;
   sprite power_button = sprite{"power_180x632.png", 158*button_scale, button_scale};
   sprite phase_button = sprite{"phase_180x632.png", 158*button_scale, button_scale};
   sprite mail_button = sprite{"mail_180x632.png", 158*button_scale, button_scale};
   sprite transpo_button = sprite{"transpo_180x632.png", 158*button_scale, button_scale};

   auto  sprite_buttons =
         group("Sprite Buttons",
            margin({10, 10, 20, 10},
               vtile(
                  margin_top(35,
                     htile(
                        align_center(toggle_button(power_button)),
                        align_center(toggle_button(phase_button)),
                        align_center(momentary_button(mail_button)),
                        align_center(toggle_button(transpo_button))
                     )
                  )
               )
            )
         );

   return
      vtile(
         htile(
            make_buttons(view_),
            vtile(
               margin({20, 20, 20, 20}, check_boxes),
               margin({20, 20, 20, 20}, radio_buttons)
            )
         ),
         htile(
            hmin_size(250, margin({20, 20, 20, 20}, icon_buttons)),
            hmin_size(250, margin({20, 20, 20, 20}, sprite_buttons))
         )
      );
}

int main(int argc, char* argv[])
{
   app _app("Buttons");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_controls(view_),
      background
   );

   _app.run();
   return 0;
}
