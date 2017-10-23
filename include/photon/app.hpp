/*=======================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(PHOTON_GUI_LIB_APP_OCTOBER_23_2017)
#define PHOTON_GUI_LIB_APP_OCTOBER_23_2017

#include <photon/view.hpp>
#include <functional>

namespace photon
{
   class app
   {
   public:

      template <typename F>
      app(F init_view)
      {
         // Initialize the host's view creation function
         photon::new_view =
            [init_view](host_view* h)
            {
               auto view_p = std::make_unique<view>(h);
               init_view(*view_p.get());
               return std::move(view_p);
            };
      }

      app(app const&) = delete;
      app& operator=(app const&) = delete;

      int main(int argc, const char* argv[])
      {
         return app_main(argc, argv);
      }
   };
}

#endif
