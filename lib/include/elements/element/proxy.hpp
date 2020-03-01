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

   // Image

      view_limits             limits(basic_context const& ctx) const override;
      view_stretch            stretch() const override;
      element*                hit_test(context const& ctx, point p) override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override;
      void                    refresh(context const& ctx, element& element, int outward = 0) override;
      bool                    scroll(context const& ctx, point dir, point p) override;
      virtual void            prepare_subject(context& ctx);
      virtual void            prepare_subject(context& ctx, point& p);
      virtual void            restore_subject(context& ctx);

      using element::refresh;

   // Control

      element*                click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    text(context const& ctx, text_info info) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;

      bool                    wants_focus() const override;
      void                    begin_focus() override;
      void                    end_focus() override;
      element const*          focus() const override;
      element*                focus() override;
      bool                    is_control() const override;

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
      using subject_type = typename std::decay<Subject>::type;

                              template <typename... T>
                              proxy(Subject&& subject_, T&&... args)
                               : Base(std::forward<T>(args)...)
                               , _subject(std::forward<Subject>(subject_)) {}

      void                    subject(Subject&& subject_);
      element const&          subject() const override { return _subject; }
      element&                subject() override { return _subject; }

   private:

      subject_type            _subject;
   };

   template <typename Subject, typename Base>
   inline void proxy<Subject, Base>::subject(Subject&& subject_)
   {
      _subject = std::forward<Subject>(subject_);
   }

   ////////////////////////////////////////////////////////////////////////////
   // find_subject utility finds the outermost subject of the given type
   // or nullptr if not found.
   ////////////////////////////////////////////////////////////////////////////
   template <typename E>
   inline E* find_subject(element* e)
   {
      proxy_base* proxy = dynamic_cast<proxy_base*>(e);
      while (proxy)
      {
         auto* subject = &proxy->subject();
         if (auto e = dynamic_cast<E*>(subject))
            return e;
         proxy = dynamic_cast<proxy_base*>(subject);
      }
      return nullptr;
   }
}}

#endif
