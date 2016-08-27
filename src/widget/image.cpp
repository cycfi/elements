/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/image.hpp>
#include <photon/support.hpp>
#include <photon/support/context.hpp>

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

   rect image::source_rect(context const& ctx) const
   {
      return rect{ 0, 0, ctx.bounds.width(), ctx.bounds.height() };
   }

   void image::draw(context const& ctx)
   {
      auto src = source_rect(ctx);
      ctx.canvas().draw(get_image(), src, ctx.bounds);
   }
}
