/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_COMPOSITE_APRIL_10_2016)
#define PHOTON_GUI_LIB_COMPOSITE_APRIL_10_2016

#include <photon/widget/widget.hpp>
#include <vector>
#include <array>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Composites
   //
   // Class for a widget that is composed of other widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class composite_base : public widget
   {
   public:

   // image

      virtual rect            limits(basic_context const& ctx) const = 0;
      virtual widget*         hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx) = 0;
      virtual bool            scroll(context const& ctx, point p);

   // control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      //virtual bool            key(context const& ctx, key_info const& k);
      virtual bool            text(context const& ctx, text_info const& info);
      virtual bool            cursor(context const& ctx, point p);

      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;
      virtual widget*         focus();
      virtual void            focus(std::size_t index);
      virtual bool            is_control() const;
      virtual void            idle(basic_context const& ctx);

   // composite

      struct hit_info
      {
         widget*   element;
         rect      bounds;
         int       index;
      };

      using iterator = widget_ptr*;
      using const_iterator = widget_ptr const*;

      virtual hit_info        hit_element(context const& ctx, point p) const;
      virtual rect            bounds_of(context const& ctx, std::size_t index) const = 0;

      virtual std::size_t     size() const = 0;
      virtual iterator        begin() = 0;
      virtual const_iterator  begin() const = 0;

      iterator                end()                            { return begin()+size(); }
      const_iterator          end() const                      { return begin()+size(); }
      bool                    empty() const                    { return size() == 0; }
      widget_ptr&             operator[](std::size_t ix)       { return begin()[ix]; }
      widget_ptr const&       operator[](std::size_t ix) const { return begin()[ix]; }

   private:

   	int			            _focus = -1;
      int                     _drag_tracking = -1;
      hit_info                _click_info;
   };

   template <typename Container, typename Base>
   class composite : public Base
   {
   public:

      using iterator = typename Base::iterator;
      using const_iterator = typename Base::const_iterator;

      virtual std::size_t     size() const   { return elements.size(); };
      virtual iterator        begin()        { return &elements[0]; }
      virtual const_iterator  begin() const  { return &elements[0]; }

      Container               elements;
   };

   template <size_t N, typename Base>
   using array_composite = composite<std::array<widget_ptr, N>, Base>;

   template <typename Base>
   using vector_composite = composite<std::vector<widget_ptr>, Base>;
}

#endif