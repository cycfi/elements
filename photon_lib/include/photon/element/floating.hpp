/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_FLOATING_JUNE_9_2016)
#define PHOTON_GUI_LIB_WIDGET_FLOATING_JUNE_9_2016

#include <photon/element/proxy.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Floating elements
   ////////////////////////////////////////////////////////////////////////////
   class floating_element : public proxy_base
   {
   public:
                              floating_element(rect bounds)
                              : _bounds(bounds)
                              {}

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);

      rect                    bounds() const { return _bounds; }
      void                    bounds(rect bounds_) { _bounds = bounds_; }

   private:

      rect                    _bounds;
   };

   template <typename Subject>
   inline proxy<typename std::decay<Subject>::type, floating_element>
   floating(rect bounds, Subject&& subject)
   {
      return { std::forward<Subject>(subject), bounds };
   }
}

#endif