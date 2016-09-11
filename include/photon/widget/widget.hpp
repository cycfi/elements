/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_APRIL_10_2016)
#define PHOTON_GUI_LIB_WIDGET_APRIL_10_2016

#include <photon/support/rect.hpp>
#include <photon/support/misc.hpp>

#include <memory>
#include <string>
#include <type_traits>

namespace photon
{
   class basic_context;
   class context;

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Mouse Button
   ////////////////////////////////////////////////////////////////////////////////////////////////
   struct mouse_button
   {
      enum what { left, middle, right };

      bool     down;
      int      num_clicks;
      what     state;
      point    pos;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Cursor tracking
   ////////////////////////////////////////////////////////////////////////////////////////////////
   enum class cursor_tracking
   {
      entering,   // Sent when the cursor is entering a widget
      hovering,   // Sent when the cursor is hovering over a widget
      leaving     // Sent when the cursor is leaving a widget
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Text info
   ////////////////////////////////////////////////////////////////////////////////////////////////
   struct text_info
   {
      uint32_t codepoint;
      int      modifiers;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Widget Limits
   ////////////////////////////////////////////////////////////////////////////////////////////////
   struct widget_limits
   {
      point    min;
      point    max;
   };

   constexpr float         full_extent    = 1E6;
   constexpr widget_limits full_limits    = { { 0.0, 0.0 }, { full_extent, full_extent } };
   constexpr widget_limits empty_limits   = { { 0.0, 0.0 }, { 0.0, 0.0 } };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Focus request
   ////////////////////////////////////////////////////////////////////////////////////////////////
   enum class focus_request
   {
      wants_focus,
      begin_focus,
      end_focus
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Widgets
   //
   // This is the class that deals with the graphic representation of fine-grained
   // elements inside a window which may be static images or active controls.
   ////////////////////////////////////////////////////////////////////////////////////////////////
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

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual widget*         hit_test(context const& ctx, point p);
      virtual void            draw(context const& ctx);
      virtual void            layout(context const& ctx);
      virtual bool            scroll(context const& ctx, point p);

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

   // Receiver

      virtual void            value(bool val);
      virtual void            value(int val);
      virtual void            value(double val);
      virtual void            value(std::string val);
   };

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