/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#import "PhotonView.hpp"

namespace photon
{
   class platform_access
   {
   public:

      static void init_view(view& v, PhotonView* self_)
      {
         v._impl = (__bridge view_impl*) self_;
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

   _view.draw(
      {
         float(dirty.origin.x),
         float(dirty.origin.y),
         float(dirty.size.width),
         float(dirty.size.height)
      }
   );
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
