/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_CONTEXT_APRIL_17_2016)
#define PHOTON_GUI_LIB_CONTEXT_APRIL_17_2016

#include <photon/support/point.hpp>
#include <photon/support/rect.hpp>
#include <cairo.h>

namespace photon
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Contexts
	////////////////////////////////////////////////////////////////////////////////////////////////
   class view;
   class element;
   class canvas;

   struct basic_context
   {
      basic_context(photon::view& view_, photon::canvas& canvas_)
       : view(view_)
       , canvas(canvas_)
      {}

      basic_context(basic_context const&) = default;
      basic_context& operator=(basic_context const&) = default;

      photon::view&        view;
      photon::canvas&      canvas;
   };

	////////////////////////////////////////////////////////////////////////////////////////////////
   struct context : basic_context
   {
      context(context const& rhs, photon::rect bounds_)
       : basic_context(rhs.view, rhs.canvas), element(rhs.element)
       , parent(rhs.parent), bounds(bounds_)
      {}

      context(context const& parent_, element* element_, photon::rect bounds_)
       : basic_context(parent_.view, parent_.canvas), element(element_)
       , parent(&parent_), bounds(bounds_)
      {}

      context(class view& view_, class canvas& canvas_, element* element_, photon::rect bounds_)
       : basic_context(view_, canvas_), element(element_)
       , parent(0), bounds(bounds_)
      {}

      context(context const&) = default;
      context& operator=(context const&) = default;

      photon::element*  element;
      context const*    parent;
      photon::rect      bounds;
   };


}
#endif
