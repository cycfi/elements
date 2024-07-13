/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_CHILD_WINDOW_JANUARY_19_2021)
#define ELEMENTS_STYLE_CHILD_WINDOW_JANUARY_19_2021

#include <elements/element/child_window.hpp>
#include <elements/element/tile.hpp>
#include <elements/element/style/pane.hpp>
#include <elements/element/style/icon_button.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Child Window (movable, non-closable)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   auto basic_child_window(std::string title, rect bounds, Content&& content)
   {
      return child_window(bounds,
         pane_ex(
            movable(title_bar{}),
            title,
            std::forward<Content>(content),
            get_theme().child_window_title_size,
            get_theme().child_window_opacity
         )
      );
   }

   template <typename Content>
   auto basic_child_window(std::string title, point pos, Content&& content)
   {
      return basic_child_window(
         title,
         rect{pos.x, pos.y, pos.x, pos.y},
         std::forward<Content>(content)
      );
   }

   ////////////////////////////////////////////////////////////////////////////
   // Closable Child Window (movable and closable)
   ////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   [[deprecated("Use standard_child_window(...) instead")]]
   auto closable_child_window(std::string title, rect bounds, Content&& content)
   {
      return child_window(bounds,
         pane_ex(
            movable(
               layer(
                  closable(
                     align_right_middle(
                        plain_icon_button(icons::cancel, 0.8)
                     )
                  ),
                  title_bar{}
               )
            ),
            title,
            std::forward<Content>(content),
            get_theme().child_window_title_size,
            get_theme().child_window_opacity
         )
      );
   }

   template <typename Content>
   [[deprecated("Use standard_child_window(...) instead")]]
   auto closable_child_window(std::string title, point pos, Content&& content)
   {
      return closable_child_window(
         title,
         rect{pos.x, pos.y, pos.x, pos.y},
         std::forward<Content>(content)
      );
   }

   enum : unsigned char
   {
      window_closable       = 1,
      window_minimizable    = 2,
      window_maximizable    = 4,
   };

   template <typename Content>
   auto standard_child_window(
      std::string title,
      rect bounds, Content&& content,
      unsigned char options = window_closable | window_minimizable | window_maximizable
   )
   {
      auto buttons = htile_composite{};
      if (options & window_minimizable)
      {
         buttons.push_back(
            share(minimizable(plain_icon_button(icons::angle_down, 0.8)))
         );
      }
      if (options & window_maximizable)
      {
         buttons.push_back(
            share(maximizable(plain_icon_button(icons::angle_up, 0.8)))
         );
      }
      if (options & window_closable)
      {
         buttons.push_back(
            share(closable(plain_icon_button(icons::cancel, 0.8)))
         );
      }
      return child_window(bounds,
         resizable(
            pane_ex(
               movable(
                  layer(
                     align_right(buttons),
                     title_bar{}
                  )
               ),
               title,
               std::forward<Content>(content),
               get_theme().child_window_title_size,
               get_theme().child_window_opacity
            )
         )
      );
   }

   template <typename Content>
   auto standard_child_window(std::string title, point pos, Content&& content)
   {
      return standard_child_window(
         title,
         rect{pos.x, pos.y, pos.x, pos.y},
         std::forward<Content>(content)
      );
   }
}

#endif
