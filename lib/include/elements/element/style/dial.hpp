/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_DIAL_AUGUST_30_2016)
#define ELEMENTS_STYLE_DIAL_AUGUST_30_2016

#include <elements/element/dial.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Knob (You can use this as the subject of dial)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size>
   class basic_knob_styler : public element
   {
   public:

      static std::size_t const size = _size;

                              basic_knob_styler(color c = colors::black);

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

   private:

      color                   _color;
   };

   template <std::size_t size>
   inline basic_knob_styler<size> basic_knob(color c = colors::black);

   ////////////////////////////////////////////////////////////////////////////
   // Radial Element Base (common base class for radial elements)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size, concepts::Element Subject>
   class radial_styler_base : public proxy<Subject>
   {
   public:

      static std::size_t const size = _size;

      using base_type = proxy<Subject>;

                              radial_styler_base(Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Radial Marks (You can use this to place dial tick marks around dials)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size, concepts::Element Subject>
   class radial_marks_styler : public radial_styler_base<_size, Subject>
   {
   public:

      static std::size_t const size = _size;
      using base_type = radial_styler_base<_size, Subject>;
      using base_type::base_type;

      void                    draw(context const& ctx) override;
   };

   template <std::size_t size, concepts::Element Subject>
   inline radial_marks_styler<size, remove_cvref_t<Subject>>
   radial_marks(Subject&& subject);

   ////////////////////////////////////////////////////////////////////////////
   // Radial Labels (You can use this to place dial labels around dials)
   ////////////////////////////////////////////////////////////////////////////
   template <std::size_t _size, concepts::Element Subject, std::size_t num_labels>
   class radial_labels_styler : public radial_styler_base<_size, Subject>
   {
   public:

      static std::size_t const size = _size;
      using base_type = radial_styler_base<_size, Subject>;
      using string_array = std::array<std::string, num_labels>;

                              radial_labels_styler(Subject subject, float font_size);

      void                    draw(context const& ctx) override;

      string_array            _labels;
      float                   _font_size;
   };

   template <std::size_t size, concepts::Element Subject, typename... S>
   inline radial_labels_styler<size, remove_cvref_t<Subject>, sizeof...(S)>
   radial_labels(Subject&& subject, float font_size, S&&... s);

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   template <std::size_t size>
   inline basic_knob_styler<size>::basic_knob_styler(color c)
    : _color(c)
   {}

   template <std::size_t size>
   inline view_limits basic_knob_styler<size>::limits(basic_context const& /*ctx*/) const
   {
      auto pt = point{float(size), float(size)};
      return view_limits{pt, pt};
   }

   template <std::size_t size>
   inline void basic_knob_styler<size>::draw(context const& ctx)
   {
      auto dial = find_parent<basic_dial*>(ctx);
      if (!dial)
         return;

      auto& thm = get_theme();
      auto& cnv = ctx.canvas;
      auto  indicator_color = thm.indicator_color.level(1.5);
      auto  cp = circle{center_point(ctx.bounds), ctx.bounds.width()/2};

      draw_knob(cnv, cp, _color);
      draw_radial_indicator(cnv, cp, dial->value(), indicator_color);
   }

   template <std::size_t size>
   inline basic_knob_styler<size> basic_knob(color c)
   {
      return {c};
   }

   template <std::size_t _size, concepts::Element Subject>
   inline radial_styler_base<_size, Subject>::radial_styler_base(Subject subject)
    : base_type(std::move(subject))
   {}

   template <std::size_t size, concepts::Element Subject>
   inline view_limits
   radial_styler_base<size, Subject>::limits(basic_context const& ctx) const
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

   template <std::size_t size, concepts::Element Subject>
   inline void
   radial_styler_base<size, Subject>::prepare_subject(context& ctx)
   {
      auto size_div2 = size /2;
      ctx.bounds.top += size_div2;
      ctx.bounds.left += size_div2;
      ctx.bounds.bottom -= size_div2;
      ctx.bounds.right -= size_div2;
   }

   template <std::size_t size, concepts::Element Subject>
   inline void
   radial_marks_styler<size, Subject>::draw(context const& ctx)
   {
      // Draw the subject
      base_type::draw(ctx);

      // Draw radial lines
      auto cp = circle{center_point(ctx.bounds), ctx.bounds.width()/2};
      draw_radial_marks(ctx.canvas, cp, size-2, colors::light_gray);
   }

   template <std::size_t size, concepts::Element Subject>
   inline radial_marks_styler<size, remove_cvref_t<Subject>>
   radial_marks(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <std::size_t _size, concepts::Element Subject, std::size_t num_labels>
   inline radial_labels_styler<_size, Subject, num_labels>
      ::radial_labels_styler(Subject subject, float font_size)
    : base_type(std::move(subject))
    , _font_size(font_size)
   {}

   template <std::size_t size, concepts::Element Subject, std::size_t num_labels>
   inline void
   radial_labels_styler<size, Subject, num_labels>::draw(context const& ctx)
   {
      // Draw the subject
      base_type::draw(ctx);

      // Draw the labels
      auto cp = circle{center_point(ctx.bounds), ctx.bounds.width()/2};
      draw_radial_labels(
         ctx.canvas, cp, _font_size, _labels.data(), num_labels);
   }

   template <std::size_t size, concepts::Element Subject, typename... S>
   inline radial_labels_styler<size, remove_cvref_t<Subject>, sizeof...(S)>
   radial_labels(Subject&& subject, float font_size, S&&... s)
   {
      auto r = radial_labels_styler<size, remove_cvref_t<Subject>, sizeof...(S)>
         {std::move(subject), font_size};
      r._labels = {{std::move(s)...}};
      return r;
   }
}

#endif
