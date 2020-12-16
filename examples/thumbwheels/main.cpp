/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <sstream>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

template <typename E>
auto decorate(E&& e)
{
   return hsize(170, align_center(margin({ 25, 5, 25, 5 },
      std::forward<E>(e)
   )));
}

auto make_label(std::string text = "")
{
   return decorate(heading(text)
      .font_color(get_theme().indicator_hilite_color)
      .font_size(24)
   );
};

auto make_thumbwheel1()
{
   auto&& compose =
      [](std::size_t index)
      {
         auto text = "Item no.  " + std::to_string(index+1);
         return share(make_label(text));
      };

   auto tw = share(vthumbwheel(20, compose));

   return top_margin(20,
      layer(
         hold(tw),
         frame{}
      )
   );
}

auto make_thumbwheel2(char const* unit, float offset, float scale, int precision)
{
   auto label = make_label();

   auto&& as_string =
      [=](double val)
      {
         std::ostringstream out;
         out.precision(precision);
         out << std::fixed << ((val * scale) + offset) << unit;
         return out.str();
      };

   auto tw = share(thumbwheel(as_label<double>(as_string, label)));

   return top_margin(20,
      layer(
         hold(tw),
         frame{}
      )
   );
}

auto make_xy_thumbwheel()
{
   auto label = make_label();

   auto&& draw =
      [=](context const& ctx, point val)
      {
         auto& cnv = ctx.canvas;
         cnv.begin_path();
         cnv.round_rect(ctx.bounds, get_theme().frame_corner_radius);
         cnv.fill_style(color(val.x, 0.0, 1.0).level(val.y));
         cnv.fill();
      };

   auto tw = share(thumbwheel(fixed_size({ 170, 36 }, draw_value<point>(draw))));
   tw->value({ 0.25f, 0.25f });

   return top_margin(20, hold(tw));
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Thumbwheels", "com.cycfi.thumbwheels");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(

      margin({ 20, 0, 20, 20 },
         vtile(
            align_center_middle(make_thumbwheel1()),
            align_center_middle(make_thumbwheel2(" Hz", 20, 100, 2)),
            align_center_middle(make_thumbwheel2(" ms", 0, 10000, 0)),
            align_center_middle(make_xy_thumbwheel())
         )
      ),

      background
   );

   _app.run();
   return 0;
}
