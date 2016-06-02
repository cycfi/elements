/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_KNOB_MAY_16_2016)
#define PHOTON_GUI_LIB_WIDGET_KNOB_MAY_16_2016

#include <photon/widget/widget.hpp>
#include <photon/canvas.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Knobs
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class knob : public widget
   {
   public:
                        knob(bool pan_ = false)
                         : _pan(pan_)
                         , _pos(pan_ ? 0.5 : 0)
                         , _tracking(false)
                        {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual widget*   hit_test(context const& ctx, point p);
      virtual void      draw(context const& ctx);
      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      scroll(context const& ctx, point p);
      virtual bool      is_control() const;

      virtual void      draw_knob(theme& thm, rect bounds, bool hilite);
      virtual point     draw_indicator(theme& thm, rect bounds, bool hilite);
      virtual void      draw_gauge(theme& thm, rect bounds, bool hilite);

      double            position() const           { return _pos; }
      void              position(double pos)       { _pos = pos; }

   protected:

      bool              tracking() const           { return _tracking; }
      point             indicator_pos() const      { return _indicator_pos; }
      void              indicator_pos(point pos)   { _indicator_pos = pos; }

   private:

      void              reposition(context const& ctx);

      bool              _pan;
      double            _pos;
      bool              _tracking;
      point             _offset;
      point             _indicator_pos;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   class image_knob : public knob
   {
   public:

      using image_ptr = std::shared_ptr<canvas::image>;

                        image_knob(
                           image_ptr img_
                         , float size_
                         , std::size_t num_images_
                         , bool pan_ = false
                        );

      virtual rect      limits(basic_context const& ctx) const;

      virtual void      draw(context const& ctx);
      virtual void      draw_knob(theme& thm, rect bounds, bool hilite);
      virtual point     draw_indicator(theme& thm, rect bounds, bool hilite);

   private:

      image_ptr         _img;
      float             _size;
      std::size_t       _num_images;
   };
}

#endif
