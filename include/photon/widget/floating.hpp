/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_FLOATING_JUNE_9_2016)
#define PHOTON_GUI_LIB_WIDGET_FLOATING_JUNE_9_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Floating widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class floating_widget_base
   {
   public:

                     floating_widget_base(rect bounds)
                      : _bounds(bounds)
                     {}

      rect           bounds() const { return _bounds; }
      void           bounds(rect bounds_) { _bounds = bounds_; }

   private:

      rect           _bounds;
   };

   template <typename Subject>
   class floating_widget : public proxy<Subject>, public floating_widget_base
   {
   public:

      using base_type = proxy<Subject>;

                     floating_widget(rect bounds, Subject&& subject);
                     floating_widget(rect bounds, Subject const& subject);

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);
   };

   template <typename Subject>
   inline floating_widget<Subject>::floating_widget(rect bounds, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , floating_widget_base(bounds)
   {}

   template <typename Subject>
   inline floating_widget<Subject>::floating_widget(rect bounds, Subject const& subject)
    : base_type(subject)
    , floating_widget_base(bounds)
   {}

   template <typename Subject>
   inline floating_widget<typename std::decay<Subject>::type>
   floating(rect bounds, Subject&& subject)
   {
      return { bounds, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect floating_widget<Subject>::limits(basic_context const& ctx) const
   {
      return full_limits;
   }

   template <typename Subject>
   inline void floating_widget<Subject>::prepare_subject(context& ctx)
   {
      rect  e_limits = this->subject().limits(ctx);
      ctx.bounds = bounds();
      float w = ctx.bounds.width();
      float h = ctx.bounds.height();

      if (w < e_limits.left)
         ctx.bounds.width(e_limits.left);
      else if (w > e_limits.right)
         ctx.bounds.width(e_limits.right);

      if (h < e_limits.top)
         ctx.bounds.height(e_limits.top);
      else if (w > e_limits.bottom)
         ctx.bounds.height(e_limits.bottom);
   }
}

#endif