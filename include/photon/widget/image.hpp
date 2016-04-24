/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_IMAGE_APRIL_24_2016)
#define PHOTON_GUI_LIB_WIDGET_IMAGE_APRIL_24_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Images
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class image_widget : public widget
   {
   public:

                           image_widget(char const* filename);
                           ~image_widget();

      point                size(context const& ctx) const;
      virtual void         draw(context const& ctx);

   private:

      char const*          _filename;
      mutable int          _handle;
      mutable NVGcontext*  _canvas;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////////////////////////
   inline widget_ptr image(char const* filename)
   {
      return widget_ptr{ new image_widget{ filename } };
   }
}

#endif