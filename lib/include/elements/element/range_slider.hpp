#if !defined(ELEMENTS_RANGE_SLIDER_NOVEMBER_30_2023)
#define ELEMENTS_RANGE_SLIDER_NOVEMBER_30_2023

#include <elements/element/slider.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/tracker.hpp>
#include <elements/view.hpp>
#include <elements/support.hpp>
#include <infra/support.hpp>

#include <cmath>
#include <functional>
#include <type_traits>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Sliders
   ////////////////////////////////////////////////////////////////////////////
   class range_slider_base : public tracker<>, public receiver<std::pair<double, double>>
   {
   using val_type = std::pair<double, double>;
   public:

                              range_slider_base(std::pair<double, double> init_value)
                               : _value_min(init_value.first), _value_max(init_value.second)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;

      bool                    scroll(context const& ctx, point dir, point p) override;
      void                    begin_tracking(context const& ctx, tracker_info& track_info) override;
      void                    keep_tracking(context const& ctx, tracker_info& track_info) override;
      void                    end_tracking(context const& ctx, tracker_info& track_info) override;

      val_type                value() const override;
      void                    value(val_type val) override;
      void                    edit(view& view_, val_type val) override;
      virtual void            edit_value(val_type val) { value(val);}

      rect                    track_bounds(context const& ctx) const;
      std::pair<rect, rect>   thumb_bounds(context const& ctx) const;
      virtual double          value_from_point(context const& ctx, point p);

      virtual std::pair<std::reference_wrapper<element const>, std::reference_wrapper<element const>> thumb() const = 0;
      virtual std::pair<std::reference_wrapper<element>, std::reference_wrapper<element>>&            thumb() = 0;
      virtual element const&  track() const = 0;
      virtual element&        track() = 0;

   private:

      double                  _value_min;
      double                  _value_max;
      mutable bool            _is_horiz = false;
   };

   inline void slider_base::edit(view& view_, std::pair<double, double> val)
   {
      edit_value(val);
      receiver<double>::notify_edit(view_);
   }

   class basic_range_slider_base : public slider_base
   {
   public:

      using slider_function = std::function<void(double pos)>;
      using slider_base::slider_base;

      slider_function         on_change;
      void                    edit_value(double val) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   template <typename Thumb1, typename Thumb2, typename Track, typename Base = basic_range_slider_base>
   class basic_range_slider : public Base
   {
   public:

      using thumb1_type = typename std::decay<Thumb1>::type;
      using thumb2_type = typename std::decay<Thumb2>::type;
      using track_type = typename std::decay<Track>::type;

                              inline basic_range_slider(Thumb1&& thumb_min, Thumb2&& thumb_max, Track&& track, std::pair<double, double> init_value)
                               : Base(init_value)
                               , _thumb1(std::forward<Thumb1>(thumb_min))
                               , _thumb2(std::forward<Thumb2>(thumb_max))
                               , _body(std::forward<Track>(track))
                              {}

      std::pair<std::reference_wrapper<element const>, std::reference_wrapper<element const>> const   thumb() const override { return {std::cref(_thumb1), std::cref(_thumb2)}; }
      std::pair<std::reference_wrapper<element const>, std::reference_wrapper<element const>>         thumb()       override { return {std::ref(_thumb1), std::ref(_thumb2)}; }
      element const&          track() const override { return _body; }
      element&                track()       override { return _body; }

   private:

      thumb1_type              _thumb1;
      thumb2_type              _thumb2;
      track_type              _body;
   };

   template <typename Thumb1, typename Thumb2, typename Track>
   inline basic_range_slider<Thumb1, Thumb2, Track, basic_slider_base>
   range_slider(Thumb1&& thumb_min, Thumb2&& thumb_max, Track&& track, std::pair<double, double> init_value = {0.0, 1.0})
   {
      return {
         std::forward<Thumb1>(thumb_min),
         std::forward<Thumb2>(thumb_max),
         std::forward<Track>(track),
         init_value
      };
   }
}}