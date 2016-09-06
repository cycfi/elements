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
   };
};

@implementation PhotonView

- (void) awakeFromNib
{
   photon::platform_access::init_view(_view, self);
   _tracking_area = nil;
   [self updateTrackingAreas];
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
         float(dirty.origin.x + dirty.size.width),
         float(dirty.origin.y + dirty.size.height)
      }
   );
}

- (void) mouseDown:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn = { true, 1, photon::mouse_button::left, { float(pos.x), float(pos.y) } };
   _view.click(btn);
   [self displayIfNeeded];
}

- (void) mouseDragged:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn = { true, 1, photon::mouse_button::left, { float(pos.x), float(pos.y) } };
   _view.drag(btn);
   [self displayIfNeeded];
}

- (void) mouseUp:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn = { false, 0, photon::mouse_button::left, { float(pos.x), float(pos.y) } };
   _view.click(btn);
   [self displayIfNeeded];
}

- (void)updateTrackingAreas
{
   if (_tracking_area != nil)
      [self removeTrackingArea : _tracking_area];

//   NSTrackingAreaOptions const options =
//      NSTrackingMouseEnteredAndExited |
//      NSTrackingActiveInKeyWindow |
//      NSTrackingEnabledDuringMouseDrag |
//      NSTrackingCursorUpdate |
//      NSTrackingInVisibleRect |
//      NSTrackingAssumeInside
//   ;

   NSTrackingAreaOptions const options =
         NSTrackingMouseEnteredAndExited |
         NSTrackingActiveAlways |
         NSTrackingMouseMoved
      ;

   _tracking_area =
      [[NSTrackingArea alloc]
         initWithRect : [self bounds]
              options : options
                owner : self
             userInfo : nil
      ];

    [self addTrackingArea:_tracking_area];
    [super updateTrackingAreas];
}

- (void)mouseEntered:(NSEvent*) event
{
   [[self window] setAcceptsMouseMovedEvents:YES];
   [[self window] makeFirstResponder:self];
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   _view.cursor({ float(pos.x), float(pos.y) }, photon::cursor_tracking::entering);
   [self displayIfNeeded];
}

- (void)mouseExited:(NSEvent*) event
{
   [[self window] setAcceptsMouseMovedEvents:NO];
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   _view.cursor({ float(pos.x), float(pos.y) }, photon::cursor_tracking::leaving);
   [self displayIfNeeded];
}

- (void)mouseMoved:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   _view.cursor({ float(pos.x), float(pos.y) }, photon::cursor_tracking::hovering);
   [self displayIfNeeded];
   [super mouseMoved: event];
}

- (void)scrollWheel:(NSEvent*) event
{
   photon::point delta = { float([event scrollingDeltaX]), float([event scrollingDeltaY]) };

   if ([event hasPreciseScrollingDeltas])
   {
      delta.x *= 0.1;
      delta.y *= 0.1;
   }

   if (fabs(delta.x) > 0.0 || fabs(delta.y) > 0.0)
      _view.scroll(delta);

   [self displayIfNeeded];
   [super scrollWheel: event];
}


@end
