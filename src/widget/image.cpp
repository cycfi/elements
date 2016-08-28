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

   point image::size() const
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

   gizmo::gizmo(char const* filename)
    : image(filename)
   {}

   gizmo::gizmo(image_ptr img_)
    : image(img_)
   {}

   rect gizmo::limits(basic_context const& ctx) const
   {
      auto size_ = size();
      return { size_.x, size_.y, full_extent, full_extent };
   }

   namespace
   {
      //void gizmo_parts(rect src, rect dest, rect parts[9])
      //{
      //   // Subdivide a rect into 9 parts. src is the
      //   // original size and dest is the resized rect.
      //
      //   float div_h = std::min<float>(src.width() / 2.4, dest.width() / 2);
      //   float div_v = std::min<float>(src.height() / 2.4, dest.height() / 2);
      //
      //   auto corner = rect{ 0, 0, div_h, div_v };
      //
      //   parts[0] = corner.move(dest.left, dest.top);
      //   parts[1] = corner.move(dest.left, dest.bottom - div_v);
      //   parts[2] = corner.move(dest.right - div_h, dest.top);
      //   parts[3] = corner.move(dest.right - div_h, dest.bottom - div_v);
      //
      //   parts[4] = max(parts[0], parts[1]).inset(0, div_v);
      //   parts[5] = max(parts[0], parts[2]).inset(div_h, 0);
      //   parts[6] = max(parts[2], parts[3]).inset(0, div_v);
      //   parts[7] = max(parts[1], parts[3]).inset(div_h, 0);
      //   parts[8] = dest.inset(div_h, div_v);
      //}

      void gizmo_parts(rect src, rect dest, rect parts[9])
      {
         // Subdivide a rect into 9 parts. src is the
         // original size and dest is the resized rect.

         float div_h = std::min<float>(src.width() / 2.4, dest.width() / 2);
         float div_v = std::min<float>(src.height() / 2.4, dest.height() / 2);

         auto corner = rect{ 0, 0, div_h+1, div_v+1 };

         parts[0] = corner.move(dest.left, dest.top);
         parts[1] = corner.move(dest.left, dest.bottom - (div_v+1));
         parts[2] = corner.move(dest.right - (div_h+1), dest.top);
         parts[3] = corner.move(dest.right - (div_h+1), dest.bottom - (div_v+1));

         parts[4] = max(parts[0], parts[1]).inset(0, div_v);
         parts[5] = max(parts[0], parts[2]).inset(div_h, 0);
         parts[6] = max(parts[2], parts[3]).inset(0, div_v);
         parts[7] = max(parts[1], parts[3]).inset(div_h, 0);
         parts[8] = dest.inset(div_h-1, div_v-1);
      }
   }

   void gizmo::draw(context const& ctx)
   {
      rect  src[9];
      rect  dest[9];
      auto  size_ = size();
      rect  src_bounds{ 0, 0, size_.x, size_.y };

      gizmo_parts(src_bounds, src_bounds, src);

      //src_bounds.width(src_bounds.width()/2);
      //src_bounds.height(src_bounds.height()/2);

      gizmo_parts(src_bounds, ctx.bounds, dest);

      for (int i = 0; i < 9; i++)
         ctx.canvas().draw(get_image(), src[i], dest[i]);
   }
}
