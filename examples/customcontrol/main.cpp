/*=============================================================================
   Copyright (c) 2020 Aravindhan K

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <iostream>

using namespace cycfi::elements;

//Function to calculate distance 
float distance(point a, point b) 
{ 
   return sqrt(pow(b.x - a.x, 2) +  pow(b.y - a.y, 2) * 1.0); 
} 

class CustomControl : public element {
   bool mouseover = false;
   point cursor_pos;
   int choosenknob = -1; 
   std::array<point,4> knobs;

public:
   void draw(context const& ctx){
      //Draw a background.
      ctx.canvas.fill_style(color(0.8, 0.8, 0.8));
      ctx.canvas.fill_rect(ctx.bounds);

      //Draw a circle.
      ctx.canvas.line_width(1.0);
      ctx.canvas.stroke_style(color(0, 0, 0.5));
      auto outerring = circle(
              center_point(ctx.bounds), ctx.bounds.width()/2.5);
      ctx.canvas.circle(outerring);
      ctx.canvas.stroke();

      //Center points for four knobs.
      knobs = std::array<point,4>{
          point(outerring.bounds().left, center_point(ctx.bounds).y),
          point(outerring.bounds().right, center_point(ctx.bounds).y),
          point(center_point(ctx.bounds).x, outerring.bounds().top),
          point(center_point(ctx.bounds).x, outerring.bounds().bottom),
      };

      //Draw four knobs.
      for (auto knob : knobs) {
          ctx.canvas.line_width(1.0);
          ctx.canvas.stroke_style(color(0, 0, 0.6));
          ctx.canvas.fill_style(color(1.0, 1.0, 1.0));
          ctx.canvas.circle(circle(knob, 5.0));
          ctx.canvas.fill_preserve();
          ctx.canvas.stroke();
          //Hightlight the knob which is near cursor.
          auto d = distance(knob, cursor_pos);
          if (mouseover && (d < 10.0)) {
              ctx.canvas.line_width(2.0);
              ctx.canvas.stroke_style(color(0, 0, 0.8));
              ctx.canvas.circle(circle(knob, 7.0));
              ctx.canvas.stroke();
          }
      }

      //Highlight choose knob with green circle.
      if (choosenknob != -1) {
          std::cout<<"highlighting choosen knob" <<std::endl;
          ctx.canvas.line_width(2.0);
          ctx.canvas.stroke_style(color(0, 0.8, 0));
          ctx.canvas.circle(circle(knobs[choosenknob], 7.0));
          ctx.canvas.stroke();
      }

      if (mouseover) {
         ctx.canvas.line_width(1.0);
         ctx.canvas.stroke_style(color(0, 0.5, 0.5, 0.2));
         ctx.canvas.circle(circle(cursor_pos.x, cursor_pos.y, 10.0));
         ctx.canvas.stroke();
      }
   }
   bool cursor(context const& ctx, point p, cursor_tracking status) {
      bool hit = ctx.bounds.includes(p);
      if (!hit) {
         false;
      }
      if (status == cursor_tracking::hovering) {
         mouseover = true;
      }
      if (status == cursor_tracking::leaving) {
         mouseover = false;
      }
      cursor_pos = p;
      ctx.view.refresh(ctx);
      return true;
   }

   bool click(context const& ctx, mouse_button btn) {
       if(btn.state == mouse_button::left) {
          //lock the knob clicked over if any.
          for (std::size_t i = 0, e = knobs.size(); i != e; ++i){
            auto d = distance(knobs[i], btn.pos);
            if (d < 10.0) {
                choosenknob =  i;
            }
          }
       }
       return true;
   }

   bool wants_focus() const
   {
      return true;
   }

   bool wants_control() const
   {
      return true;
   }

};

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Custom control example", "com.cycfi.customcontrol");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
       align_center_middle(
         fixed_size({ 400, 400 }, CustomControl())
         ),
         fixed_size(
             { 800, 600},
             image{ "wall.jpg" }
         )
   );

   _app.run();
   return 0;
}
