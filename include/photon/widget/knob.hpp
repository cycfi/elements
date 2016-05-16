/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_KNOB_MAY_16_2016)
#define PHOTON_GUI_LIB_WIDGET_KNOB_MAY_16_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Knobs
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class knob : public widget
   {
   public:

      knob()
       : _pos(0)
       , _tracking(false)
      {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);
      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      is_control() const;

   private:

      void              reposition(context const& ctx);
      double            _pos;
      point             _offset;
      bool              _tracking;
      point             _indicator_pos;
   };
}

#endif