/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_LABEL_APRIL_17_2016)
#define PHOTON_GUI_LIB_WIDGET_LABEL_APRIL_17_2016

#include <photon/widget/widget.hpp>
#include <photon/widget/port.hpp>
#include <photon/widget/margin.hpp>
#include <photon/widget/layer.hpp>
#include <string>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Headings
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class heading : public widget
   {
   public:

      heading(std::string const& text)
       : _text(text)
      {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);

   private:

      std::string       _text;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Labels
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class label : public widget
   {
   public:

      label(std::string const& text)
       : _text(text)
      {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);

   private:

      std::string       _text;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Text Boxes
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class static_text_box : public widget
   {
   public:

      static_text_box(std::string const& text)
       : _text(text)
      {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);

   private:

      std::string       _text;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_text_box : public widget
   {
   public:

      basic_text_box(std::string const& text, double width)
      : _text(text)
      , _select_start(-1)
      , _select_end(-1)
      , _width(width)
      , _current_x(0)
      , _is_focus(false)
      {}

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         draw(context const& ctx);
      virtual widget*      click(context const& ctx, mouse_button btn);
      virtual void         drag(context const& ctx, mouse_button btn);
      virtual bool         cursor(context const& ctx, point const& p);
      virtual bool         text(context const& ctx, text_info const& info);
      virtual bool         key(context const& ctx, key_info const& k);
      virtual bool         focus(focus_request r);
      virtual bool         is_control() const;

      using widget::focus;

      int                  select_start() const { return _select_start; }
      int                  select_end() const { return _select_end; }
      double               width() const { return _width; }
      std::string const&   text() const { return _text; }
      void                 text(std::string const& text_) { _text = text_; }

   private:

      void                 delete_();
      void                 cut(window& w, int start, int end);
      void                 copy(window& w, int start, int end);
      void                 paste(window& w, int start, int end);

      struct state_saver;
      using state_saver_f = std::function<void()>;

      state_saver_f        capture_state();
      void                 scroll_into_view(context const& ctx, bool save_x);

      std::string          _text;
      int                  _select_start;
      int                  _select_end;
      double               _width;
      double               _current_x;
      state_saver_f        _typing_state;
      bool                 _is_focus;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_input_box : public basic_text_box
   {
   public:

      basic_input_box(std::string const& placeholder)
       : basic_text_box("", full_extent)
       , _placeholder(placeholder)
      {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);
      virtual bool      key(context const& ctx, key_info const& k);

   private:

      std::string       _placeholder;
   };

   class input_panel : public widget
   {
   public:

      virtual void draw(context const& ctx);
   };

   inline auto input_box(
      std::string const& placeholder
    , rect const& pad  = rect{ 7, 7, 7, 4 }
   )
   {
      return layer(
         margin(
            pad,
            scroller(
               basic_input_box{ placeholder },
               no_scrollbars | no_vscroll
            )
         ),
         input_panel()
      );
   }
}

#endif