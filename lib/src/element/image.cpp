/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/image.hpp>
#include <elements/support.hpp>
#include <elements/support/context.hpp>
#include <algorithm>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // image implementation
   ////////////////////////////////////////////////////////////////////////////
   image::image(fs::path const& path, float scale)
    : _pixmap(std::make_shared<artist::image>(path))
    , _scale(scale)
   {
   }

   image::image(image_ptr pixmap_)
    : _pixmap(pixmap_)
   {}

   point image::size() const
   {
      auto s = _pixmap->size();
      return { s.x * _scale, s.y * _scale };
   }

   rect image::source_rect(context const& ctx) const
   {
      return { 0, 0, ctx.bounds.width() / _scale, ctx.bounds.height() / _scale };
   }

   view_limits image::limits(basic_context const& /* ctx */) const
   {
      auto size_ = size();
      return { { size_.x, size_.y }, { size_.x, size_.y } };
   }

   void image::draw(context const& ctx)
   {
      auto src = source_rect(ctx);
      ctx.canvas.draw(pixmap(), src, ctx.bounds);
   }

   basic_sprite::basic_sprite(fs::path const& path, float height, float scale)
    : image(path, scale)
    , _index(0)
    , _height(height)
   {}

   view_limits basic_sprite::limits(basic_context const& /* ctx */) const
   {
      auto width = pixmap().size().x;
      return { { width * scale(), _height }, { width * scale(), _height } };
   }

   std::size_t basic_sprite::num_frames() const
   {
      return (pixmap().size().y * scale()) / _height;
   }

   void basic_sprite::index(std::size_t index_)
   {
      if (index_ < num_frames())
         _index = index_;
   }

   point basic_sprite::size() const
   {
      return { pixmap().size().x * scale(), _height };
   }

   rect basic_sprite::source_rect(context const& /* ctx */) const
   {
      auto sc = scale();
      auto width = pixmap().size().x;
      return rect{ 0, (_height/sc) * _index, width, (_height/sc) * (_index + 1) };
   }
}}
