/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <bitset>
#include <iostream>

using namespace cycfi::elements;

static int rows = 8;
static int cols = 8;
static int square_side = 100;
static int square_area = square_side * square_side;
static int width = cols * square_side;
static int height = rows * square_side;
static size_t pix_buf_size = rows * cols * square_area;
static uint32_t white = 0xf0000000;
static uint32_t black = 0xf0ffffff;

int main(int argc, char* argv[])
{
   // Create a chess board
   std::shared_ptr<uint32_t> pix_buf(new uint32_t[pix_buf_size], [&](auto ptr) {
      if (ptr)
         delete[] ptr;
   });
   for (int y = 0; y < rows; y++)
   {
      uint32_t* row_slice = &(pix_buf.get()[y * rows * square_area]);
      uint32_t color = white;
      if (y % 2 != 0)
        color = black;
      for (int x = 0; x < cols * square_area; x++)
      {
        if (x % square_side == 0)
          if (color == white)
            color = black;
          else
            color = white;

        row_slice[x] = color;
      }
   }

   auto pixmap_buffer = std::make_shared<pixmap>(pixmap(
      reinterpret_cast<unsigned char*>(pix_buf.get()),
      pixmap_fmt::ARGB32,
      {static_cast<float>(width), static_cast<float>(height)}
   ));
   auto img = image(pixmap_buffer);

   app _app(argc, argv, "Hello Pixmap Buffer Image", "com.cycfi.hello-pixmap-buffer-image");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);
   view_.content(
      scroller(img)
   );

   _app.run();

   return 0;
}
