/*=============================================================================
   Copyright (c) 2024 Flole

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_BUSY_BAR_JUNE_13_2024)
#define ELEMENTS_BUSY_BAR_JUNE_13_2024

#include "elements/element/progress_bar.hpp"

namespace cycfi::elements
{
   class busy_bar_base : public progress_bar_base
   {
   public:
                              busy_bar_base(double init_value = 0.0, double start_value = 0.0)
                               : progress_bar_base(init_value), _start(start_value), _status(-0.2)
                              {}

      void                    start(double val);
      double                  start() const { return _start; }
      void                    animate(view& view_, duration time);

      rect                    foreground_bounds(context const& ctx) const override;

   private:
      void                    animate(view& view_);

      double _start;  // Start position
      double _status;
      duration _time;
   };

   class basic_busy_bar_base : public busy_bar_base
   {
   public:
      basic_busy_bar_base(double init_value, double start_value)
               : busy_bar_base(init_value, start_value)
      {}
   };

   namespace concepts
   {
      template <typename T>
      concept BusyBar = std::is_base_of_v<busy_bar_base, std::decay_t<T>>;
   }

   template <
            concepts::Element Background
            , concepts::Element Foreground
            , concepts::BusyBar Base = basic_busy_bar_base
    >
   class basic_busy_bar : public Base
   {
   public:

      using background_type = std::decay_t<Background>;
      using foreground_type = std::decay_t<Foreground>;

      basic_busy_bar(
               Background&& bg
               , Foreground&& fg
               , double init_value
               , double start_value
      )
               : Base(init_value, start_value)
               , _background(std::forward<Foreground>(bg))
               , _foreground(std::forward<Background>(fg))
      {}

      basic_busy_bar(Background const& bg, Foreground const& fg, double init_value, double start_value)
               : Base(init_value, start_value)
               , _foreground(fg)
               , _background(bg)
      {}

         element const&          background() const override { return _background; }
         element&                background()       override { return _background; }
         element const&          foreground() const override { return _foreground; }
         element&                foreground()       override { return _foreground; }

    private:

        background_type         _background;
        foreground_type         _foreground;
    };

    template <concepts::Element Background, concepts::Element Foreground>
    basic_busy_bar<Background, Foreground, basic_busy_bar_base>
    busy_bar(Background&& bg, Foreground&& fg, double init_value = 0.0, double start_value = 0.0)
    {
      return {
               std::forward<Background>(bg),
               std::forward<Foreground>(fg),
               init_value,
               start_value
      };
    }
}

#endif
