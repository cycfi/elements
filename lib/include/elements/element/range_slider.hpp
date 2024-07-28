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

namespace cycfi::elements
{
   //--------------------------------------------------------------------------
   // Range Slider Base
   //--------------------------------------------------------------------------

   using double_range = std::pair<double, double>;

   /*!
    * \class range_slider_base
    *
    * \brief
    *    A base class for creating range sliders that allow users to select a
    *    range of values.
    *
    *    - Manages a range of values (`double_range`) representing the
    *      selected range.\n
    *    - Supports tracking and modifying both ends of the range
    *      independently or together.\n
    *    - Handles user interactions such as scrolling and dragging to adjust
    *      the range.\n\n
    *
    *    Derived classes are expected to provide implementations for the pure
    *    virtual functions to define the appearance and behavior of the
    *    thumbs and track. This includes methods to retrieve references to
    *    the thumb and track elements.
    */
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

   //--------------------------------------------------------------------------
   // Basic Range Slider Base
   //--------------------------------------------------------------------------

   /*!
    * \class basic_range_slider_base
    *
    * \brief
    *    Basic implementation of a range slider. It introduces mechanisms to
    *    handle value changes through user-defined callback functions.
    *
    *    - Install an `on_change` callback function to assign that will be
    *      executed when the slider's values are modified.
    *    - Override `edit_value_first` and `edit_value_second` to customize
    *      how changes to the slider values are handled.
    */
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

   //--------------------------------------------------------------------------
   // Basic Range Slider Offset Base
   //--------------------------------------------------------------------------

   /*!
    * \class basic_range_slider_offset_base

    * \brief
    *    basic_range_slider_offset_base introduces an offset handling
    *    mechanism for the range slider, designed to manage sliders where the
    *    thumb positions needs to be adjusted based on a specified offset
    *    from the center, useful in scenarios where the visual representation
    *    of the slider's thumbs might overlap or require adjustment for
    *    better user interaction.
    */
   class basic_range_slider_offset_base : public basic_range_slider_base
   {
   public:
                                 basic_range_slider_offset_base(
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

   //--------------------------------------------------------------------------
   // Basic Range Slider Template
   //--------------------------------------------------------------------------

   /*!
    * \class basic_range_slider
    *
    * \brief
    *    A template class designed to create a customizable range slider with
    *    user-specified thumb and track elements.
    *
    * \tparam Thumb1
    *    The type of the first thumb component. It must conform to the
    *    Element concept.
    *
    * \tparam Thumb2
    *    The type of the second thumb component. It must conform to the
    *    Element concept.
    *
    * \tparam Track
    *    The type of the track component. It must conform to the Element
    *    concept.
    *
    * \tparam Base
    *    The base class from which `basic_range_slider` inherits, defaulting
    *    to `basic_range_slider_base`.
    */
   template <
      concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track
    , typename Base = basic_range_slider_base>
   class basic_range_slider : public Base
   {
      using typename Base::element_cref_pair;
      using typename Base::element_ref_pair;

   public:

      using thumb1_type = typename std::decay<Thumb1>::type;
      using thumb2_type = typename std::decay<Thumb2>::type;
      using track_type = typename std::decay<Track>::type;

                              basic_range_slider(
                                 Thumb1&& thumb_min
                               , Thumb2&& thumb_max
                               , Track&& track
                               , double_range init_value
                              );

                              basic_range_slider(
                                 Thumb1&& thumb_min
                               , Thumb2&& thumb_max
                               , Track&& track
                               , double_range init_value
                               , float offset_from_center
                              );

      element_cref_pair       thumb() const override;
      element_ref_pair        thumb() override;
      element const&          track() const override;
      element&                track() override;

   private:

      thumb1_type             _thumb1;
      thumb2_type             _thumb2;
      track_type              _body;
   };

   template <concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track>
   inline basic_range_slider<Thumb1, Thumb2, Track, basic_range_slider_base>
   range_slider(
      Thumb1&& thumb_min
    , Thumb2&& thumb_max
    , Track&& track
    , double_range init_value = {0.0, 1.0}
   );

   template <concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track>
   inline basic_range_slider<Thumb1, Thumb2, Track, basic_range_slider_offset_base>
   range_slider(
      Thumb1&& thumb_min
    , Thumb2&& thumb_max
    , Track&& track
    , double_range init_value
    , float offset_from_center
   );

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /*!
    * \brief
    *    Create a `basic_range_slider` with specific thumb and track
    *    components.
    *
    * \tparam Thumb1
    *    The type of the first thumb component. It must conform to the
    *    Element concept.
    *
    * \tparam Thumb2
    *    The type of the second thumb component. It must conform to the
    *    Element concept.
    *
    * \tparam Track
    *    The type of the track component. It must conform to the Element
    *    concept.
    *
    * \param thumb_min
    *    The first thumb component, representing the minimum value selector.
    *
    * \param thumb_max
    *    The second thumb component, representing the maximum value selector.
    *
    * \param track
    *    The track component on which the thumbs slide.
    *
    * \param init_value
    *    The initial value range of the slider.
    *
    * \return
    *    An instance of `basic_range_slider` configured with the provided
    *    thumb and track components, and initialized with the specified value
    *    range.
    */
   template <concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track>
   inline basic_range_slider<Thumb1, Thumb2, Track, basic_range_slider_base>
   range_slider(
      Thumb1&& thumb_min
    , Thumb2&& thumb_max
    , Track&& track
    , double_range init_value
   )
   {
      return {
         std::forward<Thumb1>(thumb_min),
         std::forward<Thumb2>(thumb_max),
         std::forward<Track>(track),
         init_value
      };
   }

   /*!
    * \brief
    *    Create a `basic_range_slider` with specific thumb and track
    *    components, along with an offset for handling thumb positioning.
    *
    * \tparam Thumb1
    *    The type of the first thumb component. It must conform to the
    *    Element concept.
    *
    * \tparam Thumb2
    *    The type of the second thumb component. It must conform to the
    *    Element concept.
    *
    * \tparam Track
    *    The type of the track component. It must conform to the Element
    *    concept.
    *
    * \param thumb_min
    *    The first thumb component, representing the minimum value selector.
    *
    * \param thumb_max
    *    The second thumb component, representing the maximum value selector.
    *
    * \param track
    *    The track component on which the thumbs slide.
    *
    * \param init_value
    *    The initial value range of the slider.
    *
    * \param offset_from_center
    *    The offset from the center for the thumb components, used for custom
    *    positioning or collision avoidance.
    *
    * \return
    *    An instance of `basic_range_slider` configured with the provided
    *    thumb and track components, initialized with the specified value
    *    range, and adjusted for the specified offset from the center.
    */
   template <concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track>
   inline basic_range_slider<Thumb1, Thumb2, Track, basic_range_slider_offset_base>
   range_slider(
      Thumb1&& thumb_min
    , Thumb2&& thumb_max
    , Track&& track
    , double_range init_value
    , float offset_from_center
   )
   {
      return {
         std::forward<Thumb1>(thumb_min),
         std::forward<Thumb2>(thumb_max),
         std::forward<Track>(track),
         init_value,
         offset_from_center
      };
   }

   /*!
    * \brief
    *    Get the value of the first thumb.
    *
    * \return
    *    The value of the first thumb as a double.
    */
   inline double range_slider_base::value_first() const
   {
      return _value.first;
   }

   /*!
    * \brief
    *    Get the value of the second thumb.
    *
    * \return
    *    The value of the second thumb as a double.
    */
   inline double range_slider_base::value_second() const
   {
      return _value.second;
   }

   /*!
    * \brief
    *    Set the value of the first thumb.
    *
    * \param val
    *    The new value to set for the first thumb.
    */
   inline void range_slider_base::value_first(double val)
   {
      value({val, _value.second});
   }

   /*!
    * \brief
    *    Set the value of the second thumb.
    *
    * \param val
    *    The new value to set for the second thumb.
    */
   inline void range_slider_base::value_second(double val)
   {
      value({_value.first, val});
   }
   inline void range_slider_base::edit_value_first(double val)
   {
      value_first(val);
   }

   /*!
    * \brief
    *    Edits the value of the first thumb.
    *
    * \param val
    *    The new value to set for the first thumb.
    */
   inline void range_slider_base::edit_value_second(double val)
   {
      value_second(val);
   }

   /*!
    * \brief
    *    Edits the value of the second thumb.
    *
    * \param val
    *    The new value to set for the second thumb.
    */
   inline void range_slider_base::edit_value(val_type val)
   {
      edit_value_first(val.first);
      edit_value_second(val.second);
   }

   /*!
    * \brief
    *    Edits the values of both thumbs.
    *
    * \param val
    *    A pair of double values where `val.first` is the new value for the
    *    first thumb, and `val.second` is the new value for the second thumb.
    */
   inline void range_slider_base::edit(view& view_, double_range val)
   {
      edit_value(val);
      receiver<double_range>::notify_edit(view_);
   }

   /*!
    * \brief
    *    Constructor for `basic_range_slider` without offset handling.
    *
    *    Initializes a `basic_range_slider` instance using provided thumb
    *    components, a track component, and an initial value range. This
    *    constructor does not handle offset adjustments for the thumb components.
    *
    * \tparam Thumb1
    *    The type of the first thumb component. It must conform to the
    *    Element concept.
    *
    * \tparam Thumb2
    *    The type of the second thumb component. It must conform to the
    *    Element concept.
    *
    * \tparam Track
    *    The type of the track component. It must conform to the Element
    *    concept.
    *
    * \tparam Base
    *    The base class from which `basic_range_slider` inherits.
    *
    * \param thumb_min
    *    The first thumb component, representing the minimum value selector.
    *
    * \param thumb_max
    *    The second thumb component, representing the maximum value selector.
    *
    * \param track
    *    The track component on which the thumbs slide.
    *
    * \param init_value
    *    The initial value range of the slider.
    */
   template <concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track, typename Base>
   inline basic_range_slider<Thumb1, Thumb2, Track, Base>::basic_range_slider(
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

   /*!
    * \brief
         Constructor for `basic_range_slider` with offset handling.
    *
    *    Initializes a `basic_range_slider` instance using provided thumb
    *    components, a track component, an initial value range, and an offset
    *    from the center for the thumb components.
    *
    * \tparam Thumb1
    *    The type of the first thumb component. It must conform to the
    *    Element concept.
    *
    * \tparam Thumb2
    *    The type of the second thumb component. It must conform to the
    *    Element concept.
    *
    * \tparam Track
    *    The type of the track component. It must conform to the Element
    *    concept.
    *
    * \tparam Base
    *    The base class from which `basic_range_slider` inherits.
    *
    * \param thumb_min
    *    The first thumb component, representing the minimum value selector.
    *
    * \param thumb_max
    *    The second thumb component, representing the maximum value selector.
    *
    * \param track
    *    The track component on which the thumbs slide.
    *
    * \param init_value
    *    The initial value range of the slider.
    *
    * \param offset_from_center
    *    The offset from the center for the thumb components, used for custom
    *    positioning or collision avoidance.
    */
   template <concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track, typename Base>
   inline basic_range_slider<Thumb1, Thumb2, Track, Base>::basic_range_slider(
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

   /*!
    * \brief Get constant references to the thumb elements.
    *
    * \return
    *    A pair of `std::reference_wrapper` to the constant thumb elements.
    */
   template <concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track, typename Base>
   inline typename basic_range_slider<Thumb1, Thumb2, Track, Base>::element_cref_pair
   basic_range_slider<Thumb1, Thumb2, Track, Base>::thumb() const
   {
      return {std::cref(_thumb1), std::cref(_thumb2)};
   }

   /*!
    * \brief Get references to the thumb elements.
    *
    * \return
    *    A pair of `std::reference_wrapper` to the thumb elements.
    */
   template <concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track, typename Base>
   inline typename basic_range_slider<Thumb1, Thumb2, Track, Base>::element_ref_pair
   basic_range_slider<Thumb1, Thumb2, Track, Base>::thumb()
   {
      return {std::ref(_thumb1), std::ref(_thumb2)};
   }

   /*!
    * \brief
    *    Get a constant reference to the track component.
   *
    * \return
    *    A constant reference to the track component.
   */
   template <concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track, typename Base>
   inline element const& basic_range_slider<Thumb1, Thumb2, Track, Base>::track() const
   {
      return _body;
   }

   /*!
    * \brief
    *    Get a reference to the track component.
   *
    * \return
    *    A reference to the track component.
    */
   template <concepts::Element Thumb1, concepts::Element Thumb2, concepts::Element Track, typename Base>
   inline element& basic_range_slider<Thumb1, Thumb2, Track, Base>::track()
   {
      return _body;
   }
}

#endif
