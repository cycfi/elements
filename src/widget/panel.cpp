/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/panel.hpp>
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>

namespace photon
{
   void panel_widget::draw(context const& ctx)
   {
      ctx.theme().draw_panel(ctx.bounds);
   }

   void frame_widget::draw(context const& ctx)
   {
      ctx.theme().draw_frame(ctx.bounds);
   }
}
