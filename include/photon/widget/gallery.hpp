/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016)
#define PHOTON_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016

#include <photon/support/theme.hpp>
#include <photon/widget.hpp>

namespace photon
{
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
   // Captions
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Control>
   inline auto caption(Control&& control, std::string const& title, float size = 1.0)
   {
      return
         vtile(
            std::forward<Control>(control),
            align_center(top_margin(5.0, label(title, size)))
         );
   }
}

#endif