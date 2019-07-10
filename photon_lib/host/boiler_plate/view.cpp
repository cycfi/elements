/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elemental/base_view.hpp>

namespace cycfi { namespace elemental
{
   ///////////////////////////////////////////////////////////////////////////
   // Main view creation callback
   std::function<std::unique_ptr<base_view>(host_view* h)> new_view;

   point base_view::cursor_pos() const
   {
      return {};
   }

   point base_view::size() const
   {
      return {};
   }

   void base_view::size(point p)
   {
   }

   void base_view::refresh()
   {
   }

   void base_view::refresh(rect area)
   {
   }

   void base_view::limits(view_limits limits_, bool maintain_aspect)
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

