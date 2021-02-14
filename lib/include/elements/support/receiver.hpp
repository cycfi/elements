/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_RECEIVER_MARCH_1_2020)
#define ELEMENTS_RECEIVER_MARCH_1_2020

#include <infra/string_view.hpp>
#include <string>

namespace cycfi { namespace elements
{
   class element;
   class view;

   struct receiver_base
   {
      enum editing { none, begin_editing, while_editing, end_editing };

      virtual              ~receiver_base() = default;
      void                 begin_edit(view& view_);
      void                 notify_edit(view& view_);
      void                 end_edit(view& view_);
   };

   template <typename T>
   struct receiver : receiver_base
   {
      using receiver_type = T;
      using getter_type = T;
      using param_type = T;

      virtual void         value(param_type val) = 0;
      virtual getter_type  value() const = 0;
      virtual void         edit(view& view_, param_type val);
   };

   template <>
   struct receiver<std::string> : receiver_base
   {
      using receiver_type = std::string;
      using getter_type = std::string const&;
      using param_type = string_view;

      virtual void         value(param_type str) = 0;
      virtual getter_type  value() const = 0;
      virtual void         edit(view& view_, param_type val);
   };

   template <typename T>
   struct basic_receiver : receiver<T>
   {
      using receiver_type = typename receiver<T>::receiver_type;
      using getter_type = typename receiver<T>::getter_type;
      using param_type = typename receiver<T>::param_type;

      void                 value(param_type val) override { _val = val; }
      T                    value() const override { return _val; }

   private:

      receiver_type        _val = receiver_type{};
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   template <typename T>
   inline void receiver<T>::edit(view& view_, param_type val)
   {
      value(val);
      receiver_base::notify_edit(view_);
   }

   inline void receiver<std::string>::edit(view& view_, param_type val)
   {
      value(val);
      receiver_base::notify_edit(view_);
   }
}}

#endif
