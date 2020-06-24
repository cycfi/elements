/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/element.hpp>
#include <elements/support.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // element class implementation
   ////////////////////////////////////////////////////////////////////////////
   view_limits element::limits(basic_context const& ctx) const
   {
      ignore(ctx);
      return full_limits;
   }

   view_stretch element::stretch() const
   {
      return { 1.0f, 1.0f };
   }

   element* element::hit_test(context const& ctx, point p)
   {
      ignore(ctx);
      return (ctx.bounds.includes(p)) ? this : nullptr;
   }

   void element::draw(context const& ctx)
   {
      ignore(ctx);
   }

   void element::layout(context const& ctx)
   {
      ignore(ctx);
   }

   void element::refresh(context const& ctx, element& element, int outward)
   {
      if (&element == this)
         ctx.view.refresh(ctx, outward);
   }

   element* element::click(context const& ctx, mouse_button btn)
   {
      ignore(ctx, btn);
      return nullptr;
   }

   void element::drag(context const& ctx, mouse_button btn)
   {
      ignore(ctx, btn);
   }

   bool element::key(context const& ctx, key_info k)
   {
      ignore(ctx, k);
      return false;
   }

   bool element::text(context const& ctx, text_info info)
   {
      ignore(ctx, info);
      return false;
   }

   bool element::cursor(context const& ctx, point p, cursor_tracking status)
   {
      ignore(ctx, p, status);
      return false;
   }

   bool element::scroll(context const& ctx, point dir, point p)
   {
      ignore(ctx, dir, p);
      return false;
   }

   bool element::wants_focus() const
   {
      return false;
   }

   void element::begin_focus()
   {
   }

   void element::end_focus()
   {
   }

   element const* element::focus() const
   {
      return this;
   }

   element* element::focus()
   {
      return this;
   }

   bool element::wants_control() const
   {
      return false;
   }

   void element::on_tracking(context const& ctx, tracking state)
   {
      ctx.view.manage_on_tracking(*this, state);
   }
}}
