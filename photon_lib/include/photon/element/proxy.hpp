/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_PROXY_APRIL_10_2016)
#define CYCFI_PHOTON_GUI_LIB_PROXY_APRIL_10_2016

#include <photon/element/element.hpp>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // Proxies
   //
   // A element that encapsulates another element. The proxy delegates its methods
   // to an enlosed element.
   ////////////////////////////////////////////////////////////////////////////
   class proxy_base : public element
   {
   public:

   // Image

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual element*        hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual void            refresh(context const& ctx, element& element);
      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            prepare_subject(context& ctx);
      virtual void            prepare_subject(context& ctx, point& p);
      virtual void            restore_subject(context& ctx);

      using element::refresh;

   // Control

      virtual element*        click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            text(context const& ctx, text_info info);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);
      virtual void            idle(basic_context const& ctx);

      virtual bool            focus(focus_request r);
      virtual element const*  focus() const;
      virtual element*        focus();
      virtual bool            is_control() const;

   // Proxy

      virtual element const& subject() const = 0;
      virtual element&       subject() = 0;

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

      virtual element const&  subject() const { return _subject; }
      virtual element&        subject() { return _subject; }

   private:

      Subject                 _subject;
   };
}}

#endif
