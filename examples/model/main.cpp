/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

/*=============================================================================
   Elements does not dictate a particular method for organizing a GUI
   application. You have the flexibility to structure your application
   according to your preferences. Here's a breakdown of the essential steps:

   1. Build an elements hierarchy and install that as your content into the
      view.

   2. Establish a connection between the GUI elements and the application
      through callbacks such as the button's `on_click` and the input text
      box's `on_enter` hooks. These callbacks are invoked when the user
      clicks the button or presses the enter key in the text box, for
      example.

   3. Ensure the GUI is updated to accurately represent the application state
      whenever changes occur. The application may need the ability to inspect
      the state of the GUI elements or set them to a specific state at any
      given time.

   A typical approach for steps 2 and 3 involves holding different GUI
   elements as private members within an application or GUI class. These
   elements are managed as shared pointers, created using the `share(e)`
   function and held in the element hieiarchy using the `hold(p)` function.
   The GUI class oversees the interconnections and presents a higher-level
   view to the application through a well-defined API.

   This example presents a more elegant way to structure an elements
   application using Models. The Model (elements/model.hpp) serves as an
   abstraction for a data type that is linked to one or more user interface
   elements. The actual data is accessed and modified through the `get` and
   `set` member functions of the derived class. A user interface element can
   be linked to a `model` by supplying an `update_function` via the
   `on_update(f)` member function.

   The Model does not care about the GUI element types it is interacting
   with. It is an abstract data type that models its underlying data type
   (e.g. float, int, enum, etc.). It looks and acts like a concrete type.

   For example, a `value_model<float>` acts just like a float. You can assign
   a value to it:

      m = 1.0;

   or extract its value:

      float i = m;

   You can also update the GUI elements that are linked to it:

      m.update(0.5);

   These capabilities implement 2 and 3 of the requirements.

   The `value_model<float>` is a derived class of the template class `model`.
   It specifically addresses the typical scenario where the data is
   internally stored by value within the class. The class includes `get` and
   `set` member functions following the `Model` concept required by the model
   template, facilitating the retrieval and modification of data.

      Note: While this example utilizes the `value_model<float>`, you have
      the flexibility to create your custom subclass of the template class
      model to implement more tailored methods of accessing your data.

   Adopting the model paradigm, the application and the user interface engage
   with the model independently, unaware of each other's existence. The model
   serves as a central hub, facilitating and coordinating interactions
   between them:

      application <----> model <----> GUI

   The model decouples the application from the GUI.

   1. The application is aware of the model, manages and interacts with it.
   2. The GUI is also aware of the model and links to it via callbacks.
   3. The model is unaware of the GUI.
   4. The model may be aware of the application, e.g. if it needs to get data
      from the application, but not necessarily, e.g. if it holds the data
      itself.
   5. The GUI and the application are both unaware of each other.

   Number 5 is an important design principle known as "decoupling,"
   emphasizing the separation of concerns and promoting independence between
   the GUI and the application components. The advantages of decoupling in a
   software design context include, modularity, scalability, and reusability,
   and ease of maintainance.

   In this example, we present a very simple model, comprising of a floating
   point value and a preset. As the GUI elements are being built, they attach
   themselves to the model by utilizing its on_update(f) member function at
   different nodes within the elements hierarchy. This illustrates the
   approach of designing the user interface based on models.
=============================================================================*/
namespace elements = cycfi::elements;
namespace icons = elements::icons;

using elements::rgba;
using elements::box;
using elements::value_model;
using elements::dial;
using elements::radial_marks;
using elements::radial_labels;
using elements::basic_knob;
using elements::selection_menu;
using elements::input_box;
using elements::label;
using elements::hspace;
using elements::view;
using elements::app;
using elements::window;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = elements::box(bkd_color);

// This is our simple model, comprising of a preset (an enum)
// and a value normalized to 0.0 to 1.0.
struct my_model
{
   enum preset
   {
      preset_none,
      preset_100_percent,
      preset_75_percent,
      preset_50_percent,
      preset_25_percent,
      preset_0_percent,
   };

   value_model<float>   _value = 1.0;
   value_model<preset>  _preset = preset_100_percent;
};

// Create a dial and establish its connection with the model.
auto make_dial(my_model& model, view& view_)
{
   // Make a dial
   auto dial_ptr =
      share(
         dial(
            radial_marks<20>(basic_knob<80>()),
            1.0
         )
      );

   // When the user interacts with the dial, this will be called.
   // We assign the new value to the model.
   dial_ptr->on_change =
      [&model](double val)
      {
         if (model._value != val)
         {
            model._value = val;
            model._preset = my_model::preset_none;
         }
      };

   // When a new value is assigned to the model, we want to update
   // the dial and refresh the view.
   model._value.on_update(
      [&view_, dial_ptr](double val)
      {
         dial_ptr->value(val);
         view_.refresh(*dial_ptr);
      }
   );

   auto control = radial_labels<15>(
      hold(dial_ptr),
      0.7,                                // Label font size (relative size)
      "0", "1", "2", "3", "4",            // Labels
      "5", "6", "7", "8", "9", "10"
   );

   return align_center_middle(control);
}

// Create a preset menu and establish its connection with the model.
auto make_preset_menu(my_model& model, view& view_)
{
   // These are the menu labels
   static char const* preset_labels[] = {
         "100 Percent",
         "75 Percent",
         "50 Percent",
         "25 Percent",
         "0 Percent",
      };

   // We use a map to associate the menu labels with the preset enumeration.
   static auto preset_map =
      std::unordered_map<std::string_view, my_model::preset> {
         { preset_labels[0], my_model::preset_100_percent  },
         { preset_labels[1], my_model::preset_75_percent },
         { preset_labels[2], my_model::preset_50_percent },
         { preset_labels[3], my_model::preset_25_percent  },
         { preset_labels[4], my_model::preset_0_percent  }
      };

   // We make a selection menu.
   auto preset_menu =
      selection_menu(
         [&model](std::string_view select_str)
         {
            // This is called when the user selects an item in the menu.

            // Convert the selected menu item to the preset enumeration.
            auto select = preset_map[select_str];

            // Set the model's preset
            model._preset = select;

            // Set the value of the model accordingly.
            switch (select)
            {
               case my_model::preset_100_percent:
                  model._value = 1.0;
                  break;
               case my_model::preset_75_percent:
                  model._value = 0.75;
                  break;
               case my_model::preset_50_percent:
                  model._value = 0.5;
                  break;
               case my_model::preset_25_percent:
                  model._value = 0.25;
                  break;
               case my_model::preset_0_percent:
                  model._value = 0.0;
                  break;
               default:
                  break;
            };
         },
         preset_labels
      );

   // When a new preset is assigned to the model, we want to update
   // the menu text and refresh the view.
   model._preset.on_update(
      [&view_, label = preset_menu.second, &model](my_model::preset val)
      {
         if (val == my_model::preset_none)
         {
            // Prepend '*' to the string to indicate that it is being edited.
            auto text = label->get_text();
            if (text[0] != '*')
               label->set_text("*" + std::string{text});
         }
         else
         {
            label->set_text(preset_labels[int(val)-1]);
         }
         view_.refresh(*label);
      }
   );

   return
      align_top(
         align_center(
            hsize(180,
               htile(
                  label("Preset:"),
                  hspace(10),
                  preset_menu.first
               )
            )
         )
      );
}

// Create an input text box and establish its connection with the model.
auto make_input_box(my_model& model, view& view_)
{
   auto tbox = input_box("value");

   // When a new value is assigned to the model, we want to update
   // the input text box and refresh the view.
   model._value.on_update(
      [&view_, input = tbox.second](double val)
      {
         input->set_text(std::to_string(val));
         input->select_all();
         view_.refresh(*input);
      }
   );

   // When the user presses the enter key, we'll convert the text to a number
   // and then update the model with the new value. We will deal with input
   // validation and bring up a message box when errors are encountered.
   tbox.second->on_enter =
      [&model, &view_](std::string_view text)->bool
      {
         std::string::size_type pos;
         std::string error{""};
         double val;

         try
         {
            val = std::stod(std::string{text}, &pos);
         }
         catch(std::invalid_argument const& e)
         {
            error = "Illegal characters in input.";
         }

         if (pos != text.size())
         {
            error = "Illegal characters in input.";
         }
         else if (val < 0.0 || val > 1.0)
         {
            error =
               "Number is out of range. "
               "Expected range is from 0.0 to 1.0."
               ;
         }
         else
         {
            model._value = val;
            model._preset = my_model::preset_none;
         }

         if (error != "")
         {
            auto&& on_ok =
               [&model]()
               {
                  // When errors are enountered, reset the model's value to
                  // its previous state.
                  model._value.update();
               };

            // Bring up a message box.
            auto popup = message_box1(view_, error, icons::attention, on_ok);
            view_.add(popup);
            return false;
         }
         return true;
      };

   return
      align_bottom(
         align_center(
            hsize(70, tbox.first)
         )
      );
}

// Finally, we have our main content.
auto make_content(my_model& model, view& view_)
{
   static float const grid_coords[] = {0.1, 0.9, 1.0};

   return
      margin({20, 20, 20, 20},
         group(
            margin({20, 20, 20, 20},
               vgrid(grid_coords,
                  make_preset_menu(model, view_),
                  make_dial(model, view_),
                  make_input_box(model, view_)
               )
            )
         )
      );
}

int main(int argc, char* argv[])
{
   app _app("Model");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   // Our simple model
   my_model model;

   view_.content(
      make_content(model, view_),
      background
   );

   model._value = 1.0;

   _app.run();
   return 0;
}
