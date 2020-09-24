/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_PORT_APRIL_24_2016)
#define ELEMENTS_PORT_APRIL_24_2016

#include <elements/element/proxy.hpp>
#include <infra/support.hpp>
#include <memory>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Port elements
   ////////////////////////////////////////////////////////////////////////////
   class port_base : public proxy_base
   {
   public:

      void                    draw(context const& ctx) override;

      virtual double          halign() const = 0;
      virtual void            halign(double val) = 0;
      virtual double          valign() const = 0;
      virtual void            valign(double val) = 0;
   };

   class port_element : public port_base
   {
   public:
                              port_element()
                               : _halign(0.0)
                               , _valign(0.0)
                              {}

                              ~port_element() {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      double                  halign() const override       { return _halign; }
      void                    halign(double val) override   { _halign = val; }
      double                  valign() const override       { return _valign; }
      void                    valign(double val) override   { _valign = val; }

   private:

      double                  _halign;
      double                  _valign;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, port_element>
   port(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   class vport_element : public port_base
   {
   public:
                              vport_element()
                               : _valign(0.0)
                              {}

                              ~vport_element() {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      double                  halign() const override             { return 0; }
      void                    halign(double /*val*/) override     {}
      double                  valign() const override             { return _valign; }
      void                    valign(double val) override         { _valign = val; }

   private:

      double                  _valign;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, vport_element>
   vport(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   class hport_element : public port_base
   {
   public:
                              hport_element()
                               : _halign(0.0)
                              {}

                              ~hport_element() {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

      double                  halign() const override             { return _halign; }
      void                    halign(double val) override         { _halign = val; }
      double                  valign() const override             { return 0; }
      void                    valign(double /*val*/) override     {}

   private:

      double                  _halign;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, hport_element>
   hport(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Scrollers
   ////////////////////////////////////////////////////////////////////////////

   // scrollable: Mixin class for a element that is scrollable
   // scroll the rectangle, r into view
   class scrollable
   {
   public:
      virtual ~scrollable() = default;

      struct scrollable_context
      {
         context const* context_ptr;
         scrollable*    scrollable_ptr;

         // scroll the rectangle, r into view
         bool scroll_into_view(rect r_)
         {
            if (scrollable_ptr && context_ptr)
            {
               rect r = r_;
               return scrollable_ptr->scroll_into_view(*context_ptr, r);
            }
            return false;
         }
      };

      virtual bool               scroll_into_view(context const& ctx, rect r) = 0;
      static scrollable_context  find(context const& ctx);
   };

   enum
   {
      no_scrollbars  = 1,
      no_hscroll     = 1 << 1,
      no_vscroll     = 1 << 2
   };

   // Base proxy class for views that are scrollable
   class scroller_base : public port_element, public scrollable
   {
   public:

      static float            scrollbar_width;

                              scroller_base(int traits = 0)
                               : _tracking(none)
                               , _traits(traits)
                              {}

                              ~scroller_base() {}

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;
      element*                hit_test(context const& ctx, point p) override;
      void                    draw(context const& ctx) override;

      bool                    wants_control() const override;
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    scroll(context const& ctx, point dir, point p) override;
      bool                    scroll_into_view(context const& ctx, rect r) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    key(context const& ctx, key_info k) override;

      struct scrollbar_info
      {
         double   pos;
         float    extent;
         rect     bounds;
      };

      virtual void            draw_scroll_bar(context const& ctx, scrollbar_info const& info, point mp);
      virtual rect            scroll_bar_position(context const& ctx, scrollbar_info const& info);

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
      bool              reposition(context const& ctx, point p);

      bool              has_scrollbars() const { return !(_traits & no_scrollbars); }
      bool              allow_hscroll() const { return !(_traits & no_hscroll); }
      bool              allow_vscroll() const { return !(_traits & no_vscroll); }

      point             _offset;
      tracking_status   _tracking;
      int               _traits;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, scroller_base>
   scroller(Subject&& subject, int traits = 0)
   {
      return { std::forward<Subject>(subject), traits };
   }

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, scroller_base>
   vscroller(Subject&& subject, int traits = 0)
   {
      return { std::forward<Subject>(subject), traits | no_hscroll };
   }

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, scroller_base>
   hscroller(Subject&& subject, int traits = 0)
   {
      return { std::forward<Subject>(subject), traits | no_vscroll };
   }
}}

#endif
