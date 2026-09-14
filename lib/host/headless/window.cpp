/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The headless host's window: a rectangle and its limits. Sizing it sizes
   the view made on it, the way a real window's resize reaches its view.
=============================================================================*/
#include <elements/window.hpp>
#include "headless_host.hpp"
#include <algorithm>

namespace cycfi::elements
{
   window::window(
      std::string const& /*name*/, int /*style_*/, rect const& bounds)
    : _window(new host_window{bounds})
   {
   }

   window::~window()
   {
      if (_window->view)
         _window->view->host()->window = nullptr;
      delete _window;
   }

   point window::size() const
   {
      return {_window->bounds.width(), _window->bounds.height()};
   }

   void window::size(point const& p)
   {
      if (_window->view)
      {
         _window->view->size(p);
      }
      else
      {
         _window->bounds.right = _window->bounds.left + p.x;
         _window->bounds.bottom = _window->bounds.top + p.y;
      }
   }

   void window::limits(view_limits limits_)
   {
      // Keep the window within its limits, as a window manager does.
      _window->limits = limits_;
      auto s = size();
      point clamped{
         std::clamp(s.x, limits_.min.x, std::max(limits_.min.x, limits_.max.x))
       , std::clamp(s.y, limits_.min.y, std::max(limits_.min.y, limits_.max.y))
      };
      if (clamped != s)
         size(clamped);
   }

   point window::position() const
   {
      return {_window->bounds.left, _window->bounds.top};
   }

   void window::position(point const& p)
   {
      _window->bounds = _window->bounds.move_to(p.x, p.y);
   }
}
