/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_PIXMAP_SEPTEMBER_5_2016)
#define ELEMENTS_PIXMAP_SEPTEMBER_5_2016

#include <vector>
#include <memory>
#include <cairo.h>
#include <elements/support/point.hpp>
#include <stdexcept>

namespace cycfi { namespace elements
{
   class canvas;

   ////////////////////////////////////////////////////////////////////////////
   // Pixmaps
   ////////////////////////////////////////////////////////////////////////////
   struct failed_to_load_pixmap : std::runtime_error
   {
      using std::runtime_error::runtime_error;
   };

   class pixmap
   {
   public:

      explicit          pixmap(point size, float scale = 1);
      explicit          pixmap(char const* filename, float scale = 1);
                        pixmap(pixmap const& rhs) = delete;
                        pixmap(pixmap&& rhs);
                        ~pixmap();

      pixmap&           operator=(pixmap const& rhs) = delete;
      pixmap&           operator=(pixmap&& rhs);

      extent            size() const;
      float             scale() const;
      void              scale(float val);

   private:

      friend class canvas;
      friend class pixmap_context;

      cairo_surface_t*  _surface;
   };

   using pixmap_ptr = std::shared_ptr<pixmap>;

   ////////////////////////////////////////////////////////////////////////////
   // pixmap_context allows drawing into a pixmap
   ////////////////////////////////////////////////////////////////////////////
   class pixmap_context
   {
   public:

      explicit          pixmap_context(pixmap& pm)
                        {
                           _context = cairo_create(pm._surface);
                        }

                        ~pixmap_context()
                        {
                           if (_context)
                              cairo_destroy(_context);
                        }

                        pixmap_context(pixmap_context&& rhs) noexcept
                         : _context(rhs._context)
                        {
                           rhs._context = nullptr;
                        }

      cairo_t*          context() const { return _context; }

   private:
                        pixmap_context(pixmap_context const&) = delete;

      cairo_t*          _context;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline pixmap::pixmap(pixmap&& rhs)
    : _surface(rhs._surface)
   {
      rhs._surface = nullptr;
   }

   inline pixmap& pixmap::operator=(pixmap&& rhs)
   {
      if (this != &rhs)
      {
         _surface = rhs._surface;
         rhs._surface = nullptr;
      }
      return *this;
   }
}}

#endif
