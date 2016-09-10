/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#import <Cocoa/Cocoa.h>
#include <photon/view.hpp>
#include "PhotonView.hpp"
#import <cairo-quartz.h>

namespace photon
{
   namespace
   {
      PhotonView* get_mac_view(view_impl* impl)
      {
         return (__bridge PhotonView*) impl;
      }
   }

   view::view()
    : _impl(nullptr)
   {
      client::init(*this);
   }

   cairo_t* view::setup_context()
   {
      auto ns_view = get_mac_view(_impl);
      auto w = [ns_view bounds].size.width;
      auto h = [ns_view bounds].size.height;
      auto locked = [ns_view lockFocusIfCanDraw];

      auto context_ref = CGContextRef(NSGraphicsContext.currentContext.graphicsPort);
      cairo_surface_t* surface = surface = cairo_quartz_surface_create_for_cg_context(context_ref, w, h);
      cairo_t* context = cairo_create(surface);
      cairo_surface_destroy(surface);

      if (locked)
         [ns_view unlockFocus];
      return context;
   }

   point view::cursor_pos() const
   {
      auto  ns_view = get_mac_view(_impl);
      auto  frame_height = [ns_view frame].size.height;
      auto  pos = [[ns_view window] mouseLocationOutsideOfEventStream];
      return { float(pos.x), float(frame_height - pos.y - 1) };
   }

   point view::size() const
   {
      auto frame = [get_mac_view(_impl) frame];
      return { float(frame.size.width), float(frame.size.height) };
   }

   void view::size(point size_)
   {
      [[get_mac_view(_impl) window] setContentSize:NSMakeSize(size_.x, size_.y)];
   }

   void view::limits(widget_limits limits_) const
   {
      auto ns_view = get_mac_view(_impl);
      [[ns_view window] setContentMinSize : NSSize{ limits_.min.x, limits_.min.y }];
      [[ns_view window] setContentMaxSize : NSSize{ limits_.max.y, limits_.max.y }];
   }

   void view::refresh()
   {
      get_mac_view(_impl).needsDisplay = true;
   }

   void view::refresh(rect area)
   {
      [get_mac_view(_impl) setNeedsDisplayInRect
         : CGRectMake(area.left, area.top, area.width(), area.height())
      ];
   }
}
