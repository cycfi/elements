/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(ELF_PICKUP_MAY_14_2016)
#define ELF_PICKUP_MAY_14_2016

#include <photon/widget/tracker.hpp>

namespace elf
{
   using photon::tracker;
   using photon::basic_context;
   using photon::context;
   using photon::point;
   using photon::rect;
   using photon::cursor_tracking;
   using photon::mouse_button;

   class pickup : public tracker
   {
   public:

      enum type { single, double_ };

                        pickup(float pos, type type_, float slant)
                         : _pos(pos), _type(type_), _slant(slant), _tracking(none)
                        {}

                        pickup(pickup&& rhs) = default;
      pickup&           operator=(pickup&& rhs) = default;

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);
      virtual widget*   hit_test(context const& ctx, point p);
      virtual bool      cursor(context const& ctx, point p, cursor_tracking status);

      virtual void      begin_tracking(context const& ctx, info& track_info);
      virtual void      keep_tracking(context const& ctx, info& track_info);
      virtual void      end_tracking(context const& ctx, info& track_info);

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

      bool              reposition(context const& ctx, point mp);
      hit_item          hit(context const& ctx, point p) const;
      void              pickup_bounds(context const& ctx, rect& r1, rect& r2) const;

      float             _pos;
      type              _type;
      float             _slant;
      tracking_status   _tracking;
      point             _offset;
      point             _rotator_pos;
   };
}

#endif