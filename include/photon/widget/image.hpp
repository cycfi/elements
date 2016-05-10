/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_IMAGE_APRIL_24_2016)
#define PHOTON_GUI_LIB_WIDGET_IMAGE_APRIL_24_2016

#include <photon/widget/widget.hpp>
#include <photon/canvas.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Images
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class image : public widget
   {
   public:

      image(char const* filename);
      
      image(image&&) = default;
      image(image const&) = default;
      image& operator=(image&&) = default;
      image& operator=(image const&) = default;

      point                size(context const& ctx) const;
      virtual void         draw(context const& ctx);

   private:
   
      using image_ptr = std::shared_ptr<canvas::image>;

      char const*          _filename;
      mutable image_ptr    _img;
   };
}

#endif