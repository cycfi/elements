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

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////////////////////////
   inline icon::icon(std::uint32_t code_, float size_)
    : _code(code_)
    , _size(size_)
   {}
   
   inline rect icon::limits(basic_context const& ctx) const
   {
      float font_size = (_size > 0)? _size : ctx.theme().icon_font_size;
      point s = text_utils(ctx.theme()).measure_icon(_code, font_size);
      return { s.x, s.y, s.x, s.y };
   }

   inline void icon::draw(context const& ctx)
   {
      float font_size = (_size > 0)? _size : ctx.theme().icon_font_size;
      text_utils(ctx.theme()).draw_icon(ctx.bounds, _code, font_size);
   }
}

#endif