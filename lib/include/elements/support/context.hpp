/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_CONTEXT_APRIL_17_2016)
#define ELEMENTS_CONTEXT_APRIL_17_2016

#include <elements/support/point.hpp>
#include <elements/support/rect.hpp>
#include <cairo.h>
#include <functional>
#include <string_view>

namespace cycfi { namespace elements
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Contexts
	////////////////////////////////////////////////////////////////////////////////////////////////
   class view;
   class element;
   class canvas;

   struct basic_context
   {
      basic_context(elements::view& view_, elements::canvas& canvas_)
       : view(view_)
       , canvas(canvas_)
      {}

      basic_context(basic_context const&) = default;
      basic_context& operator=(basic_context const&) = default;

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
       , parent(0), bounds(bounds_)
      {}

      context(context const&) = default;
      context& operator=(context const&) = default;

      using feedback_function = std::function<void(elements::element*, std::string_view what)>;

      template <typename F>
      feedback_function feedback(F&& f) const
      {
         auto save = _feedback;
         _feedback = std::forward<F>(f);
         return save;
      }

      void give_feedback(elements::element* e, char const* what) const
      {
         if (_feedback)
            _feedback(e, what);
         if (parent)
            parent->give_feedback(e, what);
      }

      elements::element*            element;
      context const*                parent;
      elements::rect                bounds;

   private:

      mutable feedback_function     _feedback;
   };
}}

#endif
