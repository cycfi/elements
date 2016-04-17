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
   template <typename Rect>
   class margin_widget : public proxy
   {
   public:

      margin_widget(Rect const& margin_, std::shared_ptr<widget> subject)
       : proxy(subject)
       , _margin(margin_)
      {}

      ~margin_widget() {}

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      Rect           _margin;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Rect>
   inline rect margin_widget<Rect>::limits(basic_context const& ctx) const
   {
      auto r = subject()->limits(ctx);

      r.left += _margin.left + _margin.right;
      r.right += _margin.left + _margin.right;
      r.top += _margin.top + _margin.bottom;
      r.bottom += _margin.top + _margin.bottom;

      return r;
   }

   template <typename Rect>
   inline void margin_widget<Rect>::prepare_subject(context& ctx)
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
   inline widget_ptr
   margin(rect const& margin_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new margin_widget<rect>{ margin_, subject } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Left Margin
   struct left_margin_rect : static_empty_rect
   {
      left_margin_rect(float left) : left(left) {}
      double left;
   };

   inline widget_ptr
   left_margin(left_margin_rect const& margin_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new margin_widget<left_margin_rect>{ margin_, subject } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Right Margin
   struct right_margin_rect : static_empty_rect
   {
      right_margin_rect(float right) : right(right) {}
      double right;
   };

   inline widget_ptr
   right_margin(right_margin_rect const& margin_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new margin_widget<right_margin_rect>{ margin_, subject } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Top Margin
   struct top_margin_rect : static_empty_rect
   {
      top_margin_rect(float top) : top(top) {}
      double top;
   };

   inline widget_ptr
   top_margin(top_margin_rect const& margin_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new margin_widget<top_margin_rect>{ margin_, subject } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Bottom Margin
   struct bottom_margin_rect : static_empty_rect
   {
      bottom_margin_rect(float bottom) : bottom(bottom) {}
      double bottom;
   };

   inline widget_ptr
   bottom_margin(bottom_margin_rect const& margin_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new margin_widget<bottom_margin_rect>{ margin_, subject } };
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

   inline widget_ptr
   xside_margin(xside_margin_rect const& margin_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new margin_widget<xside_margin_rect>{ margin_, subject } };
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

   inline widget_ptr
   yside_margin(yside_margin_rect const& margin_, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new margin_widget<yside_margin_rect>{ margin_, subject } };
   }
}

#endif