/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_IMAGE_APRIL_24_2016)
#define ELEMENTS_IMAGE_APRIL_24_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/support/receiver.hpp>
#include <artist/image.hpp>
#include <artist/canvas.hpp>
#include <memory>

namespace cycfi::elements
{
   using artist::image_ptr;

   /**
    * \class image
    *
    * \brief
    *    A class representing an image.
    *
    *    The `image` class provides functionalities such as scaling, fitting
    *    to available space, drawing, and setting/retrieving the image
    *    source. The image source can be either an `image_ptr` (a pointer to
    *    an image) or a filesystem path to an image file (`fs::path`). JPEG,
    *    PNG and WEBP images are supported.
    */
   class image : public element
   {
   public:

      struct fit_enum{};

      // Use `fit` as constructor param to allow the image to fit available
      // space while keeping source image aspect ratio.
      static constexpr auto fit = fit_enum{};

                              image(fs::path const& path, float scale = 1);
                              image(image_ptr img, float scale = 1);
                              image(fs::path const& path, fit_enum);
                              image(image_ptr pixmap_, fit_enum);

      virtual point           size() const;
      float                   scale() const { return _scale; }
      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      virtual rect            source_rect(context const& ctx) const;

      void                    set_image(image_ptr img);
      void                    set_image(fs::path const& path);
      image_ptr               get_image() const { return _image; }

   private:

      image_ptr               _image;
      float                   _scale;
   };

   /**
    * \class basic_sprite
    *
    * \brief
    *    Represents a basic sprite image for use in controls.
    *
    *    `basic_sprite` extends the `image` class to provide sprite-specific
    *    functionality. It subdivides an image into slices that can be
    *    indexed to display a particular frame.
    */
   class basic_sprite : public image
   {
   public:
                              basic_sprite(fs::path const& path, float height, float scale = 1);

      view_limits             limits(basic_context const& ctx) const override;

      std::size_t             num_frames() const;
      std::size_t             index() const;
      void                    index(std::size_t index_);
      point                   size() const override;

      rect                    source_rect(context const& ctx) const override;

   private:

      size_t                  _index;
      float                   _height;
   };

   using sprite = basic_sprite;

   namespace detail
   {
      template <typename T>
      constexpr auto is_sprite()
      {
         if constexpr (std::is_base_of_v<proxy_base, T>)
            return is_sprite<typename T::subject_type>();
         else
            return std::false_type{};
      }

      template <>
      constexpr auto is_sprite<sprite>()
      {
         return std::true_type{};
      }
   }

   namespace concepts
   {
      template <typename T>
      concept SpriteSubject
         = concepts::Element<T> &&
         decltype(detail::is_sprite<std::decay_t<T>>())::value;
   }

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    Retrieves the current frame index within the basic_sprite.
    *
    * \returns
    *    The current frame index as `std::size_t`.
    */
   inline std::size_t basic_sprite::index() const
   {
      return _index;
   }
}

#endif
