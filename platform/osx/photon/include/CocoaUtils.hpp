/*=======================================================================================
   Copyright (c) 2016 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=======================================================================================*/
#if !defined(PHOTON_COCOA_UTILS_OCTOBER_15_2016)
#define PHOTON_COCOA_UTILS_OCTOBER_15_2016

#include <Carbon/Carbon.h>

CFBundleRef GetBundleFromExecutable(const char* filepath);
CFBundleRef GetCurrentBundle();

#endif
