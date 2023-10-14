/*=============================================================================
   Copyright (c) 2022 Johann Philippe

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

template<size_t Size>
class fixed_size_base : public default_label
{
public:
    view_limits limits(const basic_context &ctx) const override
    {
        point size = measure_text(ctx.canvas, "9", get_font(), get_font_size());
        size.x *= Size;
        return { { size.x, size.y }, { size.x, size.y } };
    }
};

template<size_t Size>
using basic_fixed_size = basic_label_base<fixed_size_base<Size>>;

template<size_t Size>
using fixed_size_label = label_gen<basic_fixed_size<Size>>;

inline auto  make_icon_label(std::string name, int i)
{

    auto h = htile(fixed_size_label<10>(name), hspacer(50), icon_button(i,1));
    return share(h);
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "elements_icons_list", "com.johannphilippe.elements_icons_list");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   vtile_composite comp;
   comp.push_back(make_icon_label("left", icons::left));
   comp.push_back(make_icon_label("right", icons::right));
   comp.push_back(make_icon_label("up", icons::up));
   comp.push_back(make_icon_label("down", icons::down));
   comp.push_back(make_icon_label("left_dir", icons::left_dir));
   comp.push_back(make_icon_label("right_dir", icons::right_dir));
   comp.push_back(make_icon_label("up_dir", icons::up_dir));
   comp.push_back(make_icon_label("down_dir", icons::down_dir));
   comp.push_back(make_icon_label("left_circled", icons::left_circled));
   comp.push_back(make_icon_label("right_circled", icons::right_circled));
   comp.push_back(make_icon_label("up_circled", icons::up_circled));
   comp.push_back(make_icon_label("down_circled", icons::down_circled));
   comp.push_back(make_icon_label("left_open", icons::left_open));
   comp.push_back(make_icon_label("right_open", icons::right_open));
   comp.push_back(make_icon_label("up_open", icons::up_open));
   comp.push_back(make_icon_label("down_open", icons::down_open));
   comp.push_back(make_icon_label("angle_left", icons::angle_left));
   comp.push_back(make_icon_label("angle_right", icons::angle_right));
   comp.push_back(make_icon_label("angle_up", icons::angle_up));
   comp.push_back(make_icon_label("angle_down", icons::angle_down));
   comp.push_back(make_icon_label("angle_double_left", icons::angle_double_left));
   comp.push_back(make_icon_label("angle_double_right", icons::angle_double_right));
   comp.push_back(make_icon_label("angle_double_up", icons::angle_double_up));
   comp.push_back(make_icon_label("angle_double_down", icons::angle_double_down));
   comp.push_back(make_icon_label("angle_circled_left", icons::angle_circled_left));
   comp.push_back(make_icon_label("angle_circled_right", icons::angle_circled_right));
   comp.push_back(make_icon_label("angle_circled_up", icons::angle_circled_up));
   comp.push_back(make_icon_label("angle_circled_down", icons::angle_circled_down));
   comp.push_back(make_icon_label("exclamation", icons::exclamation));
   comp.push_back(make_icon_label("block", icons::block));
   comp.push_back(make_icon_label("pencil", icons::pencil));
   comp.push_back(make_icon_label("pin", icons::pin));
   comp.push_back(make_icon_label("resize_vertical", icons::resize_vertical));
   comp.push_back(make_icon_label("resize_horizontal", icons::resize_horizontal));
   comp.push_back(make_icon_label("move", icons::move));
   comp.push_back(make_icon_label("resize_full_alt", icons::resize_full_alt));
   comp.push_back(make_icon_label("resize_full", icons::resize_full));
   comp.push_back(make_icon_label("resize_small", icons::resize_small));
   comp.push_back(make_icon_label("magnifying_glass", icons::magnifying_glass));
   comp.push_back(make_icon_label("zoom_in", icons::zoom_in));
   comp.push_back(make_icon_label("zoom_out", icons::zoom_out));
   comp.push_back(make_icon_label("volume_off", icons::volume_off));
   comp.push_back(make_icon_label("volume_down", icons::volume_down));
   comp.push_back(make_icon_label("volume_up", icons::volume_up));
   comp.push_back(make_icon_label("cw", icons::cw));
   comp.push_back(make_icon_label("ccw", icons::ccw));
   comp.push_back(make_icon_label("cycle", icons::cycle));
   comp.push_back(make_icon_label("level_up", icons::level_up));
   comp.push_back(make_icon_label("level_down", icons::level_down));
   comp.push_back(make_icon_label("shuffle", icons::shuffle));
   comp.push_back(make_icon_label("exchange", icons::exchange));
   comp.push_back(make_icon_label("power", icons::power));
   comp.push_back(make_icon_label("play", icons::play));
   comp.push_back(make_icon_label("stop", icons::stop));
   comp.push_back(make_icon_label("pause", icons::pause));
   comp.push_back(make_icon_label("record", icons::record));
   comp.push_back(make_icon_label("to_end", icons::to_end));
   comp.push_back(make_icon_label("to_start", icons::to_start));
   comp.push_back(make_icon_label("fast_forward", icons::fast_forward));
   comp.push_back(make_icon_label("fast_backward", icons::fast_backward));
   comp.push_back(make_icon_label("wrench", icons::wrench));
   comp.push_back(make_icon_label("trash", icons::trash));
   comp.push_back(make_icon_label("trash_empty", icons::trash_empty));
   comp.push_back(make_icon_label("ok", icons::ok));
   comp.push_back(make_icon_label("cancel", icons::cancel));
   comp.push_back(make_icon_label("plus", icons::plus));
   comp.push_back(make_icon_label("minus", icons::minus));
   comp.push_back(make_icon_label("cog", icons::cog));
   comp.push_back(make_icon_label("doc", icons::doc));
   comp.push_back(make_icon_label("docs", icons::docs));
   comp.push_back(make_icon_label("lock_open", icons::lock_open));
   comp.push_back(make_icon_label("lock", icons::lock));
   comp.push_back(make_icon_label("sliders", icons::sliders));
   comp.push_back(make_icon_label("floppy", icons::floppy));
   comp.push_back(make_icon_label("attention", icons::attention));
   comp.push_back(make_icon_label("info", icons::info));
   comp.push_back(make_icon_label("error", icons::error));
   comp.push_back(make_icon_label("lightbulb", icons::lightbulb));
   comp.push_back(make_icon_label("mixer", icons::mixer));
   comp.push_back(make_icon_label("hand", icons::hand));
   comp.push_back(make_icon_label("question", icons::question));
   comp.push_back(make_icon_label("menu", icons::menu));
   view_.content(
               margin({10, 10, 10, 10},
               vscroller(margin({40, 20, 40, 20}, comp))),
                     // Add more content layers here. The order
                     // specifies the layering. The lowest layer
                     // is at the bottom of this list.

      background     // Replace background with your main element,
                     // or keep it and add another layer on top of it.
   );

   _app.run();
   return 0;
}
