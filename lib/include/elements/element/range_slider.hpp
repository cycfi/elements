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
   using double_range = std::pair<double, double>;

   class range_slider_base
    : public tracker<>
    , public receiver<double_range>
   {
   protected:

      using val_type = double_range;
      using element_const_ref = std::reference_wrapper<element const>;
      using element_ref = std::reference_wrapper<element>;
      using element_cref_pair = std::pair<element_const_ref, element_const_ref>;
      using element_ref_pair = std::pair<element_ref, element_ref>;

      enum class state
      {
         idle_1, // idle, last moved thumb 1
         idle_2, // idle, last moved thumb 2
         moving_thumb1,
         moving_thumb2,
         scrolling_thumb1,
         scrolling_thumb2
      };

   public:
                                 range_slider_base(double_range init_value)
                                  : _value(init_value)
                                 {}

      view_limits                limits(basic_context const& ctx) const override;
      void                       draw(context const& ctx) override;
      void                       layout(context const& ctx) override;

      bool                       scroll(context const&, point, point) override;
      void                       begin_tracking(context const& ctx, tracker_info& track_info) override;
      void                       keep_tracking(context const& ctx, tracker_info& track_info) override;
      void                       end_tracking(context const& ctx, tracker_info& track_info) override;

      val_type                   value() const override;
      void                       value(val_type val) override;
      double                     value_first() const;
      double                     value_second() const;
      void                       value_first(double val);
      void                       value_second(double val);
      void                       edit(view& view_, val_type val) override;
      virtual void               edit_value_first(double val);
      virtual void               edit_value_second(double val);
      void                       edit_value(val_type val);

      rect                       track_bounds(context const& ctx) const;
      std::pair<rect, rect>      thumb_bounds(context const& ctx) const;
      virtual float              handle_possible_collision_first(context const& ctx, float val) const;
      virtual float              handle_possible_collision_second(context const& ctx, float val) const;

      virtual element_cref_pair  thumb() const = 0;
      virtual element_ref_pair   thumb() = 0;
      virtual element const&     track() const = 0;
      virtual element&           track() = 0;

   private:

      state                      _state = state::idle_1;
      double_range               _value;
      mutable bool               _is_horiz = false;

      void                       move_first(context const& ctx, tracker_info& track_info);
      void                       move_second(context const& ctx, tracker_info& track_info);
      void                       move_both_from_first(context const& ctx, tracker_info& track_info);
      void                       move_both_from_second(context const& ctx, tracker_info& track_info);
   };

   inline void range_slider_base::edit(view& view_, double_range val)
   {
      edit_value(val);
      receiver<double_range>::notify_edit(view_);
   }

   class basic_range_slider_base : public range_slider_base
   {
   public:

      using slider_function = std::function<void(double pos)>;
      using range_slider_base::range_slider_base;
      using slider_function_pair = std::pair<slider_function, slider_function>;

      slider_function_pair       on_change;
      void                       edit_value_first(double val) override;
      void                       edit_value_second(double val) override;
   };

   class basic_range_slider_offset_base : public basic_range_slider_base
   {
   public:
                                 inline basic_range_slider_offset_base(
                                    std::pair<double, double> init_value
                                  , float offset_from_center
                                 )
                                  : basic_range_slider_base(init_value)
                                  , offset_from_center(offset_from_center)
                                 {}

      float                      handle_possible_collision_first(context const& ctx, float val) const override;
      float                      handle_possible_collision_second(context const& ctx, float val) const override;

   private:

      float offset_from_center = 0;
   };

   template <
      typename Thumb1, typename Thumb2, typename Track
    , typename Base = basic_range_slider_base>
   class basic_range_slider : public Base
   {
      using typename Base::element_cref_pair;
      using typename Base::element_ref_pair;

   public:

      using thumb1_type = typename std::decay<Thumb1>::type;
      using thumb2_type = typename std::decay<Thumb2>::type;
      using track_type = typename std::decay<Track>::type;

                              inline basic_range_slider(
                                 Thumb1&& thumb_min
                               , Thumb2&& thumb_max
                               , Track&& track
                               , double_range init_value
                              )
                               : Base(init_value)
                               , _thumb1(std::forward<Thumb1>(thumb_min))
                               , _thumb2(std::forward<Thumb2>(thumb_max))
                               , _body(std::forward<Track>(track))
                              {}

                              inline basic_range_slider(
                                 Thumb1&& thumb_min
                               , Thumb2&& thumb_max
                               , Track&& track
                               , double_range init_value
                               , float offset_from_center
                              )
                               : Base(init_value, offset_from_center)
                               , _thumb1(std::forward<Thumb1>(thumb_min))
                               , _thumb2(std::forward<Thumb2>(thumb_max))
                               , _body(std::forward<Track>(track))
                              {}

      element_cref_pair       thumb() const override;
      element_ref_pair        thumb() override;
      element const&          track() const override;
      element&                track() override;

   private:

      thumb1_type             _thumb1;
      thumb2_type             _thumb2;
      track_type              _body;
   };

   template <typename Thumb1, typename Thumb2, typename Track>
   inline basic_range_slider<Thumb1, Thumb2, Track, basic_range_slider_base>
   range_slider(
      Thumb1&& thumb_min
    , Thumb2&& thumb_max
    , Track&& track
    , double_range init_value = {0.0, 1.0}
   )
   {
      return {
         std::forward<Thumb1>(thumb_min),
         std::forward<Thumb2>(thumb_max),
         std::forward<Track>(track),
         init_value
      };
   }

   template <typename Thumb1, typename Thumb2, typename Track>
   inline basic_range_slider<Thumb1, Thumb2, Track, basic_range_slider_offset_base>
   range_slider(
      Thumb1&& thumb_min
    , Thumb2&& thumb_max
    , Track&& track
    , double_range init_value
    , float offset_from_center)
   {
      return {
         std::forward<Thumb1>(thumb_min),
         std::forward<Thumb2>(thumb_max),
         std::forward<Track>(track),
         init_value,
         offset_from_center
      };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline double range_slider_base::value_first() const
   {
      return _value.first;
   }

   inline double range_slider_base::value_second() const
   {
      return _value.second;
   }

   inline void range_slider_base::value_first(double val)
   {
      value({val, _value.second});
   }

   inline void range_slider_base::value_second(double val)
   {
      value({_value.first, val});
   }
   inline void range_slider_base::edit_value_first(double val)
   {
      value_first(val);
   }

   inline void range_slider_base::edit_value_second(double val)
   {
      value_second(val);
   }

   inline void range_slider_base::edit_value(val_type val)
   {
      edit_value_first(val.first);
      edit_value_second(val.second);
   }

   template <typename Thumb1, typename Thumb2, typename Track, typename Base>
   inline typename basic_range_slider<Thumb1, Thumb2, Track, Base>::element_cref_pair
   basic_range_slider<Thumb1, Thumb2, Track, Base>::thumb() const
   {
      return {std::cref(_thumb1), std::cref(_thumb2)};
   }

   template <typename Thumb1, typename Thumb2, typename Track, typename Base>
   inline typename basic_range_slider<Thumb1, Thumb2, Track, Base>::element_ref_pair
   basic_range_slider<Thumb1, Thumb2, Track, Base>::thumb()
   {
      return {std::ref(_thumb1), std::ref(_thumb2)};
   }

   template <typename Thumb1, typename Thumb2, typename Track, typename Base>
   inline element const& basic_range_slider<Thumb1, Thumb2, Track, Base>::track() const
   {
      return _body;
   }

   template <typename Thumb1, typename Thumb2, typename Track, typename Base>
   inline element& basic_range_slider<Thumb1, Thumb2, Track, Base>::track()
   {
      return _body;
   }
}}

#endif
