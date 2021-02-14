/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/receiver.hpp>
#include <elements/element.hpp>

namespace cycfi { namespace elements
{
   void receiver_base::begin_edit(view& view_)
   {
      if (auto e = dynamic_cast<element*>(this))
         view_.manage_on_tracking(*e, element::begin_tracking);
   }

   void receiver_base::notify_edit(view& view_)
   {
      if (auto e = dynamic_cast<element*>(this))
         view_.manage_on_tracking(*e, element::while_tracking);
   }

   void receiver_base::end_edit(view& view_)
   {
      if (auto e = dynamic_cast<element*>(this))
         view_.manage_on_tracking(*e, element::end_tracking);
   }
}}

