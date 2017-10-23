/*=======================================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=======================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_APRIL_10_2016)
#define PHOTON_GUI_LIB_WIDGET_APRIL_10_2016

#include <photon/host.hpp>
#include <photon/support/rect.hpp>
#include <photon/support/misc.hpp>

#include <memory>
#include <string>
#include <type_traits>

namespace photon
{
   struct basic_context;
   struct context;

   //////////////////////////////////////////////////////////////////////////////////////
   // Widgets
   //
   // This is the class that deals with the graphic representation of fine-grained
   // elements inside a window which may be static graphics or active controls.
   //////////////////////////////////////////////////////////////////////////////////////
   class widget : public std::enable_shared_from_this<widget>
   {
   public:

      using widget_ptr = std::shared_ptr<widget>;
      using widget_const_ptr = std::shared_ptr<widget const>;

                              widget() {}
                              virtual ~widget() {}

                              widget(widget&&) = default;
                              widget(widget const&) = default;
      widget&                 operator=(widget&&) = default;
      widget&                 operator=(widget const&) = default;

   // Image

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point dir, point p);
      virtual void            refresh(context const& ctx, widget& widget);

   // Control

      virtual widget*         click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            text(context const& ctx, text_info info);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);
      virtual void            idle(basic_context const& ctx);

      virtual bool            focus(focus_request r);
      virtual widget const*   focus() const;
      virtual widget*         focus();
      virtual bool            is_control() const;

   // Receiver

      virtual void            value(bool val);
      virtual void            value(int val);
      virtual void            value(double val);
      virtual void            value(std::string val);
   };

   //////////////////////////////////////////////////////////////////////////////////////
   using widget_ptr = std::shared_ptr<widget>;
   using widget_const_ptr = std::shared_ptr<widget const>;

   template <typename T, typename Enavle = void>
   struct is_reference : std::false_type {};

   template <typename Widget>
   inline typename std::enable_if<
      (!is_reference<typename std::remove_reference<Widget>::type>::value)
    , widget_ptr>::type
   share(Widget&& w)
   {
      using widget_type = typename std::decay<Widget>::type;
      return std::make_shared<widget_type>(std::forward<Widget>(w));
   }

   template <typename Widget>
   inline typename std::enable_if<
      (is_reference<typename std::remove_reference<Widget>::type>::value)
    , widget_ptr>::type
   share(Widget&& w)
   {
      return w.get_ptr();
   }
}

#endif
