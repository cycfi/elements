/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/

   Based on GLFW3

   Copyright (c) 2009-2016 Camilla Berglund <elmindreda@glfw.org>

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would
      be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not
      be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.

=================================================================================================*/
#include <Cocoa/Cocoa.h>
#include <photon/view.hpp>
#include <PhotonView.hpp>
#include <CocoaUtils.hpp>
#include <cairo-quartz.h>

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
    , _maintain_aspect(false)
   {
      // Before calling client::init, set the working directory so we can access our resources
      CFBundleRef mainBundle = GetCurrentBundle();
      CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
      char path[PATH_MAX];
      CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX);
      CFRelease(resourcesURL);
      chdir(path);
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
      auto  ns_view = get_mac_view(_impl);
      ns_view.bounds.origin = NSMakePoint(0, 0);
      //[[ns_view window] setContentSize:NSMakeSize(size_.x, size_.y)];

      [ns_view setFrameSize : NSMakeSize(size_.x, size_.y)];
   }

   void view::limits(widget_limits limits_) const
   {
      auto ns_view = get_mac_view(_impl);
      [[ns_view window] setContentMinSize : NSSize{ limits_.min.x, limits_.min.y }];
      [[ns_view window] setContentMaxSize : NSSize{ limits_.max.x, limits_.max.y }];
      if (_maintain_aspect)
         [[ns_view window] setContentAspectRatio:NSSize{ limits_.min.x, limits_.min.y } ];
   }

   void view::refresh()
   {
      get_mac_view(_impl).needsDisplay = true;
   }

   void view::refresh(context const& ctx)
   {
      auto p1 = ctx.canvas.user_to_device(ctx.bounds.top_left());
      auto p2 = ctx.canvas.user_to_device(ctx.bounds.bottom_right());
      [get_mac_view(_impl) setNeedsDisplayInRect
         : CGRectMake(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y)
      ];
   }

   std::string view::clipboard() const
   {
      NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];

      if (![[pasteboard types] containsObject:NSStringPboardType])
      {
         //_glfwInputError(GLFW_FORMAT_UNAVAILABLE,
         //                "Cocoa: Failed to retrieve string from pasteboard");
         return {};
      }

      NSString* object = [pasteboard stringForType:NSStringPboardType];
      if (!object)
      {
         //_glfwInputError(GLFW_PLATFORM_ERROR,
         //                 "Cocoa: Failed to retrieve object from pasteboard");
         return {};
      }

      return [object UTF8String];
   }

   void view::clipboard(std::string const& text) const
   {
      NSArray* types = [NSArray arrayWithObjects:NSStringPboardType, nil];

      NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
      [pasteboard declareTypes:types owner:nil];
      [pasteboard setString:[NSString stringWithUTF8String:text.c_str()]
                    forType:NSStringPboardType];
   }

   bool view::is_focus() const
   {
      return [[get_mac_view(_impl) window] isKeyWindow];
   }

   void view::set_cursor(cursor_type type)
   {
      switch (type)
      {
         case cursor_type::arrow:
            [[NSCursor arrowCursor] set];
            break;
         case cursor_type::ibeam:
            [[NSCursor IBeamCursor] set];
            break;
         case cursor_type::cross_hair:
            [[NSCursor crosshairCursor] set];
            break;
         case cursor_type::hand:
            [[NSCursor openHandCursor] set];
            break;
         case cursor_type::h_resize:
            [[NSCursor resizeLeftRightCursor] set];
            break;
         case cursor_type::v_resize:
            [[NSCursor resizeUpDownCursor] set];
            break;
      }
   }
}
