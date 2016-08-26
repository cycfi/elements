/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <photon/support.hpp>
#include <photon/widget.hpp>

namespace client
{
   using namespace photon;

   void  init(view& v)
   {
      auto box = basic(
         [](context const& ctx)
         {
            auto c = ctx.canvas();
            c.fill_style(colors::gold.opacity(0.8));
            c.fill_round_rect(ctx.bounds, 4);
         }
      );

      v.content.elements.push_back(new_(box));
   }
}
