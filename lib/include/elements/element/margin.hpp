/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MARGIN_APRIL_11_2016)
#define ELEMENTS_MARGIN_APRIL_11_2016

#include <elements/element/proxy.hpp>
#include <elements/support/context.hpp>
#include <infra/support.hpp>
#include <memory>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Margins
   ////////////////////////////////////////////////////////////////////////////
   template <typename Rect, typename Subject>
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
   template <typename Rect, typename Subject>
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

   template <typename Rect, typename Subject>
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
   template <typename Subject>
   inline margin_element<rect, remove_cvref_t<Subject>>
   margin(rect margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Left Margin
   struct left_margin_rect : static_empty_rect
   {
      left_margin_rect(float left) : left(left) {}
      float left;
   };

   template <typename Subject>
   inline margin_element<left_margin_rect, remove_cvref_t<Subject>>
   left_margin(left_margin_rect margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Right Margin
   struct right_margin_rect : static_empty_rect
   {
      right_margin_rect(float right) : right(right) {}
      float right;
   };

   template <typename Subject>
   inline margin_element<right_margin_rect, remove_cvref_t<Subject>>
   right_margin(right_margin_rect margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Top Margin
   struct top_margin_rect : static_empty_rect
   {
      top_margin_rect(float top) : top(top) {}
      float top;
   };

   template <typename Subject>
   inline margin_element<top_margin_rect, remove_cvref_t<Subject>>
   top_margin(top_margin_rect margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Bottom Margin
   struct bottom_margin_rect : static_empty_rect
   {
      bottom_margin_rect(float bottom) : bottom(bottom) {}
      float bottom;
   };

   template <typename Subject>
   inline margin_element<bottom_margin_rect, remove_cvref_t<Subject>>
   bottom_margin(bottom_margin_rect margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Left Top Margin
   struct left_top_margin_rect : static_empty_rect
   {
      left_top_margin_rect(float left, float top) : left(left), top(top) {}
      float left;
      float top;
   };

   template <typename Subject>
   inline margin_element<left_top_margin_rect, remove_cvref_t<Subject>>
   left_top_margin(left_top_margin_rect margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // X-Side Margin
   struct xside_margin_rect : static_empty_rect
   {
      xside_margin_rect(float left, float right) : left(left), right(right) {}
      xside_margin_rect(float left_right) : left(left_right), right(left_right) {}
      float left;
      float right;
   };

   template <typename Subject>
   inline margin_element<xside_margin_rect, remove_cvref_t<Subject>>
   xside_margin(xside_margin_rect margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Y-Side Margin
   struct yside_margin_rect : static_empty_rect
   {
      yside_margin_rect(float top, float bottom) : top(top), bottom(bottom) {}
      yside_margin_rect(float top_bottom) : top(top_bottom), bottom(top_bottom) {}
      float top;
      float bottom;
   };

   template <typename Subject>
   inline margin_element<yside_margin_rect, remove_cvref_t<Subject>>
   yside_margin(yside_margin_rect margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }
}}

#endif
