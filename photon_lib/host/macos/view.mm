/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon/host.hpp>
#import <Cocoa/Cocoa.h>
#include <cairo-quartz.h>
#include "nsstring.h"

namespace ph = cycfi::photon;

@interface PhotonView : NSView
{
   ph::base_view* _pview;
}
@end

@implementation PhotonView

- (void) set_pico_view : (ph::base_view*) pview
{
   self->_pview = pview;
}

-(BOOL) isFlipped
{
   return YES;
}

- (void) drawRect : (NSRect)dirty
{
   [super drawRect : dirty];

   auto w = [self bounds].size.width;
   auto h = [self bounds].size.height;

   auto context_ref = NSGraphicsContext.currentContext.CGContext;
   cairo_surface_t* surface = cairo_quartz_surface_create_for_cg_context(context_ref, w, h);
   cairo_t* context = cairo_create(surface);

   _pview->draw(context,
      {
         float(dirty.origin.x),
         float(dirty.origin.y),
         float(dirty.origin.x + dirty.size.width),
         float(dirty.origin.y + dirty.size.height)
      }
   );

   cairo_surface_destroy(surface);
   cairo_destroy(context);
}

@end

namespace cycfi { namespace photon
{
   namespace
   {
      PhotonView* get_mac_view(ph::host_view h)
      {
         return (__bridge PhotonView*) h;
      }
   }

   base_view::base_view(host_window h)
   {
      PhotonView* content = [[PhotonView alloc] init];
      _view = (__bridge void*) content;
      content.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
      [content set_pico_view : this];
      NSWindow* window_ = (__bridge NSWindow*) h;
      [window_ setContentView : content];
      // [window_.contentView addSubview : content];
   }

   base_view::~base_view()
   {
   }

   point base_view::cursor_pos() const
   {
      auto  ns_view = get_mac_view(host());
      auto  frame_height = [ns_view frame].size.height;
      auto  pos = [[ns_view window] mouseLocationOutsideOfEventStream];
      return { float(pos.x), float(frame_height - pos.y - 1) };
   }

   point base_view::size() const
   {
      auto frame = [get_mac_view(host()) frame];
      return { float(frame.size.width), float(frame.size.height) };
   }

   void base_view::size(point p)
   {
      auto ns_view = get_mac_view(host());
      auto frame = [ns_view frame];
      auto title_bar_height = [ns_view window].frame.size.height - frame.size.height;

      frame.size.width = p.x;
      frame.size.height = p.y + title_bar_height;
      [[ns_view window] setFrame : frame display : YES animate : false];
   }

   void base_view::refresh()
   {
      get_mac_view(host()).needsDisplay = true;
   }

   void base_view::refresh(rect area)
   {
      [get_mac_view(host()) setNeedsDisplayInRect
         : CGRectMake(area.left, area.top, area.width(), area.height())
      ];
   }

   void base_view::limits(view_limits limits_)
   {
      auto ns_view = get_mac_view(host());
      [[ns_view window] setContentMinSize : NSSize{ limits_.min.x, limits_.min.y }];
      [[ns_view window] setContentMaxSize : NSSize{ limits_.max.x, limits_.max.y }];
   }

   bool base_view::is_focus() const
   {
      return [[get_mac_view(host()) window] isKeyWindow];
   }

   std::string clipboard()
   {
      NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
      if (![[pasteboard types] containsObject:NSPasteboardTypeString])
         return {};

      NSString* object = [pasteboard stringForType:NSPasteboardTypeString];
      if (!object)
         return {};
      return [object UTF8String];
   }

   void clipboard(std::string const& text)
   {
      NSArray* types = [NSArray arrayWithObjects:NSPasteboardTypeString, nil];

      NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
      [pasteboard declareTypes:types owner:nil];
      [pasteboard setString:[NSString stringWithUTF8String:text.c_str()]
                    forType:NSPasteboardTypeString];
   }

   void set_cursor(cursor_type type)
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
}}

