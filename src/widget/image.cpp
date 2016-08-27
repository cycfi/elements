/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/image.hpp>
#include <photon/support.hpp>

namespace photon
{
   image::image(char const* filename)
    : _img(std::make_shared<canvas::image>(filename))
   {
   }

   image::image(image_ptr img_)
    : _img(img_)
   {}

   point image::size(context const& ctx) const
   {
      return _img->size();
   }

   void image::draw(context const& ctx)
   {
      auto src = rect{ 0, 0, ctx.bounds.width(), ctx.bounds.height() };
      ctx.canvas().draw(get_image(), src, ctx.bounds);
   }

   sprite::sprite(char const* filename, point size_)
    : image(filename)
    , _size(size_)
    , _index(0)
   {}

   sprite::sprite(image_ptr img_, point size_)
    : image(img_)
    , _size(size_)
    , _index(0)
   {}

   rect sprite::limits(basic_context const& ctx) const
   {
      return { _size.x, _size.y, _size.x, _size.y };
   }

   void sprite::draw(context const& ctx)
   {
      auto src = rect{ 0, _size.y * _index, _size.x, _size.y * (_index+1) };
      ctx.canvas().draw(get_image(), src, ctx.bounds);
   }

   point sprite::size(context const& ctx) const
   {
      return _size;
   }
}
