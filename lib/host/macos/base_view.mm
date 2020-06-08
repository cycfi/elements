/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>
#include <elements/support/resource_paths.hpp>
#include <elements/support/font.hpp>
#import <Cocoa/Cocoa.h>
#include <dlfcn.h>
#include <memory>
#include <map>
#include <cairo-quartz.h>
#include <infra/filesystem.hpp>

#if ! __has_feature(objc_arc)
# error "ARC is off"
#endif

namespace ph = cycfi::elements;
namespace fs = cycfi::fs;
using key_map = std::map<ph::key_code, ph::key_action>;

///////////////////////////////////////////////////////////////////////////////
// Helper utils
namespace
{
   CFBundleRef get_bundle_from_executable(const char* filepath)
   {
      NSString* exec_str = [NSString stringWithCString:filepath encoding : NSUTF8StringEncoding];
      NSString* mac_os_str = [exec_str stringByDeletingLastPathComponent];
      NSString* contents_str = [mac_os_str stringByDeletingLastPathComponent];
      NSString* bundleStr = [contents_str stringByDeletingLastPathComponent];
      return CFBundleCreate(0, (CFURLRef)[NSURL fileURLWithPath:bundleStr isDirectory : YES]);
   }

   CFBundleRef get_current_bundle()
   {
      Dl_info info;
      if (dladdr((const void*)get_current_bundle, &info) && info.dli_fname)
         return get_bundle_from_executable(info.dli_fname);
      return 0;
   }

   void activate_font(fs::path font_path)
   {
      NSArray* available_fonts = [[NSFontManager sharedFontManager] availableFonts];
      if (![available_fonts containsObject : [NSString stringWithUTF8String : font_path.stem().c_str()]])
      {
         auto furl = [NSURL fileURLWithPath : [NSString stringWithUTF8String : font_path.c_str()]];
         assert(furl);

         CFErrorRef error = nullptr;
         CTFontManagerRegisterFontsForURL((__bridge CFURLRef) furl, kCTFontManagerScopeProcess, &error);
      }
   }

   void get_resource_path(char resource_path[])
   {
      CFBundleRef main_bundle = get_current_bundle();
      CFURLRef resources_url = CFBundleCopyResourcesDirectoryURL(main_bundle);
      CFURLGetFileSystemRepresentation(resources_url, TRUE, (UInt8*) resource_path, PATH_MAX);
      CFRelease(resources_url);
      CFRelease(main_bundle);
   }

   struct resource_setter
   {
      resource_setter()
      {
         // Before anything else, set the working directory so we can access
         // our resources
         char resource_path[PATH_MAX];
         get_resource_path(resource_path);
         cycfi::elements::resource_paths.push_back(resource_path);

         // Load the user fonts from the Resource folder. Normally this is automatically
         // done on application startup, but for plugins, we need to explicitly load
         // the user fonts ourself.
         for (fs::directory_iterator it{ resource_path }; it != fs::directory_iterator{}; ++it)
            if (it->path().extension() == ".ttf")
               activate_font(it->path());
      }
   };
}

namespace cycfi { namespace elements
{
   // These functions are defined in key.mm:
   key_code    translate_key(unsigned int key);
   int         translate_flags(NSUInteger flags);
   NSUInteger  translate_key_to_modifier_flag(key_code key);

   // This is declared in font.hpp
   fs::path get_user_fonts_directory()
   {
      char resource_path[PATH_MAX];
      get_resource_path(resource_path);
      return fs::path(resource_path);
   }
}}

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

      xpos = content_rect.origin.x;
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
// ElementsView Interface

#define ELEMENTS_VIEW_CLASS ELEMENTS_CLASS_PREFIX##ElementsView

@interface ELEMENTS_VIEW_CLASS : NSView <NSTextInputClient>
{
   NSTimer*                         _task;
   NSTrackingArea*                  _tracking_area;
   NSMutableAttributedString*       _marked_text;
   key_map                          _keys;
   bool                             _start;
   ph::base_view*                   _view;
}
@end

@compatibility_alias ElementsView ELEMENTS_VIEW_CLASS;

@implementation ElementsView

- (void) elements_init : (ph::base_view*) view_
{
   static resource_setter init_resources;

   _view = view_;
   _start = true;
   _task =
      [NSTimer scheduledTimerWithTimeInterval : 0.016 // 60Hz
           target : self
         selector : @selector(on_tick:)
         userInfo : nil
          repeats : YES
      ];

   _tracking_area = nil;
   [self updateTrackingAreas];

   _marked_text = [[NSMutableAttributedString alloc] init];
}

- (void) dealloc
{
   _task = nil;
   _view = nullptr;
}

- (void) on_tick : (id) sender
{
   _view->poll();
}

- (void) attach_notifications
{
   [[NSNotificationCenter defaultCenter]
      addObserver : self
         selector : @selector(windowDidBecomeKey:)
             name : NSWindowDidBecomeKeyNotification
           object : [self window]
   ];

   [[NSNotificationCenter defaultCenter]
      addObserver : self
         selector : @selector(windowDidResignKey:)
             name : NSWindowDidResignMainNotification
           object : [self window]
   ];
}

- (void) detach_notifications
{
   [[NSNotificationCenter defaultCenter]
      removeObserver : self
                name : NSWindowDidBecomeKeyNotification
              object : [self window]
   ];

   [[NSNotificationCenter defaultCenter]
      removeObserver : self
                name : NSWindowDidResignMainNotification
              object : [self window]
   ];
}

- (void) detach_timer
{
   [_task invalidate];
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

- (void) drawRect : (NSRect)dirty
{
   [super drawRect : dirty];

   auto w = [self bounds].size.width;
   auto h = [self bounds].size.height;

   auto context_ref = NSGraphicsContext.currentContext.CGContext;
   cairo_surface_t* surface = cairo_quartz_surface_create_for_cg_context(context_ref, w, h);
   cairo_t* context = cairo_create(surface);

   _view->draw(context,
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

- (void) mouseDown : (NSEvent*) event
{
   _view->click(get_button(event, self));
   [self displayIfNeeded];
}

- (void) mouseDragged : (NSEvent*) event
{
   _view->drag(get_button(event, self));
   [self displayIfNeeded];
}

- (void) mouseUp : (NSEvent*) event
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

    [self addTrackingArea : _tracking_area];
    [super updateTrackingAreas];
}

- (void) mouseEntered : (NSEvent*) event
{
   [[self window] setAcceptsMouseMovedEvents : YES];
   [[self window] makeFirstResponder : self];
   auto pos = [event locationInWindow];
   pos = [self convertPoint : pos fromView : nil];
   _view->cursor({ float(pos.x), float(pos.y) }, ph::cursor_tracking::entering);
   [self displayIfNeeded];
}

- (void) mouseExited : (NSEvent*) event
{
   [[self window] setAcceptsMouseMovedEvents : NO];
   auto pos = [event locationInWindow];
   pos = [self convertPoint : pos fromView : nil];
   _view->cursor({ float(pos.x), float(pos.y) }, ph::cursor_tracking::leaving);
   [self displayIfNeeded];
}

- (void) mouseMoved : (NSEvent*) event
{
   auto pos = [event locationInWindow];
   pos = [self convertPoint : pos fromView : nil];
   _view->cursor({ float(pos.x), float(pos.y) }, ph::cursor_tracking::hovering);
   [self displayIfNeeded];
   [super mouseMoved: event];
}

- (void) scrollWheel : (NSEvent*) event
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

- (void) keyDown : (NSEvent*) event
{
   auto const key = ph::translate_key([event keyCode]);
   auto const mods = ph::translate_flags([event modifierFlags]);
   handle_key(_keys, *_view, { key, ph::key_action::press, mods });
   [self interpretKeyEvents : [NSArray arrayWithObject:event]];
}

- (void) flagsChanged : (NSEvent*) event
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

   handle_key(_keys, *_view, { key, action, mods });
}

- (void) keyUp : (NSEvent*) event
{
   auto const key = ph::translate_key([event keyCode]);
   auto const mods = ph::translate_flags([event modifierFlags]);

   handle_key(_keys, *_view, { key, ph::key_action::release, mods });
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

- (NSUInteger) characterIndexForPoint : (NSPoint)point
{
   return 0;
}

- (NSRect) firstRectForCharacterRange : (NSRange)range
                          actualRange : (NSRangePointer)actualRange
{
   int xpos, ypos;
   get_window_pos([self window], xpos, ypos);
   NSRect const content_rect = [[self window] frame];
   return NSMakeRect(xpos, transformY(ypos + content_rect.size.height), 0.0, 0.0);
}

- (void) insertText:(id)string replacementRange : (NSRange)replacementRange
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
     handle_text(*_view, { codepoint, mods });
   }
}

- (void) doCommandBySelector : (SEL) selector
{
}

-(void) windowDidBecomeKey : (NSNotification*) notification
{
   _view->begin_focus();
}

-(void) windowDidResignKey : (NSNotification*) notification
{
   _view->end_focus();
}

@end // @implementation ElementsView

namespace cycfi { namespace elements
{
   namespace
   {
      ElementsView* get_mac_view(ph::host_view_handle h)
      {
         return (__bridge ElementsView*) h;
      }
   }

   base_view::base_view(extent size_)
   {
      auto frame = NSMakeRect(0, 0, size_.x, size_.y);
      ElementsView* content = [[ElementsView alloc] initWithFrame : frame];
      content.autoresizingMask = NSViewWidthSizable |  NSViewHeightSizable;

      _view = (__bridge host_view_handle) content;
      [content elements_init : this];
      [get_mac_view(host()) attach_notifications];
   }

   base_view::base_view(host_window_handle h)
   {
      ElementsView* content = [[ElementsView alloc] init];
      _view = (__bridge host_view_handle) content;
      content.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
      [content elements_init : this];

      NSWindow* window_ = (__bridge NSWindow*) h;
      [window_ setContentView : content];
      [get_mac_view(host()) attach_notifications];
   }

   base_view::base_view(host_view_handle h)
   {
      auto const default_frame = NSMakeRect(0, 0, 100, 100);
      NSView* parent_view = (__bridge NSView*) h;
      auto parent_frame = parent_view? [parent_view frame] : default_frame;
      auto frame = NSMakeRect(0, 0, parent_frame.size.width, parent_frame.size.height);
      ElementsView* content = [[ElementsView alloc] initWithFrame : frame];
      content.autoresizingMask = NSViewWidthSizable |  NSViewHeightSizable;

      _view = (__bridge host_view_handle) content;
      [content elements_init : this];
      if (parent_view)
         [parent_view addSubview : content];
      [get_mac_view(host()) attach_notifications];
   }

   base_view::~base_view()
   {
      auto ns_view = get_mac_view(host());
      [ns_view detach_notifications];
      [ns_view detach_timer];
      [ns_view removeFromSuperview];
      _view = nil;
   }

   point base_view::cursor_pos() const
   {
      auto  ns_view = get_mac_view(host());
      auto  pos = [[ns_view window] mouseLocationOutsideOfEventStream];
      pos = [ns_view convertPoint : pos fromView : nil];
      return { float(pos.x), float(pos.y) };
   }

   extent base_view::size() const
   {
      auto frame = [get_mac_view(host()) frame];
      return { float(frame.size.width), float(frame.size.height) };
   }

   void base_view::size(elements::extent size_)
   {
      [get_mac_view(host()) setFrameSize : NSSize{ size_.x, size_.y }];
   }

   void base_view::refresh()
   {
      [get_mac_view(host()) setNeedsDisplay : YES];
   }

   void base_view::refresh(rect area)
   {
      [get_mac_view(host()) setNeedsDisplayInRect
         : CGRectMake(area.left, area.top, area.width(), area.height())
      ];
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

