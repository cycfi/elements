/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(INFINITY_PICKUP_MAY_14_2016)
#define INFINITY_PICKUP_MAY_14_2016

#include <photon/widget/tracker.hpp>
#include <functional>

namespace infinity
{
   using photon::tracker;
   using photon::basic_context;
   using photon::context;
   using photon::point;
   using photon::rect;
   using photon::cursor_tracking;
   using photon::mouse_button;
   using photon::widget_limits;

   class pickup : public tracker<>
   {
   public:

      enum type { single, double_ };
      using pickup_function = std::function<void(double val)>;

                              pickup(double pos, type type_, double slant, char id)
                               : _pos(pos)
                               , _type(type_)
                               , _slant(slant)
                               , _tracking(none)
                               , _id(id)
                               , _is_visible(true)
                              {}

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);
      virtual widget*         hit_test(context const& ctx, point p);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            begin_tracking(context const& ctx, info& track_info);
      virtual void            keep_tracking(context const& ctx, info& track_info);
      virtual void            end_tracking(context const& ctx, info& track_info);

      void                    visible(bool val)       { _is_visible = val; }
      void                    set_type(type type_)    { _type = type_; }
      void                    position(double pos);
      void                    angle(double slant);

      pickup_function         on_position_change;
      pickup_function         on_slant_change;

   private:

      enum tracking_status
      {
         none,
         start,
         tracking_move,
         tracking_rotate
      };

      enum hit_item
      {
         hit_none,
         hit_pickup,
         hit_rotator
      };

      bool                    reposition(context const& ctx, point mp);
      hit_item                hit(context const& ctx, point p) const;
      void                    pickup_bounds(context const& ctx, rect& r1, rect& r2) const;

      double                  _pos;
      type                    _type;
      double                  _slant;
      char                    _id;
      tracking_status         _tracking;
      point                   _offset;
      point                   _rotator_pos;
      bool                    _is_visible;
   };
}

#endif