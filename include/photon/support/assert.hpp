/*=======================================================================================
   Copyright (c) 2016 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(PHOTON_GUI_LIB_ASSERT_OCTOBER_4_2016)
#define PHOTON_GUI_LIB_ASSERT_OCTOBER_4_2016

#define PHOTON_ASSERT(x, msg) assert((x && msg)); (void)x, void(msg)

#endif
