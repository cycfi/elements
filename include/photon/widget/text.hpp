/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_LABEL_APRIL_17_2016)
#define PHOTON_GUI_LIB_WIDGET_LABEL_APRIL_17_2016

#include <photon/widget/widget.hpp>
#include <string>
#include <functional>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Headings
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class heading_widget : public widget
   {
   public:
                        heading_widget(std::string const& text)
                         : text(text)
                        {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);

   private:

      std::string       text;
   };

   inline widget_ptr heading(std::string const& text)
   {
      return widget_ptr{ new heading_widget{ text } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Labels
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class label_widget : public widget
   {
   public:
                        label_widget(std::string const& text)
                         : text(text)
                        {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);

   private:

      std::string       text;
   };

   inline widget_ptr label(std::string const& text)
   {
      return widget_ptr{ new label_widget{ text } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Text Boxes
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class static_text_box_widget : public widget
   {
   public:
                        static_text_box_widget(std::string const& text)
                         : text(text)
                        {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);

   private:

      std::string       text;
   };

   inline widget_ptr static_text_box(std::string const& text)
   {
      return widget_ptr{ new static_text_box_widget{ text } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   class text_box_widget : public widget
   {
   public:
                           text_box_widget(std::string const& text, double width)
                           : _text(text)
                           , _select_start(-1)
                           , _select_end(-1)
                           , _width(width)
                           , _current_x(0)
                           {}

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         draw(context const& ctx);
      virtual widget*      click(context const& ctx, mouse_button btn);
      virtual void         drag(context const& ctx, mouse_button btn);
      virtual bool         cursor(context const& ctx, point const& p);
      virtual bool         text(context const& ctx, text_info const& info);
      virtual bool         key(context const& ctx, key_info const& k);
      virtual bool         is_control() const;

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
   };

   inline widget_ptr text_box(std::string const& text, double width)
   {
      return widget_ptr{ new text_box_widget{ text, width } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   class input_box_widget : public text_box_widget
   {
   public:
                        input_box_widget(std::string const& placeholder)
                         : text_box_widget("", full_extent)
                         , _placeholder(placeholder)
                        {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);
      virtual bool      key(context const& ctx, key_info const& k);

   private:

      std::string       _placeholder;
   };

   inline widget_ptr input_box(std::string const& placeholder)
   {
      return widget_ptr{ new input_box_widget{ placeholder } };
   }

   class input_panel_widget : public widget
   {
   public:

      virtual void draw(context const& ctx);
   };

   inline widget_ptr input_panel()
   {
      return widget_ptr{ new input_panel_widget{} };
   }
}

#endif