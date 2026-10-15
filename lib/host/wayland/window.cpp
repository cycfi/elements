/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>
#include "wayland_host.hpp"

#include <wayland-client.h>
#include <libdecor.h>
#include <functional>
#include <algorithm>
#include <cmath>

namespace cycfi::elements
{
   struct host_window
   {
      wl_surface*           surface = nullptr;
      libdecor_frame*       frame = nullptr;
      std::function<void()> on_close;
      int                   init_w = 660, init_h = 500;   // requested logical size
      int                   cur_w = 660, cur_h = 500;     // current logical size
      view_limits           limits = {{0, 0}, {10E6, 10E6}};
   };

   wl_surface*     get_surface(host_window& h) { return h.surface; }
   libdecor_frame* get_frame(host_window& h)   { return h.frame; }

   namespace
   {
      void frame_configure(libdecor_frame* frame, libdecor_configuration* config, void* user)
      {
         auto* hw = static_cast<host_window*>(user);
         int w = hw->init_w, h = hw->init_h;
         libdecor_configuration_get_content_size(config, frame, &w, &h);
         hw->cur_w = w;
         hw->cur_h = h;

         auto* st = libdecor_state_new(w, h);
         libdecor_frame_commit(frame, st, config);
         libdecor_state_free(st);

         view_configure(*hw, w, h);
         view_render(*hw);
      }

      void frame_close(libdecor_frame*, void* user)
      {
         auto* hw = static_cast<host_window*>(user);
         if (hw->on_close)
            hw->on_close();
      }

      void frame_commit(libdecor_frame*, void*) {}

      libdecor_frame_interface frame_iface = {
         frame_configure, frame_close, frame_commit
      };
   }

   window::window(std::string const& name, int /*style_*/, rect const& bounds)
    : _window(new host_window)
   {
      _window->init_w = _window->cur_w = int(std::lround(bounds.width()));
      _window->init_h = _window->cur_h = int(std::lround(bounds.height()));

      _window->surface = wl_compositor_create_surface(get_compositor());
      _window->frame = libdecor_decorate(
         get_libdecor(), _window->surface, &frame_iface, _window);
      libdecor_frame_set_title(_window->frame, name.c_str());
      libdecor_frame_set_app_id(_window->frame, "org.cycfi.elements");
      _window->on_close = [this]() { if (on_close) on_close(); };
      libdecor_frame_map(_window->frame);
   }

   window::~window()
   {
      if (_window->frame)   libdecor_frame_unref(_window->frame);
      if (_window->surface) wl_surface_destroy(_window->surface);
      delete _window;
   }

   point window::size() const
   {
      return {float(_window->cur_w), float(_window->cur_h)};
   }

   // A client picks its own content size on Wayland: a commit of the new
   // size, kept within the limits, as frame_configure makes one.
   void window::size(point const& p)
   {
      auto const& l = _window->limits;
      int const w = int(std::lround(std::clamp(p.x, l.min.x, std::max(l.min.x, l.max.x))));
      int const h = int(std::lround(std::clamp(p.y, l.min.y, std::max(l.min.y, l.max.y))));
      if (w == _window->cur_w && h == _window->cur_h)
         return;
      _window->cur_w = _window->init_w = w;
      _window->cur_h = _window->init_h = h;
      if (!_window->frame)
         return;
      auto* st = libdecor_state_new(w, h);
      libdecor_frame_commit(_window->frame, st, nullptr);
      libdecor_state_free(st);
      view_configure(*_window, w, h);
      view_render(*_window);
   }

   void window::limits(view_limits limits_)
   {
      _window->limits = limits_;
      if (!_window->frame)
         return;
      constexpr float max = 10E6;
      libdecor_frame_set_min_content_size(_window->frame,
         int(std::lround(limits_.min.x)), int(std::lround(limits_.min.y)));
      libdecor_frame_set_max_content_size(_window->frame,
         int(std::lround(std::min<float>(limits_.max.x, max))),
         int(std::lround(std::min<float>(limits_.max.y, max))));
   }

   point window::position() const
   {
      return {0, 0};   // Wayland clients can't query absolute position
   }

   void window::position(point const& /*p*/)
   {
      // Wayland clients can't position their own windows.
   }
}
