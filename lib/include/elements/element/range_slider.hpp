#if !defined(ELEMENTS_RANGE_SLIDER_NOVEMBER_30_2023)
#define ELEMENTS_RANGE_SLIDER_NOVEMBER_30_2023

// #include <elements/element/slider.hpp>
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
   protected:
      using val_type = std::pair<double, double>;
      using element_cref_pair_type = std::pair<std::reference_wrapper<element const>, std::reference_wrapper<element const>>;
      using element_ref_pair_type = std::pair<std::reference_wrapper<element>, std::reference_wrapper<element>>;
      enum class state { idle, moving_thumb1, moving_thumb2, scrolling_thumb1, scrolling_thumb2 };
   public:

                              range_slider_base(std::pair<double, double> init_value)
                               : _value(init_value)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;

      bool                    scroll(context const&, point, point) override;
      void                    begin_tracking(context const& ctx, tracker_info& track_info) override;
      void                    keep_tracking(context const& ctx, tracker_info& track_info) override;
      void                    end_tracking(context const& ctx, tracker_info& track_info) override;

      val_type                value() const override;
      void                    value(val_type val) override;
      double                  value_first() const { return _value.first; }
      double                  value_second() const { return _value.second; }
      void                    value_first(double val) { value({val, _value.second});}
      void                    value_second(double val) { value({_value.first, val});}
      void                    edit(view& view_, val_type val) override;
      virtual void            edit_value_first(double val) { value_first(val);}
      virtual void            edit_value_second(double val) { value_second(val);}
      void                    edit_value(val_type val) {edit_value_first(val.first); edit_value_second(val.second);}

      rect                    track_bounds(context const& ctx) const;
      std::pair<rect, rect>   thumb_bounds(context const& ctx) const;

      virtual element_cref_pair_type thumb() const = 0;
      virtual element_ref_pair_type  thumb() = 0;
      virtual element const&         track() const = 0;
      virtual element&               track() = 0;

   private:
      state                          _state = state::idle;
      std::pair<double, double>      _value;
      mutable bool                   _is_horiz = false;
   };

   inline void range_slider_base::edit(view& view_, std::pair<double, double> val)
   {
      edit_value(val);
      receiver<std::pair<double, double>>::notify_edit(view_);
   }

   class basic_range_slider_base : public range_slider_base
   {
   public:

      using slider_function = std::function<void(double pos)>;
      using range_slider_base::range_slider_base;

      std::pair<slider_function, slider_function> on_change;
      void                                        edit_value_first(double val) override;
      void                                        edit_value_second(double val) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   template <typename Thumb1, typename Thumb2, typename Track, typename Base = basic_range_slider_base>
   class basic_range_slider : public Base
   {
   using typename Base::element_cref_pair_type;
   using typename Base::element_ref_pair_type;
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

      element_cref_pair_type  thumb() const override { return {std::cref(_thumb1), std::cref(_thumb2)}; }
      element_ref_pair_type   thumb()       override { return {std::ref(_thumb1), std::ref(_thumb2)}; }
      element const&          track() const override { return _body; }
      element&                track()       override { return _body; }

   private:

      thumb1_type             _thumb1;
      thumb2_type             _thumb2;
      track_type              _body;
   };

   template <typename Thumb1, typename Thumb2, typename Track>
   inline basic_range_slider<Thumb1, Thumb2, Track, basic_range_slider_base>
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

#endif
