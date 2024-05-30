/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/image.hpp>
#include <elements/support.hpp>
#include <elements/support/context.hpp>
#include <algorithm>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // image implementation
   ////////////////////////////////////////////////////////////////////////////
   image::image(fs::path const& path, float scale)
    : _image(std::make_shared<artist::image>(path))
    , _scale(scale)
   {
      if (!_image->impl())
         throw std::runtime_error{"Error: Invalid image."};
   }

   image::image(image_ptr image_, float scale)
    : _image(image_)
    , _scale(scale)
   {
      if (!_image->impl())
         throw std::runtime_error{"Error: Invalid image."};
   }

   image::image(fs::path const& path, fit_enum)
    : image{path, -1}
   {
   }

   image::image(image_ptr image_, fit_enum)
    : image{image_, -1}
   {
   }

   point image::size() const
   {
      auto s = _image->size();
      if (_scale > 0)
         return {s.x * _scale, s.y * _scale};
      else // fit
         return {-1, -1}; // We do not know the actual size
   }

   rect image::source_rect(context const& ctx) const
   {
      if (_scale > 0)
      {
         return {0, 0, ctx.bounds.width() / _scale, ctx.bounds.height() / _scale};
      }
      else // fit
      {
         auto s = _image->size();
         return {0, 0, s.x, s.y};
      }
   }

   view_limits image::limits(basic_context const& /* ctx */) const
   {
      if (_scale > 0)
      {
         auto size_ = size();
         return {{size_.x, size_.y}, {size_.x, size_.y}};
      }
      else // fit
      {
         return full_limits;
      }
   }

   void image::draw(context const& ctx)
   {
      auto src = source_rect(ctx);
      if (_scale > 0)
      {
         ctx.canvas.draw(*get_image().get(), src, ctx.bounds);
      }
      else
      {
         float aspect_ratio = src.width() / src.height();
         auto dest = ctx.bounds;
         if (auto h = dest.width() / aspect_ratio; h <= ctx.bounds.height())
            dest.height(dest.width() / aspect_ratio);
         else
            dest.width(dest.height() * aspect_ratio);
         ctx.canvas.draw(*get_image().get(), src, center(dest, ctx.bounds));
      }
   }

   void image::set_image(image_ptr img)
   {
      _image = img;
      if (!_image->impl())
         throw std::runtime_error{"Error: Invalid image."};
   }

   void image::set_image(fs::path const& path)
   {
      _image = std::make_shared<artist::image>(path);
      if (!_image->impl())
         throw std::runtime_error{"Error: Invalid image."};
   }

   basic_sprite::basic_sprite(fs::path const& path, float height, float scale)
    : image(path, scale)
    , _index(0)
    , _height(height)
   {}

   view_limits basic_sprite::limits(basic_context const& /* ctx */) const
   {
      auto width = get_image()->size().x;
      return {{width * scale(), _height}, {width * scale(), _height}};
   }

   std::size_t basic_sprite::num_frames() const
   {
      return (get_image()->size().y * scale()) / _height;
   }

   void basic_sprite::index(std::size_t index_)
   {
      if (index_ < num_frames())
         _index = index_;
   }

   point basic_sprite::size() const
   {
      return {get_image()->size().x * scale(), _height};
   }

   rect basic_sprite::source_rect(context const& /* ctx */) const
   {
      auto sc = scale();
      auto width = get_image()->size().x;
      return rect{0, (_height/sc) * _index, width, (_height/sc) * (_index + 1)};
   }
}
