/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

/*=============================================================================
   Elements does not dictate a particular method for organizing a GUI
   application. You have the flexibility to structure your application
   according to your preferences. For this example, we will keep it simple.
   We will create our elements as global variables. Of course, as any
   competent C++ programmer would say, we want to avoid globals as much as
   possible. In a real-world application, you will want to place your objects
   encapsulated in classes.

   In this example, we will make six sliders and three dials, lay them out in
   groups and link each one so that moving one will move the others.
=============================================================================*/

// We normally don't want to use `using namespace`, but for the sake of
// demonstartion, we will do that here.
using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

// We make six sliders. Three horizontal and three vertical.
using slider_ptr = std::shared_ptr<basic_slider_base>;
slider_ptr hsliders[3];
slider_ptr vsliders[3];

// We make three dials.
using dial_ptr = std::shared_ptr<basic_dial>;
dial_ptr dials[3];

// We make linear slider markers that are placed below the sliders.
template <bool is_vertical>
auto make_markers()
{
   auto track = basic_track<5, is_vertical>();
   return slider_labels<10>(
      slider_marks_lin<40>(track),     // Track with marks
      0.8,                             // Label font size (relative size)
      "0", "1", "2", "3", "4",         // Labels
      "5", "6", "7", "8", "9", "10"
   );
}

// Here, we make a horizontal slider with slider markers.
auto make_hslider(int index)
{
   hsliders[index] = share(      // Use `share` to make shared elements.
      slider(
         basic_thumb<25>(),      // Basic thumb 25px in diameter
         make_markers<false>(),  // The slider markers
         (index + 1) * 0.25)     // The default slider value
      );

   // Place a margin of 20 pixels at the left and right of the slider and
   // align the slider at the middle. Use `hold` to hold shared elements.
   return align_middle(hmargin({20, 20}, hold(hsliders[index])));
}

// Make three horizontal sliders. Place them in a `vtile`, and give it a
// minimum width of 300px.
auto make_hsliders()
{
   return hmin_size(300,         // Set the minimum size to 300px
      vtile(                     // Make a `vtile`
         make_hslider(0),        // Slider 1
         make_hslider(1),        // Slider 2
         make_hslider(2)         // Slider 3
      )
   );
}

// Here, we make a vertical slider with slider markers.
auto make_vslider(int index)
{
   vsliders[index] = share(      // Use `share` to make shared elements.
      slider(
         basic_thumb<25>(),      // Basic thumb 25px in diameter
         make_markers<true>(),   // The slider markers
         (index + 1) * 0.25      // The default slider value
      )
   );

   // Place a margin of 20 pixels at the top and bottom of the slider and
   // align the slider at the center. Use `hold` to hold shared elements.
   return align_center(vmargin({20, 20}, hold(vsliders[index])));
}

// Make three vertical sliders. Place them in an `htile`, and give it a
// minimum width of 300px.
auto make_vsliders()
{
   return hmin_size(300,         // Set the minimum size to 300px
      htile(                     // Make a `vtile`
         make_vslider(0),        // Slider 1
         make_vslider(1),        // Slider 2
         make_vslider(2)         // Slider 3
      )
   );
}

// Here, we make a dial with radial markers around the dial.
auto make_dial(int index)
{
   dials[index] = share(               // Use `share` to make shared elements.
      dial(
         radial_marks<20>(             // Place radial markers with 20 tick marks
            basic_knob<50>()           // Make a basic knob 50px in diameter
         ),
         (index + 1) * 0.25            // The default slider value
      )
   );

   auto markers = radial_labels<15>(   // Add radial labels offset 15px from the knob edge
      hold(dials[index]),              // Use `hold` to hold shared elements.
      0.7,                             // Label font size (relative size)
      "0", "1", "2", "3", "4",         // Labels
      "5", "6", "7", "8", "9", "10"
   );

   // Align the knob center-middle
   return align_center_middle(markers);
}

// Make three dial. Place them in a `vtile`.
auto make_dials()
{
   return hmargin(20,            // Place a horizontal (left and right) margin of 20px
      vtile(                     // Make a `vtile`
         make_dial(0),           // Dial 1
         make_dial(1),           // Dial 2
         make_dial(2)            // Dial 3
      )
   );
}

// Make the controls. Group the sliders and dials inside panes.
auto make_controls()
{
   return
      margin({20, 10, 20, 10},               // Margin around the controls
         vmin_size(400,                      // Make the minimum height 400px
            htile(
               margin({20, 20, 20, 20},      // Margin around the vertical sliders
                  pane(                      // Place the sldiers in a pane
                     "Vertical Sliders",     // Title
                     make_vsliders(),        // Make the vertical sliders
                     0.8f                    // Relative font size of the pane title
                  )
               ),
               margin({20, 20, 20, 20},      // Margin around the horizontal sliders
                  pane(                      // Place the sliders in a pane
                     "Horizontal Sliders",   // Title
                     make_hsliders(),        // Make the horizontal sliders
                     0.8f                    // Relative font size of the pane title
                  )
               ),
               hstretch(                     // Allow the knobs pane to stretch 0.5 less
                  0.5,
                  margin({20, 20, 20, 20},   // Margin around the dials
                     pane(                   // Place the sldiers in a pane
                        "Knobs",             // Title
                        make_dials(),        // Make the dials
                        0.8f                 // Relative font size of the pane title
                     )
                  )
               )
            )
         )
      );
}

// Here, we link the controls such that the Nth slider or knob will move together.
void link_control(int index, view& view_)
{
   vsliders[index]->on_change =              // When vsliders[index] moves,
      [index, &view_](double val)
      {
         hsliders[index]->value(val);        // set the value of hsliders[index]
         dials[index]->value(val);           // set the value of dials[index]
         view_.refresh(*hsliders[index]);    // Refresh hsliders[index]
         view_.refresh(*dials[index]);       // Refresh dials[index]
      };

   hsliders[index]->on_change =              // When hsliders[index] moves,
      [index, &view_](double val)
      {
         vsliders[index]->value(val);        // set the value of vsliders[index]
         dials[index]->value(val);           // set the value of dials[index]
         view_.refresh(*vsliders[index]);    // Refresh vsliders[index]
         view_.refresh(*dials[index]);       // Refresh dials[index]
      };

   dials[index]->on_change =                 // When dials[index] moves,
      [index, &view_](double val)
      {
         vsliders[index]->value(val);        // set the value of vsliders[index]
         hsliders[index]->value(val);        // set the value of hsliders[index]
         view_.refresh(*vsliders[index]);    // Refresh vsliders[index]
         view_.refresh(*hsliders[index]);    // Refresh hsliders[index]
      };
}

// Link the controls.
void link_controls(view& view_)
{
   link_control(0, view_);
   link_control(1, view_);
   link_control(2, view_);
}

int main(int argc, char* argv[])
{
   app _app("Basic Sliders And Knobs");      // Make an app
   window _win(_app.name());                 // Make a window
   _win.on_close =                           // Set the app to stop when window is closed
      [&_app]()
      {
         _app.stop();
      };

   view view_(_win);                         // Make a view

   view_.content(
      make_controls(),                       // Make our controls
      background                             // Add a background
   );

   link_controls(view_);                     // Link the controls.
   _app.run();                               // Run the app
   return 0;
}
