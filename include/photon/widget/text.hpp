/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_LABEL_APRIL_17_2016)
#define PHOTON_GUI_LIB_WIDGET_LABEL_APRIL_17_2016

#include <photon/widget/widget.hpp>
#include <string>

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
                        text_box_widget(std::string const& text)
                         : text(text), select_start(-1), select_end(-1)
                        {}

      virtual rect      limits(basic_context const& ctx) const;
      virtual void      draw(context const& ctx);
      virtual widget*   click(context const& ctx, mouse_button btn);
      virtual void      drag(context const& ctx, mouse_button btn);
      virtual bool      cursor(context const& ctx, point const& p);
      virtual bool      is_control() const;

   private:

      std::string       text;
      int               select_start;
      int               select_end;
   };

   inline widget_ptr text_box(std::string const& text)
   {
      return widget_ptr{ new text_box_widget{ text } };
   }
}

#endif