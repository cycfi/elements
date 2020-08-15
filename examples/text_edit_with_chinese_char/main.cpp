/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   amended little by github.com/faywong to demo the display/edit capability of chinese char

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background
auto make_bkd()
{
   return port(image{ "dark-bkd.jpg" });
}

std::string const text =
   "仁、义、礼、智、信"
;

std::string const text2 =
   "开放包容、视野宽广\n\n"
;

auto make_edit_box()
{
   // set the font of text box to 文泉驿 open source font: http://wenq.org/wqy2/index.cgi
   char const* font_family = "文泉驿微米黑, \"WenQuanYi Micro Hei\"";
   return
      scroller(
         margin(
            { 20, 20, 20, 20 },
            align_left_top(hsize(800, basic_text_box(text2+text, font_descr{ font_family })))
         )
      );
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "TextEdit", "com.cycfi.text-edit");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_edit_box(),
      make_bkd()
   );

   _app.run();
   return 0;

}
