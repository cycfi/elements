/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SLIDER_APRIL_16_2016)
#define PHOTON_GUI_LIB_WIDGET_SLIDER_APRIL_16_2016

#include <photon/widget/widget.hpp>
#include <photon/support.hpp>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Sliders
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class slider : public widget
   {
   public:

      struct constants
      {
         float aspect_ratio   = 0.2;   // the slider's aspect ratio
         float slot_size      = 0.25;  // fraction of width
         float knob_size      = 0.6;   // fraction of size (width or height)
      };

      using slider_function = std::function<double(double pos)>;

                        slider(bool draw_gauge = true)
                         : _draw_gauge(draw_gauge)
                         , _pos(0.0)
                         , _final_pos(0.0)
                         , _tracking(false)
                        {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual widget*   hit_test(context const& ctx, point p);
      virtual void      draw(context const& ctx);
      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      scroll(context const& ctx, point p);
      virtual bool      is_control() const;
      virtual void      idle(basic_context const& ctx);

      virtual void      draw_slot(theme& thm, rect knob_r, rect bounds, bool hilite);
      virtual void      draw_gauge(theme& thm, rect knob_r, rect bounds, bool hilite);
      virtual void      draw_knob(theme& thm, rect bounds, bool horiz, bool hilite);
      virtual void      draw_indicator(theme& thm, rect bounds, bool horiz, bool hilite);

      virtual constants dimensions() const   { return constants{}; }
      double            position() const     { return _pos; }
      void              position(double pos) { _pos = pos; }

      slider_function   on_change;

   private:

      void              reposition(context const& ctx);

      double            _pos;
      double            _final_pos;
      point             _offset;
      bool              _tracking;
      bool              _draw_gauge;
      time_point        _last_update;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   class vimage_slider : public slider
   {
   public:

      using image_ptr = std::shared_ptr<canvas::image>;

                        vimage_slider(
                           image_ptr img_, float aspect_ratio_
                         , rect knob_bounds
                         , bool draw_gauge = true
                        );

      virtual void      draw_knob(theme& thm, rect bounds, bool horiz, bool hilite);
      virtual void      draw_indicator(theme& thm, rect bounds, bool horiz, bool hilite);
      virtual constants dimensions() const;

   private:

      image_ptr         _img;
      float             _aspect_ratio;
      rect             _knob_bounds;
   };
}

#endif