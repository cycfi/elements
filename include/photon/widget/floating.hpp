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
   template <typename Subject>
   class floating_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                     floating_widget(rect bounds, Subject&& subject);
                     floating_widget(rect bounds, Subject const& subject);

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      rect           _bounds;
   };

   template <typename Subject>
   inline floating_widget<Subject>::floating_widget(rect bounds, Subject&& subject)
    : base_type(std::forward<Subject>(subject))
    , _bounds(bounds)
   {}

   template <typename Subject>
   inline floating_widget<Subject>::floating_widget(rect bounds, Subject const& subject)
    : base_type(subject)
    , _bounds(bounds)
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
      ctx.bounds = _bounds;
   }
}

#endif