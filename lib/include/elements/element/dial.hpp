/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DIAL_AUGUST_30_2016)
#define ELEMENTS_DIAL_AUGUST_30_2016

#include <elements/element/proxy.hpp>
#include <elements/element/tracker.hpp>
#include <elements/support.hpp>
#include <infra/support.hpp>
#include <functional>

namespace cycfi { namespace elements
{
   enum class dial_mode_enum : int
   {
      linear,
      radial,
   };

   ////////////////////////////////////////////////////////////////////////////
   // Dials
   ////////////////////////////////////////////////////////////////////////////
   class dial_base : public tracker<proxy_base>, public receiver<double>
   {
   public:

      using dial_function = std::function<void(double pos)>;

                           dial_base(double init_value = 0.0);

      void                 prepare_subject(context& ctx) override;
      element*             hit_test(context const& ctx, point p) override;

      bool                 scroll(context const& ctx, point dir, point p) override;
      void                 keep_tracking(context const& ctx, tracker_info& track_info) override;

      double               value() const override;
      void                 value(double val) override;
      void                 edit(view& view_, double val) override;

      dial_function        on_change;

   private:

      double               radial_value(context const& ctx, tracker_info& track_info);
      double               linear_value(context const& ctx, tracker_info& track_info);
      double               compute_value(context const& ctx, tracker_info& track_info);

      double               _value;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, dial_base>
   dial(Subject&& subject, double init_value = 0.0)
   {
      return { std::forward<Subject>(subject), init_value };
   }

   inline double dial_base::value() const
   {
      return _value;
   }

   inline element* dial_base::hit_test(context const& ctx, point p)
   {
      return element::hit_test(ctx, p);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Knob (You can use this as the subject of dial)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size>
   class basic_knob_element : public element, public receiver<double>
   {
   public:

      static std::size_t const size = _size;

                              basic_knob_element(color c = colors::black)
                               : _color(c), _value(0)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      double                  value() const override { return _value; }
      void                    value(double val) override;

   private:

      color                   _color;
      float                   _value;
   };

   template <std::size_t size>
   inline view_limits basic_knob_element<size>::limits(basic_context const& /* ctx */) const
   {
	  auto pt = point{ float(size), float(size) };
      return view_limits{ pt, pt };
   }

   template <std::size_t size>
   inline void basic_knob_element<size>::draw(context const& ctx)
   {
      auto& thm = get_theme();
      auto& cnv = ctx.canvas;
      auto  indicator_color = thm.indicator_color.level(1.5);
      auto  cp = circle{ center_point(ctx.bounds), ctx.bounds.width()/2 };

      draw_knob(cnv, cp, _color);
      draw_radial_indicator(cnv, cp, _value, indicator_color);
   }

   template <std::size_t size>
   inline void basic_knob_element<size>::value(double val)
   {
      _value = val;
   }

   template <std::size_t size>
   inline basic_knob_element<size> basic_knob(color c = colors::black)
   {
      return {c};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Radial Element Base (common base class for radial elements)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size, typename Subject>
   class radial_element_base : public proxy<Subject>
   {
   public:

      static std::size_t const size = _size;

      using base_type = proxy<Subject>;

                              radial_element_base(Subject subject)
                               : base_type(std::move(subject))
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;
   };

   template <std::size_t size, typename Subject>
   inline view_limits
   radial_element_base<size, Subject>::limits(basic_context const& ctx) const
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

   template <std::size_t size, typename Subject>
   inline void
   radial_element_base<size, Subject>::prepare_subject(context& ctx)
   {
      auto size_div2 = size /2;
      ctx.bounds.top += size_div2;
      ctx.bounds.left += size_div2;
      ctx.bounds.bottom -= size_div2;
      ctx.bounds.right -= size_div2;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Radial Marks (You can use this to place dial tick marks around dials)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size, typename Subject>
   class radial_marks_element : public radial_element_base<_size, Subject>
   {
   public:

      static std::size_t const size = _size;
      using base_type = radial_element_base<_size, Subject>;
      using base_type::base_type;

      void                    draw(context const& ctx) override;
   };

   template <std::size_t size, typename Subject>
   inline void
   radial_marks_element<size, Subject>::draw(context const& ctx)
   {
      // Draw the subject
      base_type::draw(ctx);

      // Draw radial lines
      auto cp = circle{ center_point(ctx.bounds), ctx.bounds.width()/2 };
      draw_radial_marks(ctx.canvas, cp, size-2, colors::light_gray);
   }

   template <std::size_t size, typename Subject>
   inline radial_marks_element<size, remove_cvref_t<Subject>>
   radial_marks(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Radial Labels (You can use this to place dial labels around dials)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size, typename Subject, std::size_t num_labels>
   class radial_labels_element : public radial_element_base<_size, Subject>
   {
   public:

      static std::size_t const size = _size;
      using base_type = radial_element_base<_size, Subject>;
      using string_array = std::array<std::string, num_labels>;

                              radial_labels_element(Subject subject, float font_size)
                               : base_type(std::move(subject))
                               , _font_size(font_size)
                              {}

      void                    draw(context const& ctx) override;

      string_array            _labels;
      float                   _font_size;
   };

   template <std::size_t size, typename Subject, std::size_t num_labels>
   inline void
   radial_labels_element<size, Subject, num_labels>::draw(context const& ctx)
   {
      // Draw the subject
      base_type::draw(ctx);

      // Draw the labels
      auto cp = circle{ center_point(ctx.bounds), ctx.bounds.width()/2 };
      draw_radial_labels(
         ctx.canvas, cp, _font_size, _labels.data(), num_labels);
   }

   template <std::size_t size, typename Subject, typename... S>
   inline radial_labels_element<size, remove_cvref_t<Subject>, sizeof...(S)>
   radial_labels(Subject&& subject, float font_size, S&&... s)
   {
      auto r = radial_labels_element<size, remove_cvref_t<Subject>, sizeof...(S)>
         { std::move(subject), font_size };
      r._labels = {{ std::move(s)... }};
      return r;
   }
}}

#endif
