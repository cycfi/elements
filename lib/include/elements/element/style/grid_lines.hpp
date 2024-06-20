/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_GRID_LINES_APRIL_11_2016)
#define ELEMENTS_STYLE_GRID_LINES_APRIL_11_2016

#include <elements/element/element.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines - Vertical
   ////////////////////////////////////////////////////////////////////////////
   class vgrid_lines : public element
   {
   public:

                     vgrid_lines(float major_divisions, float minor_divisions)
                      : _major_divisions(major_divisions)
                      , _minor_divisions(minor_divisions)
                     {}

      void           draw(context const& ctx) override;

   private:

      float          _major_divisions;
      float          _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines - Horizontal
   ////////////////////////////////////////////////////////////////////////////
   class hgrid_lines : public element
   {
   public:

                     hgrid_lines(float major_divisions, float minor_divisions)
                      : _major_divisions(major_divisions)
                      , _minor_divisions(minor_divisions)
                     {}

      void           draw(context const& ctx) override;

   private:

      float          _major_divisions;
      float          _minor_divisions;
   };
}

#endif
