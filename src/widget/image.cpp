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
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // image implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////
   image::image(char const* filename, float scale)
    : _pixmap(std::make_shared<photon::pixmap>(filename, scale))
   {
   }

   image::image(pixmap_ptr pixmap_)
    : _pixmap(pixmap_)
   {}

   point image::size() const
   {
      return _pixmap->size();
   }

   rect image::source_rect(context const& ctx) const
   {
      return rect{ 0, 0, ctx.bounds.width(), ctx.bounds.height() };
   }

   void image::draw(context const& ctx)
   {
      auto src = source_rect(ctx);
      ctx.canvas().draw(pixmap(), src, ctx.bounds);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // gizmo implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////
   namespace
   {
      void gizmo_parts(rect src, rect dest, rect parts[9])
      {
         // Subdivide a rect into 9 parts. src is the original size and dest
         // is the resized rect. The 9 patches have a 1-pixel overlap to
         // maintain seemless rendering.

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

      void hgizmo_parts(rect src, rect dest, rect parts[3])
      {
         // Variation of gizmo_parts allowing horizontal resizing only.

         float div_h = std::min<float>(src.width() / 2.4, dest.width() / 2);
         auto corner = rect{ 0, 0, div_h+1, src.height() };

         parts[0] = corner.move(dest.left, dest.top);
         parts[1] = corner.move(dest.right - (div_h+1), dest.top);
         parts[2] = max(parts[0], parts[1]).inset(div_h, 0);
      }

      void vgizmo_parts(rect src, rect dest, rect parts[9])
      {
         // Variation of gizmo_parts allowing vertical resizing only.

         float div_v = std::min<float>(src.height() / 2.4, dest.height() / 2);
         auto corner = rect{ 0, 0, src.width(), div_v+1 };

         parts[0] = corner.move(dest.left, dest.top);
         parts[1] = corner.move(dest.left, dest.bottom - (div_v+1));
         parts[2] = max(parts[0], parts[1]).inset(0, div_v);
      }
   }

   gizmo::gizmo(char const* filename, float scale)
    : image(filename, scale)
   {}

   gizmo::gizmo(pixmap_ptr pixmap_)
    : image(pixmap_)
   {}

   rect gizmo::limits(basic_context const& ctx) const
   {
      auto size_ = size();
      return { size_.x, size_.y, full_extent, full_extent };
   }

   void gizmo::draw(context const& ctx)
   {
      rect  src[9];
      rect  dest[9];
      auto  size_ = size();
      rect  src_bounds{ 0, 0, size_.x, size_.y };

      gizmo_parts(src_bounds, src_bounds, src);
      gizmo_parts(src_bounds, ctx.bounds, dest);

      for (int i = 0; i < 9; i++)
         ctx.canvas().draw(pixmap(), src[i], dest[i]);
   }

   hgizmo::hgizmo(char const* filename)
    : image(filename)
   {}

   hgizmo::hgizmo(pixmap_ptr pixmap_)
    : image(pixmap_)
   {}

   rect hgizmo::limits(basic_context const& ctx) const
   {
      auto size_ = size();
      return { size_.x, size_.y, size_.y, full_extent };
   }

   void hgizmo::draw(context const& ctx)
   {
      rect  src[3];
      rect  dest[3];
      auto  size_ = size();
      rect  src_bounds{ 0, 0, size_.x, size_.y };

      hgizmo_parts(src_bounds, src_bounds, src);
      hgizmo_parts(src_bounds, ctx.bounds, dest);
      ctx.canvas().draw(pixmap(), src[0], dest[0]);
      ctx.canvas().draw(pixmap(), src[1], dest[1]);
      ctx.canvas().draw(pixmap(), src[2], dest[2]);
   }

   vgizmo::vgizmo(char const* filename)
    : image(filename)
   {}

   vgizmo::vgizmo(pixmap_ptr pixmap_)
    : image(pixmap_)
   {}

   rect vgizmo::limits(basic_context const& ctx) const
   {
      auto size_ = size();
      return { size_.x, size_.y, size_.x, full_extent };
   }

   void vgizmo::draw(context const& ctx)
   {
      rect  src[3];
      rect  dest[3];
      auto  size_ = size();
      rect  src_bounds{ 0, 0, size_.x, size_.y };

      vgizmo_parts(src_bounds, src_bounds, src);
      vgizmo_parts(src_bounds, ctx.bounds, dest);
      ctx.canvas().draw(pixmap(), src[0], dest[0]);
      ctx.canvas().draw(pixmap(), src[1], dest[1]);
      ctx.canvas().draw(pixmap(), src[2], dest[2]);
   }
}
