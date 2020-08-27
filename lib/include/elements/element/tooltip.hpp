/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TOOLTIP_AUGUST_27_2020)
#define ELEMENTS_TOOLTIP_AUGUST_27_2020

#include <elements/element/proxy.hpp>
#include <infra/support.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Tooltip elements
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject, typename Tip>
   class tooltip_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              tooltip_element(Subject subject, Tip tip, duration delay)
                               : base_type(std::move(subject))
                               , _tip(std::move(tip))
                               , _delay(delay)
                              {}

      void                    draw(context const& ctx) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;

      Subject const&          tip() const { return _tip; }
      Subject&                tip()       { return _tip; }

   private:

      enum status { tip_hidden, tip_delayed, tip_visible };

      Tip                     _tip;
      status                  _tip_status = tip_hidden;
      duration                _delay;
   };

   template <typename Subject, typename Tip>
   inline void tooltip_element<Subject, Tip>::draw(context const& ctx)
   {
      base_type::draw(ctx);
      if (_tip_status == tip_visible)
      {
         auto limits_ = _tip.limits(ctx);
         auto w = limits_.min.x;
         auto h = limits_.min.y;
         auto tip_bounds = rect{ 0, 0, w, h}
            .move_to(ctx.bounds.left, ctx.bounds.top-h);
         context tctx { ctx, &_tip, tip_bounds };
         _tip.draw(tctx);
      }
   }

   template <typename Subject, typename Tip>
   inline bool tooltip_element<Subject, Tip>::cursor(context const& ctx, point p, cursor_tracking status)
   {
      if (status != cursor_tracking::leaving)
      {
         if (_tip_status != tip_visible)
         {
            _tip_status = tip_delayed;
            ctx.view.post(std::chrono::duration_cast<std::chrono::milliseconds>(_delay),
               [this, &view = ctx.view]()
               {
                  if (_tip_status == tip_delayed)
                  {
                     _tip_status = tip_visible;
                     view.refresh();
                  }
               }
            );
         }
      }
      else
      {
         _tip_status = tip_hidden;
         ctx.view.refresh();
      }

      return base_type::cursor(ctx, p, status);
   }

   template <typename Subject, typename Tip>
   inline tooltip_element<remove_cvref_t<Subject>, remove_cvref_t<Tip>>
   tooltip(Subject&& subject, Tip&& tip, duration delay = milliseconds{ 500 })
   {
      return { std::forward<Subject>(subject), std::forward<Tip>(tip), delay };
   }
}}

#endif
