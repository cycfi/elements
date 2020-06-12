/*=============================================================================
   Copyright (c) 2020 Michał Urbański

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_PROGRESS_BAR_JANUARY_20_2020)
#define ELEMENTS_PROGRESS_BAR_JANUARY_20_2020

#include <elements/element/element.hpp>
#include <type_traits>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Progress Bar
   ////////////////////////////////////////////////////////////////////////////
   class progress_bar_base : public element, public receiver<double>
   {
   public:
                              progress_bar_base(double init_value)
                               : _value(init_value)
                              {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;

      double                  value() const override { return _value; }
      void                    value(double val) override;

      rect                    background_bounds(context const& ctx) const;
      rect                    foreground_bounds(context const& ctx) const;

      virtual element const&  background() const = 0;
      virtual element&        background()       = 0;
      virtual element const&  foreground() const = 0;
      virtual element&        foreground()       = 0;

   private:

      double                  _value;
      bool                    _is_horiz = true;
   };

   class basic_progress_bar_base : public progress_bar_base
   {
   public:
                              basic_progress_bar_base(double init_value)
                               : progress_bar_base(init_value)
                              {}
   };

   template <typename Background, typename Foreground, typename Base = basic_progress_bar_base>
   class basic_progress_bar : public Base
   {
   public:

      static_assert(std::is_base_of_v<element, Background>,
         "basic_progress_bar_base Background type needs to be or inherit from element");
      static_assert(std::is_base_of_v<element, Foreground>,
         "basic_progress_bar_base Foreground type needs to be or inherit from element");
      static_assert(std::is_base_of_v<basic_progress_bar_base, Base>,
         "basic_progress_bar_base Base type needs to be or inherit from basic_progress_bar_base");

      using background_type = std::decay_t<Background>;
      using foreground_type = std::decay_t<Foreground>;

                              basic_progress_bar(Background&& bg, Foreground&& fg, double init_value)
                               : Base(init_value)
                               , _background(std::forward<Foreground>(bg))
                               , _foreground(std::forward<Background>(fg))
                              {}

                              basic_progress_bar(Background const& bg, Foreground const& fg, double init_value)
                               : Base(init_value)
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

   template <typename Background, typename Foreground>
   basic_progress_bar<Background, Foreground, basic_progress_bar_base>
   progress_bar(Background&& bg, Foreground&& fg, double init_value = 0.0)
   {
      return {
         std::forward<Background>(bg),
         std::forward<Foreground>(fg),
         init_value
      };
   }
}

#endif
