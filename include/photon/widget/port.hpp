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

      rect                    limits(basic_context const& ctx) const;
      void                    prepare_subject(context& ctx);
      void                    draw(context const& ctx);

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

      virtual widget const&   subject() const { return base_type::subject(); }
      virtual widget&         subject() { return base_type::subject(); }
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
      base_type::draw(ctx);
   }

   //////////////////////////////////////////////////////////////////////////////////////////////////
   //enum
   //{
   //   no_scrollbars  = 1,
   //   no_hscroll     = 1 << 1,
   //   no_vscroll     = 1 << 2
   //};
   //
   //class scroller_widget : public port_widget, public scrollable
   //{
   //public:
   //
   //   scroller_widget(std::shared_ptr<widget> subject, int traits = 0)
   //    : port_widget(subject)
   //    , _tracking(none)
   //    , _traits(traits)
   //   {}
   //
   //   ~scroller_widget() {}
   //
   //   virtual rect      limits(basic_context const& ctx) const;
   //   virtual void      draw(context const& ctx);
   //   virtual widget*   click(context const& ctx, mouse_button btn);
   //   virtual void      drag(context const& ctx, mouse_button btn);
   //   virtual bool      scroll(context const& ctx, point const& p);
   //   virtual bool      scroll_into_view(context const& ctx, rect const& r);
   //   virtual bool      cursor(context const& ctx, point const& p);
   //   virtual bool      is_control() const;
   //
   //private:
   //
   //   struct scrollbar_bounds
   //   {
   //      rect vscroll_bounds;
   //      rect hscroll_bounds;
   //      bool has_v = false;
   //      bool has_h = false;
   //   };
   //
   //   enum tracking_status
   //   {
   //      none,
   //      start,
   //      tracking_v,
   //      tracking_h
   //   };
   //
   //   scrollbar_bounds  get_scrollbar_bounds(context const& ctx);
   //   bool              reposition(context const& ctx);
   //
   //   bool              has_scrollbars() const { return !(_traits & no_scrollbars); }
   //   bool              allow_hscroll() const { return !(_traits & no_hscroll); }
   //   bool              allow_vscroll() const { return !(_traits & no_vscroll); }
   //
   //   point             _offset;
   //   tracking_status   _tracking;
   //   int               _traits;
   //};
   //
   //inline widget_ptr scroller(std::shared_ptr<widget> subject, int traits = 0)
   //{
   //   return widget_ptr{ new scroller_widget{ subject, traits } };
   //}
}

#endif