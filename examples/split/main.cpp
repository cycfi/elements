/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// An editor's layout in splits: files at the left, the editor at the right
// above its output. Drag any boundary; each pane stops at its minimum size.

auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto constexpr pane_color = rgba(44, 46, 52, 255);
auto constexpr title_color = rgba(60, 70, 90, 255);

auto make_pane(std::string title, element_ptr content)
{
   return layer(
      vtile(
         layer(
            margin({10, 6, 10, 6}, align_left(label(std::move(title)))),
            box(title_color)
         ),
         margin({10, 10, 10, 10}, hold(content))
      ),
      box(pane_color)
   );
}

auto make_files()
{
   auto files = vtile_composite{};
   for (auto name : {"main.cpp", "split.hpp", "split.cpp", "tile.hpp",
         "tile.cpp", "CMakeLists.txt"})
      files.push_back(share(margin_bottom(4, align_left(label(name)))));
   return share(vscroller(align_top(files)));
}

auto make_editor()
{
   return share(vscroller(
      basic_text_box(
         "auto layout = hsplit(\n"
         "   files,\n"
         "   vsplit(editor, output)\n"
         ");\n"
      )));
}

auto make_output()
{
   return share(vscroller(
      static_text_box("Build succeeded: 0 errors, 0 warnings.")));
}

int main(int argc, char* argv[])
{
   app _app("Split");

   // Set up the fonts and search paths once, at startup, so the
   // first window does not pay for it.
   init_resources();
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      hsplit(
         hmin_size(120, make_pane("Files", make_files())),
         vsplit(
            vmin_size(120, make_pane("Editor", make_editor())),
            vmin_size(80, make_pane("Output", make_output()))
         )
      ),
      box(bkd_color)
   );

   _app.run();
   return 0;
}
