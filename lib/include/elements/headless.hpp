/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_HEADLESS_SEPTEMBER_13_2026)
#define ELEMENTS_HEADLESS_SEPTEMBER_13_2026

#include <elements/base_view.hpp>
#include <vector>

namespace cycfi::elements::headless
{
   ////////////////////////////////////////////////////////////////////////////
   // The headless host (ELEMENTS_HOST_UI_LIBRARY=headless) runs views with no
   // windowing system under them: a view keeps its own size, draws into
   // whatever canvas it is given, and takes events from direct calls. These
   // stand in for what a real host does between the view and the system.
   // They are defined only by the headless host.
   //
   // With the Artist recording backend, drawing lands in a journal instead of
   // pixels, so a test can assert on what a view drew and where.
   ////////////////////////////////////////////////////////////////////////////

   // Open the view, as a host does when its window first appears.
   void                 open(base_view& v, float scale = 1);

   // The areas the view asked to be repainted since the last call, in the
   // order asked, in view coordinates. A refresh of the whole view is its
   // full bounds.
   std::vector<rect>    take_invalidated(base_view& v);

   // Move the cursor, as a host does before it reports a mouse event.
   void                 cursor_pos(base_view& v, point p);

   // The cursor most recently asked for with set_cursor.
   cursor_type          cursor();
}

#endif
