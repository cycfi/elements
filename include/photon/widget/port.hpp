/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_PORT_APRIL_24_2016)
#define PHOTON_GUI_LIB_WIDGET_PORT_APRIL_24_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Port widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class port_base
   {
   public:

      port_base()
       : _halign(0.0)
       , _valign(0.0)
      {}

      ~port_base() {}

      virtual widget const&   subject() const = 0;
      virtual widget&         subject() = 0;
      virtual widget const&   self() const = 0;
      virtual widget&         self() = 0;

      virtual rect            limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);
      virtual void            draw(context const& ctx);
      virtual void            proxy_draw(context const& ctx) = 0;

      double                  halign() const { return _halign; }
      void                    halign(double val) { _halign = val; }
      double                  valign() const { return _valign; }
      void                    valign(double val) { _valign = val; }

   private:

      double                  _halign;
      double                  _valign;
   };

   template <typename Subject>
   class port_widget : public proxy<Subject>, public port_base
   {
   public:

      using base_type = proxy<Subject>;

      port_widget(Subject&& subject)
       : base_type(std::move(subject))
      {}

      port_widget(Subject const& subject)
       : base_type(subject)
      {}

      virtual rect            limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);
      virtual void            draw(context const& ctx);
      virtual void            proxy_draw(context const& ctx);

      virtual widget const&   subject() const { return base_type::subject(); }
      virtual widget&         subject() { return base_type::subject(); }
      virtual widget const&   self() const { return *this; }
      virtual widget&         self() { return *this; }
   };

   template <typename Subject>
   inline port_widget<typename std::decay<Subject>::type>
   port(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect
   port_widget<Subject>::limits(basic_context const& ctx) const
   {
      return port_base::limits(ctx);
   }

   template <typename Subject>
   inline void
   port_widget<Subject>::prepare_subject(context& ctx)
   {
      port_base::prepare_subject(ctx);
   }

   template <typename Subject>
   inline void
   port_widget<Subject>::draw(context const& ctx)
   {
      port_base::draw(ctx);
   }

   template <typename Subject>
   inline void
   port_widget<Subject>::proxy_draw(context const& ctx)
   {
      base_type::draw(ctx);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   enum
   {
      no_scrollbars  = 1,
      no_hscroll     = 1 << 1,
      no_vscroll     = 1 << 2
   };

   class scroller_base : public port_base, public scrollable
   {
   public:

      scroller_base(int traits = 0)
       : _tracking(none)
       , _traits(traits)
      {}

      ~scroller_base() {}

      virtual rect            limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);
      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            scroll(context const& ctx, point const& p);
      virtual bool            cursor(context const& ctx, point const& p);
      virtual bool            is_control() const;

      virtual bool            scroll_into_view(context const& ctx, rect const& r);
      virtual widget*         proxy_click(context const& ctx, mouse_button btn) = 0;
      virtual void            proxy_drag(context const& ctx, mouse_button btn) = 0;
      virtual bool            proxy_cursor(context const& ctx, point const& p) = 0;

   private:

      struct scrollbar_bounds
      {
         rect vscroll_bounds;
         rect hscroll_bounds;
         bool has_v = false;
         bool has_h = false;
      };

      enum tracking_status
      {
         none,
         start,
         tracking_v,
         tracking_h
      };

      scrollbar_bounds  get_scrollbar_bounds(context const& ctx);
      bool              reposition(context const& ctx);

      bool              has_scrollbars() const { return !(_traits & no_scrollbars); }
      bool              allow_hscroll() const { return !(_traits & no_hscroll); }
      bool              allow_vscroll() const { return !(_traits & no_vscroll); }

      point             _offset;
      tracking_status   _tracking;
      int               _traits;
   };

   template <typename Subject>
   class scroller_widget : public proxy<Subject>, public scroller_base
   {
   public:

      using base_type = proxy<Subject>;

      scroller_widget(Subject&& subject)
       : base_type(std::move(subject))
      {}

      scroller_widget(Subject const& subject)
       : base_type(subject)
      {}

      virtual rect            limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);
      virtual void            draw(context const& ctx);
      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            scroll(context const& ctx, point const& p);
      virtual bool            cursor(context const& ctx, point const& p);
      virtual bool            is_control() const;

      virtual void            proxy_draw(context const& ctx);
      virtual widget*         proxy_click(context const& ctx, mouse_button btn);
      virtual void            proxy_drag(context const& ctx, mouse_button btn);
      virtual bool            proxy_cursor(context const& ctx, point const& p);

      virtual widget const&   subject() const { return base_type::subject(); }
      virtual widget&         subject() { return base_type::subject(); }
      virtual widget const&   self() const { return *this; }
      virtual widget&         self() { return *this; }
   };

   template <typename Subject>
   inline rect
   scroller_widget<Subject>::limits(basic_context const& ctx) const
   {
      return scroller_base::limits(ctx);
   }

   template <typename Subject>
   inline void
   scroller_widget<Subject>::prepare_subject(context& ctx)
   {
      port_base::prepare_subject(ctx);
   }

   template <typename Subject>
   inline void
   scroller_widget<Subject>::draw(context const& ctx)
   {
      scroller_base::draw(ctx);
   }

   template <typename Subject>
   inline widget*
   scroller_widget<Subject>::click(context const& ctx, mouse_button btn)
   {
      return scroller_base::click(ctx, btn);
   }

   template <typename Subject>
   inline void
   scroller_widget<Subject>::drag(context const& ctx, mouse_button btn)
   {
      scroller_base::drag(ctx, btn);
   }

   template <typename Subject>
   inline bool
   scroller_widget<Subject>::scroll(context const& ctx, point const& p)
   {
      return scroller_base::scroll(ctx, p);
   }

   template <typename Subject>
   inline bool
   scroller_widget<Subject>::cursor(context const& ctx, point const& p)
   {
      return scroller_base::cursor(ctx, p);
   }

   template <typename Subject>
   inline bool
   scroller_widget<Subject>::is_control() const
   {
      return scroller_base::is_control();
   }

   template <typename Subject>
   inline void
   scroller_widget<Subject>::proxy_draw(context const& ctx)
   {
      base_type::draw(ctx);
   }

   template <typename Subject>
   inline widget*
   scroller_widget<Subject>::proxy_click(context const& ctx, mouse_button btn)
   {
      return base_type::click(ctx, btn);
   }

   template <typename Subject>
   inline void
   scroller_widget<Subject>::proxy_drag(context const& ctx, mouse_button btn)
   {
      base_type::drag(ctx, btn);
   }

   template <typename Subject>
   inline bool
   scroller_widget<Subject>::proxy_cursor(context const& ctx, point const& p)
   {
      return base_type::cursor(ctx, p);
   }

   template <typename Subject>
   inline scroller_widget<typename std::decay<Subject>::type>
   scroller(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }
}

#endif