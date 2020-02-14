#include <elements.hpp>
#include <pango/pangocairo.h>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

void draw_text(cairo_t* cr)
{
   constexpr auto radius = 150;
   constexpr auto n_words = 10;
   constexpr auto font = "Open Sans 27";

   // Center coordinates on the middle of the region we are drawing
   cairo_translate(cr, radius, radius);

   // Create a PangoLayout, set the font and text
   PangoLayout* layout = pango_cairo_create_layout(cr);

   pango_layout_set_text(layout, "Text", -1);
   PangoFontDescription* desc = pango_font_description_from_string(font);
   pango_layout_set_font_description(layout, desc);
   pango_font_description_free(desc);

   // Draw the layout n_words times in a circle
   for (int i = 0; i < n_words; i++)
   {
      int width, height;
      double angle = (360. * i) / n_words;
      double red;

      cairo_save(cr);

      /* Gradient from red at angle == 60 to blue at angle == 240 */
      red = (1 + cos ((angle - 60) * G_PI / 180.)) / 2;
      cairo_set_source_rgb(cr, red, 0, 1.0 - red);

      cairo_rotate(cr, angle * G_PI / 180.);

      // Inform Pango to re-layout the text with the new transformation
      pango_cairo_update_layout(cr, layout);

      pango_layout_get_size(layout, &width, &height);
      cairo_move_to(cr, - ((double)width / PANGO_SCALE) / 2, - radius);
      pango_cairo_show_layout(cr, layout);

      cairo_restore(cr);
   }

   // free the layout object
   g_object_unref(layout);
}

auto samples = basic(
   [](context const& ctx)
   {
      draw_text(&ctx.canvas.cairo_context());
   }
);

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
         share(samples),
         share(background)    // Replace background with your main element,
                              // or keep it and add another layer on top of it.
      }
   );

   _app.run();
   return 0;
}
