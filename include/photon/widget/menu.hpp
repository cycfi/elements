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

      virtual void         layout(context const& ctx);
      virtual widget*      click(context const& ctx, mouse_button btn);
      virtual void         drag(context const& ctx, mouse_button btn);
      virtual bool         key(context const& ctx, key_info const& k);
      virtual bool         focus(focus_request r);

                           template <typename Menu>
      void                 menu(Menu&& menu_);

   private:

      friend class menu_backdrop_widget;

      widget_ptr           _menu;
      bool                 _is_active;
   };

   template <typename W1, typename W2>
   inline basic_dropdown_menu::basic_dropdown_menu(W1&& off, W2&& on)
    : basic_button(std::forward<W1>(off), std::forward<W2>(on))
    , _is_active(false)
   {}

   class menu_backdrop_widget : public floating_widget
   {
   public:
                           menu_backdrop_widget(rect bounds)
                            : floating_widget(bounds)
                            , _owner(0)
                           {}

      virtual widget*      hit_test(context const& ctx, point p);
      virtual widget*      click(context const& ctx, mouse_button btn);

      basic_dropdown_menu* owner() const                       { return _owner; }
      void                 owner(basic_dropdown_menu* owner_)  { _owner = owner_; }

   private:

      basic_dropdown_menu* _owner;
   };

   template <typename Subject>
   inline proxy<typename std::decay<Subject>::type, menu_backdrop_widget>
   menu_backdrop(rect bounds, Subject&& subject)
   {
      return { std::forward<Subject>(subject), bounds };
   }

   template <typename Menu>
   inline void basic_dropdown_menu::menu(Menu&& menu_)
   {
      _menu = new_(menu_backdrop({0, 0, 0, 0}, menu_));
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
   class basic_menu_item_widget : public proxy_base
   {
   public:

      using menu_item_function = std::function<void()>;

      virtual void         draw(context const& ctx);
      virtual widget*      click(context const& ctx, mouse_button btn);
      virtual bool         cursor(context const& ctx, point p);
      virtual bool         is_control() const;

      menu_item_function   on_click;
   };

   template <typename Subject>
   inline proxy<typename std::decay<Subject>::type, basic_menu_item_widget>
   basic_menu_item(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }
}

#endif
