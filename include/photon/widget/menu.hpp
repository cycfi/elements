/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_MENU_JUNE_4_2016)
#define PHOTON_GUI_LIB_WIDGET_MENU_JUNE_4_2016

#include <photon/widget/widget.hpp>
#include <photon/widget/button.hpp>
#include <photon/widget/floating.hpp>
#include <photon/window.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Dropdown Menu
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_dropdown_menu : public basic_button
   {
   public:
                           template <typename W1, typename W2>
                           basic_dropdown_menu(W1&& off, W2&& on);

      virtual widget*      click(context const& ctx, mouse_button btn);
      virtual void         drag(context const& ctx, mouse_button btn);

                           template <typename Menu>
      void                 menu(Menu&& menu_);

   private:

      widget_ptr           _menu;
      bool                 _is_active;
   };

   template <typename W1, typename W2>
   inline basic_dropdown_menu::basic_dropdown_menu(W1&& off, W2&& on)
    : basic_button(std::forward<W1>(off), std::forward<W2>(on))
    , _is_active(false)
   {}

   template <typename Menu>
   inline void basic_dropdown_menu::menu(Menu&& menu_)
   {
      _menu = new_(floating({0, 0, 0, 0}, menu_));
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Menu Background
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class menu_background : public widget
   {
   public:

      static rect          shadow_offset;
      virtual void         draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Basic Menu Items
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class basic_menu_item : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;
      using menu_item_function = std::function<void()>;

                           basic_menu_item(Subject&& subject);
                           basic_menu_item(Subject const& subject);

      virtual void         draw(context const& ctx);
      virtual widget*      click(context const& ctx, mouse_button btn);
      virtual bool         cursor(context const& ctx, point p);
      virtual bool         is_control() const;

      menu_item_function   on_click;
   };

   void highlight_menu_item(context const& ctx);

   template <typename Subject>
   inline basic_menu_item<Subject>::basic_menu_item(Subject&& subject)
    : base_type(std::forward<Subject>(subject))
   {}

   template <typename Subject>
   inline basic_menu_item<Subject>::basic_menu_item(Subject const& subject)
    : base_type(subject)
   {}

   template <typename Subject>
   inline void basic_menu_item<Subject>::draw(context const& ctx)
   {
      highlight_menu_item(ctx);
      base_type::draw(ctx);
   }

   template <typename Subject>
   inline widget* basic_menu_item<Subject>::click(context const& ctx, mouse_button btn)
   {
      if (!btn.is_pressed && on_click)
         on_click();
      return base_type::click(ctx, btn);
   }

   template <typename Subject>
   inline bool basic_menu_item<Subject>::cursor(context const& ctx, point p)
   {
      if (ctx.bounds.includes(p))
         ctx.window.draw();
      return base_type::cursor(ctx, p);
   }

   template <typename Subject>
   inline bool basic_menu_item<Subject>::is_control() const
   {
      return true;
   }
}

#endif
