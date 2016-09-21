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

//   NSColor* c = [NSColor colorWithCalibratedRed:35 green:35 blue:37 alpha:1.0f];
//   [[self window] setBackgroundColor: c];

//    _textField = [[NSTextField alloc] initWithFrame:NSMakeRect(10, 10, 200, 24)];
//    [_textField setStringValue:@"Text Box"];
//    [_textField setBezeled:YES];
//    [_textField setDrawsBackground:YES];
//    [_textField setEditable:YES];
//    [_textField setSelectable:YES];
//    [self addSubview:_textField];
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
   auto click_count = [event clickCount];
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn =
   {
      true,
      int(click_count),
      photon::mouse_button::left,
      { float(pos.x), float(pos.y) }
   };
   _view.click(btn);
   [self displayIfNeeded];
}

- (void) mouseDragged:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   auto click_count = [event clickCount];
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn =
   {
      true,
      int(click_count),
      photon::mouse_button::left,
      { float(pos.x), float(pos.y) }
   };
   _view.drag(btn);
   [self displayIfNeeded];
}

- (void) mouseUp:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   auto click_count = [event clickCount];
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn =
   {
      false,
      int(click_count),
      photon::mouse_button::left,
      { float(pos.x), float(pos.y) }
   };
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

   if (event.directionInvertedFromDevice)
      delta.y = -delta.y;

   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   if (fabs(delta.x) > 0.0 || fabs(delta.y) > 0.0)
      _view.scroll(delta, { float(pos.x), float(pos.y) });

   [self displayIfNeeded];
   [super scrollWheel: event];
}

namespace
{
   void handle_key(PhotonView& ns_view, photon::view& _view, photon::key_info k)
   {
      using photon::key_action;
      bool repeated = false;

      if (k.action == key_action::release && ns_view._keys[k.key] == key_action::release)
         return;

      if (k.action == key_action::press && ns_view._keys[k.key] == key_action::press)
         repeated = true;

      ns_view._keys[k.key] = k.action;

      if (repeated)
         k.action = key_action::repeat;

      _view.key(k);
   }
}

- (void)keyDown:(NSEvent*) event
{
   auto const key = photon::translate_key([event keyCode]);
   auto const mods = photon::translate_flags([event modifierFlags]);

   handle_key(*self, _view, { key, photon::key_action::press, mods });
   //[self interpretKeyEvents:[NSArray arrayWithObject:event]];
}

- (void)flagsChanged:(NSEvent*) event
{
   auto const modifier_flags =
       [event modifierFlags] & NSDeviceIndependentModifierFlagsMask;
   auto const key = photon::translate_key([event keyCode]);
   auto const mods = photon::translate_flags(modifier_flags);
   auto const key_flag = photon::translate_key_to_modifier_flag(key);

   photon::key_action action;
   if (key_flag & modifier_flags)
   {
      if (_keys[key] == photon::key_action::press)
         action = photon::key_action::release;
      else
         action = photon::key_action::press;
   }
   else
   {
      action = photon::key_action::release;
   }

   handle_key(*self, _view, { key, action, mods });
}

- (void)keyUp:(NSEvent*) event
{
   auto const key = photon::translate_key([event keyCode]);
   auto const mods = photon::translate_flags([event modifierFlags]);

   handle_key(*self, _view, { key, photon::key_action::release, mods });
}

@end
