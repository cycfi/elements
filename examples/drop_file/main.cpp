/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

using cycfi::artist::rgba;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Drop File", "com.cycfi.drop-file");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   auto image_    = share(image{"space.jpg"});
   auto drop_box_ = share(drop_box(scroller(hold(image_)), {"text/uri-list"}));

   drop_box_->on_drop = [image_ = get(image_), &view_](drop_info const& info)
   {
      if (contains_filepaths(info.data))
      {
         auto paths = get_filepaths(info.data);
         if (paths.size() == 1)  // We accept only one file
         {
            auto const& image_path = paths[0];
            if (auto p = image_.lock())
            {
               try
               {
                  auto img = image{image_path};
                  *p = img;
                  view_.refresh(*p);
               }
               catch (std::runtime_error const&)
               {
                  // Invalid image
                  return false;
               }
               return true;
            }
         }
      }
      return false;
   };

   view_.content(
      align_center_middle(label("Drop a picture here").font_size(20)),
      margin({20, 20, 20, 20}, hold(drop_box_)),
      share(background)
   );

   _app.run();
   return 0;
}
