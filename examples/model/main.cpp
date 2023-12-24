/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <elements.hpp>

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
using elements::view;
using elements::app;
using elements::window;

using elements::empty; // $$$ for now $$$

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = elements::box(bkd_color);

// This is our simple model, comprising of a preset and
// a value normalized to 0.0 to 1.0.
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
   model._value.on_update_ui(
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

auto make_preset_menu(my_model& model, view& view_)
{
   static char const* preset_labels[] = {
         "100 Percent",
         "75 Percent",
         "50 Percent",
         "25 Percent",
         "0 Percent",
      };

   static auto preset_map =
      std::unordered_map<std::string_view, my_model::preset> {
         { preset_labels[0], my_model::preset_100_percent  },
         { preset_labels[1], my_model::preset_75_percent },
         { preset_labels[2], my_model::preset_50_percent },
         { preset_labels[3], my_model::preset_25_percent  },
         { preset_labels[4], my_model::preset_0_percent  }
      };

   auto preset_menu =
      selection_menu(
         [&model](std::string_view select_str)
         {
            auto select = preset_map[select_str];
            model._preset = select;

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
         {
            preset_labels[0],
            preset_labels[1],
            preset_labels[2],
            preset_labels[3],
            preset_labels[4]
         }
      );

   model._preset.on_update_ui(
      [&view_, label = preset_menu.second, &model](my_model::preset val)
      {
         if (val == my_model::preset_none)
         {
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

   return preset_menu.first;
}

auto make_input_box(my_model& model, view& view_)
{
   auto tbox = input_box("1.000");

   model._value.on_update_ui(
      [&view_, input = tbox.second](double val)
      {
         input->set_text(std::to_string(val));
         input->select_all();
         view_.refresh(*input);
      }
   );

   tbox.second->on_enter =
      [&model, &view_](std::string_view text)
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
               []()
               {
               };

            auto popup = message_box1(view_, error, icons::attention, on_ok);
            view_.add(popup);

            model._value.update_ui();
         }
      };

   return halign(0.5, hsize(80, tbox.first));
}

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
   app _app(argc, argv, "Model", "com.cycfi.model");
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
