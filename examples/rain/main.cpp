/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

///////////////////////////////////////////////////////////////////////////////
// Ported from Rainbow Rain animation:
// https://onaircode.com/awesome-html5-canvas-examples-source-code/
///////////////////////////////////////////////////////////////////////////////

using namespace cycfi::elements;
using namespace cycfi::artist;
using namespace std::chrono_literals;

struct rain_element : element
{
   #if defined(ARTIST_SKIA) && !defined(linux)
   constexpr static auto persistence = 0.10;
   #else
   constexpr static auto persistence = 0.04;
   #endif

   constexpr static int total = 640;
   constexpr static auto accelleration = 0.05;
   constexpr static auto repaint_color = rgb(0, 0, 0);
   constexpr static auto portion = 360.0f/total;

   using image = cycfi::artist::image;
   using image_ptr = std::unique_ptr<image>;

   void make_rain(canvas& cnv, extent size)
   {
      auto w = size.x / total;
      cnv.fill_style(repaint_color.opacity(opacity));
      cnv.fill_rect({ 0, 0, size });

      for (auto i = 0; i < total; ++i)
      {
         auto current_y = dots[i] - 1;
         dots[i] += dots_vel[i] += accelleration;
         cnv.fill_style(hsl(portion * i, 0.8, 0.5));
         cnv.fill_rect({
            float(i) * w
          , current_y
          , float(i+1) * w
          , (current_y + dots_vel[i] + 1) * 1.1f
         });

         if (dots[i] > size.y && random_size() < .01)
            dots[i] = dots_vel[i] = 0;
      }

      if (opacity > persistence)
         opacity *= 0.8;
   }

   void init(extent size)
   {
      offscreen = std::make_unique<image>(size);
      opacity = 1.0;
      for (auto i = 0; i < total; ++i)
      {
         dots[i] = size.y;
         dots_vel[i] = 10;
      }
   }

   void draw(context const& ctx) override
   {
      auto size = ctx.bounds.size();
      auto& cnv = ctx.canvas;
      if (!offscreen || offscreen->size() != size)
         init(size);

      {
         auto ctx = offscreen_image{ *offscreen };
         auto offscreen_cnv = canvas{ ctx.context() };
         make_rain(offscreen_cnv, size);
      }
      cnv.draw(*offscreen);
   }

   float random_size()
   {
      return float(std::rand()) / (RAND_MAX);
   }

   float       dots[total];
   float       dots_vel[total];
   float       opacity = 1.0;
   image_ptr   offscreen;
};

constexpr auto fps = 1000ms / 60;

void animate(view& view_)
{
   view_.post(fps, [&]() { animate(view_); });
   view_.refresh();
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Rain", "com.cycfi.rain");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(share(rain_element{}));
   view_.post(fps, [&]() { animate(view_); });

   _app.run();
   return 0;
}
