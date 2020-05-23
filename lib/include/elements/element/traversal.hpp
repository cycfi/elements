/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TRAVERSAL_MAY_23_2020)
#define ELEMENTS_TRAVERSAL_MAY_23_2020

#include <elements/element/proxy.hpp>
#include <type_traits>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // find_subject utility finds the outermost subject of the given pointer
   // type or nullptr if not found. Searches subjects of proxies only.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   class indirect;

   template <typename Element>
   class reference;

   template <typename Element>
   class shared_element;

   namespace detail
   {
      template <typename Ptr>
      inline Ptr find_element_impl(element* e_)
      {
         if (auto* e = dynamic_cast<Ptr>(e_))
            return e;

         using E = std::remove_pointer_t<Ptr>;
         if constexpr(std::is_base_of<element, E>::value)
         {
            using indirect_shared = indirect<shared_element<E>>;
            if (auto *e = dynamic_cast<indirect_shared*>(e_))
               return &e->get();

            using indirect_reference = indirect<reference<E>>;
            if (auto *e = dynamic_cast<indirect_reference*>(e_))
               return &e->get();
         }
         return nullptr;
      }
   }

   template <typename Ptr>
   inline Ptr find_subject(element* e_)
   {
      proxy_base* proxy = dynamic_cast<proxy_base*>(e_);
      while (proxy)
      {
         auto* subject = &proxy->subject();
         if (auto* e = detail::find_element_impl<Ptr>(subject))
            return e;
         proxy = dynamic_cast<proxy_base*>(subject);
      }
      return nullptr;
   }

   ////////////////////////////////////////////////////////////////////////////
   // find_element utility finds the outermost subject of the given pointer
   // type or nullptr if not found. Searches an element and if it is a proxy,
   // searches its subject too.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Ptr>
   inline Ptr find_element(element* e_)
   {
      if (auto* e = detail::find_element_impl<Ptr>(e_))
         return e;
      return find_subject<Ptr>(e_);
   }
}}

#endif
