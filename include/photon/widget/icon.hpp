/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_ICON_APRIL_19_2016)
#define PHOTON_GUI_LIB_WIDGET_ICON_APRIL_19_2016

#include <photon/widget/widget.hpp>
#include <photon/widget/text_utils.hpp>
#include <photon/widget/icon_ids.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Icons
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <uint32_t code, int size>
   class icon : public widget
   {
   public:

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <uint32_t code, int size>
   inline rect icon<code, size>::limits(basic_context const& ctx) const
   {
      point s = text_utils(ctx.theme()).measure_icon(code, size);
      return { s.x, s.y, s.x, s.y };
   }

   template <uint32_t code, int size>
   inline void icon<code, size>::draw(context const& ctx)
   {
      text_utils(ctx.theme()).draw_icon(ctx.bounds, code, size);
   }
}

#endif