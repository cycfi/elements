/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SENDER_JUNE_6_2020)
#define ELEMENTS_SENDER_JUNE_6_2020

#include <infra/string_view.hpp>
#include <string>
#include <functional>

namespace cycfi { namespace elements
{
   struct sender_base
   {
      virtual              ~sender_base() = default;
   };

   template <typename T>
   struct sender : sender_base
   {
      using sender_type = T;
      using param_type = T;
      using callback_function = std::function<void(param_type)>;

      virtual void         send(param_type val) = 0;
      virtual void         on_send(callback_function f) = 0;
   };

   template <>
   struct sender<std::string> : sender_base
   {
      using sender_type = std::string;
      using param_type = string_view;
      using callback_function = std::function<void(param_type)>;

      virtual void         send(param_type str) = 0;
      virtual void         on_send(callback_function f) = 0;
   };
}}

#endif
