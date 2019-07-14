/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_ELEMENTS_GUI_LIB_WIDGET_BUTTON_APRIL_21_2016)
#define CYCFI_ELEMENTS_GUI_LIB_WIDGET_BUTTON_APRIL_21_2016

#include <elements/element/layer.hpp>
#include <elements/element/proxy.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>
#include <functional>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Button
   ////////////////////////////////////////////////////////////////////////////
   class basic_button : public proxy_base
   {
   public:

      using button_function = std::function<void(bool)>;
      using proxy_base::value;

                        basic_button()
                         : _state(false)
                         , _hilite(0)
                        {}

      virtual element*  click(context const& ctx, mouse_button btn);
      virtual bool      cursor(context const& ctx, point p, cursor_tracking status);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      is_control() const;

      virtual void      value(int new_state);
      virtual void      value(bool new_state);
      bool              value() const;

      button_function   on_click;

   protected:

      bool              state(bool new_state);

   private:

      bool              _state : 1;
      bool              _hilite : 1;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Layered Button
   ////////////////////////////////////////////////////////////////////////////
   class layered_button : public array_composite<2, deck_element>
   {
   public:

      using base_type = array_composite<2, deck_element>;
      using button_function = std::function<void(bool)>;
      using base_type::value;

                        template <typename W1, typename W2>
                        layered_button(W1&& off, W2&& on);

      virtual element*  hit_test(context const& ctx, point p) override;
      virtual element*  click(context const& ctx, mouse_button btn) override;
      virtual void      drag(context const& ctx, mouse_button btn) override;
      virtual bool      is_control() const override;

      virtual void      value(int new_state) override;
      virtual void      value(bool new_state) override;
      bool              value() const;

      button_function   on_click;

   protected:

      bool              state(bool new_state);

   private:

      bool              _state;
   };

   template <typename W1, typename W2>
   inline layered_button::layered_button(W1&& off, W2&& on)
    : _state(false)
   {
      (*this)[0] = share(std::forward<W1>(off));
      (*this)[1] = share(std::forward<W2>(on));
   }

   ////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = layered_button>
   class basic_toggle_button : public Base
   {
   public:
                        template <typename W1>
                        basic_toggle_button(W1&& state);

                        template <typename W1, typename W2>
                        basic_toggle_button(W1&& off, W2&& on);

      virtual element*  click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);

   private:

      bool              _current_state;
   };

   template <typename Base>
   template <typename W1>
   inline basic_toggle_button<Base>::basic_toggle_button(W1&& state)
    : Base(std::forward<W1>(state))
    , _current_state(false)
   {}

   template <typename Base>
   template <typename W1, typename W2>
   inline basic_toggle_button<Base>::basic_toggle_button(W1&& off, W2&& on)
    : Base(std::forward<W1>(off), std::forward<W2>(on))
    , _current_state(false)
   {}

   template <typename Base>
   inline element* basic_toggle_button<Base>::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(btn.pos))
      {
         ctx.view.refresh(ctx);
         return 0;
      }

      if (btn.down)
      {
         if (this->state(!this->value()))    // toggle the state
         {
            ctx.view.refresh(ctx);           // we need to save the current state, the state
            _current_state = this->value();  // can change in the drag function and so we'll
         }                                   // need it later when the button is finally released
      }
      else
      {
         this->state(_current_state);
         if (this->on_click)
            this->on_click(this->value());
      }
      return this;
   }

   template <typename Base>
   inline void basic_toggle_button<Base>::drag(context const& ctx, mouse_button btn)
   {
      if (this->state(!_current_state ^ ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Latching Button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = layered_button>
   class basic_latching_button : public Base
   {
   public:
                        template <typename W1>
                        basic_latching_button(W1&& state);

                        template <typename W1, typename W2>
                        basic_latching_button(W1&& off, W2&& on);

      virtual element*  click(context const& ctx, mouse_button btn);
   };

   template <typename Base>
   template <typename W1>
   inline basic_latching_button<Base>::basic_latching_button(W1&& state)
    : Base(std::forward<W1>(state))
   {}

   template <typename Base>
   template <typename W1, typename W2>
   inline basic_latching_button<Base>::basic_latching_button(W1&& off, W2&& on)
    : Base(std::forward<W1>(off), std::forward<W2>(on))
   {}

   template <typename Base>
   inline element* basic_latching_button<Base>::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.bounds.includes(btn.pos))
         return 0;
      if (btn.down)
         return layered_button::click(ctx, btn);
      else if (this->on_click)
         this->on_click(true);
      return this;
   }
}}

#endif