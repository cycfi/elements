/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/element.hpp>
#include <photon/support.hpp>
#include <photon/view.hpp>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // element class implementation
   ////////////////////////////////////////////////////////////////////////////
   view_limits element::limits(basic_context const& ctx) const
   {
      return full_limits;
   }

   view_span element::span() const
   {
      return { 1.0f, 1.0f };
   }

   element* element::hit_test(context const& ctx, point p)
   {
      return (ctx.bounds.includes(p)) ? this : 0;
   }

   void element::draw(context const& ctx)
   {
   }

   void element::layout(context const& ctx)
   {
   }

   void element::refresh(context const& ctx, element& element)
   {
      if (&element == this)
         ctx.view.refresh(ctx);
   }

   element* element::click(context const& ctx, mouse_button btn)
   {
      return 0;
   }

   void element::drag(context const& ctx, mouse_button btn)
   {
   }

   bool element::key(context const& ctx, key_info k)
   {
      return false;
   }

   bool element::text(context const& ctx, text_info info)
   {
      return false;
   }

   bool element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      return false;
   }

   bool element::scroll(context const& ctx, point dir, point p)
   {
      return false;
   }

   bool element::focus(focus_request r)
   {
      return r == focus_request::end_focus;
   }

   element const* element::focus() const
   {
      return this;
   }

   element* element::focus()
   {
      return this;
   }

   bool element::is_control() const
   {
      return false;
   }

   void element::idle(basic_context const& ctx)
   {
   }

   void element::value(bool val)
   {
   }

   void element::value(int val)
   {
   }

   void element::value(double val)
   {
   }

   void element::value(std::string val)
   {
   }
}}
