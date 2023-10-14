/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_CONTEXT_APRIL_17_2016)
#define ELEMENTS_CONTEXT_APRIL_17_2016

#include <infra/string_view.hpp>
#include <elements/support/point.hpp>
#include <elements/support/rect.hpp>
#include <functional>

namespace cycfi { namespace elements
{
 	////////////////////////////////////////////////////////////////////////////////////////////////
   // Forward declarations

   class view;
   class element;
   class canvas;

   point    cursor_pos(view const& v);
   rect     view_bounds(view const& v);
   point    device_to_user(point p, canvas& cnv);
   rect     device_to_user(rect const& r, canvas& cnv);

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Contexts
	////////////////////////////////////////////////////////////////////////////////////////////////

   struct basic_context
   {
      basic_context(elements::view& view_, elements::canvas& cnv)
       : view(view_)
       , canvas(cnv)
      {}

      basic_context(basic_context const&) = default;
      basic_context& operator=(basic_context const&) = delete;

      rect view_bounds() const
      {
         return device_to_user(elements::view_bounds(view), canvas);
      }

      point cursor_pos() const
      {
         return device_to_user(elements::cursor_pos(view), canvas);
      }

      elements::view&        view;
      elements::canvas&      canvas;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   class context : public basic_context
   {
   public:

      context(context const& rhs, elements::rect bounds_)
       : basic_context(rhs.view, rhs.canvas), element(rhs.element)
       , parent(rhs.parent), bounds(bounds_)
      {}

      context(context const& parent_, element* element_, elements::rect bounds_)
       : basic_context(parent_.view, parent_.canvas), element(element_)
       , parent(&parent_), bounds(bounds_)
      {}

      context(class view& view_, class canvas& canvas_, element* element_, elements::rect bounds_)
       : basic_context(view_, canvas_), element(element_)
       , parent(nullptr), bounds(bounds_)
      {}

      context(context const&) = default;
      context& operator=(context const&) = delete;

      context sub_context() const
      {
         auto ctx = context{ *this };
         ctx.parent = this;
         return ctx;
      }

      template <typename T, typename F>
      void listen(F&& f)
      {
         _listener =
            [f](auto const& ctx, auto* e, auto what)
            {
               if (auto te = dynamic_cast<T*>(e))
                  f(ctx, *te, what);
            };
      }

      void notify(context const& ctx, string_view what, elements::element* e) const
      {
         if (_listener)
            _listener(ctx, e, what);
         if (parent)
            parent->notify(ctx, what, e);
      }

      elements::element*            element;
      context const*                parent;
      elements::rect                bounds;

   private:

      using listener_function =
         std::function<
            void(context const& ctx, elements::element*, string_view what)
         >;

      listener_function             _listener;
   };
}}

#endif
