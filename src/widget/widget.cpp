/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/widget.hpp>
#include <photon/support.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // widget class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////

   rect widget::limits(basic_context const& ctx) const
   {
      return full_limits;
   }

   widget* widget::hit_test(context const& ctx, point p)
   {
      return (ctx.bounds.includes(p)) ? this : 0;
   }

   void widget::draw(context const& ctx)
   {
   }

   void widget::layout(context const& ctx)
   {
   }

   widget* widget::click(context const& ctx, mouse_button btn)
   {
      return 0;
   }

   void widget::drag(context const& ctx, mouse_button btn)
   {
   }

//   bool widget::key(context const& ctx, key_info const& k)
//   {
//      return false;
//   }

   bool widget::text(context const& ctx, text_info const& info)
   {
      return false;
   }

   bool widget::cursor(context const& ctx, point p, cursor_tracking status)
   {
      return false;
   }

   bool widget::scroll(context const& ctx, point p)
   {
      return false;
   }

   bool widget::focus(focus_request r)
   {
      return r == focus_request::end_focus;
   }

   widget const* widget::focus() const
   {
      return this;
   }

   widget* widget::focus()
   {
      return this;
   }

   bool widget::is_control() const
   {
      return false;
   }

   void widget::idle(basic_context const& ctx)
   {
   }

   void widget::value(bool val)
   {
   }

   void widget::value(int val)
   {
   }

   void widget::value(double val)
   {
   }

   void widget::value(std::string val)
   {
   }
}
