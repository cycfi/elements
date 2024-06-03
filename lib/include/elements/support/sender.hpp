/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SENDER_JUNE_6_2020)
#define ELEMENTS_SENDER_JUNE_6_2020

#include <infra/string_view.hpp>
#include <string>
#include <functional>

namespace cycfi::elements
{
   /**
    *  \struct sender_base
    *
    *  \brief
    *    Base class for senders.
    *
    *    The `sender_base` serves as a base class for any object that needs
    *    to send messages.
    */
   struct sender_base
   {
      virtual              ~sender_base() = default;
   };

   /**
    * \struct sender
    *
    * \brief
    *    An abstract template class inheriting from sender_base, allows
    *    sending messages of type T.
    */
   template <typename T>
   struct sender : sender_base
   {
      using sender_type = T;
      using param_type = T;
      using callback_function = std::function<void(param_type)>;

      virtual void         send(param_type val) = 0;
      virtual void         on_send(callback_function f) = 0;
   };

   /**
    * \struct sender<std::string>
    *
    * \brief Specialization of sender for `std::string`.
    */
   template <>
   struct sender<std::string> : sender_base
   {
      using sender_type = std::string;
      using param_type = string_view;
      using callback_function = std::function<void(param_type)>;

      virtual void         send(param_type str) = 0;
      virtual void         on_send(callback_function f) = 0;
   };
}

#endif
