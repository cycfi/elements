/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_BORDER_APRIL_11_2016)
#define ELEMENTS_STYLE_BORDER_APRIL_11_2016

#include <elements/element.hpp>

namespace cycfi::elements
{
   /**
    * \struct border
    * \brief
    *    A generic border element that renders a border around the bounds of
    *    the element.
    */
   struct border : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_left
    *
    * \brief
    *    A generic border element that renders a border along the left side
    *    of the element's bounds.
    */
   struct border_left : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_right
    *
    * \brief
    *    A generic border element that renders a border along the right side
    *    of the element's bounds.
    */
   struct border_right : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_top
    *
    * \brief
    *    A generic border element that renders a border along the top side of
    *    the element's bounds.
    */
   struct border_top : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_bottom
    *
    * \brief
    *    A generic border element that renders a border along the bottom side
    *    of the element's bounds.
    */
   struct border_bottom : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_bottom
    *
    * \brief
    *    A generic border element that renders a border along the top and
    *    bottom sides of the element's bounds.
    */
   struct border_top_bottom : public element
   {
      void           draw(context const& ctx) override;
   };

   /**
    * \struct border_left_right
    *
    * \brief
    *    A generic border element that renders a border along the left and
    *    right sides of the element's bounds.
    */
   struct border_left_right : public element
   {
      void           draw(context const& ctx) override;
   };
}

#endif
