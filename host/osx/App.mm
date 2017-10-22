/*=================================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=================================================================================================*/
#import <Cocoa/Cocoa.h>

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

@end
