/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_ICON_APRIL_19_2016)
#define PHOTON_GUI_LIB_WIDGET_ICON_APRIL_19_2016

#include <photon/widget/widget.hpp>
#include <photon/widget/icon_ids.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Icons
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class icon : public widget
   {
   public:
                        icon(std::uint32_t code_, float size_ = -1);

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);

   private:

      std::uint32_t     _code;
      float             _size;
   };
}

#endif