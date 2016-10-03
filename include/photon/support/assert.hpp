/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_ASSERT_OCTOBER_4_2016)
#define PHOTON_GUI_LIB_ASSERT_OCTOBER_4_2016

#define PHOTON_ASSERT(x, msg) assert((x && msg)); (void)x, void(msg)

#endif
