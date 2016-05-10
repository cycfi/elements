/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/image.hpp>
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>
#include <nanovg.h>

namespace photon
{
   image::image(char const* filename)
    : _filename(filename)
   {}

   point image::size(context const& ctx) const
   {
      if (!_img)
         _img.reset(new canvas::image{ ctx.canvas(), _filename });
      return _img->size();
   }

   void image::draw(context const& ctx)
   {
      if (!_img)
         _img.reset(new canvas::image{ ctx.canvas(), _filename });
      _img->draw(ctx.bounds);
   }
}
