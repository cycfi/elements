/*=============================================================================
   Copyright (c) 2020 Michał Urbański
   Copyright (c) 2024 Flole
   Copyright (c) 2024 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STATUS_BAR_JANUARY_20_2020)
#define ELEMENTS_STATUS_BAR_JANUARY_20_2020

#include <elements/element/element.hpp>
#include <elements/support/receiver.hpp>
#include <type_traits>

namespace cycfi::elements
{
   class status_bar_base : public element, public receiver<double>
   {
   public:
                              status_bar_base(double init_value)
                               : _value(init_value)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;

      double                  value() const override { return _value; }
      void                    value(double val) override;

      virtual rect            background_bounds(context const& ctx) const;
      virtual rect            foreground_bounds(context const& ctx) const;

      virtual element const&  background() const = 0;
      virtual element&        background() = 0;
      virtual element const&  foreground() const = 0;
      virtual element&        foreground() = 0;

   private:

      double                  _value;
   };

   namespace concepts
   {
      template <typename T>
      concept StatusBar = std::is_base_of_v<status_bar_base, std::decay_t<T>>;
   }

   template <
      concepts::Element Background
    , concepts::Element Foreground
    , concepts::StatusBar Base = status_bar_base>
   class basic_status_bar : public Base
   {
   public:

      using background_type = std::decay_t<Background>;
      using foreground_type = std::decay_t<Foreground>;

                              template <typename... Args>
                              basic_status_bar(
                                 Background&& bg
                               , Foreground&& fg
                               , Args&& ...args
                              )
                               : Base(args...)
                               , _background(std::forward<Foreground>(bg))
                               , _foreground(std::forward<Background>(fg))
                              {}

      element const&          background() const override { return _background; }
      element&                background() override { return _background; }
      element const&          foreground() const override { return _foreground; }
      element&                foreground() override { return _foreground; }

   private:

      background_type         _background;
      foreground_type         _foreground;
   };

   template <concepts::Element Background, concepts::Element Foreground>
   inline basic_status_bar<Background, Foreground, status_bar_base>
   progress_bar(Background&& bg, Foreground&& fg, double init_value = 0.0)
   {
      return {
         std::forward<Background>(bg),
         std::forward<Foreground>(fg),
         init_value
      };
   }

   class busy_bar_base : public status_bar_base
   {
   public:
                              busy_bar_base(
                                 double init_value = 0.0
                               , double start_pos = 0.0
                              );

      void                    start_pos(double val);
      double                  start_pos() const { return _start_pos; }

      void                    animation_width(double val);
      double                  animation_width() const { return _animation_width; }

      void                    start(view& view_, duration time);
      void                    stop(view& view_);
      bool                    is_stopped() const { return _time == _time.zero(); }

      rect                    foreground_bounds(context const& ctx) const override;

   private:

      void                    animate(view& view_);

      double                  _start_pos;
      double                  _animation_width;
      double                  _status;
      duration                _time;
   };

   template <concepts::Element Background, concepts::Element Foreground>
   inline basic_status_bar<Background, Foreground, busy_bar_base>
   busy_bar(
      Background&& bg
    , Foreground&& fg
    , double init_value = 0.0
    , double start_pos = 0.0
   )
   {
      return {
         std::forward<Background>(bg),
         std::forward<Foreground>(fg),
         init_value, start_pos
      };
   }
}

#endif
