/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

class my_composer : public fixed_cell_composer<fixed_size_composer<>>
{
public:

   my_composer()
    : base_type{
         25       // line_height
       , 220      // min width
       , 1000000  // size (number of rows)
      }
   {}

   element_ptr compose(std::size_t index) override
   {
      auto text = "This is item number " + std::to_string(index+1);
      return share(margin({ 20, 2, 20, 2 }, align_left(label(text))));
   }
};

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Dynamic Lists", "com.cycfi.dynamic_lists");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   auto content = share(dynamic_list{ share(my_composer{}) });

   view_.content(
      vscroller(hold(content)),
      background
   );

   _app.run();
   return 0;
}
