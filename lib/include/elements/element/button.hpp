/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_BUTTON_APRIL_21_2016)
#define ELEMENTS_BUTTON_APRIL_21_2016

#include <elements/element/layer.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/selectable.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/context.hpp>
#include <elements/support/sender.hpp>
#include <elements/view.hpp>
#include <type_traits>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Button
   ////////////////////////////////////////////////////////////////////////////
   class basic_button : public proxy_base, public receiver<bool>, public sender<bool>
   {
   public:

      using button_function = std::function<void(bool)>;

                        basic_button()
                         : _state(false)
                         , _hilite(false)
                        {}

      bool              wants_control() const override;
      element*          click(context const& ctx, mouse_button btn) override;
      bool              cursor(context const& ctx, point p, cursor_tracking status) override;
      void              drag(context const& ctx, mouse_button btn) override;

      void              value(bool new_state) override;
      bool              value() const override;

      void              send(bool val) override;
      void              on_send(callback_function f) override;
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
   class layered_button
    : public array_composite<2, deck_element>, public receiver<bool>, public sender<bool>
   {
   public:

      using base_type = array_composite<2, deck_element>;
      using button_function = std::function<void(bool)>;

                        template <typename W1, typename W2>
                        layered_button(W1&& off, W2&& on);

      bool              wants_control() const override;
      element*          hit_test(context const& ctx, point p) override;
      element*          click(context const& ctx, mouse_button btn) override;
      void              drag(context const& ctx, mouse_button btn) override;

      void              value(bool new_state) override;
      bool              value() const override;

      void              send(bool val) override;
      void              on_send(callback_function f) override;
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

      element*          click(context const& ctx, mouse_button btn) override;
      void              drag(context const& ctx, mouse_button btn) override;

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

      element*          click(context const& ctx, mouse_button btn) override;
      void              drag(context const& ctx, mouse_button btn) override;
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
      if (this->value() || !ctx.bounds.includes(btn.pos))
         return nullptr;
      if (btn.down)
      {
         Base::click(ctx, btn);
         if (this->value() && this->on_click)
            this->on_click(true);
      }
      else if (this->on_click)
         this->on_click(true);
      return this;
   }

   template <typename Base>
   inline void basic_latching_button<Base>::drag(context const& /* ctx */, mouse_button /* btn */)
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Choice
   ////////////////////////////////////////////////////////////////////////////
   struct basic_choice_base : public selectable
   {
      virtual sender<bool>&   get_sender() = 0;
      void                    do_click(context const& ctx, bool val, bool was_selected);
   };

   template <typename Base = layered_button>
   class basic_choice : public basic_latching_button<Base>, public basic_choice_base
   {
   public:

      using basic_latching_button<Base>::basic_latching_button;

      void              select(bool state) override;
      bool              is_selected() const override;
      element*          click(context const& ctx, mouse_button btn) override;
      sender<bool>&     get_sender() override { return *this; }
   };

   template <typename Base>
   inline bool basic_choice<Base>::is_selected() const
   {
      return this->value();
   }

   template <typename Base>
   inline void basic_choice<Base>::select(bool state)
   {
      if (state != is_selected())
         this->value(state);
   }

   template <typename Base>
   element* basic_choice<Base>::click(context const& ctx, mouse_button btn)
   {
      bool was_selected = is_selected();
      auto r = basic_latching_button<Base>::click(ctx, btn);
      this->do_click(ctx, this->value(), was_selected);
      return r;
   }

   template <typename Subject>
   inline basic_choice<proxy<Subject, basic_button>>
   choice(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }
}}

#endif
