/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/receiver.hpp>
#include <elements/element.hpp>

namespace cycfi::elements
{
   /**
    * \brief
    *    Starts an editing operation.
    *
    * \param view_
    *    A reference to a `view` object.
    */
   void receiver_base::begin_edit(view& view_)
   {
      if (auto e = dynamic_cast<element*>(this))
         view_.manage_on_tracking(*e, element::begin_tracking);
   }

   /**
    * \brief
    *    Ongoing editing operation.
    *
    * \param view_
    *    A reference to a `view` object.
    */
   void receiver_base::notify_edit(view& view_)
   {
      if (auto e = dynamic_cast<element*>(this))
         view_.manage_on_tracking(*e, element::while_tracking);
   }

   /**
    * \brief
    *    Completes an editing operation.
    *
    * \param view_
    *    A reference to a `view` object.
    */
   void receiver_base::end_edit(view& view_)
   {
      if (auto e = dynamic_cast<element*>(this))
         view_.manage_on_tracking(*e, element::end_tracking);
   }
}

