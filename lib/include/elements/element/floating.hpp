/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_FLOATING_JUNE_9_2016)
#define ELEMENTS_FLOATING_JUNE_9_2016

#include <elements/element/proxy.hpp>
#include <infra/support.hpp>

namespace cycfi { namespace elements
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

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      rect                    bounds() const { return _bounds; }
      void                    bounds(rect bounds_) { _bounds = bounds_; }

   private:

      rect                    _bounds;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, floating_element>
   floating(rect bounds, Subject&& subject)
   {
      return { std::forward<Subject>(subject), bounds };
   }
}}

#endif
