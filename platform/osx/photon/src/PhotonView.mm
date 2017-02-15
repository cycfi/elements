/*=================================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)

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
#import <PhotonView.hpp>
#import <osx_view.hpp>
#import <cairo-quartz.h>

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

namespace client
{
   namespace
   {
      void (*_init_view)(photon::view& v) = nullptr;
   }

   init_view::init_view(init_view_function f)
   {
      _init_view = f;
   }
}

@implementation PhotonView

- (void) make
{
   _first_time = true;

   photon::platform_view_access::init_view(_view, self);

   PHOTON_ASSERT(&client::_init_view, "Error. init_view is uninitialized.");
   client::_init_view(_view);

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
   _view.draw(
      {
         float(dirty.origin.x),
         float(dirty.origin.y),
         float(dirty.origin.x + dirty.size.width),
         float(dirty.origin.y + dirty.size.height)
      }
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
   auto const mods = photon::translate_flags([event modifierFlags]);
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn =
   {
      true,
      int(click_count),
      photon::mouse_button::left,
      mods,
      { float(pos.x), float(pos.y) }
   };
   _view.click(btn);
   [self displayIfNeeded];
}

- (void) mouseDragged:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   auto click_count = [event clickCount];
   auto const mods = photon::translate_flags([event modifierFlags]);
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn =
   {
      true,
      int(click_count),
      photon::mouse_button::left,
      mods,
      { float(pos.x), float(pos.y) }
   };
   _view.drag(btn);
   [self displayIfNeeded];
}

- (void) mouseUp:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   auto click_count = [event clickCount];
   auto const mods = photon::translate_flags([event modifierFlags]);
   pos = [self convertPoint:pos fromView:nil];
   photon::mouse_button btn =
   {
      false,
      int(click_count),
      photon::mouse_button::left,
      mods,
      { float(pos.x), float(pos.y) }
   };
   _view.click(btn);
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
   _view.cursor({ float(pos.x), float(pos.y) }, photon::cursor_tracking::entering);
   [self displayIfNeeded];
}

- (void) mouseExited:(NSEvent*) event
{
   [[self window] setAcceptsMouseMovedEvents:NO];
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   _view.cursor({ float(pos.x), float(pos.y) }, photon::cursor_tracking::leaving);
   [self displayIfNeeded];
}

- (void) mouseMoved:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   _view.cursor({ float(pos.x), float(pos.y) }, photon::cursor_tracking::hovering);
   [self displayIfNeeded];
   [super mouseMoved: event];
}

- (void) scrollWheel:(NSEvent*) event
{
   photon::point delta = { float([event scrollingDeltaX]), float([event scrollingDeltaY]) };

   if (event.directionInvertedFromDevice)
      delta.y = -delta.y;

   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   if (fabs(delta.x) > 0.0 || fabs(delta.y) > 0.0)
      _view.scroll(delta, { float(pos.x), float(pos.y) });

   [self displayIfNeeded];
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

- (void) keyDown:(NSEvent*) event
{
   auto const key = photon::translate_key([event keyCode]);
   auto const mods = photon::translate_flags([event modifierFlags]);

   handle_key(*self, _view, { key, photon::key_action::press, mods });
   [self interpretKeyEvents:[NSArray arrayWithObject:event]];
}

- (void) flagsChanged:(NSEvent*) event
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

- (void) keyUp:(NSEvent*) event
{
   auto const key = photon::translate_key([event keyCode]);
   auto const mods = photon::translate_flags([event modifierFlags]);

   handle_key(*self, _view, { key, photon::key_action::release, mods });
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

namespace
{
   void handle_text(photon::view& _view, photon::text_info info)
   {
      if (info.codepoint < 32 || (info.codepoint > 126 && info.codepoint < 160))
        return;
      _view.text(info);
   }
}

- (void) insertText:(id)string replacementRange:(NSRange)replacementRange
{
   auto*       event = [NSApp currentEvent];
   auto const  mods = photon::translate_flags([event modifierFlags]);
   auto*       characters = ([string isKindOfClass:[NSAttributedString class]]) ?
                  [string string] : (NSString*) string;

   NSUInteger i, length = [characters length];
   for (i = 0;  i < length;  i++)
   {
      const unichar codepoint = [characters characterAtIndex:i];
      if ((codepoint & 0xff00) == 0xf700)
         continue;
      handle_text(_view, { codepoint, mods });
   }
}

- (void) doCommandBySelector:(SEL)selector
{
}

-(void) windowDidBecomeKey:(NSNotification*) notification
{
   _view.focus(photon::focus_request::begin_focus);
}

-(void) windowDidResignKey:(NSNotification*) notification
{
   _view.focus(photon::focus_request::end_focus);
}

@end
