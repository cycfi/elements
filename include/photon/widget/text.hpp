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

      heading(std::string const& text, float size_ = 1.0)
       : _text(text)
       , _size(size_)
      {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);

      std::string       text() const                     { return _text; }
      void              text(std::string const& text)    { _text = text; }
      
      using widget::text;

   private:

      std::string       _text;
      float             _size;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Labels
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class label : public widget
   {
   public:

      label(std::string const& text, float size_ = 1.0)
       : _text(text)
       , _size(size_)
      {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);
      
      std::string       text() const                     { return _text; }
      void              text(std::string const& text)    { _text = text; }
      
      using widget::text;


   private:

      std::string       _text;
      float             _size;
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
      
      std::string       text() const                     { return _text; }
      void              text(std::string const& text)    { _text = text; }
      
      using widget::text;


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
      virtual bool         cursor(context const& ctx, point p);
      virtual bool         text(context const& ctx, text_info const& info);
      virtual bool         key(context const& ctx, key_info const& k);
      virtual bool         focus(focus_request r);
      virtual bool         is_control() const;

      using widget::focus;

      int                  select_start() const { return _select_start; }
      int                  select_end() const { return _select_end; }
      float                width() const { return _width; }
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
      float                _width;
      float                _current_x;
      state_saver_f        _typing_state;
      bool                 _is_focus;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   class basic_input_box : public basic_text_box
   {
   public:
   
      using enter_function = std::function<bool(std::string const& text)>;

      basic_input_box(std::string const& placeholder)
       : basic_text_box("", full_extent)
       , _placeholder(placeholder)
      {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);
      virtual bool      key(context const& ctx, key_info const& k);
      
      enter_function    on_enter;

   private:

      std::string       _placeholder;
   };

   class input_panel : public widget
   {
   public:

      virtual void draw(context const& ctx);
   };
   
   template <typename InputBox>
   inline auto input_box(
      InputBox&& input_box
    , rect pad  = rect{ 7, 7, 7, 4 }
   )
   {
      return layer(
         margin(
            pad,
            scroller(
               std::forward<InputBox>(input_box),
               no_scrollbars | no_vscroll
            )
         ),
         input_panel()
      );
   }

   inline auto input_box(
      std::string const& placeholder
    , rect pad  = rect{ 7, 7, 7, 4 }
   )
   {
      return input_box(basic_input_box{ placeholder }, pad);
   }
   
   inline auto input_box(
      char const* placeholder
    , rect pad  = rect{ 7, 7, 7, 4 }
   )
   {
      return input_box(basic_input_box{ placeholder }, pad);
   }
}

#endif