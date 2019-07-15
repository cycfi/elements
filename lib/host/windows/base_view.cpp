/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/base_view.hpp>
#include <elements/support/resource_paths.hpp>

namespace cycfi { namespace elements
{
   base_view::base_view(host_window h)
   {
   }

   base_view::~base_view()
   {
   }

   point base_view::cursor_pos() const
   {
      return {};
   }

   elements::size base_view::size() const
   {
      return {};
   }

   void base_view::size(elements::size p)
   {
   }

   void base_view::refresh()
   {
   }

   void base_view::refresh(rect area)
   {
   }

   std::string clipboard()
   {
      return "";
   }

   void clipboard(std::string const& text)
   {
   }

   void set_cursor(cursor_type type)
   {
      switch (type)
      {
         case cursor_type::arrow:
            break;
         case cursor_type::ibeam:
            break;
         case cursor_type::cross_hair:
            break;
         case cursor_type::hand:
            break;
         case cursor_type::h_resize:
            break;
         case cursor_type::v_resize:
            break;
      }
   }
}}

