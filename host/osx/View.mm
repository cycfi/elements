/*=================================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=================================================================================================*/
#import <host.hpp>
#import <Cocoa/Cocoa.h>
#import <memory>

namespace ph = photon;

///////////////////////////////////////////////////////////////////////////////
// Default functions

ph::base_view* _new_view_(ph::host_view* h)
{
   return new ph::base_view{h};
}

///////////////////////////////////////////////////////////////////////////////
// Main callbacks

ph::base_view* (*new_view)(ph::host_view*) = &_new_view_;

///////////////////////////////////////////////////////////////////////////////
// Helper utils

namespace
{
   // Defines a constant for empty ranges in NSTextInputClient
   //
   NSRange const kEmptyRange = { NSNotFound, 0 };

   float transformY(float y)
   {
      return CGDisplayBounds(CGMainDisplayID()).size.height - y;
   }
}

///////////////////////////////////////////////////////////////////////////////
// PhotonView Interface

@interface PhotonView : NSView <NSTextInputClient>
{
   NSTrackingArea*                  _tracking_area;
   NSMutableAttributedString*       _marked_text;
   bool                             _first_time;
   std::unique_ptr<ph::base_view>   _view;
}

- (void) make;

@end

///////////////////////////////////////////////////////////////////////////////
// PhotonView Implementation
@implementation PhotonView

- (void) make
{
   _first_time = true;

   _view.reset(new_view((__bridge ph::host_view*) self));

   _tracking_area = nil;
   [self updateTrackingAreas];

   _marked_text = [[NSMutableAttributedString alloc] init];

   [[NSNotificationCenter defaultCenter]
      addObserver : self
         selector : @selector(windowDidBecomeKey:)
             name : NSWindowDidBecomeKeyNotification object:[self window]
   ];


   [[NSNotificationCenter defaultCenter]
      addObserver : self
         selector : @selector(windowDidResignKey:)
             name : NSWindowDidResignMainNotification object:[self window]
   ];

   // $$$ Black $$$
   self.window.appearance = [NSAppearance appearanceNamed:NSAppearanceNameVibrantDark];
}

- (void) awakeFromNib
{
   [self make];
}

- (void) dealloc
{
}

- (BOOL) canBecomeKeyView
{
   return YES;
}

- (BOOL) acceptsFirstResponder
{
   return YES;
}

-(BOOL) isFlipped
{
   return YES;
}

- (BOOL) canBecomeKeyWindow
{
    return YES;
}

- (BOOL) canBecomeMainWindow
{
    return YES;
}

- (void) drawRect:(NSRect)dirty
{
   [super drawRect:dirty];

   _view->draw(
      float(dirty.origin.x),
      float(dirty.origin.y),
      float(dirty.origin.x + dirty.size.width),
      float(dirty.origin.y + dirty.size.height)
   );

   // If this is our first time, let's do a redraw. The view may have been
   // resized inside _view.draw and a redraw makes sure that the view is
   // properly displayed.
   if (_first_time)
   {
      _first_time = false;
      [self display];
   }
}

- (void) mouseDown:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   auto click_count = [event clickCount];
//   auto const mods = ph::translate_flags([event modifierFlags]);
   pos = [self convertPoint:pos fromView:nil];

   // $$$ Callback here $$$

   [self displayIfNeeded];
}

- (void) mouseDragged:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   auto click_count = [event clickCount];
//   auto const mods = ph::translate_flags([event modifierFlags]);
   pos = [self convertPoint:pos fromView:nil];

   // $$$ Callback here $$$

   [self displayIfNeeded];
}

- (void) mouseUp:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   auto click_count = [event clickCount];
//   auto const mods = ph::translate_flags([event modifierFlags]);
   pos = [self convertPoint:pos fromView:nil];

   // $$$ Callback here $$$

   [self displayIfNeeded];
}

- (void) updateTrackingAreas
{
   if (_tracking_area != nil)
      [self removeTrackingArea : _tracking_area];

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

- (void) mouseEntered:(NSEvent*) event
{
   [[self window] setAcceptsMouseMovedEvents:YES];
   [[self window] makeFirstResponder:self];
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];

   // $$$ Callback here $$$

   [self displayIfNeeded];
}

- (void) mouseExited:(NSEvent*) event
{
   [[self window] setAcceptsMouseMovedEvents:NO];
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];

   // $$$ Callback here $$$

   [self displayIfNeeded];
}

- (void) mouseMoved:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];

   // $$$ Callback here $$$

   [self displayIfNeeded];
   [super mouseMoved: event];
}

- (void) scrollWheel:(NSEvent*) event
{
//   ph::point delta = { float([event scrollingDeltaX]), float([event scrollingDeltaY]) };

//   if (event.directionInvertedFromDevice)
//      delta.y = -delta.y;
//
//   auto pos = [event locationInWindow];
//   pos = [self convertPoint:pos fromView:nil];
//   if (fabs(delta.x) > 0.0 || fabs(delta.y) > 0.0)
//   {
//      // $$$ Callback here $$$
//   }

   [self displayIfNeeded];
}

//namespace
//{
//   void handle_key(PhotonView& ns_view, ph::view& _view, ph::key_info k)
//   {
//      using ph::key_action;
//      bool repeated = false;
//
//      if (k.action == key_action::release && ns_view._keys[k.key] == key_action::release)
//         return;
//
//      if (k.action == key_action::press && ns_view._keys[k.key] == key_action::press)
//         repeated = true;
//
//      ns_view._keys[k.key] = k.action;
//
//      if (repeated)
//         k.action = key_action::repeat;
//
//      _view.key(k);
//   }
//}

- (void) keyDown:(NSEvent*) event
{
   //auto const key = ph::translate_key([event keyCode]);
   //auto const mods = ph::translate_flags([event modifierFlags]);
   //
   //handle_key(*self, _view, { key, ph::key_action::press, mods });

   // $$$ Callback here $$$

   [self interpretKeyEvents:[NSArray arrayWithObject:event]];
}

- (void) flagsChanged:(NSEvent*) event
{
   //auto const modifier_flags =
   //    [event modifierFlags] & NSDeviceIndependentModifierFlagsMask;
   //auto const key = ph::translate_key([event keyCode]);
   //auto const mods = ph::translate_flags(modifier_flags);
   //auto const key_flag = ph::translate_key_to_modifier_flag(key);
   //
   //ph::key_action action;
   //if (key_flag & modifier_flags)
   //{
   //   if (_keys[key] == ph::key_action::press)
   //      action = ph::key_action::release;
   //   else
   //      action = ph::key_action::press;
   //}
   //else
   //{
   //   action = ph::key_action::release;
   //}
   //
   //handle_key(*self, _view, { key, action, mods });

   // $$$ Callback here $$$
}

- (void) keyUp:(NSEvent*) event
{
   //auto const key = ph::translate_key([event keyCode]);
   //auto const mods = ph::translate_flags([event modifierFlags]);
   //
   //handle_key(*self, _view, { key, ph::key_action::release, mods });

   // $$$ Callback here $$$
}

- (BOOL) hasMarkedText
{
   return [_marked_text length] > 0;
}

- (NSRange) markedRange
{
   if ([_marked_text length] > 0)
      return NSMakeRange(0, [_marked_text length] - 1);
   else
      return kEmptyRange;
}

- (NSRange) selectedRange
{
    return kEmptyRange;
}

- (void)setMarkedText : (id)string
        selectedRange : (NSRange)selectedRange
     replacementRange : (NSRange)replacementRange
{
   if ([string isKindOfClass:[NSAttributedString class]])
      (void)[_marked_text initWithAttributedString:string];
   else
      (void)[_marked_text initWithString:string];
}

- (void) unmarkText
{
   [[_marked_text mutableString] setString:@""];
}

- (NSArray*) validAttributesForMarkedText
{
   return [NSArray array];
}

- (NSAttributedString*) attributedSubstringForProposedRange : (NSRange)range
                                               actualRange : (NSRangePointer)actualRange
{
   return nil;
}

- (NSUInteger) characterIndexForPoint:(NSPoint)point
{
   return 0;
}

namespace
{
   void get_window_pos(NSWindow* window, int& xpos, int& ypos)
   {
      NSRect const content_rect =
         [window contentRectForFrameRect:[window frame]];

      if (xpos)
         xpos = content_rect.origin.x;
      if (ypos)
         ypos = transformY(content_rect.origin.y + content_rect.size.height);
   }
}

- (NSRect) firstRectForCharacterRange:(NSRange)range
                         actualRange:(NSRangePointer)actualRange
{
   int xpos, ypos;
   get_window_pos([self window], xpos, ypos);
   NSRect const content_rect = [[self window] frame];
   return NSMakeRect(xpos, transformY(ypos + content_rect.size.height), 0.0, 0.0);
}

//namespace
//{
//   void handle_text(ph::view& _view, ph::text_info info)
//   {
//      if (info.codepoint < 32 || (info.codepoint > 126 && info.codepoint < 160))
//        return;
//      _view.text(info);
//   }
//}

- (void) insertText:(id)string replacementRange:(NSRange)replacementRange
{
   //auto*       event = [NSApp currentEvent];
   //auto const  mods = ph::translate_flags([event modifierFlags]);
   //auto*       characters = ([string isKindOfClass:[NSAttributedString class]]) ?
   //               [string string] : (NSString*) string;
   //
   //NSUInteger i, length = [characters length];
   //for (i = 0;  i < length;  i++)
   //{
   //   const unichar codepoint = [characters characterAtIndex:i];
   //   if ((codepoint & 0xff00) == 0xf700)
   //      continue;
   //   handle_text(_view, { codepoint, mods });
   //}

   // $$$ Callback here $$$

}

- (void) doCommandBySelector:(SEL)selector
{
}

-(void) windowDidBecomeKey:(NSNotification*) notification
{
   //_view.focus(ph::focus_request::begin_focus);
   // $$$ Callback here $$$

}

-(void) windowDidResignKey:(NSNotification*) notification
{
   //_view.focus(ph::focus_request::end_focus);
   // $$$ Callback here $$$
}

@end
