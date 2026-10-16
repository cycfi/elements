/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

/*=============================================================================
   Two envelopes, attack, decay, sustain and release, each a curve editor:
   drag a point to shape it. The amplitude envelope bends its segments and
   shows guides; the filter envelope draws them straight. Below each, the
   four stages read out as you drag.
=============================================================================*/

using namespace cycfi::elements;
using cycfi::artist::rgba;

namespace
{
   // Main window background color
   auto constexpr bkd_color = rgba(35, 35, 37, 255);
   auto background = box(bkd_color);

   using editor_ptr = std::shared_ptr<basic_curve_editor>;
   using label_ptr = std::shared_ptr<basic_label>;

   // The start, then attack, decay, sustain and release, each point an
   // offset from the one before it.
   std::vector<curve_point> envelope(float attack, float sustain)
   {
      return {
         {{0.0f, 0.0f}}
       , {{attack, 1.0f}, true}
       , {{0.2f, sustain}, true}
       , {{0.3f, sustain}, true}
       , {{0.25f, 0.0f}, true}
      };
   }

   std::string stage(char const* name, float v)
   {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "%s %.2f", name, v);
      return buf;
   }

   // An envelope and its readouts, kept up to date as points move.
   struct envelope_panel
   {
      editor_ptr editor;
      label_ptr readout[4];

      void update(view& view_)
      {
         auto& e = *editor;
         readout[0]->set_text(stage("Attack", e.x(1)));
         readout[1]->set_text(stage("Decay", e.x(2)));
         readout[2]->set_text(stage("Sustain", e.y(2)));
         readout[3]->set_text(stage("Release", e.x(4)));
         for (auto& r : readout)
            view_.refresh(*r);
      }

      auto make(view& view_)
      {
         editor->movable = [](std::size_t i) { return i != 0; };
         // The two sustain points move up and down together.
         editor->on_change =
            [this, &view_](std::size_t i, point)
            {
               if (i == 2)
                  editor->y(3, editor->y(2));
               else if (i == 3)
                  editor->y(2, editor->y(3));
               view_.refresh(*editor);
               update(view_);
            };

         for (auto& r : readout)
            r = share(label(""));
         update(view_);

         auto cell = [](label_ptr const& r) { return align_center(hold(r)); };
         return vtile(
            vmin_size(200, margin({10, 10, 10, 10}, hold(editor))),
            margin({10, 4, 10, 10},
               htile(
                  cell(readout[0]), cell(readout[1]),
                  cell(readout[2]), cell(readout[3])
               )
            )
         );
      }
   };

   envelope_panel amplitude;
   envelope_panel filter;

   auto make_amplitude(view& view_)
   {
      curve_lines lines{colors::orange, colors::orange.opacity(0.2)};
      lines.shape =
         [](std::size_t /*segment*/, float t)
         {
            return 1 - std::pow(1 - t, 3);   // fast at first, then easing
         };
      lines.horizontal_guides = {0.0f, 0.5f, 1.0f};
      lines.vertical_guides = {1, 2, 3};

      amplitude.editor = share(curve_editor(
         envelope(0.15f, 0.55f), lines,
         curve_handle{colors::orange, bkd_color}
      ));
      return amplitude.make(view_);
   }

   auto make_filter(view& view_)
   {
      curve_lines lines{colors::light_sea_green, colors::light_sea_green.opacity(0.2)};
      lines.horizontal_guides = {0.0f, 0.5f, 1.0f};

      filter.editor = share(curve_editor(
         envelope(0.05f, 0.35f), lines,
         curve_handle{colors::light_sea_green, bkd_color}
      ));
      return filter.make(view_);
   }

   auto make_controls(view& view_)
   {
      return
         margin({20, 10, 20, 10},
            hmin_size(560,
               vtile(
                  margin({10, 20, 10, 10},
                     pane("Amplitude", make_amplitude(view_), 0.8f)
                  ),
                  margin({10, 10, 10, 20},
                     pane("Filter", make_filter(view_), 0.8f)
                  )
               )
            )
         );
   }
}

int main(int argc, char* argv[])
{
   app _app("Curve Editor");

   // Set up the fonts and search paths once, at startup, so the
   // first window does not pay for it.
   init_resources();
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_controls(view_),
      background
   );

   _app.run();
   return 0;
}
