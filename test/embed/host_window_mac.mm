/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include "host_window.hpp"
#import <Cocoa/Cocoa.h>

namespace embed_test
{
   window open_window(int width, int height)
   {
      [NSApplication sharedApplication];
      [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];

      NSWindow* win = [[NSWindow alloc]
         initWithContentRect:NSMakeRect(100, 100, width, height)
         styleMask:NSWindowStyleMaskTitled
         backing:NSBackingStoreBuffered
         defer:NO];
      [win setReleasedWhenClosed:NO];
      [win orderFront:nil];
      return {(__bridge_retained void*) [win contentView]};
   }

   void close_window(window const& w)
   {
      NSView* view = (__bridge_transfer NSView*) w.handle;
      [[view window] close];
   }

   void run_events(int ms)
   {
      NSDate* end = [NSDate dateWithTimeIntervalSinceNow:ms / 1000.0];
      while ([end timeIntervalSinceNow] > 0)
      {
         @autoreleasepool
         {
            NSEvent* e = [NSApp nextEventMatchingMask:NSEventMaskAny
               untilDate:[NSDate dateWithTimeIntervalSinceNow:0.01]
               inMode:NSDefaultRunLoopMode dequeue:YES];
            if (e)
               [NSApp sendEvent:e];
         }
      }
   }
}
