/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/tracker.hpp>
#include <photon/support/context.hpp>

namespace photon
{
   widget* tracker::click(context const& ctx, mouse_button btn)
   {
      if (btn.is_pressed)
      {
         state = new_state(ctx, btn.pos);
         begin_tracking(ctx, *state);
      }
      else
      {
         end_tracking(ctx, *state);
         state.reset();
      }
      return this;
   }

   void tracker::drag(context const& ctx, mouse_button btn)
   {
      state->previous = state->current;
      state->current = btn.pos;
      state->current = state->current.move(-state->offset.x, -state->offset.y);
      keep_tracking(ctx, *state);
   }

   tracker::info_ptr tracker::new_state(context const& ctx, point start)
   {
      return info_ptr{ new info{ start } };
   }

   bool tracker::is_control() const
   {
      return true;
   }
}