/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#import "PhotonView.hpp"
#import <cairo-quartz.h>

namespace photon
{
   class platform_access
   {
   public:

      static void init_view(view& v, PhotonView* self_)
      {
         v._impl = (__bridge view_impl*) self_;
      }

      static void new_surface(view& v, PhotonView* self_)
      {
         CGContextRef mac_context = (CGContextRef) NSGraphicsContext.currentContext.graphicsPort;
         v._surface = cairo_quartz_surface_create_for_cg_context(mac_context, self_.bounds.size.width, self_.bounds.size.height);
      }

      static void delete_surface(view& v)
      {
         cairo_surface_destroy(v._surface);
      }

      static void new_context(view& v)
      {
         v._context = cairo_create(v._surface);
      }

      static void delete_context(view& v)
      {
         cairo_destroy(v._context);
      }

   };
};

@implementation PhotonView

- (void) awakeFromNib
{
   photon::platform_access::init_view(_view, self);
}

- (void) dealloc
{
}

-(BOOL) isFlipped
{
   return YES;
}

- (void) drawRect:(NSRect)dirty
{
   [super drawRect:dirty];

   using photon::platform_access;

   platform_access::new_surface(_view, self);
   platform_access::new_context(_view);

	// === Use _surface and _context in platform-independent code here:
   
   _view.draw(
      {
         float(dirty.origin.x),
         float(dirty.origin.y),
         float(dirty.size.width),
         float(dirty.size.height)
      }
   );

	// === End of platform-independent code.

   platform_access::delete_context(_view);
   platform_access::delete_surface(_view);
}

- (void) mouseDown:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn = { true, 1, photon::mouse_button::left, { float(pos.x), float(pos.y) } };
   _view.click(btn);
   [self setNeedsDisplay : YES];
}

- (void) mouseDragged:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn = { true, 1, photon::mouse_button::left, { float(pos.x), float(pos.y) } };
   _view.drag(btn);
   [self setNeedsDisplay : YES];
}

- (void) mouseUp:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn = { false, 0, photon::mouse_button::left, { float(pos.x), float(pos.y) } };
   _view.click(btn);
   [self setNeedsDisplay : YES];
}



@end
