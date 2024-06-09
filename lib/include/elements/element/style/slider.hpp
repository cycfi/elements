/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_SLIDER_AUGUST_29_2016)
#define ELEMENTS_STYLE_SLIDER_AUGUST_29_2016

#include <elements/element/slider.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Thumb (You can use this as the slider's thumb)
   ////////////////////////////////////////////////////////////////////////////
   template <unsigned _size>
   class basic_thumb_styler : public element
   {
   public:

      static unsigned const size = _size;

                              basic_thumb_styler(color c = colors::black)
                               : _color(c)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

   private:

      color                   _color;
   };

   template <unsigned size>
   inline view_limits basic_thumb_styler<size>
      ::limits(basic_context const& /* ctx */) const
   {
	  auto pt = point{float(size), float(size)};
	  return view_limits{pt, pt};
   }

   template <unsigned size>
   inline void basic_thumb_styler<size>::draw(context const& ctx)
   {
      auto& thm = get_theme();
      auto& cnv = ctx.canvas;
      auto  indicator_color = thm.indicator_color.level(1.5);
      auto  cp = circle{center_point(ctx.bounds), size/2.0f};

      draw_thumb(cnv, cp, _color, indicator_color);
   }

   template <unsigned size>
   inline basic_thumb_styler<size> basic_thumb(color c = colors::black)
   {
      return {c};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Track (You can use this as the slider's track)
   ////////////////////////////////////////////////////////////////////////////
   template <unsigned _size, bool _vertical = false>
   class basic_track_styler : public element
   {
   public:

      static unsigned const size = _size;
      static bool const vertical = _vertical;
      static unsigned const min_length = 64;

                              basic_track_styler(color c = colors::black)
                               : _color(c)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

   private:

      color                   _color;
   };

   template <unsigned size, bool vertical>
   inline view_limits basic_track_styler<size, vertical>
      ::limits(basic_context const& /* ctx */) const
   {
	  auto sz = float(size);
	  auto min_length_ = float(min_length);
	  auto p1 = vertical ? point{sz, min_length} : point{ min_length_, sz};
	  auto p2 = vertical ? point{sz, full_extent} : point{ full_extent, sz};
	  return view_limits{p1, p2};
   }

   template <unsigned size, bool vertical>
   inline void basic_track_styler<size, vertical>
      ::draw(context const& ctx)
   {
      draw_track(ctx.canvas, ctx.bounds);
   }

   template <unsigned size, bool vertical = false>
   inline basic_track_styler<size, vertical>
   basic_track(color c = colors::black)
   {
      return {c};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Slider Element Base (common base class for slider elements)
   ////////////////////////////////////////////////////////////////////////////
   template <int _size, concepts::Element Subject>
   class slider_styler_base : public proxy<Subject>
   {
   public:

      static unsigned const size = _size;
      using base_type = proxy<Subject>;
      using base_type::base_type;

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;
   };

   template <int size, concepts::Element Subject>
   inline view_limits
   slider_styler_base<size, Subject>::limits(basic_context const& ctx) const
   {
      auto sl = this->subject().limits(ctx);
      if (sl.min.x < sl.min.y) // is vertical?
      {
         sl.min.x += size;
         sl.max.x += size;
         clamp_max(sl.max.x, full_extent);
      }
      else
      {
         sl.min.y += size;
         sl.max.y += size;
         clamp_max(sl.max.y, full_extent);
      }
      return sl;
   }

   template <int size, concepts::Element Subject>
   inline void
   slider_styler_base<size, Subject>::prepare_subject(context& ctx)
   {
      if (ctx.bounds.width() < ctx.bounds.height()) // is vertical?
      {
         ctx.bounds.left += size/2;
         ctx.bounds.right -= size/2;
      }
      else
      {
         ctx.bounds.top += size/2;
         ctx.bounds.bottom -= size/2;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   // Slider Marks (You can use this to place tick marks on slider)
   ////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////
   // deprecated
   ////////////////////////////////////////////////////////////////////////////
   template <
      std::size_t _size, std::size_t _num_divs
    , std::size_t _major_divs, concepts::Element Subject>
   class slider_marks_styler : public slider_styler_base<_size, Subject>
   {
   public:

      static_assert(_major_divs > 0, "Major divisions must be greater than zero");

      using base_type = slider_styler_base<_size, Subject>;
      using base_type::base_type;

      void                    draw(context const& ctx) override;
   };

   void draw_slider_marks(
      canvas& cnv, rect bounds, float size, std::size_t major_divs
    , std::size_t minor_divs, color c);

   template <
      std::size_t _size, std::size_t _num_divs
    , std::size_t _major_divs, concepts::Element Subject>
   inline void
   slider_marks_styler<_size, _num_divs, _major_divs, Subject>
      ::draw(context const& ctx)
   {
      // Draw linear lines
      draw_slider_marks(
         ctx.canvas, ctx.bounds, _size, _num_divs
       , _major_divs, colors::light_gray);

      // Draw the subject
      base_type::draw(ctx);
   }

   template <
      std::size_t _size, std::size_t _num_divs = 50
    , std::size_t _major_divs = 10, concepts::Element Subject>
   [[deprecated("Use slider_marks_lin instead")]]
   inline slider_marks_styler<_size, _num_divs, _major_divs, remove_cvref_t<Subject>>
   slider_marks(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }
   ////////////////////////////////////////////////////////////////////////////

   template <
      std::size_t _size, std::size_t _major_divs
    , std::size_t _minor_divs, concepts::Element Subject>
   class slider_marks_lin_styler : public slider_styler_base<_size, Subject>
   {
   public:

      static_assert(_major_divs > 0, "Major divisions must be greater than zero");

      using base_type = slider_styler_base<_size, Subject>;
      using base_type::base_type;

      void                    draw(context const& ctx) override;
   };

   void draw_slider_marks_lin(
      canvas& cnv, rect bounds, float size, std::size_t major_divs
    , std::size_t minor_divs, color c);

   template <
      std::size_t _size, std::size_t _major_divs
    , std::size_t _minor_divs, concepts::Element Subject>
   inline void
   slider_marks_lin_styler<_size, _major_divs, _minor_divs, Subject>
      ::draw(context const& ctx)
   {
      // Draw linear lines
      draw_slider_marks_lin(
         ctx.canvas, ctx.bounds, _size, _major_divs
       , _minor_divs, colors::light_gray);

      // Draw the subject
      base_type::draw(ctx);
   }

   template <
      std::size_t _size, std::size_t _major_divs = 10
    , std::size_t _minor_divs = 5, concepts::Element Subject>
   inline slider_marks_lin_styler<_size, _major_divs, _minor_divs, remove_cvref_t<Subject>>
   slider_marks_lin(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <
      std::size_t _size, std::size_t _major_divs
    , std::size_t _minor_divs, concepts::Element Subject>
   class slider_marks_log_element : public slider_styler_base<_size, Subject>
   {

   public:

      static_assert(_major_divs > 0, "Major divisions must be greater than zero");

      using base_type = slider_styler_base<_size, Subject>;
      using base_type::base_type;

      void                    draw(context const& ctx) override;
   };

   void draw_slider_marks_log(
      canvas& cnv, rect bounds, float size, std::size_t major_divs
    , std::size_t minor_divs, color c);

   template <
      std::size_t _size, std::size_t _major_divs
      , std::size_t _minor_divs, concepts::Element Subject>
   inline void
   slider_marks_log_element<_size, _major_divs, _minor_divs, Subject>
      ::draw(context const& ctx)
   {
      // Draw logarithmic lines
      draw_slider_marks_log(
         ctx.canvas, ctx.bounds, _size, _major_divs
         , _minor_divs, colors::light_gray);

      // Draw the subject
      base_type::draw(ctx);
   }

   template <
      std::size_t _size, std::size_t _major_divs = 5
      , std::size_t _minor_divs = 10, concepts::Element Subject>
   inline slider_marks_log_element<_size, _major_divs, _minor_divs, remove_cvref_t<Subject>>
   slider_marks_log(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Slider Labels (You can use this to place slider labels with sliders)
   ////////////////////////////////////////////////////////////////////////////
   template <int _size, concepts::Element Subject, std::size_t num_labels>
   class slider_labels_styler
    : public slider_styler_base<abs(_size), Subject>
   {
   public:

      constexpr static int const size = _size;
      using base_type = slider_styler_base<abs(_size), Subject>;
      using string_array = std::array<std::string, num_labels>;

                              slider_labels_styler(
                                 Subject subject
                               , float font_size
                              )
                               : base_type(std::move(subject))
                               , _font_size(font_size)
                              {}

      void                    draw(context const& ctx) override;

      string_array            _labels;
      float                   _font_size;
   };

   void draw_slider_labels(
      canvas& cnv
    , rect bounds
    , float size
    , float font_size
    , std::string const labels[]
    , std::size_t _num_labels
   );

   template <int size, concepts::Element Subject, std::size_t num_labels>
   inline void
   slider_labels_styler<size, Subject, num_labels>::draw(context const& ctx)
   {
      // Draw the subject
      base_type::draw(ctx);

      // Draw the labels
      draw_slider_labels(
         ctx.canvas, ctx.bounds, size
       , _font_size, _labels.data(), num_labels);
   }

   template <int size, concepts::Element Subject, typename... S>
   inline slider_labels_styler<size, remove_cvref_t<Subject>, sizeof...(S)>
   slider_labels(Subject&& subject, float font_size, S&&... s)
   {
      auto r = slider_labels_styler<size, remove_cvref_t<Subject>, sizeof...(S)>
         {std::forward<Subject>(subject), font_size};
      r._labels = {{std::move(s)...}};
      return r;
   }
}

#endif
