/*=======================================================================================
   Copyright © 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=======================================================================================*/
#if !defined(PHOTON_OSX_VIEW_AUGUST_20_2016)
#define PHOTON_OSX_VIEW_AUGUST_20_2016

#import <Cocoa/Cocoa.h>
#import <PhotonView.hpp>
#include <photon/view.hpp>

namespace photon
{
   class platform_view_access
   {
   public:

      static void init_view(view& v, PhotonView* self_)
      {
         v._impl = (__bridge view_impl*) self_;
      }

      static PhotonView* get_view(view const& v)
      {
         return (__bridge PhotonView*) v._impl;
      }
   };
};

#endif
