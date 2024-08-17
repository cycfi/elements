/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DIAL_AUGUST_30_2016)
#define ELEMENTS_DIAL_AUGUST_30_2016

#include <elements/element/proxy.hpp>
#include <elements/element/tracker.hpp>
#include <elements/support.hpp>
#include <infra/support.hpp>
#include <functional>

namespace cycfi::elements
{
   /**
    * \enum dial_mode_enum
    *
    * \brief
    *    Enumeration to specify the behavior of a dial when it's adjusted
    *    using the mouse or trackpad. The preferred bahavior may be set via
    *    `get_theme().dial_mode`.
    *
    * \details
    * `linear`: The dial's value adjusts up (increases) or down (decreases)
    *           corresponding to a linear vertical or horizontal dragging of
    *           the mouse or trackpad (This is the default).
    *
    * `radial`: The dial's value adjusts up or down corresponding to a circular
    *           motion (clockwise or anti-clockwise) around the knob, using the
    *           mouse or trackpad, emulating the behavior of a real-world dial.
    */
   enum class dial_mode_enum : int
   {
      linear,
      radial,
   };

   /**
    * \class basic_dial
    *
    * \brief
    *   A class representing a basic GUI dial, acting as a proxy that
    *   delegates rendering and value management to a dial styler.
    *
    *   The `basic_dial` class is a foundational class for creating a GUI
    *   dial. It is a proxy which delegates rendering of the dial and
    *   tracking of its value to a dial styler. This separation of concerns
    *   allows for greater flexibility in dictating the dial's appearance,
    *   interaction, and functionality. The `basic_dial` handles user
    *   interaction (such as mouse drag) and management of the dial's current
    *   value, while the dial styler handles the dial's visual presentation.
    *   With this pattern, different stylers can be implemented for various
    *   visual representations.
    *
    *   Communication with the dial styler is done via the `receiver<double>`
    *   API. This API allows the `basic_dial` to broadcast changes in the
    *   dial's value to the dial styler. Consequently, the styler can update
    *   the visual representation in response to these changes.
    */
   class basic_dial : public tracker<proxy_base>, public receiver<double>
   {
   public:

      using dial_function = std::function<void(double pos)>;

                           basic_dial(double init_value = 0.0);

      element*             hit_test(context const& ctx, point p, bool leaf, bool control) override;
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

   using dial_base [[deprecated("Use basic_dial instead.")]] = basic_dial;

   /**
    * \brief
    *    Template function for creating a stylable basic dial.
    *
    * \tparam Styler
    *    A template parameter representing the stylizing Element.
    *
    * \param styler
    *    The stylizing Element, a user interface element conforming to the
    *    Element concept.
    *
    * \param init_value
    *    An optional parameter representing the initial value of the basic
    *    dial. Defaults to 0.0.
    *
    * \return
    *    Returns stylable basic dial given a styler, with a given initial
    *    value. Defaults to 0.0.
    */
   template <concepts::Element Styler>
   inline proxy<remove_cvref_t<Styler>, basic_dial>
   dial(Styler&& styler, double init_value = 0.0)
   {
      return {std::forward<Styler>(styler), init_value};
   }

   /**
    * \brief
    *    Returns the current value of the dial.
    *
    * \return
    *    Returns a double representing the current value of the dial.
    */
   inline double basic_dial::value() const
   {
      return _value;
   }

   inline element* basic_dial::hit_test(context const& ctx, point p, bool leaf, bool /*control*/)
   {
      return element::hit_test(ctx, p, leaf, false); // accept non-control subjects
   }
}

#endif
