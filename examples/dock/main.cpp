/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Dockable windows. Drag a window's title, or the pointer, near an edge of
// the main view: it snaps there and shows where it would dock; let go to
// dock it. Windows docked on one side share it; drag the boundaries between
// them. Drag a docked window's title out to float it again, and keep
// dragging. Drag a floating window's edges to resize it.

auto constexpr bkd_color = rgba(35, 35, 37, 255);

auto make_pane(std::shared_ptr<dock_element> d, std::string title
 , char const* text)
{
   return share(dockable(d,
      pane_ex(
         movable(title_bar{}),
         label(std::move(title)),
         margin({10, 10, 10, 10}, align_left_top(label(text))),
         get_theme().child_window_opacity
      )));
}

auto make_window(std::shared_ptr<dock_element> d, std::string title
 , rect bounds, char const* text)
{
   auto pane = make_pane(d, std::move(title), text);
   return share(child_window(bounds, resizable(hold(pane))));
}

int main(int argc, char* argv[])
{
   app _app("Dock");

   // Set up the fonts and search paths once, at startup, so the
   // first window does not pay for it.
   init_resources();
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   auto main_content = layer(
      align_center_middle(label("Main content").font_size(24)),
      box(bkd_color)
   );
   auto d = share(dock(std::move(main_content)));

   // Output starts docked; Tools and Layers float.
   d->add(make_pane(d, "Output", "Output goes here."), dock_side::bottom);

   view_.content(
      make_window(d, "Tools", {40, 40, 220, 200}, "Tools go here."),
      make_window(d, "Layers", {260, 80, 440, 260}, "Layers go here."),
      d
   );

   _app.run();
   return 0;
}
