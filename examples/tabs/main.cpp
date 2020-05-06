/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto make_tabs(view& view_)
{
   auto tab1 = share(tab("Dream"));
   auto tab2 = share(tab("Insanity"));
   auto tab3 = share(tab("Imagination"));
   tab1->select(true);

   auto make_content = [](auto text)
   {
      return layer(align_center_middle(label(text).font_size(100)), frame{});
   };

   auto content1 = make_content("Dream");
   auto content2 = make_content("Insanity");
   auto content3 = make_content("Imagination");

   auto content = share(
      deck(
         content1,
         content2,
         content3
      ));

   tab1->on_click =
      [content, &view_](bool state)
      {
         if (state)
         {
            content->select(0);
            view_.refresh(*content);
         }
      };

   tab2->on_click =
      [content, &view_](bool state)
      {
         if (state)
         {
            content->select(1);
            view_.refresh(*content);
         }
      };

   tab3->on_click =
      [content, &view_](bool state)
      {
         if (state)
         {
            content->select(2);
            view_.refresh(*content);
         }
      };

   return vtile(
      margin(
         { 5, 15, 20, 0 },
         align_left(htile(
            hold(tab1),
            hold(tab2),
            hold(tab3)
         ))
      ),
      hold(content)
   );
}

int main(int argc, char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(

      make_tabs(view_),

      background     // Replace background with your main element,
                     // or keep it and add another layer on top of it.
   );

   _app.run();
   return 0;
}
