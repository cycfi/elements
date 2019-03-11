/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <photon/app.hpp>
#import <Cocoa/Cocoa.h>
#include "nsstring.h"

namespace cycfi { namespace photon
{
   app::app(int argc, const char* argv[])
   {
      [NSApplication sharedApplication];
      [NSApp setActivationPolicy : NSApplicationActivationPolicyRegular];

      id menubar_ = [NSMenu new];
      _menubar = (__bridge void*) menubar_;

      id _app_menu_item = [NSMenuItem new];
      [menubar_ addItem : _app_menu_item];
      [NSApp setMainMenu : menubar_];
      id _app_menu = [NSMenu new];
      id app_name = [[NSProcessInfo processInfo] processName];
      _app_name = make_string(app_name);
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

