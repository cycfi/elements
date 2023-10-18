/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/element.hpp>
#include <elements/support.hpp>
#include <elements/view.hpp>
#include <typeinfo>

#if defined(__GNUC__) || defined(__clang__)
# include <cxxabi.h>

   static std::string demangle(const char* name)
   {
      int status = 0;
      std::unique_ptr<char, void(*)(void*)> res{
            abi::__cxa_demangle(name, nullptr, nullptr, &status),
            std::free
         };

      return status==0? res.get() : name;
   }
#else
   static std::string demangle(const char* name)
   {
      return name;
   }
#endif

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

   unsigned element::span() const
   {
      return 1;
   }

   element* element::hit_test(context const& ctx, point p, bool /*leaf*/)
   {
      return (ctx.bounds.includes(p)) ? this : nullptr;
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

   bool element::click(context const& /* ctx */, mouse_button /* btn */)
   {
      return false;
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

   void element::enable(bool /*state*/)
   {
   }

   bool element::is_enabled() const
   {
      return true;
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

   void element::track_drop(context const& /*ctx*/, drop_info const& /*info*/, cursor_tracking /*status*/)
   {
   }

   bool element::drop(context const& /*ctx*/, drop_info const& /*info*/)
   {
      return false;
   }

   void element::on_tracking(context const& ctx, tracking state)
   {
      ctx.view.manage_on_tracking(*this, state);
   }

   void element::on_tracking(view& view_, tracking state)
   {
      view_.manage_on_tracking(*this, state);
   }

   std::string element::class_name() const
   {
      return demangle(typeid(*this).name());
   }
}}
