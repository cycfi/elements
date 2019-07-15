/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>

namespace elements = cycfi::elements;

namespace cycfi { namespace elements
{
   window::window(std::string const& name, int style_, rect const& bounds)
   {
   }

   window::~window()
   {
   }

   point window::size() const
   {
      return {};
   }

   void window::size(point const& p)
   {
   }

   void window::limits(view_limits limits_)
   {
   }

   point window::position() const
   {
      return {};
   }

   void window::position(point const& p)
   {
   }
}}

