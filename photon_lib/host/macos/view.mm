/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon/host.hpp>
#import <Cocoa/Cocoa.h>
#include <cairo-quartz.h>
#include "nsstring.h"

namespace photon = cycfi::photon;

@interface PhotonView : NSView
{
   photon::base_view* _pview;
}
@end

@implementation PhotonView

- (void) set_pico_view : (photon::base_view*) pview
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
   base_view::base_view(host_window h)
   {
      id content = [[PhotonView alloc] init];
      [content set_pico_view : this];
      id window_ = (__bridge id) h;
      [window_ setContentView : content];
   }

   base_view::~base_view()
   {
   }
}}

