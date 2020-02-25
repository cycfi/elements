/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DETAIL_SCRATCH_CONTEXT_SEPTEMBER_26_2016)
#define ELEMENTS_DETAIL_SCRATCH_CONTEXT_SEPTEMBER_26_2016

#include "cairo.h"

namespace cycfi { namespace elements { namespace detail
{
   class scratch_context
   {
   public:

      scratch_context()
      {
         _surface = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
         _context = cairo_create(_surface);
      }

      ~scratch_context()
      {
         cairo_surface_destroy(_surface);
         cairo_destroy(_context);
      }

      cairo_t*          context() const { return _context; }

   private:

      scratch_context(scratch_context const&) = delete;

      cairo_surface_t*  _surface;
      cairo_t*          _context;
   };
}}}

#endif
