/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/style/thumbwheel.hpp>
#include <elements/view.hpp>
#include <cmath>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // basic_thumbwheel_element
   ////////////////////////////////////////////////////////////////////////////
   basic_thumbwheel_element::basic_thumbwheel_element(basic_thumbwheel_element const& rhs)
    : receiver<double>(rhs)
    , _quantize(rhs._quantize)
   {
   }

   basic_thumbwheel_element::~basic_thumbwheel_element()
   {
      *_alive = false;
   }

   double basic_thumbwheel_element::value() const
   {
      return align();
   }

   void basic_thumbwheel_element::value(double val)
   {
      if (_quantize > 0 && _aligner)
         _aligner(std::round(val / _quantize) * _quantize);
      else
         align(val);
   }

   void basic_thumbwheel_element::make_aligner(context const& ctx)
   {
      if (_quantize > 0)
      {
         // The view refreshes in device coordinates, as view::refresh
         // converts a context's bounds.
         auto tl = ctx.canvas.user_to_device(ctx.bounds.top_left());
         auto br = ctx.canvas.user_to_device(ctx.bounds.bottom_right());
         auto bounds = rect{tl.x, tl.y, br.x, br.y};
         _aligner =
            [this, alive = _alive, &view = ctx.view, bounds](double val)
            {
               view.post(
                  [this, alive, &view, val, bounds]()
                  {
                     if (*alive)
                        do_align(view, bounds, val);
                  }
               );
            };
      }
   }

   void basic_thumbwheel_element::do_align(view& view_, rect const& bounds, double val)
   {
      // A tenth of the way at each step, until there.
      auto curr = align();
      auto diff = val - curr;
      if (std::abs(diff) < (_quantize / 10))
      {
         align(val);
         if (diff != 0)
            view_.refresh(bounds);
      }
      else
      {
         align(curr + diff/10);
         view_.refresh(bounds);
         view_.post(
            [this, alive = _alive, &view_, bounds, val]()
            {
               if (*alive)
                  do_align(view_, bounds, val);
            }
         );
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_vthumbwheel_element
   ////////////////////////////////////////////////////////////////////////////
   // Quantized, one item of the subject shows; continuous, the wheel is a
   // port of the size it is given.
   view_limits basic_vthumbwheel_element::limits(basic_context const& ctx) const
   {
      if (quantize() <= 0)
         return vport_element::limits(ctx);
      auto lim = port_base::limits(ctx);
      lim.min.y /= std::round(1.0 / quantize()) + 1;
      lim.max.y = lim.min.y;
      return lim;
   }

   void basic_vthumbwheel_element::draw(context const& ctx)
   {
      make_aligner(ctx);
      vport_element::draw(ctx);
   }

   void basic_vthumbwheel_element::align(double val)
   {
      clamp(val, 0.0, 1.0);
      valign(val);
   }

   double basic_vthumbwheel_element::align() const
   {
      return valign();
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_hthumbwheel_element
   ////////////////////////////////////////////////////////////////////////////
   view_limits basic_hthumbwheel_element::limits(basic_context const& ctx) const
   {
      if (quantize() <= 0)
         return hport_element::limits(ctx);
      auto lim = port_base::limits(ctx);
      lim.min.x /= std::round(1.0 / quantize()) + 1;
      lim.max.x = lim.min.x;
      return lim;
   }

   void basic_hthumbwheel_element::draw(context const& ctx)
   {
      make_aligner(ctx);
      hport_element::draw(ctx);
   }

   void basic_hthumbwheel_element::align(double val)
   {
      clamp(val, 0.0, 1.0);
      halign(val);
   }

   double basic_hthumbwheel_element::align() const
   {
      return halign();
   }
}
