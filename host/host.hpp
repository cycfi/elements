/*=======================================================================================
   Copyright © 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(PHOTON_VIEW_AUGUST_20_2016)
#define PHOTON_VIEW_AUGUST_20_2016

namespace photon
{
   struct host_view;

   class base_view
   {
   public:

      base_view(host_view* h)
       : h(h)
      {}

      virtual ~base_view()
      {}

      virtual void draw(float left, float top, float right, float bottom) = 0;

   private:

      host_view* h;
   };

   extern base_view* (*new_view)(host_view* h);
}

#endif
