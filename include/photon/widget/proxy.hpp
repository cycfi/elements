/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_PROXY_APRIL_10_2016)
#define PHOTON_GUI_LIB_PROXY_APRIL_10_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Proxies
   //
   // A widget that encapsulates another widget. The proxy delegates its methods
   // to an enlosed widget.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class proxy_base : public widget
   {
   public:

   // Image

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            prepare_subject(context& ctx);
      virtual void            restore_subject(context& ctx);

   // Control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
//      virtual bool            key(context const& ctx, key_info const& k);
      virtual bool            text(context const& ctx, text_info const& info);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);
      virtual void            idle(basic_context const& ctx);

      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;
      virtual widget*         focus();
      virtual bool            is_control() const;

   // Proxy

      virtual widget const&  subject() const = 0;
      virtual widget&        subject() = 0;
      
   // Receiver

      virtual void            value(bool val);
      virtual void            value(int val);
      virtual void            value(double val);
      virtual void            value(std::string val);
   };

   template <typename Subject, typename Base = proxy_base>
   class proxy : public Base
   {
   public:
                              template <typename... T>
                              proxy(Subject&& subject_, T&&... args)
                               : Base(args...)
                               , _subject(std::move(subject_)) {}

                              template <typename... T>
                              proxy(Subject const& subject_, T&&... args)
                               : Base(args...)
                               , _subject(subject_) {}

      virtual widget const&   subject() const { return _subject; }
      virtual widget&         subject() { return _subject; }

   private:

      Subject                 _subject;
   };

}

#endif