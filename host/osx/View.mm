/*=============================================================================
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
=============================================================================*/
#include <photon/host.hpp>
#include <memory>
#include <map>
#include <cairo-quartz.h>
#import <Cocoa/Cocoa.h>

namespace ph = photon;
using key_map = std::map<ph::key_code, ph::key_action>;

///////////////////////////////////////////////////////////////////////////////
// Main callback
namespace photon
{
   std::function<std::unique_ptr<ph::base_view>(ph::host_view* h)> new_view =
      [](ph::host_view* h)
      {
         return std::make_unique<ph::base_view>(h);
      };
}

///////////////////////////////////////////////////////////////////////////////
// Helper utils

namespace photon
{
   // These functions are defined in key.mm:
   key_code    translate_key(unsigned int key);
   int         translate_flags(NSUInteger flags);
   NSUInteger  translate_key_to_modifier_flag(key_code key);
}

namespace
{
   // Defines a constant for empty ranges in NSTextInputClient
   NSRange const kEmptyRange = { NSNotFound, 0 };

   float transformY(float y)
   {
      return CGDisplayBounds(CGMainDisplayID()).size.height - y;
   }

   ph::mouse_button get_button(NSEvent* event, NSView* self, bool down = true)
   {
      auto pos = [event locationInWindow];
      auto click_count = [event clickCount];
      auto const mods = ph::translate_flags([event modifierFlags]);
      pos = [self convertPoint:pos fromView:nil];

      return {
         down,
         int(click_count),
         ph::mouse_button::left,
         mods,
         { float(pos.x), float(pos.y) }
      };
   }

   void handle_key(key_map& keys, ph::base_view& _view, ph::key_info k)
   {
      using ph::key_action;
      bool repeated = false;

      if (k.action == key_action::release && keys[k.key] == key_action::release)
         return;

      if (k.action == key_action::press && keys[k.key] == key_action::press)
         repeated = true;

      keys[k.key] = k.action;

      if (repeated)
         k.action = key_action::repeat;

      _view.key(k);
   }

   void get_window_pos(NSWindow* window, int& xpos, int& ypos)
   {
      NSRect const content_rect =
         [window contentRectForFrameRect:[window frame]];

      if (xpos)
         xpos = content_rect.origin.x;
      if (ypos)
         ypos = transformY(content_rect.origin.y + content_rect.size.height);
   }

   void handle_text(ph::base_view& _view, ph::text_info info)
   {
      if (info.codepoint < 32 || (info.codepoint > 126 && info.codepoint < 160))
         return;
      _view.text(info);
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
   key_map                          _keys;
}

- (void) make;

@end

namespace
{
   PhotonView* get_mac_view(ph::host_view* h)
   {
      return (__bridge PhotonView*) h;
   }
}

///////////////////////////////////////////////////////////////////////////////
// PhotonView Implementation
@implementation PhotonView

- (void) make
{
   _first_time = true;

   _view = ph::new_view((__bridge ph::host_view*) self);

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
   _view->click(get_button(event, self));
   [self displayIfNeeded];
}

- (void) mouseDragged:(NSEvent*) event
{
   _view->drag(get_button(event, self));
   [self displayIfNeeded];
}

- (void) mouseUp:(NSEvent*) event
{
   _view->click(get_button(event, self, false));
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
   _view->cursor({ float(pos.x), float(pos.y) }, ph::cursor_tracking::entering);
   [self displayIfNeeded];
}

- (void) mouseExited:(NSEvent*) event
{
   [[self window] setAcceptsMouseMovedEvents:NO];
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   _view->cursor({ float(pos.x), float(pos.y) }, photon::cursor_tracking::leaving);
   [self displayIfNeeded];
}

- (void) mouseMoved:(NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   _view->cursor({ float(pos.x), float(pos.y) }, photon::cursor_tracking::hovering);
   [self displayIfNeeded];
   [super mouseMoved: event];
}

- (void) scrollWheel:(NSEvent*) event
{
   float delta_x = [event scrollingDeltaX];
   float delta_y = [event scrollingDeltaY];

   if (event.directionInvertedFromDevice)
      delta_y = -delta_y;

   auto pos = [event locationInWindow];
   pos = [self convertPoint:pos fromView:nil];
   if (fabs(delta_x) > 0.0 || fabs(delta_y) > 0.0)
      _view->scroll({ delta_x, delta_y }, { float(pos.x), float(pos.y) });
   [self displayIfNeeded];
}

- (void) keyDown:(NSEvent*) event
{
   auto const key = ph::translate_key([event keyCode]);
   auto const mods = ph::translate_flags([event modifierFlags]);
   handle_key(_keys, *_view.get(), { key, ph::key_action::press, mods });
   [self interpretKeyEvents:[NSArray arrayWithObject:event]];
}

- (void) flagsChanged:(NSEvent*) event
{
   auto const modifier_flags =
      [event modifierFlags] & NSEventModifierFlagDeviceIndependentFlagsMask;
   auto const key = ph::translate_key([event keyCode]);
   auto const mods = ph::translate_flags(modifier_flags);
   auto const key_flag = ph::translate_key_to_modifier_flag(key);

   ph::key_action action;
   if (key_flag & modifier_flags)
   {
     if (_keys[key] == ph::key_action::press)
        action = ph::key_action::release;
     else
        action = ph::key_action::press;
   }
   else
   {
     action = ph::key_action::release;
   }

   handle_key(_keys, *_view.get(), { key, action, mods });
}

- (void) keyUp:(NSEvent*) event
{
   auto const key = ph::translate_key([event keyCode]);
   auto const mods = ph::translate_flags([event modifierFlags]);

   handle_key(_keys, *_view.get(), { key, ph::key_action::release, mods });
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

- (NSRect) firstRectForCharacterRange:(NSRange)range
                         actualRange:(NSRangePointer)actualRange
{
   int xpos, ypos;
   get_window_pos([self window], xpos, ypos);
   NSRect const content_rect = [[self window] frame];
   return NSMakeRect(xpos, transformY(ypos + content_rect.size.height), 0.0, 0.0);
}

- (void) insertText:(id)string replacementRange:(NSRange)replacementRange
{
   auto*       event = [NSApp currentEvent];
   auto const  mods = ph::translate_flags([event modifierFlags]);
   auto*       characters = ([string isKindOfClass:[NSAttributedString class]]) ?
                 [string string] : (NSString*) string;

   NSUInteger i, length = [characters length];
   for (i = 0;  i < length;  i++)
   {
     const unichar codepoint = [characters characterAtIndex:i];
     if ((codepoint & 0xff00) == 0xf700)
        continue;
     handle_text(*_view.get(), { codepoint, mods });
   }
}

- (void) doCommandBySelector:(SEL)selector
{
}

-(void) windowDidBecomeKey:(NSNotification*) notification
{
   _view->focus(ph::focus_request::begin_focus);
}

-(void) windowDidResignKey:(NSNotification*) notification
{
   _view->focus(ph::focus_request::end_focus);
}

@end

namespace photon
{
   point base_view::cursor_pos() const
   {
      auto  ns_view = get_mac_view(h);
      auto  frame_height = [ns_view frame].size.height;
      auto  pos = [[ns_view window] mouseLocationOutsideOfEventStream];
      return { float(pos.x), float(frame_height - pos.y - 1) };
   }

   point base_view::size() const
   {
      auto frame = [get_mac_view(h) frame];
      return { float(frame.size.width), float(frame.size.height) };
   }

   void base_view::size(point p)
   {
      auto  ns_view = get_mac_view(h);
      ns_view.bounds.origin = NSMakePoint(0, 0);
      [ns_view setFrameSize : NSMakeSize(p.x, p.y)];
   }

   void base_view::refresh()
   {
      get_mac_view(h).needsDisplay = true;
   }

   void base_view::refresh(rect area)
   {
      [get_mac_view(h) setNeedsDisplayInRect
         : CGRectMake(area.left, area.top, area.width(), area.height())
      ];
   }

   void base_view::limits(view_limits limits_, bool maintain_aspect)
   {
      auto ns_view = get_mac_view(h);
      [[ns_view window] setContentMinSize : NSSize{ limits_.min.x, limits_.min.y }];
      [[ns_view window] setContentMaxSize : NSSize{ limits_.max.x, limits_.max.y }];
      if (maintain_aspect)
         [[ns_view window] setContentAspectRatio:NSSize{ limits_.min.x, limits_.min.y } ];
   }

   bool base_view::is_focus() const
   {
      return [[get_mac_view(h) window] isKeyWindow];
   }

   cairo_t* base_view::setup_context()
   {
      auto ns_view = get_mac_view(h);
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

   std::string clipboard()
   {
      NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
      if (![[pasteboard types] containsObject:NSStringPboardType])
         return {};

      NSString* object = [pasteboard stringForType:NSStringPboardType];
      if (!object)
         return {};
      return [object UTF8String];
   }

   void clipboard(std::string const& text)
   {
      NSArray* types = [NSArray arrayWithObjects:NSStringPboardType, nil];

      NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
      [pasteboard declareTypes:types owner:nil];
      [pasteboard setString:[NSString stringWithUTF8String:text.c_str()]
                    forType:NSStringPboardType];
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
}

