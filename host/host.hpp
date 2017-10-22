/*=============================================================================
   Copyright © 2016 Cycfi Research. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_VIEW_AUGUST_20_2016)
#define PHOTON_VIEW_AUGUST_20_2016

#include <host/key.hpp>

namespace photon
{
   struct host_view;

   ////////////////////////////////////////////////////////////////////////////
   // Mouse Button
   ////////////////////////////////////////////////////////////////////////////
   struct mouse_button
   {
      enum what { left, middle, right };

      bool     down;
      int      num_clicks;
      what     state;
      int      modifiers;
      float    x;
      float    y;
   };

   //////////////////////////////////////////////////////////////////////////////////////
   // Cursor tracking
   //////////////////////////////////////////////////////////////////////////////////////
   enum class cursor_tracking
   {
      entering,   // Sent when the cursor is entering the view
      hovering,   // Sent when the cursor is hovering over the view
      leaving     // Sent when the cursor is leaving the view
   };

   //////////////////////////////////////////////////////////////////////////////////////
   // Text info
   //////////////////////////////////////////////////////////////////////////////////////
   struct text_info
   {
      uint32_t codepoint;
      int      modifiers;
   };

   //////////////////////////////////////////////////////////////////////////////////////
   // Focus request
   //////////////////////////////////////////////////////////////////////////////////////
   enum class focus_request
   {
      wants_focus,
      begin_focus,
      end_focus
   };

   /////////////////////////////////////////////////////////////////////////////
   // The view base class
   /////////////////////////////////////////////////////////////////////////////
   class base_view
   {
   public:

      base_view(host_view* h)
       : h(h)
      {}

      virtual ~base_view()
      {}

      virtual void draw(float left, float top, float right, float bottom) {};
      virtual void click(mouse_button btn) {}
      virtual void drag(mouse_button btn) {}
      virtual void cursor(float x, float y, cursor_tracking status) {}
      virtual void scroll(float dirx, float diry, float x, float y) {}
      virtual void key(key_info const& k) {}
      virtual void text(text_info const& info) {}
      virtual void focus(focus_request r) {}

   private:

      host_view* h;
   };

   /////////////////////////////////////////////////////////////////////////////
   // Application event loop entry
   int app_main(int argc, const char* argv[]);

   /////////////////////////////////////////////////////////////////////////////
   // View creation callback
   extern base_view* (*new_view)(host_view* h);
}

#endif
