/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_DIAL_AUGUST_30_2016)
#define PHOTON_GUI_LIB_WIDGET_DIAL_AUGUST_30_2016

#include <photon/element/proxy.hpp>
#include <photon/element/tracker.hpp>
#include <photon/support.hpp>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Dials
   ////////////////////////////////////////////////////////////////////////////
   class dial_base : public tracker<proxy_base>
   {
   public:

      using dial_function = std::function<void(double pos)>;
      using tracker<proxy_base>::value;

                           dial_base(double init_value = 0.0);
      virtual              ~dial_base() {}

      virtual void         prepare_subject(context& ctx);

      virtual bool         scroll(context const& ctx, point dir, point p);
      virtual void         begin_tracking(context const& ctx, info& track_info);
      virtual void         keep_tracking(context const& ctx, info& track_info);
      virtual void         end_tracking(context const& ctx, info& track_info);

      double               value() const;
      void                 value(double val);
      virtual double       value_from_point(context const& ctx, point p);

      dial_function        on_change;

   private:

      double               _value;
   };

   template <typename Subject>
   inline proxy<typename std::decay<Subject>::type, dial_base>
   dial(Subject&& subject, double init_value = 0.0)
   {
      return { std::forward<Subject>(subject), init_value };
   }

   inline double dial_base::value() const
   {
      return _value;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Knob (You can use this as the subject of dial)
   ////////////////////////////////////////////////////////////////////////////
   template <int _size>
   class basic_knob_element : public element
   {
   public:

      static int const size = _size;

                              basic_knob_element() : _value(0) {}
      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);
      virtual void            value(double val);

   private:

      float                   _value;
   };

   template <int size>
   inline view_limits basic_knob_element<size>::limits(basic_context const& ctx) const
   {
      return { { size, size }, { size, size } };
   }

   template <int size>
   inline void basic_knob_element<size>::draw(context const& ctx)
   {
      void draw_indicator(canvas& cnv, circle cp, float val, color c);

      auto& thm = get_theme();
      auto& cnv = ctx.canvas;
      auto  indicator_color = thm.indicator_color.level(1.5);
      auto  cp = circle{ center_point(ctx.bounds), ctx.bounds.width()/2 };

      draw_knob(cnv, cp, colors::black);
      draw_indicator(cnv, cp, _value, indicator_color);
   }

   template <int size>
   inline void basic_knob_element<size>::value(double val)
   {
      _value = val;
   }

   template <int size>
   inline basic_knob_element<size> basic_knob()
   {
      return {};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Radial Marks (You can use this to place dial tick marks on dials)
   ////////////////////////////////////////////////////////////////////////////
   namespace radial_consts
   {
      constexpr double const _2pi = 2 * M_PI;
      constexpr double const travel = 0.83;
      constexpr double const range = _2pi * travel;
      constexpr double const start_angle = _2pi * (1-travel)/2;
      constexpr double offset = (2 * M_PI) * (1-travel)/2;
   }

   template <int _size, typename Subject>
   class radial_marks_element : public proxy<Subject>
   {
   public:

      static int const size = _size;

      using base_type = proxy<Subject>;

                              radial_marks_element(Subject&& subject)
                               : base_type(std::move(subject))
                              {}

                              radial_marks_element(Subject const& subject)
                               : base_type(subject)
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);
      virtual void            draw(context const& ctx);
   };

   template <int size, typename Subject>
   inline view_limits
   radial_marks_element<size, Subject>::limits(basic_context const& ctx) const
   {
      auto sl = this->subject().limits(ctx);

      sl.min.x += size;
      sl.max.x += size;
      sl.min.y += size;
      sl.max.y += size;

      clamp_max(sl.max.x, full_extent);
      clamp_max(sl.max.y, full_extent);
      return sl;
   }

   template <int size, typename Subject>
   inline void
   radial_marks_element<size, Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.top += size;
      ctx.bounds.left += size;
      ctx.bounds.bottom -= size;
      ctx.bounds.right -= size;
   }

   template <int size, typename Subject>
   inline void
   radial_marks_element<size, Subject>::draw(context const& ctx)
   {
      void draw_radial_marks(canvas& cnv, circle cp, float size, color c);

      // Draw the subject
      base_type::draw(ctx);

      // Draw radial lines
      auto& cnv = ctx.canvas;
      auto cp = circle{ center_point(ctx.bounds), ctx.bounds.width()/2 };
      draw_radial_marks(cnv, cp, size, colors::light_gray);
   }

   template <int size, typename Subject>
   inline radial_marks_element<size, Subject>
   radial_marks(Subject&& subject)
   {
      return {std::move(subject)};
   }
}

#endif
