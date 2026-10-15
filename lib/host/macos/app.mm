/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#import <Cocoa/Cocoa.h>

namespace cycfi::elements
{
   // Note: Unlike Windows and Linux, MacOS contains the app name and id in the
   //       plist.info file. We ignore the ctor params here and use those instead.
   app::app(std::string name)
    : _app_name(std::move(name))
   {
      [NSApplication sharedApplication];
      [NSApp setActivationPolicy : NSApplicationActivationPolicyRegular];

      id menubar_ = [NSMenu new];
      _menubar = (__bridge void*) menubar_;

      id _app_menu_item = [NSMenuItem new];
      [menubar_ addItem : _app_menu_item];
      [NSApp setMainMenu : menubar_];
      id _app_menu = [NSMenu new];
      // The name given, or the process's own without one
      if (_app_name.empty())
         _app_name = [[NSRunningApplication currentApplication].localizedName UTF8String];
      id app_name = [NSString stringWithUTF8String : _app_name.c_str()];
      id quitTitle = [@"Quit " stringByAppendingString : app_name];
      id quitMenuItem = [[NSMenuItem alloc] initWithTitle : quitTitle
         action : @selector(terminate:) keyEquivalent : @"q"];
      [_app_menu addItem : quitMenuItem];
      [_app_menu_item setSubmenu : _app_menu];
   }

   app::~app()
   {
   }

   void app::run()
   {
      on_start();
      [NSApp activateIgnoringOtherApps : YES];
      [NSApp run];
   }

   // Stop the run loop, so run() returns, as on every host. The event
   // posted wakes the loop to see the stop.
   void app::stop()
   {
      on_stop();
      [NSApp stop : nil];
      NSEvent* wake = [NSEvent otherEventWithType : NSEventTypeApplicationDefined
         location : NSZeroPoint
         modifierFlags : 0
         timestamp : 0
         windowNumber : 0
         context : nil
         subtype : 0
         data1 : 0
         data2 : 0];
      [NSApp postEvent : wake atStart : YES];
   }
}

