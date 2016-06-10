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
   class floating_widget : public proxy_base
   {
   public:
                     floating_widget(rect bounds)
                      : _bounds(bounds)
                     {}

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

      rect           bounds() const { return _bounds; }
      void           bounds(rect bounds_) { _bounds = bounds_; }

   private:

      rect           _bounds;
   };

   template <typename Subject>
   inline proxy<typename std::decay<Subject>::type, floating_widget>
   floating(rect bounds, Subject&& subject)
   {
      return { std::forward<Subject>(subject), bounds };
   }
}

#endif