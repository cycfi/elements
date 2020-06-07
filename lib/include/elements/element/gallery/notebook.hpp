/*=============================================================================
   Copyright (c) 2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_GALLERY_NOTEBOOK_MAY_23_2020)
#define ELEMENTS_GALLERY_NOTEBOOK_MAY_23_2020

#include <elements/element/gallery/tab.hpp>
#include <elements/element/tile.hpp>
#include <elements/element/align.hpp>
#include <elements/element/margin.hpp>
#include <elements/element/layer.hpp>
#include <elements/element/layer.hpp>
#include <elements/element/traversal.hpp>
#include <elements/view.hpp>
#include <stdexcept>
#include <string>

namespace cycfi { namespace elements
{
   namespace detail
   {
      template <typename Pages, typename Tab, typename... RestTabs>
      void link_tabs(view& view_, Pages pages_, std::size_t index, Tab tab_, RestTabs... rest)
      {
         auto* tab = find_element<basic_choice_base*>(tab_.get());
         auto* pages = find_element<deck_element*>(pages_.get());
         if (tab && pages)
         {
            tab->get_sender().on_send(
               [index, pages, &view_](bool state)
               {
                  if (state)
                  {
                     pages->select(index);
                     view_.refresh(*pages);
                  }
               });
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
      auto const num_pages = pages->size();
      auto const num_tabs = sizeof...(RestTabs) + 1;
      if (num_pages != num_tabs)
         throw std::runtime_error{ "Error: The number of pages (" + std::to_string(num_pages) + ") "
            "and tabs (" + std::to_string(num_tabs) + ") do not match." };

      // Select the first tab
      tab->select(true);

      detail::link_tabs(view_, pages, 0, tab, rest...);
   }

   template <typename Pages, typename... Tabs>
   auto vnotebook(view& view_, Pages&& pages, Tabs&&... tabs)
   {
      return detail::vnotebook(
         view_
       , share(std::forward<Pages>(pages))
       , share(std::forward<Tabs>(tabs))...
      );
   }

   template <typename Pages, typename... Tabs>
   auto hnotebook(view& view_, Pages&& pages, Tabs&&... tabs)
   {
      return detail::hnotebook(
         view_
       , share(std::forward<Pages>(pages))
       , share(std::forward<Tabs>(tabs))...
      );
   }
}}

#endif
