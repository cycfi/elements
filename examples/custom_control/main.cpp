/*=============================================================================
   Copyright (c) 2020 Aravindhan K

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <infra/support.hpp>

using namespace cycfi::elements;
using namespace cycfi;

auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = rbox(bkd_color, 5);

// Function to calculate distance
float distance(point a, point b)
{
  double c = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
  return sqrt(c);
}

///////////////////////////////////////////////////////////////////////////////
// There are many ways to structure an application using elements. The user is
// not forced to do it one way or another. However, this specific examplar is
// the recommended way and can be considered as good practice.
//
// This example demonstrates:
//
//    1. How to make shared elements that connect to each other via
//       callbacks.
//    2. How to make custom elements that can interact with other elements.
//    3. How to create element class member variables that are either held by
//       value or by shared_ptr.
//    4. How to hold or link the elements in a composition.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// A Custom control element
///////////////////////////////////////////////////////////////////////////////
class my_custom_control : public tracker<>, public receiver<float>
{
public:

   using point_array = std::array<point, 4>;
   constexpr static float default_value = 100;

   // Element API
   void        draw(context const& ctx) override;
   bool        click(context const& ctx, mouse_button btn) override;
   void        keep_tracking(context const& ctx, tracker_info& track_info) override;
   bool        cursor(context const& ctx, point p, cursor_tracking status) override;

   // Receiver API
   float       value() const override     { return _radius; }
   void        value(float val) override  { _radius = val; }

   // Provide an on_change callback
   using on_change_f = std::function<void(float)>;
   on_change_f on_change;

private:

   bool        _mouse_over = false;
   int         _choosen_knob = -1;
   point_array _knobs;
   float       _radius = default_value;
};

///////////////////////////////////////////////////////////////////////////////
// Drawing
///////////////////////////////////////////////////////////////////////////////
void my_custom_control::draw(context const& ctx)
{
   // Draw a background.
   ctx.canvas.fill_style(color(0.8, 0.8, 0.8));
   ctx.canvas.fill_rect(ctx.bounds);

   point cursor_pos = ctx.cursor_pos();

   // Draw a circle.
   ctx.canvas.line_width(1.0);
   ctx.canvas.stroke_style(color(0, 0, 0.5));
   auto outer_ring = circle(center_point(ctx.bounds), _radius);
   ctx.canvas.circle(outer_ring);
   ctx.canvas.stroke();

   // Center points for four knobs.
   _knobs = point_array
      {
         point(outer_ring.bounds().left, center_point(ctx.bounds).y),
         point(outer_ring.bounds().right, center_point(ctx.bounds).y),
         point(center_point(ctx.bounds).x, outer_ring.bounds().top),
         point(center_point(ctx.bounds).x, outer_ring.bounds().bottom),
      };

   // Draw four knobs.
   for (auto knob : _knobs)
   {
      ctx.canvas.line_width(1.0);
      ctx.canvas.stroke_style(color(0, 0, 0.6));
      ctx.canvas.fill_style(color(1.0, 1.0, 1.0));
      ctx.canvas.circle(circle(knob, 5.0));
      ctx.canvas.fill_preserve();
      ctx.canvas.stroke();

      // Hightlight the knob which is near cursor.
      auto d = distance(knob, cursor_pos);
      if (_mouse_over && (d < 10.0))
      {
         ctx.canvas.line_width(2.0);
         ctx.canvas.stroke_style(color(0, 0, 0.8));
         ctx.canvas.circle(circle(knob, 7.0));
         ctx.canvas.stroke();
      }
   }

   // Highlight choose knob with green circle.
   if (_choosen_knob != -1)
   {
      ctx.canvas.line_width(2.0);
      ctx.canvas.stroke_style(color(0, 0.8, 0));
      ctx.canvas.circle(circle(_knobs[_choosen_knob], 7.0));
      ctx.canvas.stroke();
   }

   if (_mouse_over)
   {
      ctx.canvas.line_width(1.0);
      ctx.canvas.stroke_style(color(0, 0.5, 0.5, 0.2));
      ctx.canvas.circle(circle(cursor_pos.x, cursor_pos.y, 10.0));
      ctx.canvas.stroke();
   }
}

///////////////////////////////////////////////////////////////////////////////
// Responding to mouse click
///////////////////////////////////////////////////////////////////////////////
bool my_custom_control::click(context const& ctx, mouse_button btn)
{
   // Call the tracker<> click member function
   tracker<>::click(ctx, btn);

   if (btn.state == mouse_button::left)
   {
      // lock the knob clicked over if any.
      for (std::size_t i = 0, e = _knobs.size(); i != e; ++i)
      {
         auto d = distance(_knobs[i], btn.pos);
         if (d < 10.0)
            _choosen_knob =  i;
      }
   }
   return true;
}

///////////////////////////////////////////////////////////////////////////////
// Tracking the mouse after button click
///////////////////////////////////////////////////////////////////////////////
void my_custom_control::keep_tracking(context const& ctx, tracker_info& track_info)
{
   auto center = center_point(ctx.bounds);
   _radius = std::abs(distance(center, track_info.current));
   clamp(_radius, 50, 150);
   if (on_change)
      on_change(_radius);
   ctx.view.refresh(ctx.bounds);
}

///////////////////////////////////////////////////////////////////////////////
// Responding cursor hover
///////////////////////////////////////////////////////////////////////////////
bool my_custom_control::cursor(context const& ctx, point p, cursor_tracking status)
{
   switch (status)
   {
      case cursor_tracking::hovering:
      case cursor_tracking::entering:
         _mouse_over = true;
         ctx.view.refresh(ctx.bounds);
         break;
      case cursor_tracking::leaving:
         _mouse_over = false;
   };
   return false;
}

///////////////////////////////////////////////////////////////////////////////
// Our application
///////////////////////////////////////////////////////////////////////////////
class my_app : public app
{
public:

                     my_app(int argc, char* argv[]);

private:

   constexpr static float default_value = my_custom_control::default_value;
   using dial_ptr = std::shared_ptr<dial_base>;
   using label_ptr = decltype(share(label("")));

   auto              make_control();
   auto              make_button();
   auto              make_dial();
   auto              make_label();
   void              set_value(float val);
   auto              make_control_panel();

   window            _win;
   view              _view;
   my_custom_control _my_control;
   dial_ptr          _dial;
   label_ptr         _label;
};

auto my_app::make_control()
{
   return
      layer(
         align_center_middle(
            fixed_size({ 400, 400 },
               link(_my_control)             // Link our control
            )),
         image{ "wall.jpg" }
      );
}

auto my_app::make_button()
{
   // Make a button for sharing (making it a shared_ptr)
   auto btn = share(button("Reset Size"));

   // Attach an on_click function
   btn->on_click =
      [this](bool /*state*/)
      {
         set_value(default_value);           // Reset default value
      };

   return
      margin({ 50, 10, 50, 10 },
         hold(btn)                           // Hold our shared button
      );
}

auto my_app::make_dial()
{
   float const knob_scale = 1.0;
   sprite knob = sprite{
      "knob_sprites_150x150_darker.png",
      150 * knob_scale, knob_scale
   };

   _dial = share(dial(knob, 0.5));           // Share a dial

   // Attach an on_change function
   _dial->on_change =
      [this](double val)
      {
         // Un-normalize from 0.0...1.0 to 50...150
         set_value(50 + (val * 100));
      };

   // Connect our control via _my_control.on_change
   _my_control.on_change =
      [this](double val)
      {
         set_value(val);
      };

   return align_center_middle(hold(_dial));  // Hold the dial
}

auto my_app::make_label()
{
   _label = share(label(""));                // Share a label
   return align_center_middle(hold(_label)); // Hold the label
}

void my_app::set_value(float val)
{
   _my_control.value(val);                   // Set _my_control radius
   _dial->value((val - 50) / 100);           // Set _dial value (normalized to 0.0...1.0)

   // Set the label text
   _label->set_text("Radius: " + std::to_string(int(std::round(val))));

   // Refresh the view
   _view.refresh();
}

auto my_app::make_control_panel()
{
   return margin(
      { 50, 20, 50, 20 },
      layer(
         vgrid(
            span(5, make_dial()),
            span(1, make_label()),
            span(1, make_button())
         ),
         panel{},
         background
      )
   );
}

my_app::my_app(int argc, char* argv[])
 : app{ argc, argv, "Custom Control", "com.cycfi.custom-control" }
 , _win{ name() }
 , _view{ _win }
{
   _win.on_close = [this]() { stop(); };

   _view.content(
      htile(                  // Top layer
         make_control_panel(),
         make_control()
      ),
      background              // Background layer
   );

   // Set the default value
   set_value(default_value);
}

///////////////////////////////////////////////////////////////////////////////
// main function
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
   my_app _app(argc, argv);
   _app.run();
   return 0;
}
