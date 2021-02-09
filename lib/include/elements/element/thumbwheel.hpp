/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_THUMBWHEEL_SEPTEMBER_22_2020)
#define ELEMENTS_THUMBWHEEL_SEPTEMBER_22_2020

#include <elements/element/proxy.hpp>
#include <elements/element/tracker.hpp>
#include <elements/support.hpp>
#include <infra/support.hpp>
#include <functional>
#include <array>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Thumbwheels
   ////////////////////////////////////////////////////////////////////////////
   class thumbwheel_base :
      public tracker<proxy_base>
    , public receiver<point>
   {
   public:

      using thumbwheel_function = std::function<void(point)>;

                           thumbwheel_base(point init = { 0.0f, 0.0f });

      void                 prepare_subject(context& ctx) override;
      element*             hit_test(context const& ctx, point p) override;

      bool                 scroll(context const& ctx, point dir, point p) override;
      void                 keep_tracking(context const& ctx, tracker_info& track_info) override;

      point                value() const override;
      void                 value(point val) override;
      void                 edit(view& view_, point val) override;

      thumbwheel_function  on_change;

   private:

      point          compute_value(context const& ctx, tracker_info& track_info);

      point          _value;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, thumbwheel_base>
   thumbwheel(Subject&& subject, point init = { 0.0f, 0.0f })
   {
      return { std::forward<Subject>(subject), init };
   }

   inline point thumbwheel_base::value() const
   {
      return _value;
   }

   inline element* thumbwheel_base::hit_test(context const& ctx, point p)
   {
      return element::hit_test(ctx, p);
   }
}}

#endif
