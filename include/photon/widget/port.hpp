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
   class port_base : public proxy_base
   {
   public:

      port_base()
       : _halign(0.0)
       , _valign(0.0)
      {}

      ~port_base() {}

      virtual rect            limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);
      virtual void            draw(context const& ctx);

      double                  halign() const { return _halign; }
      void                    halign(double val) { _halign = val; }
      double                  valign() const { return _valign; }
      void                    valign(double val) { _valign = val; }

   private:

      double                  _halign;
      double                  _valign;
   };

   template <typename Subject>
   inline proxy<typename std::decay<Subject>::type, port_base>
   port(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
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

      static float            width;

      scroller_base(int traits = 0)
       : _tracking(none)
       , _traits(traits)
      {}

      ~scroller_base() {}

      virtual rect            limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            scroll(context const& ctx, point p);
      virtual bool            scroll_into_view(context const& ctx, rect r);
      virtual bool            cursor(context const& ctx, point p);
      virtual bool            is_control() const;

      struct scrollbar_info
      {
         double   pos;
         float    extent;
         rect     bounds;
      };

      virtual void            draw_scroll_bar(theme& thm, scrollbar_info const& info, point mp);
      virtual rect            scroll_bar_position(theme& thm, scrollbar_info const& info);

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
   inline proxy<typename std::decay<Subject>::type, scroller_base>
   scroller(Subject&& subject, int traits = 0)
   {
      return { std::forward<Subject>(subject), traits };
   }
}

#endif