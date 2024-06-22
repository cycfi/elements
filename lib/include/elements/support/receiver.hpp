/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_RECEIVER_MARCH_1_2020)
#define ELEMENTS_RECEIVER_MARCH_1_2020

#include <infra/string_view.hpp>
#include <string>

namespace cycfi::elements
{
   class element;
   class view;

   /**
    * \brief
    *    An abstract base class to provide editing functionality.
    *
    *    `receiver_base` defines the core editing functionalities: starting
    *    an edit operation, doing an edit operation, and ending an edit
    *    operation.
    */
   struct receiver_base
   {
      /**
       * \enum receiver_base::editing
       * \brief Represents the states of an editing operation.
       *
       * \value none No ongoing editing operation.
       * \value begin_editing The beginning of an editing operation.
       * \value while_editing Ongoing editing operation.
       * \value end_editing End of an editing operation.
       */
      enum editing { none, begin_editing, while_editing, end_editing };

      virtual              ~receiver_base() = default;
      void                 begin_edit(view& view_);
      void                 notify_edit(view& view_);
      void                 end_edit(view& view_);
   };

   /**
    *  \struct receiver<T>
    *
    *  \brief
    *    An abstract base class to provide value control for a type `T`.
    *
    *  \tparam T The type of value that the receiver will handle.
    */
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

   /**
    *  \struct receiver<std::string>
    *
    *  \brief
    *    Specialization of `receiver<T>` for `std::string`.
    */
   template <>
   struct receiver<std::string> : receiver_base
   {
      using receiver_type = std::string;
      using getter_type = std::string const&;
      using param_type = string_view;

      virtual void         value(param_type val) = 0;
      virtual getter_type  value() const = 0;
      virtual void         edit(view& view_, param_type val);
   };

   /**
    *  \struct basic_receiver<T>
    *
    *  \brief
    *    Basic value receiver that saves the value in a member variable.
    *
    *  \tparam T The type of value that this receiver will handle.
    */
   template <typename T>
   struct basic_receiver : receiver<T>
   {
      using receiver_type = typename receiver<T>::receiver_type;
      using getter_type = typename receiver<T>::getter_type;
      using param_type = typename receiver<T>::param_type;

      void                 value(param_type val) override;
      T                    value() const override;

   private:

      receiver_type        _val = receiver_type{};
   };

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    Set the receiver's value.
    *
    * \param val
    *    The new value for the receiver.
    */
   template <typename T>
   inline void basic_receiver<T>::value(param_type val)
   {
      _val = val;
   }

   /**
    * \brief
    *    get the receiver's value.
    *
    * \return
    *    The value of the receiver.
    */
   template <typename T>
   inline T basic_receiver<T>::value() const
   {
      return _val;
   }

   /**
    * \brief
    *    An edit function for setting the new value of type `T` and notifying
    *    the view of the changes.
    *
    *    This function sets the new value using the provided `value()`
    *    function and then notifies the view `view_` that the edit operation
    *    has been performed via `notify_edit()` function of `receiver_base`.
    *
    * \tparam T
    *    The type of value that the receiver manages.
    * \param view_
    *    A reference to a `view` object.
    * \param val
    *    The new value of the receiver of type `T`.
    */
   template <typename T>
   inline void receiver<T>::edit(view& view_, param_type val)
   {
      value(val);
      receiver_base::notify_edit(view_);
   }

   /**
    * \brief
    *    An edit function specialized for `std::string` type.
    *
    * \param view_
    *    A reference to a `view` object.
    * \param val
    *    The new value of the receiver of type `std::string`.
    */
   inline void receiver<std::string>::edit(view& view_, param_type val)
   {
      value(val);
      receiver_base::notify_edit(view_);
   }
}

#endif
