/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;
using namespace cycfi::artist;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

class composer : public dynamic::composer
{
public:

   std::size_t       size() const override;
   element_ptr       compose(std::size_t index) override;
   limits            width_limits() const override;
   float             line_height(std::size_t index) const override;
};

std::size_t composer::size() const
{
   return 100000;
}

element_ptr composer::compose(std::size_t index)
{
   auto text = "This is item number " + std::to_string(index);
   return share(margin({ 20, 2, 20, 2 }, align_left(label(text))));
}

composer::limits composer::width_limits() const
{
   return { 220, full_extent };
}

float composer::line_height(std::size_t index) const
{
   return 25;
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Dynamic", "com.cycfi.dynamic");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   auto content = share(dynamic{ std::make_shared<composer>() });
   content->build();

   view_.content(
      vscroller(hold(content)),
      background
   );

   _app.run();
   return 0;
}
