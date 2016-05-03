/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_MARGIN_APRIL_11_2016)
#define PHOTON_GUI_LIB_WIDGET_MARGIN_APRIL_11_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Margins
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Rect, typename Subject>
   class margin_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

      margin_widget(Rect const& margin_, Subject&& subject)
       : base_type(std::move(subject))
       , _margin(margin_)
      {}

      margin_widget(Rect const& margin_, Subject const& subject)
       : base_type(subject)
       , _margin(margin_)
      {}

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      Rect           _margin;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Rect, typename Subject>
   inline rect margin_widget<Rect, Subject>::limits(basic_context const& ctx) const
   {
      auto r = this->subject().limits(ctx);

      r.left += _margin.left + _margin.right;
      r.right += _margin.left + _margin.right;
      r.top += _margin.top + _margin.bottom;
      r.bottom += _margin.top + _margin.bottom;

      return r;
   }

   template <typename Rect, typename Subject>
   inline void margin_widget<Rect, Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.top += _margin.top;
      ctx.bounds.left += _margin.left;
      ctx.bounds.bottom -= _margin.bottom;
      ctx.bounds.right -= _margin.right;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Builders
   ////////////////////////////////////////////////////////////////////////////////////////////////
   struct static_empty_rect
   {
      static constexpr int left = 0;
      static constexpr int right = 0;
      static constexpr int top = 0;
      static constexpr int bottom = 0;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Full Margin
   template <typename Subject>
   inline margin_widget<rect, typename std::decay<Subject>::type>
   margin(rect const& margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Left Margin
   struct left_margin_rect : static_empty_rect
   {
      left_margin_rect(float left) : left(left) {}
      double left;
   };

   template <typename Subject>
   inline margin_widget<left_margin_rect, typename std::decay<Subject>::type>
   left_margin(left_margin_rect const& margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Right Margin
   struct right_margin_rect : static_empty_rect
   {
      right_margin_rect(float right) : right(right) {}
      double right;
   };

   template <typename Subject>
   inline margin_widget<right_margin_rect, typename std::decay<Subject>::type>
   right_margin(right_margin_rect const& margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Top Margin
   struct top_margin_rect : static_empty_rect
   {
      top_margin_rect(float top) : top(top) {}
      double top;
   };

   template <typename Subject>
   inline margin_widget<top_margin_rect, typename std::decay<Subject>::type>
   top_margin(top_margin_rect const& margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Bottom Margin
   struct bottom_margin_rect : static_empty_rect
   {
      bottom_margin_rect(float bottom) : bottom(bottom) {}
      double bottom;
   };

   template <typename Subject>
   inline margin_widget<bottom_margin_rect, typename std::decay<Subject>::type>
   bottom_margin(bottom_margin_rect const& margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Left Top Margin
   struct left_top_margin_rect : static_empty_rect
   {
      left_top_margin_rect(float left, float top) : left(left), top(top) {}
      double left;
      double top;
   };

   template <typename Subject>
   inline margin_widget<left_top_margin_rect, typename std::decay<Subject>::type>
   left_top_margin(left_top_margin_rect const& margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // X-Side Margin
   struct xside_margin_rect : static_empty_rect
   {
      xside_margin_rect(float left, float right) : left(left), right(right) {}
      xside_margin_rect(float left_right) : left(left_right), right(left_right) {}
      double left;
      double right;
   };

   template <typename Subject>
   inline margin_widget<xside_margin_rect, typename std::decay<Subject>::type>
   xside_margin(xside_margin_rect const& margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Y-Side Margin
   struct yside_margin_rect : static_empty_rect
   {
      yside_margin_rect(float top, float bottom) : top(top), bottom(bottom) {}
      yside_margin_rect(float top_bottom) : top(top_bottom), bottom(top_bottom) {}
      double top;
      double bottom;
   };

   template <typename Subject>
   inline margin_widget<yside_margin_rect, typename std::decay<Subject>::type>
   yside_margin(yside_margin_rect const& margin_, Subject&& subject)
   {
      return { margin_, std::forward<Subject>(subject) };
   }
}

#endif