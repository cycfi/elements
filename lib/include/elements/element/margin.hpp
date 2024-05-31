/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MARGIN_APRIL_11_2016)
#define ELEMENTS_MARGIN_APRIL_11_2016

#include <elements/element/proxy.hpp>
#include <elements/support/context.hpp>
#include <infra/support.hpp>
#include <memory>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Margins
   ////////////////////////////////////////////////////////////////////////////
   template <typename Rect, concepts::Element Subject>
   class margin_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              margin_element(Rect margin_, Subject subject)
                               : base_type(std::move(subject))
                               , _margin(margin_)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

   private:

      Rect                    _margin;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   template <typename Rect, concepts::Element Subject>
   inline view_limits margin_element<Rect, Subject>::limits(basic_context const& ctx) const
   {
      auto r = this->subject().limits(ctx);

      r.min.x += _margin.left + _margin.right;
      r.max.x += _margin.left + _margin.right;
      r.min.y += _margin.top + _margin.bottom;
      r.max.y += _margin.top + _margin.bottom;

      clamp_max(r.max.x, full_extent);
      clamp_max(r.max.y, full_extent);
      return r;
   }

   template <typename Rect, concepts::Element Subject>
   inline void margin_element<Rect, Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.top += _margin.top;
      ctx.bounds.left += _margin.left;
      ctx.bounds.bottom -= _margin.bottom;
      ctx.bounds.right -= _margin.right;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Builders
   ////////////////////////////////////////////////////////////////////////////
   struct static_empty_rect
   {
      static constexpr int left = 0;
      static constexpr int right = 0;
      static constexpr int top = 0;
      static constexpr int bottom = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Full Margin
   template <concepts::Element Subject>
   inline margin_element<rect, remove_cvref_t<Subject>>
   margin(rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Left Margin
   struct margin_left_rect : static_empty_rect
   {
      margin_left_rect(float left) : left(left) {}
      float left;
   };

   template <concepts::Element Subject>
   inline margin_element<margin_left_rect, remove_cvref_t<Subject>>
   margin_left(margin_left_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_left(...) instead.")]]
   inline margin_element<margin_left_rect, remove_cvref_t<Subject>>
   left_margin(margin_left_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Right Margin
   struct margin_right_rect : static_empty_rect
   {
      margin_right_rect(float right) : right(right) {}
      float right;
   };

   template <concepts::Element Subject>
   inline margin_element<margin_right_rect, remove_cvref_t<Subject>>
   margin_right(margin_right_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_right(...) instead.")]]
   inline margin_element<margin_right_rect, remove_cvref_t<Subject>>
   right_margin(margin_right_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Top Margin
   struct margin_top_rect : static_empty_rect
   {
      margin_top_rect(float top) : top(top) {}
      float top;
   };

   template <concepts::Element Subject>
   inline margin_element<margin_top_rect, remove_cvref_t<Subject>>
   margin_top(margin_top_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_top(...) instead.")]]
   inline margin_element<margin_top_rect, remove_cvref_t<Subject>>
   top_margin(margin_top_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Bottom Margin
   struct margin_bottom_rect : static_empty_rect
   {
      margin_bottom_rect(float bottom) : bottom(bottom) {}
      float bottom;
   };

   template <concepts::Element Subject>
   inline margin_element<margin_bottom_rect, remove_cvref_t<Subject>>
   margin_bottom(margin_bottom_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_bottom(...) instead.")]]
   inline margin_element<margin_bottom_rect, remove_cvref_t<Subject>>
   bottom_margin(margin_bottom_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Left Top Margin
   struct margin_left_top_rect : static_empty_rect
   {
      margin_left_top_rect(float left, float top) : left(left), top(top) {}
      float left;
      float top;
   };

   template <concepts::Element Subject>
   inline margin_element<margin_left_top_rect, remove_cvref_t<Subject>>
   margin_left_top(margin_left_top_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_left_top(...) instead.")]]
   inline margin_element<margin_left_top_rect, remove_cvref_t<Subject>>
   left_top_margin(margin_left_top_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Horizontal / Left Right Margin
   struct hmargin_rect : static_empty_rect
   {
      hmargin_rect(float left, float right) : left(left), right(right) {}
      hmargin_rect(float left_right) : left(left_right), right(left_right) {}
      float left;
      float right;
   };

   template <concepts::Element Subject>
   inline margin_element<hmargin_rect, remove_cvref_t<Subject>>
   hmargin(hmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline margin_element<hmargin_rect, remove_cvref_t<Subject>>
   margin_left_right(hmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_left_right(...) instead.")]]
   inline margin_element<hmargin_rect, remove_cvref_t<Subject>>
   left_right_margin(hmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use hmargin(...) instead.")]]
   inline margin_element<hmargin_rect, remove_cvref_t<Subject>>
   xside_margin(hmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Vertical / Top Bottom Margin
   struct vmargin_rect : static_empty_rect
   {
      vmargin_rect(float top, float bottom) : top(top), bottom(bottom) {}
      vmargin_rect(float top_bottom) : top(top_bottom), bottom(top_bottom) {}
      float top;
      float bottom;
   };

   template <concepts::Element Subject>
   inline margin_element<vmargin_rect, remove_cvref_t<Subject>>
   vmargin(vmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   inline margin_element<vmargin_rect, remove_cvref_t<Subject>>
   margin_top_bottom(vmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_top_bottom(...) instead.")]]
   inline margin_element<vmargin_rect, remove_cvref_t<Subject>>
   top_bottom_margin(vmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use vmargin(...) instead.")]]
   inline margin_element<vmargin_rect, remove_cvref_t<Subject>>
   yside_margin(vmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }
}

#endif
