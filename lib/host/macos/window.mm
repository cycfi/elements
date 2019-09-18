/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>
#import <Cocoa/Cocoa.h>

namespace elements = cycfi::elements;

#define ELEMENTS_WINDOW_CLASS ELEMENTS_CLASS_PREFIX##ElementsWindow

@interface ELEMENTS_WINDOW_CLASS : NSWindow
{
   elements::window* _pwin;
   NSRect _saved_frame;
}
@end

@compatibility_alias ElementsWindow ELEMENTS_WINDOW_CLASS;

@implementation ElementsWindow

- (void) set_elements_window : (elements::window*) pwin
{
   self->_pwin = pwin;
}

- (void)zoom:(id)sender
{
   if (!self.zoomed)
      _saved_frame = self.frame;
   [super zoom : sender];
}

- (void) close
{
   [super close];
   auto title = [self title];
   if ([title length] != 0)
   {
      bool zoomed = self.zoomed;
      if (zoomed)
         [self setFrame : _saved_frame display : NO animate : false];
      [self saveFrameUsingName : title];

      bool is_fullscreen = [self styleMask] & NSWindowStyleMaskFullScreen;

      NSMutableString* key =[title mutableCopy];
      [key appendString : @"_zoom_state"];

      auto zoom_state = zoomed? (is_fullscreen? @"fullscreen" : @"zoomed") : @"standard";
      [[NSUserDefaults standardUserDefaults] setObject : zoom_state forKey : key];
      [[NSUserDefaults standardUserDefaults] synchronize];
   }
   if (_pwin->on_close)
      _pwin->on_close();
}

@end

namespace cycfi { namespace elements
{
   namespace
   {
      inline int window_style(int option)
      {
         return
            ((option & window::with_title)? NSWindowStyleMaskTitled : 0)
          | ((option & window::closable)? NSWindowStyleMaskClosable : 0)
          | ((option & window::miniaturizable)? NSWindowStyleMaskMiniaturizable : 0)
          | ((option & window::resizable)? NSWindowStyleMaskResizable : 0)
          ;
      }
   }

   window::window(std::string const& name, int style_, rect const& bounds)
   {
      auto style = window_style(style_);

      ElementsWindow* window_ =
         [[ElementsWindow alloc]
            initWithContentRect : NSMakeRect(0, 0, 0, 0)
            styleMask : window_style(style_)
            backing : NSBackingStoreBuffered
            defer : NO
         ];
      _window = (__bridge host_window*) window_;

      window_.appearance = [NSAppearance appearanceNamed : NSAppearanceNameVibrantDark];
      [window_ setTitle : [NSString stringWithUTF8String : name.c_str()]];
      [window_ makeKeyAndOrderFront : nil];
      [window_ set_elements_window : this];

      // Make the window background transparent
      [window_ setBackgroundColor: [NSColor colorWithCalibratedRed:0 green:0 blue:0 alpha:0]];

      auto title = [window_ title];
      if ([title length] != 0)
      {
         if (![window_ setFrameUsingName : title])
         {
            size({ bounds.width(), bounds.height() });
            position({ bounds.left, bounds.top });
         }
         else
         {
            NSMutableString* key =[title mutableCopy];
            [key appendString : @"_zoom_state"];

            auto zoom_state = [[NSUserDefaults standardUserDefaults] stringForKey : key];
            if ([zoom_state isEqualToString : @"zoomed"])
               [window_ zoom : window_];
            else if ([zoom_state isEqualToString : @"fullscreen"])
               [window_ toggleFullScreen : window_];
         }
      }
   }

   window::~window()
   {
   }

   point window::size() const
   {
      id const& window_ = (__bridge id) _window;
      auto const& frame = [window_ frame];
      return { float(frame.size.width), float(frame.size.height) };
   }

   void window::size(point const& p)
   {
      id window_ = (__bridge id) _window;
      auto frame = [window_ frame];
      frame.size.width = p.x;
      frame.size.height = p.y;
      [window_ setFrame : frame display : YES animate : false];
   }

   void window::limits(view_limits limits_)
   {
      auto minx = std::max<float>(150, limits_.min.x);
      auto miny = std::max<float>(100, limits_.min.y);
      id const window_ = (__bridge id) _window;
      [window_ setContentMinSize : NSSize{ minx, miny }];
      [window_ setContentMaxSize : NSSize{ limits_.max.x, limits_.max.y }];


      auto _size = [[window_ contentView] frame].size;
      elements::clamp(_size.width, minx, limits_.max.x);
      elements::clamp(_size.height, miny, limits_.max.y);
      [window_ setContentSize : _size];
   }

   point window::position() const
   {
      id const window_ = (__bridge id) _window;
      auto const& frame = [window_ frame];
      NSRect sframe = [[NSScreen mainScreen] frame];

      return {
         float(frame.origin.x - sframe.origin.x)
       , float((sframe.size.height - frame.size.height) - (frame.origin.y - sframe.origin.y))
      };
   }

   void window::position(point const& p)
   {
      id window_ = (__bridge id) _window;
      NSRect sframe = [[NSScreen mainScreen] frame];

      auto frame = [window_ frame];
      frame.origin.x = sframe.origin.x + p.x;
      frame.origin.y = sframe.origin.y + (sframe.size.height - frame.size.height) - p.y;

      [window_ setFrame : frame display : YES animate : false];
   }
}}

