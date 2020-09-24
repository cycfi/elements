/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_THUMBWHEEL_SEPTEMBER_23_2020)
#define ELEMENTS_GALLERY_THUMBWHEEL_SEPTEMBER_23_2020

#include <elements/element/dial.hpp>
#include <elements/element/dynamic_list.hpp>

namespace cycfi { namespace elements
{
   template <typename F>
   inline auto vthumbwheel(std::size_t num_items, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      auto body = basic_vthumbwheel(
         dynamic_list{ basic_cell_composer(num_items, std::forward<ftype>(compose)) }
       , 1.0 / (num_items-1)
      );
      return thumbwheel(std::move(body), 0.0);
   }
}}

#endif
