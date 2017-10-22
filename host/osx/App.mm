/*=================================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=================================================================================================*/
#import <host.hpp>
#import <Cocoa/Cocoa.h>
#import <AppKit/NSApplication.h>

///////////////////////////////////////////////////////////////////////////////
// AppDelegate Interface

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (assign, nonatomic) IBOutlet NSWindow *window;
@end

///////////////////////////////////////////////////////////////////////////////
// AppDelegate Implementation

@implementation AppDelegate

@synthesize window = _windows;

- (void) applicationDidFinishLaunching : (NSNotification*) notification
{
}

- (void) applicationWillTerminate : (NSNotification*) notification
{
}

///////////////////////////////////////////////////////////////////////////////
// Application event loop entry
namespace photon
{
   int app_main(int argc, const char* argv[])
   {
      return NSApplicationMain(argc, argv);
   }
}

@end
