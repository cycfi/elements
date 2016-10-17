/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <CocoaUtils.hpp>
#include <dlfcn.h>

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
