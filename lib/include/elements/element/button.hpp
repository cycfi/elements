/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_BUTTON_APRIL_21_2016)
#define ELEMENTS_BUTTON_APRIL_21_2016

#include <elements/element/layer.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/selection.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/context.hpp>
#include <elements/support/sender.hpp>
#include <elements/view.hpp>
#include <type_traits>
#include <concepts>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Button
   ////////////////////////////////////////////////////////////////////////////
   namespace concepts
   {
      template <typename T>
      concept ButtonCallback = requires(T f)
      {
         { f(true) } -> std::same_as<void>;
      };
   }

   /**
    *  \struct button_state
    *
    *  \brief
    *    A struct for maintaining and managing the state of a button.
    *
    *    This structure captures the various states that a button can have:
    *    - `value`: The button's value; 0(off) or 1(on).
    *    - `hilite`: True if the button is highlighted.
    *    - `tracking`: True if the mouse button being pressed.
    *    - `enabled`: True if the button is enabled.
    */
   struct button_state
   {
      bool              value : 1      = false;
      bool              hilite : 1     = false;
      bool              tracking : 1   = false;
      bool              enabled : 1    = false;
   };

   /**
    *  \class basic_button
    *
    *  \brief
    *    A class that represents a basic GUI button, acting as a proxy which
    *    delegates the rendering to a "button styler".
    *
    *    The `basic_button` class is a foundational class for creating a GUI
    *    button. This class is a proxy which delegates the rendering of the
    *    actual button to a button styler subject. This division of
    *    responsibilities allows for more flexibility in dictating the
    *    button's appearance and interaction. The `basic_button` class
    *    handles user interactions, while the button styler manages the
    *    button's visual presentation. With this pattern, different stylers
    *    can be implemented for various visual representations, for instance,
    *    plain buttons, radio buttons, slide switches, checkboxes, and more.
    *
    *    The communication with the button styler is done via the
    *    `receiver<button_state>` or a `receiver<int>` APIs. These APIs
    *    provide a means for the `basic_button` to update the button styler
    *    about changes in button's state to allow the styler to adjust the
    *    visual representation accordingly.
    *
    *    If the button styler follows a `receiver<int>` API, it will receive
    *    in integer with these possible values:
    *
    *       0: value=`false`, hilite=`false`
    *       1: value=`false`, hilite=`true`
    *       2: value=`true`,  hilite=`false`
    *       3: value=`true`,  hilite=`true`
    *
    *    If the button styler follows a `receiver<button_state>` API, it will
    *    receive a `button_state` when the button's state changes. This has a
    *    richer API compared to the former, allowing more nuanced button
    *    rendering. See `button_state`.
    *
    *    Take note that the button styler is just an element and does not
    *    have to follow the `receiver` API. If that's the case, then the
    *    button rendering will be static, and not adjust to state changes.
    *    This may still be useful in certain cases.
    */
   class basic_button : public proxy_base, public receiver<bool>
   {
   public:

      using button_function = std::function<void(bool)>;

      bool              wants_control() const override;
      bool              click(context const& ctx, mouse_button btn) override;
      bool              cursor(context const& ctx, point p, cursor_tracking status) override;
      void              drag(context const& ctx, mouse_button btn) override;
      element*          hit_test(context const& ctx, point p, bool leaf, bool control) override;

      void              enable(bool state = true) override;
      bool              is_enabled() const override;

      void              value(bool val) override;
      bool              value() const override;
      bool              tracking() const;
      bool              hilite() const;

      void              edit(view& view_, bool val) override;

      button_function   on_click;

   protected:

      bool              state(bool val);
      void              tracking(bool val);
      void              hilite(bool val);

   private:

      bool              update_receiver();

      button_state      _state;
   };

   namespace concepts
   {
      template <typename T>
      concept MomentaryButton = std::is_base_of_v<basic_button, std::decay_t<T>>;
   }

   /**
    * \brief
    *    A function template to create a stylable momentary button.
    *
    *    The `momentary_button` function template creates a proxy object for
    *    a momentary button from a given styler.
    *
    *    The function uses two template parameters: `Base` and `Styler`. The
    *    `Base` should be a momentary button that adheres to `
    *    MomentaryButton` concept with `basic_button` as the default type.
    *    The `Styler` should follow the `Element` concept.
    *
    * \param styler
    *    An rvalue reference to the styler that will be used to render the
    *    button.
    *
    * \returns
    *    A proxy object that wraps the passed styler with momentary button
    *    functionality, allowing for custom button styling.
    */
   template <
      concepts::MomentaryButton Base = basic_button
    , concepts::Element Styler
   >
   inline proxy<remove_cvref_t<Styler>, Base>
   momentary_button(Styler&& styler)
   {
      return {std::forward<Styler>(styler)};
   }

   /**
    * \brief
    *    Get the value of the button
    *
    * \returns
    *    `true` = ON, `false` = OPF.
    */
   inline bool basic_button::value() const
   {
      return _state.value;
   }

   /**
    * \brief
    *    Get the tracking state of the button
    *
    * \returns
    *    `true` if user interaction is currently in ongoing, `false` otherwise.
    */
   inline bool basic_button::tracking() const
   {
      return _state.tracking;
   }

   /**
    * \brief
    *    Get the hilite state of the button
    *
    * \returns
    *    `true` if the cursor is hovering over button, `false` otherwise.
    */
   inline bool basic_button::hilite() const
   {
      return _state.hilite;
   }

   /**
    * \class basic_toggle_button
    *
    * \brief
    *    A derived class from `basic_button` that provides toggle
    *    functionality.
    *
    *    The `basic_toggle_button` class is derived from the `basic_button`
    *    class and adds a stateful toggle feature that switches between an
    *    active (pressed) and inactive (unpressed) state.
    */
   class basic_toggle_button : public basic_button
   {
   public:

      using basic_button::basic_button;

      bool              click(context const& ctx, mouse_button btn) override;
      void              drag(context const& ctx, mouse_button btn) override;

   private:

      bool              _current_state;
   };

   namespace concepts
   {
      template <typename T>
      concept ToggleButton = std::is_base_of_v<basic_toggle_button, std::decay_t<T>>;
   }

   /**
    * \brief
    *    A function template to create a stylable toggle button.
    *
    *    The `toggle_button` function template creates a proxy object for a
    *    toggle button from a given styler.
    *
    *    The function uses two template parameters: `Base` and `Styler`. The
    *    `Base` should be a toggle button that adheres to ` ToggleButton`
    *    concept with `basic_toggle_button` as the default type. The `Styler`
    *    should follow the `Element` concept.
    *
    * \param styler
    *    An rvalue reference to the styler that will be used to render the
    *    button.
    *
    * \returns
    *    A proxy object that wraps the passed styler with toggle button
    *    functionality, allowing for custom button styling.
    */
   template <
      concepts::ToggleButton Base = basic_toggle_button
    , concepts::Element Styler
   >
   inline proxy<remove_cvref_t<Styler>, Base>
   toggle_button(Styler&& styler)
   {
      return {std::forward<Styler>(styler)};
   }

   /**
    *  \class basic_latching_button
    *
    *  \brief
    *    An extension of the `basic_button` class providing latching button
    *    functionality.
    *
    *    The `basic_latching_button` class is derived from the `basic_button`
    *    class. It enhances the basic button by introducing a latching
    *    feature. A latching button maintains its state after being clicked
    *    and holds this state indefinitely, until it is reset
    *    programmatically.
    */
   class basic_latching_button : public basic_button
   {
   public:

      using basic_button::basic_button;

      bool              click(context const& ctx, mouse_button btn) override;
   };

   namespace concepts
   {
      template <typename T>
      concept LatchingButton = std::is_base_of_v<basic_latching_button, std::decay_t<T>>;
   }

   /**
    * \brief
    *    A function template to create a stylable latching button.
    *
    *    The `latching_button` function template creates a proxy object for a
    *    latching button from a given styler.
    *
    *    The function uses two template parameters: `Base` and `Styler`. The
    *    `Base` should be a latching button that adheres to `LatchingButton`
    *    concept with `basic_latching_button` as the default type. The
    *    `Styler` should follow the `Element` concept.
    *
    * \param styler
    *    An rvalue reference to the styler that will be used to render the
    *    button.
    *
    * \returns
    *    A proxy object that wraps the passed styler with latching button
    *    functionality, allowing for custom button styling.
    */
   template <
      concepts::LatchingButton Base = basic_latching_button
    , concepts::Element Styler
   >
   inline proxy<remove_cvref_t<Styler>, Base>
   latching_button(Styler&& styler)
   {
      return {std::forward<Styler>(styler)};
   }

   /**
    *  \class basic_choice
    *
    *  \brief
    *    A class that represents a selectable latching button, typically in a
    *    group, where one button is active at a given time. An example is the
    *    radio button.
    *
    *    The `basic_choice` class inherits from both the `selectable` and
    *    `basic_latching_button` classes. The `basic_latching_button` class
    *    provides the latching functionality, while the `selectable`
    *    interface enables elements to be selected or unselected.
    */
   class basic_choice : public basic_latching_button, public selectable
   {
   public:

      using basic_latching_button::basic_latching_button;

      void              select(bool state) override;
      bool              is_selected() const override;
      bool              click(context const& ctx, mouse_button btn) override;
   };

   /**
    *  \brief
    *    Get the current selection state of the button.
    *
    *    This function uses the `value()` method to get the current selection
    *    state of the button.
    *
    * \return
    *    The current selection state of the button: `true` if the button is
    *    selected, and `false` otherwise.
    */
   inline bool basic_choice::is_selected() const
   {
      return this->value();
   }

   /**
    *  \brief
    *    Set the selection state of the button.
    *
    *    This function uses the `value()` member function to set the new
    *    state of the button.
    *
    * \param
    *    state The desired selection state: `true` if the button should be
    *    selected, and `false` otherwise.
    */
   inline void basic_choice::select(bool state)
   {
      if (state != is_selected())
         this->value(state);
   }

   /**
    *  \brief
    *    Function template to create a selectable latching button like a
    *    radio button.
    *
    *    The `choice` function creates a proxy object for a selectable
    *    latching button (like a radio button) from a given subject.
    *
    * \param subject
    *    An rvalue reference to the subject that will be used as the subject
    *    for the selectable button.
    *
    * \return
    *    A proxy object that wraps the passed subject with selectable button
    *    functionality.
    */
   template <concepts::Element Subject>
   inline proxy<Subject, basic_choice>
   choice(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }
}

#endif
