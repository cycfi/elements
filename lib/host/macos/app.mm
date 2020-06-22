/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#import <Cocoa/Cocoa.h>

namespace cycfi { namespace elements
{
   app::app(
      int         argc
    , char*       argv[]
    , std::string // name     Note: Unlike Windows and Linux, MacOS contains
    , std::string // id             the app name and id in the plist.info file.
                  //                We ignore the ctor params here and use those
                  //                instead.
   )
   {
      [NSApplication sharedApplication];
      [NSApp setActivationPolicy : NSApplicationActivationPolicyRegular];

      id menubar_ = [NSMenu new];
      _menubar = (__bridge void*) menubar_;

      id _app_menu_item = [NSMenuItem new];
      [menubar_ addItem : _app_menu_item];
      [NSApp setMainMenu : menubar_];
      id _app_menu = [NSMenu new];
      id app_name = [NSRunningApplication currentApplication].localizedName;
      _app_name = [app_name UTF8String];
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
      [NSApp activateIgnoringOtherApps : YES];
      [NSApp run];
   }

   void app::stop()
   {
      [NSApp terminate : nil];
   }
}}

