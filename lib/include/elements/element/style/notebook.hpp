/*=============================================================================
   Copyright (c) 2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_NOTEBOOK_MAY_23_2020)
#define ELEMENTS_STYLE_NOTEBOOK_MAY_23_2020

#include <elements/element/style/tab.hpp>
#include <elements/element/tile.hpp>
#include <elements/element/align.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/layer.hpp>
#include <elements/element/traversal.hpp>
#include <elements/view.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>

namespace cycfi::elements
{
   namespace detail
   {
      // The number of pages a deck holds, when it is fixed: a deck made
      // with deck(...) holds a std::array.
      template <typename Pages, typename = void>
      struct fixed_pages : std::false_type {};

      template <typename Pages>
      struct fixed_pages<Pages, std::void_t<
         decltype(std::tuple_size<typename Pages::container_type>::value)>>
       : std::true_type
      {
         static constexpr std::size_t size =
            std::tuple_size<typename Pages::container_type>::value;
      };

      template <typename Pages, std::size_t num_tabs>
      constexpr void check_pages()
      {
         if constexpr (fixed_pages<Pages>::value)
            static_assert(fixed_pages<Pages>::size == num_tabs,
               "The number of pages and tabs do not match.");
      }

      template <typename Pages, typename Tab, typename... RestTabs>
      void link_tabs(view& view_, Pages pages_, std::size_t index, Tab tab_, RestTabs... rest)
      {
         auto* tab = find_element<basic_button*>(tab_.get());
         auto* pages = find_element<deck_element*>(pages_.get());
         if (tab && pages)
         {
            tab->on_click =
               [index, pages, &view_](bool state)
               {
                  if (state)
                  {
                     pages->select(index);
                     view_.refresh(*pages);
                  }
               };
         }

         if constexpr(sizeof...(RestTabs) > 0)
            link_tabs(view_, pages_, index+1, rest...);
      }

      template <typename Pages, typename... Tabs>
      auto vnotebook(view& view_, Pages&& pages, Tabs&&... tabs)
      {
         // Link the notebook
         link_tabs(view_, pages, tabs...);

         return vtile(
            align_left(htile(hold(tabs)...)),
            hold(pages)
         );
      }

      template <typename Pages, typename... Tabs>
      auto hnotebook(view& view_, Pages&& pages, Tabs&&... tabs)
      {
         // Link the notebook
         link_tabs(view_, pages, tabs...);

         return htile(
            align_top(vtile(hold(tabs)...)),
            hold(pages)
         );
      }
   }

   template <typename Pages, typename Tab, typename... RestTabs>
   void link_tabs(view& view_, Pages pages, Tab tab, RestTabs... rest)
   {
      detail::check_pages<
         typename std::pointer_traits<Pages>::element_type
       , sizeof...(RestTabs) + 1
      >();

      auto const num_pages = pages->size();
      auto const num_tabs = sizeof...(RestTabs) + 1;
      if (num_pages != num_tabs)
         throw std::runtime_error{"Error: The number of pages (" + std::to_string(num_pages) + ") "
            "and tabs (" + std::to_string(num_tabs) + ") do not match."};

      // Select the first tab
      tab->select(true);

      detail::link_tabs(view_, pages, 0, tab, rest...);
   }

   template <typename Pages, typename... Tabs>
   auto vnotebook(view& view_, Pages&& pages, Tabs&&... tabs)
   {
      detail::check_pages<remove_cvref_t<Pages>, sizeof...(Tabs)>();
      return detail::vnotebook(
         view_
       , share(std::forward<Pages>(pages))
       , share(std::forward<Tabs>(tabs))...
      );
   }

   template <typename Pages, typename... Tabs>
   auto hnotebook(view& view_, Pages&& pages, Tabs&&... tabs)
   {
      detail::check_pages<remove_cvref_t<Pages>, sizeof...(Tabs)>();
      return detail::hnotebook(
         view_
       , share(std::forward<Pages>(pages))
       , share(std::forward<Tabs>(tabs))...
      );
   }
}

#endif
