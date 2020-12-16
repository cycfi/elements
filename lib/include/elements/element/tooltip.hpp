/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TOOLTIP_AUGUST_27_2020)
#define ELEMENTS_TOOLTIP_AUGUST_27_2020

#include <elements/element/proxy.hpp>
#include <elements/element/popup.hpp>
#include <infra/support.hpp>
#include <functional>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Tooltip elements
   ////////////////////////////////////////////////////////////////////////////
   class tooltip_element : public proxy_base
   {
   public:

      using base_type = proxy_base;
      using on_hover_function = std::function<void(bool visible)>;

                              template <typename Tip>
                              tooltip_element(Tip&& tip, duration delay)
                               : _tip(share(basic_popup(std::forward<Tip>(tip))))
                               , _delay(delay)
                              {}

      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    key(context const& ctx, key_info k) override;

      on_hover_function       on_hover = [](bool){};

   private:

      using popup_ptr = std::shared_ptr<basic_popup_element>;
      enum status { tip_hidden, tip_delayed, tip_visible };

      rect                    tip_bounds(context const& ctx) const;
      void                    close_tip(view& view_);

      popup_ptr               _tip;
      status                  _tip_status = tip_hidden;
      duration                _delay;
      bool                    _cursor_in_tip = false;
   };

   template <typename Subject, typename Tip>
   inline proxy<remove_cvref_t<Subject>, tooltip_element>
   tooltip(Subject&& subject, Tip&& tip, duration delay = milliseconds{ 500 })
   {
      return { std::forward<Subject>(subject), std::forward<Tip>(tip), delay };
   }
}}

#endif
