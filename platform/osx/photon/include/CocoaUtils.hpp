/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_COCOA_UTILS_OCTOBER_15_2016)
#define PHOTON_COCOA_UTILS_OCTOBER_15_2016

#include <Carbon/Carbon.h>

CFBundleRef GetBundleFromExecutable(const char* filepath);
CFBundleRef GetCurrentBundle();

#endif
