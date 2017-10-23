/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include "cairo.h"

namespace photon { namespace detail
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
}}
