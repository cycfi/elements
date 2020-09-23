/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

// using slider_ptr = std::shared_ptr<basic_slider_base>;
// slider_ptr vsliders[3];

// using thumbwheel_ptr = std::shared_ptr<thumbwheel_base>;
// thumbwheel_ptr thumbwheels[3];

// template <bool is_vertical>
// auto make_markers()
// {
//    auto track = basic_track<5, is_vertical>();
//    return slider_labels<10>(
//       slider_marks<40>(track),         // Track with marks
//       0.8,                             // Label font size (relative size)
//       "0", "1", "2", "3", "4",         // Labels
//       "5", "6", "7", "8", "9", "10"
//    );
// }

// auto make_vslider(int index)
// {
//    vsliders[index] = share(slider(
//       basic_thumb<25>(),
//       make_markers<true>(),
//       (index + 1) * 0.25
//    ));
//    return align_center(vmargin({ 20, 20 }, hold(vsliders[index])));
// }

// auto make_vsliders()
// {
//    return hmin_size(300,
//       htile(
//          make_vslider(0),
//          make_vslider(1),
//          make_vslider(2)
//       )
//    );
// }

auto make_thumbwheel()
{
   auto&& make_label =
      [](std::size_t index)
      {
         auto text = "Item no.  " + std::to_string(index+1);
         return share(margin({ 25, 5, 25, 5 },
            heading(text)
               .font_color(get_theme().indicator_hilite_color)
               .font_size(24)
         ));
      };

   auto thumbwheel = share(vthumbwheel(20, make_label));

   return align_center(
      layer(
         hold(thumbwheel),
         frame{}
      )
   );
}

// auto make_thumbwheels()
// {
//    return hmargin(20,
//       vtile(
//          make_thumbwheel(0),
//          make_thumbwheel(1),
//          make_thumbwheel(2)
//       )
//    );
// }

// auto make_controls()
// {
//    return
//       margin({ 20, 10, 20, 10 },
//          vmin_size(400,
//             htile(
//                margin({ 20, 20, 20, 20 }, pane("Vertical Sliders", make_vsliders(), 0.8f)),
//                hstretch(0.5, margin({ 20, 20, 20, 20 }, pane("Thumbwheels", make_thumbwheels(), 0.8f)))
//             )
//          )
//       );
// }

// void link_control(int index, view& view_)
// {
//    vsliders[index]->on_change =
//       [index, &view_](double val)
//       {
//          thumbwheels[index]->thumbwheel_base::value(val);
//          view_.refresh(*thumbwheels[index]);
//       };

//    thumbwheels[index]->on_change =
//       [index, &view_](double val)
//       {
//          vsliders[index]->slider_base::value(val);
//          view_.refresh(*vsliders[index]);
//       };
// }

// void link_controls(view& view_)
// {
//    link_control(0, view_);
//    link_control(1, view_);
//    link_control(2, view_);
// }

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Thumbwheels", "com.cycfi.thumbwheels");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(

      align_center_middle(make_thumbwheel()),

      // make_controls(),
      background
   );

   // link_controls(view_);
   _app.run();
   return 0;
}
