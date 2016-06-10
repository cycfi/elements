/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WINDOW_APRIL_10_2016)
#define PHOTON_GUI_LIB_WINDOW_APRIL_10_2016

#include <stdio.h>
#ifdef __APPLE__
# define GLFW_INCLUDE_GLCOREARB
#endif

#include <GLFW/glfw3.h>
#include <nanovg.h>

#include <photon/widget/widget.hpp>
#include <photon/widget/layer.hpp>
#include <photon/color.hpp>
#include <photon/point.hpp>
#include <photon/theme.hpp>
#include <photon/cursor.hpp>
#include <photon/canvas.hpp>

#include <memory>
#include <vector>

namespace photon
{
   class app;

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Windows
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class window
   {
   public:

      window(
         char const*    title
       , point          size
       , color          bkd_color
       , app&           app_
       , theme_ptr      theme = std::make_shared<photon::theme>()
      );

      ~window();

      window(window const&) = delete;
      window& operator=(window const&) = delete;

      point                size() const;
      void                 size(point s);
      point                cursor_pos() const;

      virtual void         draw(bool relayout = false);
      virtual void         key(key_info const& k);
      virtual void         text(text_info const& info);
      virtual void         click(mouse_button const& btn);
      virtual void         mouse(point p);
      virtual void         scroll(point p);
      virtual void         focus(bool is_focus);
      virtual void         close();
      virtual void         idle();
      virtual void         refresh();

      void                 set_cursor(cursor::type t) const;
      void                 reset_cursor() const;
      bool                 is_focus() const;

      std::string          clipboard() const;
      void                 clipboard(std::string const& text) const;

      key_info             key() const                   { return _current_key; }

      class canvas&        canvas()                      { return _canvas; }
      app&                 app() const                   { return _app; }
      class theme const&   theme() const                 { return *_theme.get(); }
      class theme&         theme()                       { return *_theme.get(); }

      layer_composite      content;

   private:

      color                _bkd_color;
      class app&           _app;
      GLFWwindow*          _window;
      NVGcontext*          _context;
      class canvas         _canvas;
      rect                 _current_bounds;
      rect                 _current_limits;
      mouse_button         _btn;
      theme_ptr            _theme;
      double               _click_time;
      int                  _num_clicks;
      bool                 _refresh;
      key_info             _current_key;
   };
}

#endif