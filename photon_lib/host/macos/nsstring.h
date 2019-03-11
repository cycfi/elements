/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(CYCFI_PHOTON_HOST_MACOS_NSSTRING_MARCH_6_2019)
#define CYCFI_PHOTON_HOST_MACOS_NSSTRING_MARCH_6_2019

#import <Cocoa/Cocoa.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// NSString conversions
///////////////////////////////////////////////////////////////////////////////

inline NSString* make_nsstring(std::string const& str)
{
   NSString* result = [[NSString alloc] initWithUTF8String : str.c_str()];
   return result;
}

inline std::string make_string(NSString const* str)
{
   return [str UTF8String];
}

#endif