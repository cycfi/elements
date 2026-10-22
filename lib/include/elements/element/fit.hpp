/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FIT_OCTOBER_22_2026)
#define ELEMENTS_FIT_OCTOBER_22_2026

#include <elements/element/proxy.hpp>
#include <elements/view.hpp>
#include <algorithm>
#include <cmath>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Fit
   //
   // A fit scales its subject uniformly to fill the view. The subject is
   // laid out at its minimum size, its view box, and the view's scale is
   // set so that size fits the view, keeping its proportion. The space
   // left over, along one axis, is the subject's from its left-top, up to
   // its maximum size: a fixed size subject stays left-top, and one that
   // aligns itself (align_center_middle) can center.
   //
   // The fit asks the view to keep the subject's proportion, which the
   // window keeps while it is resized, where the platform can; elsewhere
   // the space over is left empty.
   //
   // A fit is a top-level proxy: it is the view's content, not inside
   // another element. It sets the scale of the whole view, so what the
   // content opens (menus, tooltips, child windows) scales with it.
   //
   // fit_element takes any scale. bounded_fit_element keeps it within a
   // range, the view box times the range being its limits, so the window
   // is held to them.
   ////////////////////////////////////////////////////////////////////////////
   template <concepts::Element Subject>
   class fit_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;
      using base_type::base_type;

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      void                    prepare_subject(context& ctx) override;
      std::string             class_name() const override { return "fit"; }

   protected:

      // The scale the view is set to, from the one that fits
      virtual float           scale_of(float fit_scale) const
                              { return fit_scale; }
   };

   template <concepts::Element Subject>
   class bounded_fit_element : public fit_element<Subject>
   {
   public:

      using base_type = fit_element<Subject>;

                              bounded_fit_element(
                                 float min_scale, float max_scale
                               , Subject subject
                              )
                               : base_type(std::move(subject))
                               , _min_scale(min_scale)
                               , _max_scale(std::max(min_scale, max_scale))
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      std::string             class_name() const override
                              { return "bounded_fit"; }

      float                   min_scale() const { return _min_scale; }
      float                   max_scale() const { return _max_scale; }

   protected:

      float                   scale_of(float fit_scale) const override
                              {
                                 return std::clamp(
                                    fit_scale, _min_scale, _max_scale);
                              }

   private:

      float                   _min_scale;
      float                   _max_scale;
   };

   template <concepts::Element Subject>
   inline fit_element<remove_cvref_t<Subject>>
   fit(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline bounded_fit_element<remove_cvref_t<Subject>>
   fit(float min_scale, float max_scale, Subject&& subject)
   {
      return {min_scale, max_scale, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline view_limits
   fit_element<Subject>::limits(basic_context const& /*ctx*/) const
   {
      // Any size: the scale follows it
      return {{0, 0}, {full_extent, full_extent}};
   }

   template <concepts::Element Subject>
   inline void fit_element<Subject>::layout(context const& ctx)
   {
      auto box = this->subject().limits(ctx).min;
      if (box.x > 0 && box.y > 0)
      {
         auto& view_ = ctx.view;
         view_.aspect_ratio(box.x / box.y);

         // The bounds are in the view's scaled space: at scale s, the view
         // is the bounds times s. The scale that fits the box is set after
         // this layout, and the view laid out again at it.
         auto s = view_.scale();
         auto fit_s = scale_of(std::min(
            ctx.bounds.width() * s / box.x
          , ctx.bounds.height() * s / box.y
         ));
         if (fit_s > 0 && std::abs(fit_s - s) > s * 1e-4f)
            view_.post([&view_, fit_s]
            {
               view_.scale(fit_s);
               view_.layout();
            });
      }
      base_type::layout(ctx);
   }

   template <concepts::Element Subject>
   inline view_limits
   bounded_fit_element<Subject>::limits(basic_context const& ctx) const
   {
      // The view box times the range, in the window's units. The limits
      // are taken inside the view's scale, so they are divided by it.
      auto box = this->subject().limits(ctx).min;
      auto s = ctx.view.scale();
      if (box.x <= 0 || box.y <= 0)
         return base_type::limits(ctx);
      return {
         {box.x * _min_scale / s, box.y * _min_scale / s}
       , {box.x * _max_scale / s, box.y * _max_scale / s}
      };
   }

   template <concepts::Element Subject>
   inline void fit_element<Subject>::prepare_subject(context& ctx)
   {
      // From the left-top, up to the subject's maximum size
      auto max = this->subject().limits(ctx).max;
      auto& b = ctx.bounds;
      b.right = b.left + std::min(b.width(), max.x);
      b.bottom = b.top + std::min(b.height(), max.y);
   }
}

#endif
