/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_BUTTON_APRIL_21_2016)
#define ELEMENTS_BUTTON_APRIL_21_2016

#include <elements/element/layer.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/selection.hpp>
#include <elements/element/traversal.hpp>
#include <elements/support/context.hpp>
#include <elements/support/sender.hpp>
#include <elements/view.hpp>
#include <type_traits>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic Button
   ////////////////////////////////////////////////////////////////////////////
   struct button_state
   {
                        button_state()
                         : value{false}
                         , hilite{false}
                         , tracking{false}
                         , enabled{true}
                        {}

      bool              value : 1;
      bool              hilite : 1;
      bool              tracking : 1;
      bool              enabled : 1;
   };

   class basic_button
    : public proxy_base
    , public receiver<bool>
    , public sender<bool>
   {
   public:

      using button_function = std::function<void(bool)>;

      bool              wants_control() const override;
      bool              click(context const& ctx, mouse_button btn) override;
      bool              cursor(context const& ctx, point p, cursor_tracking status) override;
      void              drag(context const& ctx, mouse_button btn) override;
      element*          hit_test(context const& ctx, point p, bool leaf, bool control) override;

      void              enable(bool state = true) override;
      bool              is_enabled() const override;

      void              value(bool val) override;
      bool              value() const override  { return _state.value; }
      bool              tracking() const        { return _state.tracking; }
      bool              hilite() const          { return _state.hilite; }

      void              send(bool val) override;
      void              edit(view& view_, bool val) override;
      void              on_send(callback_function f) override;

      button_function   on_click;

   protected:

      bool              state(bool val);
      void              tracking(bool val);
      void              hilite(bool val);

   private:

      bool              update_receiver();

      button_state      _state;
   };

   inline void basic_button::edit(view& view_, bool val)
   {
      send(val);
      receiver<bool>::notify_edit(view_);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Toggle Button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   class basic_toggle_button : public Base
   {
   public:

      using Base::Base;

      bool              click(context const& ctx, mouse_button btn) override;
      void              drag(context const& ctx, mouse_button btn) override;

   private:

      bool              _current_state;
   };

   template <typename Base>
   inline bool basic_toggle_button<Base>::click(context const& ctx, mouse_button btn)
   {
      if (!ctx.enabled || !this->is_enabled())
         return false;

      if (btn.state != mouse_button::left || !ctx.bounds.includes(btn.pos))
      {
         this->tracking(false);
         ctx.view.refresh(ctx);
         return false;
      }

      if (btn.down)
      {
         this->tracking(true);
         if (this->state(!this->value()))    // toggle the state
         {
            ctx.view.refresh(ctx);           // we need to save the current state, the state
            _current_state = this->value();  // can change in the drag function and so we'll
         }                                   // need it later when the button is finally released
      }
      else
      {
         this->tracking(false);
         this->state(_current_state);
         if (this->on_click)
            this->on_click(this->value());
         ctx.view.refresh(ctx);
      }
      return true;
   }

   template <typename Base>
   inline void basic_toggle_button<Base>::drag(context const& ctx, mouse_button btn)
   {
      this->hilite(ctx.bounds.includes(btn.pos));
      if (this->state(!_current_state ^ ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Latching Button
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base>
   class basic_latching_button : public Base
   {
   public:

      using Base::Base;

      bool              click(context const& ctx, mouse_button btn) override;
   };

   template <typename Base>
   inline bool basic_latching_button<Base>::click(context const& ctx, mouse_button btn)
   {
      if (btn.down && this->value())
         return false;

      if (btn.state != mouse_button::left || !ctx.bounds.includes(btn.pos))
      {
         this->tracking(false);
         ctx.view.refresh(ctx);
         return false;
      }

      if (btn.down)
      {
         this->tracking(true);
         this->on_tracking(ctx, this->begin_tracking);
      }
      else
      {
         this->tracking(false);
         this->on_tracking(ctx, this->end_tracking);
         if (this->on_click)
            this->on_click(true);
         ctx.view.refresh(ctx);
      }
      if (btn.down && this->state(ctx.bounds.includes(btn.pos)))
         ctx.view.refresh(ctx);
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Choice
   ////////////////////////////////////////////////////////////////////////////
   struct basic_choice_base : public selectable
   {
      virtual sender<bool>&   get_sender() = 0;
      void                    do_click(context const& ctx);
   };

   template <typename Base>
   class basic_choice : public basic_latching_button<Base>, public basic_choice_base
   {
   public:

      using basic_latching_button<Base>::basic_latching_button;

      void              select(bool state) override;
      bool              is_selected() const override;
      bool              click(context const& ctx, mouse_button btn) override;
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
   bool basic_choice<Base>::click(context const& ctx, mouse_button btn)
   {
      if (btn.state == mouse_button::left)
      {
         if (btn.down)
         {
            return basic_latching_button<Base>::click(ctx, btn);
         }
         else
         {
            auto r = basic_latching_button<Base>::click(ctx, btn);
            if (this->value())
               this->do_click(ctx);
            return r;
         }
      }
      return false;
   }

   template <typename Subject>
   inline basic_choice<proxy<Subject, basic_button>>
   choice(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }
}

#endif
