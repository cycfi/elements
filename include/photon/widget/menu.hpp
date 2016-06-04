/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_MENU_JUNE_4_2016)
#define PHOTON_GUI_LIB_WIDGET_MENU_JUNE_4_2016

#include <photon/widget/widget.hpp>
#include <photon/widget/margin.hpp>
#include <photon/widget/align.hpp>
#include <photon/widget/text.hpp>
#include <photon/canvas.hpp>

namespace photon
{
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

   inline auto menu_item_text(std::string const& text)
   {
      return xside_margin({ 20, 20 },  align_left(heading(text)));
   }

   template <typename Subject>
   inline basic_menu_item<typename std::decay<Subject>::type>
   make_basic_menu_item(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   inline auto menu_item(std::string const& text)
   {
      return make_basic_menu_item(menu_item_text(text));
   }

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
      if (ctx.bounds.includes(ctx.cursor_pos()))
      {
         auto& canvas_ = ctx.canvas();

         canvas_.begin_path();
         canvas_.round_rect(ctx.bounds, 2);
         canvas_.fill_color(ctx.theme().indicator_color.opacity(0.6));
         canvas_.fill();
      }
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
