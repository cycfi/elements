/*=============================================================================
   Copyright (c) 2020 Aravindhan K

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <infra/support.hpp>

using namespace cycfi::elements;
using namespace cycfi;

// Function to calculate distance
float distance(point a, point b)
{
   return sqrt(pow(b.x - a.x, 2) +  pow(b.y - a.y, 2) * 1.0);
}

class custom_control : public tracker<>
{
public:

   void draw(context const& ctx) override
   {
      // Draw a background.
      ctx.canvas.fill_style(color(0.8, 0.8, 0.8));
      ctx.canvas.fill_rect(ctx.bounds);

      point cursor_pos = ctx.cursor_pos();

      // Draw a circle.
      ctx.canvas.line_width(1.0);
      ctx.canvas.stroke_style(color(0, 0, 0.5));
      auto outer_ring = circle(center_point(ctx.bounds), _radius);
      ctx.canvas.circle(outer_ring);
      ctx.canvas.stroke();

      // Center points for four knobs.
      _knobs = std::array<point, 4>
         {
            point(outer_ring.bounds().left, center_point(ctx.bounds).y),
            point(outer_ring.bounds().right, center_point(ctx.bounds).y),
            point(center_point(ctx.bounds).x, outer_ring.bounds().top),
            point(center_point(ctx.bounds).x, outer_ring.bounds().bottom),
         };

      // Draw four knobs.
      for (auto knob : _knobs)
      {
         ctx.canvas.line_width(1.0);
         ctx.canvas.stroke_style(color(0, 0, 0.6));
         ctx.canvas.fill_style(color(1.0, 1.0, 1.0));
         ctx.canvas.circle(circle(knob, 5.0));
         ctx.canvas.fill_preserve();
         ctx.canvas.stroke();

         // Hightlight the knob which is near cursor.
         auto d = distance(knob, cursor_pos);
         if (_mouse_over && (d < 10.0))
         {
            ctx.canvas.line_width(2.0);
            ctx.canvas.stroke_style(color(0, 0, 0.8));
            ctx.canvas.circle(circle(knob, 7.0));
            ctx.canvas.stroke();
         }
      }

      // Highlight choose knob with green circle.
      if (_choosen_knob != -1)
      {
         ctx.canvas.line_width(2.0);
         ctx.canvas.stroke_style(color(0, 0.8, 0));
         ctx.canvas.circle(circle(_knobs[_choosen_knob], 7.0));
         ctx.canvas.stroke();
      }

      if (_mouse_over)
      {
         ctx.canvas.line_width(1.0);
         ctx.canvas.stroke_style(color(0, 0.5, 0.5, 0.2));
         ctx.canvas.circle(circle(cursor_pos.x, cursor_pos.y, 10.0));
         ctx.canvas.stroke();
      }
   }

   void keep_tracking(context const& ctx, tracker_info& track_info) override
   {
      auto center = center_point(ctx.bounds);
      _radius = std::abs(distance(center, track_info.current));
      clamp(_radius, 50, ctx.bounds.width()/2);
      ctx.view.refresh(ctx.bounds);
   }

   bool cursor(context const& ctx, point p, cursor_tracking status) override
   {
      switch (status)
      {
         case cursor_tracking::hovering:
         case cursor_tracking::entering:
            _mouse_over = true;
            ctx.view.refresh(ctx.bounds);
            break;
         case cursor_tracking::leaving:
            _mouse_over = false;
      };
      return false;
   }

   bool click(context const& ctx, mouse_button btn) override
   {
      // Call the tracker<> click member function
      tracker<>::click(ctx, btn);

      if (btn.state == mouse_button::left)
      {
         // lock the knob clicked over if any.
         for (std::size_t i = 0, e = _knobs.size(); i != e; ++i)
         {
            auto d = distance(_knobs[i], btn.pos);
            if (d < 10.0)
               _choosen_knob =  i;
         }
      }
      return true;
   }

private:

   bool _mouse_over = false;
   int _choosen_knob = -1;
   std::array<point, 4> _knobs;
   float _radius = 150;
};

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Custom control example", "com.cycfi.custom_control");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      align_center_middle(fixed_size({ 400, 400 }, custom_control())),
      fixed_size({ 800, 600}, image{ "wall.jpg" })
   );

   _app.run();
   return 0;
}
