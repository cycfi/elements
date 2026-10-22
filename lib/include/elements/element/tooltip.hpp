/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TOOLTIP_AUGUST_27_2020)
#define ELEMENTS_TOOLTIP_AUGUST_27_2020

#include <elements/element/proxy.hpp>
#include <elements/element/popup.hpp>
#include <infra/support.hpp>
#include <functional>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Tooltip elements
   ////////////////////////////////////////////////////////////////////////////
   enum class tooltip_position
   {
      left,
      top,
      right,
      bottom
   };

   ////////////////////////////////////////////////////////////////////////////
   // A bubble around a tip, with a pointer toward the element. Room for the
   // pointer is kept on every side; the tooltip says, as it places the tip,
   // which side of the element the tip is on and where the element is, and
   // the pointer is drawn from the bubble toward it. See tooltip_bubble in
   // element/style/tooltip.hpp.
   ////////////////////////////////////////////////////////////////////////////
   class tooltip_bubble_base : public proxy_base
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;
      void                    draw(context const& ctx) override;
      std::string             class_name() const override;

      // Where the tip is: on `side` of the element, `at` the middle of that
      // side of the element, in view coordinates.
      void                    anchor(tooltip_position side, point at);

      static constexpr float  pointer = 7;      // the pointer's length
      static constexpr float  shadow = 24;      // the shadow's reach past it

   private:

      tooltip_position        _side = tooltip_position::top;
      point                   _at;
   };

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, tooltip_bubble_base>
   basic_tooltip_bubble(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   class tooltip_element : public proxy_base
   {
   public:

      using base_type = proxy_base;
      using on_hover_function = std::function<void(bool visible)>;

                              template <typename Tip>
                              tooltip_element(Tip&& tip, duration delay, tooltip_position pos)
                               : _tip{share(basic_popup(std::forward<Tip>(tip)))}
                               , _delay{delay}
                               , _position{pos}
                              {}

                              tooltip_element(tooltip_element const& rhs);
                              tooltip_element(tooltip_element&& rhs) = default;
                              ~tooltip_element();

      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    key(context const& ctx, key_info k) override;
      std::string             class_name() const override;

      on_hover_function       on_hover = [](bool){};

      // While false the tip does not show. Tooltips given the same
      // pointer are armed and disarmed together, as by a help switch.
      std::shared_ptr<bool>   armed = std::make_shared<bool>(true);

   private:

      using popup_ptr = std::shared_ptr<basic_popup_element>;
      enum status {tip_hidden, tip_delayed, tip_visible};

      rect                    tip_bounds(context const& ctx, tooltip_position& side) const;
      void                    close_tip(view& view_);

      popup_ptr               _tip;
      status                  _tip_status = tip_hidden;
      duration                _delay;
      bool                    _cursor_in_tip = false;
      tooltip_position        _position;

      // Cleared when the tooltip is destroyed: a step still posted to the
      // view, or a cursor on its tip, then does nothing to it.
      std::shared_ptr<bool>   _alive = std::make_shared<bool>(true);
   };

   template <concepts::Element Subject, typename Tip>
   inline proxy<remove_cvref_t<Subject>, tooltip_element>
   tooltip(
      Subject&& subject
    , Tip&& tip
    , duration delay = milliseconds{500}
    , tooltip_position pos = tooltip_position::top)
   {
      return {std::forward<Subject>(subject), std::forward<Tip>(tip), delay, pos};
   }
}

#endif
