/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_THUMBWHEEL_SEPTEMBER_23_2020)
#define ELEMENTS_GALLERY_THUMBWHEEL_SEPTEMBER_23_2020

#include <elements/element/dial.hpp>
#include <elements/element/dynamic_list.hpp>
// #include <elements/element/align.hpp>
// #include <elements/element/layer.hpp>
// #include <elements/element/indirect.hpp>
// #include <elements/element/tile.hpp>
// #include <elements/element/gallery/caption.hpp>

namespace cycfi { namespace elements
{
   template <typename F>
   inline auto vthumbwheel(double init_value, std::size_t num_items, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      auto body = basic_vthumbwheel(
         dynamic_list{ basic_cell_composer(num_items, std::forward<ftype>(compose)) }
       , 1.0 / (num_items-1)
      );
      return thumbwheel(std::move(body), init_value);
   }

   template <typename F>
   inline auto vthumbwheel(std::size_t num_items, F&& compose)
   {
      using ftype = remove_cvref_t<F>;
      return vthumbwheel(0.0, num_items, std::forward<ftype>(compose));
   }

   // ////////////////////////////////////////////////////////////////////////////
   // // vthumbwheel
   // ////////////////////////////////////////////////////////////////////////////
   // template <typename FirstLabel, typename... Label>
   // inline auto vthumbwheel(double init_value, FirstLabel&& first_label, Label&&... label)
   // {
   //    auto body = basic_vthumbwheel(
   //       vtile(std::forward<FirstLabel>(first_label), std::forward<Label>(label)...)
   //     , 1.0 / sizeof...(Label)
   //    );
   //    return thumbwheel(std::move(body), init_value);
   // }

   // template <typename... Label>
   // inline auto vthumbwheel(Label&&... label)
   // {
   //   return vthumbwheel(0.0, std::forward<Label>(label)...);
   // }

   // ////////////////////////////////////////////////////////////////////////////
   // // hthumbwheel
   // ////////////////////////////////////////////////////////////////////////////
   // template <typename FirstLabel, typename... Label>
   // inline auto hthumbwheel(double init_value, FirstLabel&& first_label, Label&&... label)
   // {
   //    auto body = basic_hthumbwheel(
   //       htile(std::forward<FirstLabel>(first_label), std::forward<Label>(label)...)
   //     , 1.0 / sizeof...(Label)
   //    );
   //    return thumbwheel(std::move(body), init_value);
   // }

   // template <typename... Label>
   // inline auto hthumbwheel(Label&&... label)
   // {
   //   return hthumbwheel(0.0, std::forward<Label>(label)...);
   // }

}}

#endif
