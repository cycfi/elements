/*=================================================================================================
   Copyright (c) 2016-2024 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=================================================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using namespace cycfi::artist;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

struct my_element : element, selectable
{
   my_element(int n)
    : _n{n}
   {}

   void draw(context const& ctx) override
   {
      auto& cnv = ctx.canvas;
      auto state = cnv.new_state();
      auto const& theme_ = get_theme();

      if (_is_selected)
      {
         cnv.begin_path();
         cnv.add_rect(ctx.bounds);
         cnv.fill_style(theme_.indicator_color.opacity(0.6));
         cnv.fill();
      }

      auto middle = ctx.bounds.top + (ctx.bounds.height()/2);
      cnv.fill_style(theme_.label_font_color);
      cnv.font(theme_.label_font);
      cnv.text_align(cnv.left | cnv.middle);
      cnv.fill_text(std::string{"Item "} + std::to_string(_n), point{ctx.bounds.left+10, middle});
   }

   bool is_selected() const override
   {
      return _is_selected;
   }

   void select(bool state) override
   {
      _is_selected = state;
   }

   int _n;
   bool _is_selected = false;
};

int main(int argc, char* argv[])
{
   app _app("Selection Lists");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   auto&& draw_cell =
      [](std::size_t index)
      {
         return share(
            margin({20, 0, 20, 0},
               align_left(
                  vsize(25, my_element(index+1))
               )
            )
         );
      };

   auto my_composer =
      basic_cell_composer(
         200,           // size (number of rows)
         draw_cell      // Composer function
      );

   auto content = share(
      selection_list(
         list{my_composer, false}
      )
   );

   view_.content(
      vscroller(hold(content)),
      background
   );

   _app.run();
   return 0;
}
