/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <host/host.hpp>
#include <dlfcn.h>
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

namespace
{
   CFBundleRef GetBundleFromExecutable(const char* filepath)
   {
      NSString* execStr = [NSString stringWithCString:filepath encoding:NSUTF8StringEncoding];
      NSString* macOSStr = [execStr stringByDeletingLastPathComponent];
      NSString* contentsStr = [macOSStr stringByDeletingLastPathComponent];
      NSString* bundleStr = [contentsStr stringByDeletingLastPathComponent];
      return CFBundleCreate (0, (CFURLRef)[NSURL fileURLWithPath:bundleStr isDirectory:YES]);
   }

   CFBundleRef GetCurrentBundle()
   {
      Dl_info info;
      if (dladdr ((const void*)GetCurrentBundle, &info))
      {
         if (info.dli_fname)
         {
            return GetBundleFromExecutable(info.dli_fname);
         }
      }
      return 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
// Application event loop entry
namespace photon
{
   int app_main(int argc, const char* argv[])
   {
      // Before anything else, set the working directory so we can access
      // our resources
      CFBundleRef mainBundle = GetCurrentBundle();
      CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
      char path[PATH_MAX];
      CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX);
      CFRelease(resourcesURL);
      chdir(path);

      return NSApplicationMain(argc, argv);
   }
}

@end
