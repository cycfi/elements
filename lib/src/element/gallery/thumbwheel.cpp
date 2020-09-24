/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/gallery/thumbwheel.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // basic_thumbwheel_element
   ////////////////////////////////////////////////////////////////////////////
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
         _aligner =
            [this, &view = ctx.view, bounds = ctx.bounds](double val)
            {
               view.post(
                  [this, &view, val, bounds]()
                  {
                     do_align(view, bounds, val);
                  }
               );
            };
      }
   }

   void basic_thumbwheel_element::do_align(view& view_, rect const& bounds, double val)
   {
      auto curr = align();
      auto diff = val - curr;
      if (std::abs(diff) < (_quantize / 10))
      {
         align(val);
         if (diff > 0)
            view_.refresh(bounds);
      }
      else
      {
         align(curr + diff/10);
         view_.refresh(bounds);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // basic_vthumbwheel_element
   ////////////////////////////////////////////////////////////////////////////
   view_limits basic_vthumbwheel_element::limits(basic_context const& ctx) const
   {
      auto lim = port_base::limits(ctx);
      auto num_elements = (1.0 / quantize()) + 1;
      lim.min.y /= num_elements;
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
      auto lim = port_base::limits(ctx);
      lim.min.x *= quantize();
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
}}
