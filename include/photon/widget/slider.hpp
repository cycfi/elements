/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SLIDER_APRIL_16_2016)
#define PHOTON_GUI_LIB_WIDGET_SLIDER_APRIL_16_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Sliders
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class slider : public widget
   {
   public:

      static float      aspect_ratio;     // the slider's aspect ratio
      static float      slot_size;        // fraction of width (or height)
      static float      knob_size;        // fraction of width (or height)

                        slider()
                         : _pos(0.0), _tracking(false)
                        {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);
      virtual void      draw_slot(theme& thm, float pos, rect knob_r, rect bounds, bool hilite);
      virtual void      draw_knob(theme& thm, float pos, rect bounds, bool hilite);
      virtual void      draw_indicator(theme& thm, float pos, rect bounds, bool hilite);

      virtual widget*   hit_test(context const& ctx, point p);
      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      scroll(context const& ctx, point p);
      virtual bool      is_control() const;

   private:

      void              reposition(context const& ctx);

      double            _pos;
      point             _offset;
      bool              _tracking;
   };
}

#endif