/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_PROXY_APRIL_10_2016)
#define ELEMENTS_PROXY_APRIL_10_2016

#include <elements/element/element.hpp>
#include <type_traits>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Proxies
   //
   // A element that encapsulates another element (its 'subject'). The proxy
   // delegates its methods to its encapsulated subject, but may augment
   // or totally override its behavior.
   ////////////////////////////////////////////////////////////////////////////
   class proxy_base : public element
   {
   public:

   // Display

      view_limits             limits(basic_context const& ctx) const override;
      view_stretch            stretch() const override;
      unsigned                span() const override;
      element*                hit_test(context const& ctx, point p) override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;
      void                    refresh(context const& ctx, element& element, int outward = 0) override;
      virtual void            prepare_subject(context& ctx);
      virtual void            prepare_subject(context& ctx, point& p);
      virtual void            restore_subject(context& ctx);

      using element::refresh;

   // Control

      bool                    wants_control() const override;
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    text(context const& ctx, text_info info) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    scroll(context const& ctx, point dir, point p) override;

      bool                    wants_focus() const override;
      void                    begin_focus() override;
      void                    end_focus() override;
      element const*          focus() const override;
      element*                focus() override;

   // Proxy

      virtual element const&  subject() const = 0;
      virtual element&        subject() = 0;
   };

   template <typename Subject, typename Base = proxy_base>
   class proxy : public Base
   {
   public:

      static_assert(std::is_base_of_v<proxy_base, Base>,
         "proxy Base type needs to be or inherit from proxy_base");
      static_assert(!std::is_reference_v<Subject>,
         "Subject must not be a reference type - maybe you want to use reference class instead");
      static_assert(!std::is_const_v<Subject>, "Subject must not be const");

                              template <typename... T>
                              proxy(Subject subject_, T&&... args)
                               : Base(std::forward<T>(args)...)
                               , _subject(std::move(subject_)) {}

      void                    subject(Subject&& subject_);
      void                    subject(Subject const& subject_);
      element const&          subject() const override { return _subject; }
      element&                subject() override { return _subject; }
      Subject const&          actual_subject() const { return _subject; }
      Subject&                actual_subject() { return _subject; }

   private:

      Subject                 _subject;
   };

   template <typename Subject, typename Base>
   inline void proxy<Subject, Base>::subject(Subject&& subject_)
   {
      _subject = std::move(subject_);
   }

   template <typename Subject, typename Base>
   inline void proxy<Subject, Base>::subject(Subject const& subject_)
   {
      _subject = subject_;
   }
}}

#endif
