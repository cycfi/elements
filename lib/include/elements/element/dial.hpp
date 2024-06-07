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
   enum class dial_mode_enum : int
   {
      linear,
      radial,
   };

   ////////////////////////////////////////////////////////////////////////////
   // Dials
   ////////////////////////////////////////////////////////////////////////////
   class dial_base : public tracker<proxy_base>, public receiver<double>
   {
   public:

      using dial_function = std::function<void(double pos)>;

                           dial_base(double init_value = 0.0);

      void                 prepare_subject(context& ctx) override;
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

   template <concepts::Element Subject>
   inline proxy<remove_cvref_t<Subject>, dial_base>
   dial(Subject&& subject, double init_value = 0.0)
   {
      return {std::forward<Subject>(subject), init_value};
   }

   inline double dial_base::value() const
   {
      return _value;
   }

   inline element* dial_base::hit_test(context const& ctx, point p, bool leaf, bool /*control*/)
   {
      return element::hit_test(ctx, p, leaf, false); // accept non-control subjects
   }
}

#endif
