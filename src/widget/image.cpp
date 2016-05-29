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
   image::image(image_ptr img_)
    : _img(img_)
   {}

   point image::size(context const& ctx) const
   {
      return _img->size();
   }

   void image::draw(context const& ctx)
   {
      _img->draw(ctx.bounds);
   }

   sprite::sprite(image_ptr img_, point size_)
    : image(img_)
    , _size(size_)
    , _index(100)
   {}

   rect sprite::limits(basic_context const& ctx) const
   {
      return { _size.x, _size.y, _size.x, _size.y };
   }

   void sprite::draw(context const& ctx)
   {
      get_image()->draw(ctx.bounds, { 0.0f, _size.y * _index });
   }

   point sprite::size(context const& ctx) const
   {
      return _size;
   }
}
