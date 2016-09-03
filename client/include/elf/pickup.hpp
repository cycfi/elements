/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(ELF_PICKUP_MAY_14_2016)
#define ELF_PICKUP_MAY_14_2016

#include <photon/widget.hpp>

namespace elf
{
   using photon::widget;
   using photon::basic_context;
   using photon::context;
   using photon::rect;

   class pickup : public widget
   {
   public:

      pickup(float pos, bool single, float slant)
       : _pos(pos), _single(single), _slant(slant)
      {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);

      //virtual widget*   hit_test(context const& ctx, point p);
      //
      //virtual widget*   click(context const& ctx, mouse_button btn);
      //virtual void      drag(context const& ctx, mouse_button btn);
      //virtual bool      is_control() const;

   private:

      void              pickup_bounds(context const& ctx, rect& r1, rect& r2) const;

      float             _pos;
      bool              _single;
      float             _slant;
   };
}

#endif