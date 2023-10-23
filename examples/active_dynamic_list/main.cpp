/*=============================================================================
   Copyright (c) 2022 Johann Philippe

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using cycfi::artist::rgba;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

struct basic_row : htile_composite
{
   basic_row(size_t index)
    : htile_composite()
    , _index(index)
   {
      for (size_t i = 0; i < 4; i++)
      {
         auto input = input_box(std::to_string(index));
         input.second->on_text = [&](std::string_view s)
         {
            std::cout << "on text " << std::endl;
         };
         push_back(share(input.first));
      }
   }

   size_t _index;
};

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Active Dynamic List", "com.cycfi.active-dynamic-list");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   size_t list_size = 500;

   std::vector<element_ptr> ptr_list;
   ptr_list.resize(list_size, nullptr);
   auto && make_row = [&](size_t index)
   {
      if (ptr_list[index].get() == nullptr)
         ptr_list[index] = share(basic_row(index));
      return ptr_list[index];
   };

   auto cp = basic_vertical_cell_composer(list_size, make_row);
   auto content = vdynamic_list(cp);

   auto b1 = icon_button(icons::minus, 1);
   auto b2 = icon_button(icons::plus, 1);

   b1.on_click = [&](bool)
   {
      std::cout << "dn " << std::endl;
      if (list_size <= 50)
         return;
      list_size -= 50;
      ptr_list.resize(list_size);
      content.resize(list_size);
      view_.refresh();
   };

   b2.on_click = [&](bool)
   {
      std::cout << "up " << std::endl;
      list_size +=50;
      ptr_list.resize(list_size);
      content.resize(list_size);
      view_.refresh();
   };

   view_.content(
      margin({10, 10, 10, 10},
         vtile(
            vscroller(link(content)),
            htile(b1, b2)
            )
         ),
      background
   );

   _app.run();
   return 0;
}
