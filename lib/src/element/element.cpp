/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

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
   view_limits element::limits(basic_context const& /* ctx */) const
   {
      return full_limits;
   }

   view_stretch element::stretch() const
   {
      return { 1.0f, 1.0f };
   }

   element* element::hit_test(context const& ctx, point p)
   {
      return (ctx.bounds.includes(p)) ? this : 0;
   }

   void element::draw(context const& /* ctx */)
   {
   }

   void element::layout(context const& /* ctx */)
   {
   }

   void element::refresh(context const& ctx, element& element, int outward)
   {
      if (&element == this)
         ctx.view.refresh(ctx, outward);
   }

   element* element::click(context const& /* ctx */, mouse_button /* btn */)
   {
      return nullptr;
   }

   void element::drag(context const& /* ctx */, mouse_button /* btn */)
   {
   }

   bool element::key(context const& /* ctx */, key_info /* k */)
   {
      return false;
   }

   bool element::text(context const& /* ctx */, text_info /* info */)
   {
      return false;
   }

   bool element::cursor(context const& /* ctx */, point /* p */, cursor_tracking /* status */)
   {
      return false;
   }

   bool element::scroll(context const& /* ctx */, point /* dir */, point /* p */)
   {
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

   bool element::is_control() const
   {
      return false;
   }

   void element::value(bool /* val */)
   {
   }

   void element::value(int /* val */)
   {
   }

   void element::value(double /* val */)
   {
   }

   void element::value(std::string_view /* val */)
   {
   }

   void element::on_tracking(context const& ctx, tracking state)
   {
      ctx.view.manage_on_tracking(*this, state);
   }
}}
