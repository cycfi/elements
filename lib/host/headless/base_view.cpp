/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The headless host's view. There is no windowing system: the view keeps its
   own size and cursor, records the areas it asks to be repainted, and draws
   only when something calls draw with a canvas. The clipboard and cursor
   are process state. See elements/headless.hpp.
=============================================================================*/
#include <elements/base_view.hpp>
#include <elements/headless.hpp>
#include <artist/resources.hpp>
#include <infra/filesystem.hpp>
#include "headless_host.hpp"
#include <string>
#include <utility>

namespace cycfi::artist
{
   namespace
   {
      // Resources are found beside the working directory, where a test
      // binary's build copies them.
      fs::path resources_path()
      {
         return fs::current_path() / "resources";
      }
   }

   void init_paths()
   {
      add_search_path(resources_path());
   }

   fs::path get_user_fonts_directory()
   {
      return resources_path();
   }
}

namespace cycfi::elements
{
   namespace
   {
      std::string    the_clipboard;
      cursor_type    the_cursor = cursor_type::arrow;
   }

   base_view::base_view(extent size_)
    : _view(new host_view)
   {
      _view->size = size_;
   }

   // Adopt an existing host_view (the embedding entry point).
   base_view::base_view(host_view_handle h)
    : _view(h)
   {
   }

   base_view::base_view(host_window_handle h)
    : _view(new host_view)
   {
      _view->size = {h->bounds.width(), h->bounds.height()};
      _view->window = h;
      h->view = this;
   }

   base_view::~base_view()
   {
      if (_view->window && _view->window->view == this)
         _view->window->view = nullptr;
      delete _view;
      _view = nullptr;
   }

   point base_view::cursor_pos() const
   {
      return _view->cursor_position;
   }

   extent base_view::size() const
   {
      return _view->size;
   }

   void base_view::size(extent size_)
   {
      _view->size = size_;
      if (auto* w = _view->window)
      {
         w->bounds.right = w->bounds.left + size_.x;
         w->bounds.bottom = w->bounds.top + size_.y;
      }
      on_size_change(size_);
   }

   void base_view::refresh()
   {
      _view->invalidated.push_back({0, 0, _view->size.x, _view->size.y});
   }

   void base_view::refresh(rect area)
   {
      _view->invalidated.push_back(area);
   }

   std::string clipboard()
   {
      return the_clipboard;
   }

   void clipboard(std::string_view text)
   {
      the_clipboard = text;
   }

   void init_resources()
   {
   }

   void set_cursor(cursor_type type)
   {
      the_cursor = type;
   }

   point scroll_direction()
   {
      return {+1.0f, +1.0f};
   }
}

namespace cycfi::elements::headless
{
   void open(base_view& v, float scale)
   {
      v.host()->scale = scale;
      v.on_open(v.size(), scale);
   }

   std::vector<rect> take_invalidated(base_view& v)
   {
      return std::exchange(v.host()->invalidated, {});
   }

   void cursor_pos(base_view& v, point p)
   {
      v.host()->cursor_position = p;
   }

   cursor_type cursor()
   {
      return the_cursor;
   }
}
