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
    * \class sprite_as_double
    *
    * \brief
    *    A template structure to handle sprite images used as 'double'
    *    receivers.
    *
    *    Receives a 'double' value in the range of 0.0 to 1.0, which
    *    indicates which frame of the sprite image should be displayed. The
    *    double value essentially represents the frame number, scaled down to
    *    fit within the 0.0 to 1.0 range.
    *
    * \tparam Derived
    *    The derived class type.
    */
   template <typename Derived>
   struct sprite_as_double : receiver<double>
   {
      void                    value(double val) override;
      double                  value() const override;
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

   /**
    * \class sprite
    *
    * \brief
    *    A class representing a sprite image that can be used as controls.
    *
    *    `sprite` extends `basic_sprite` to utilize both `sprite_as_int` and
    *    `sprite_as_double`, making it usable as both an 'int' and a 'double'
    *    receiver.
    *
    *    Sprites are images used as controls. Various frames are laid out in
    *    a single (big) image but only one frame is drawn at any single time.
    *    Useful for switches, knobs and basic (sprite) animation.
    */
   struct sprite : basic_sprite, sprite_as_double<sprite>
   {
      using basic_sprite::basic_sprite;
   };

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

   /**
    * \brief
    *    Returns the index of the currently displayed frame in
    *    sprite_as_double as a fraction of the total frames.
    *
    * \tparam Derived
    *    The derived class type.
    *
    * \returns
    *    The index of the currently displayed frame as a fraction of the
    *    total frames (between 0.0 to 1.0).
    */
   template <typename Derived>
   double sprite_as_double<Derived>::value() const
   {
      auto this_ = static_cast<Derived const*>(this);
      return this_->index() / this_->num_frames()-1;
   }

   /**
    * \brief
    *    Sets the index of the sprite_as_double to the provided value.
    *
    * \tparam Derived
    *    The derived class type.
    *
    * \param val
    *    The value to set the index to. It's a value between 0.0 and 1.0
    *    representing the relative position in the sequence of frames.
    */
   template <typename Derived>
   void sprite_as_double<Derived>::value(double val)
   {
      auto this_ = static_cast<Derived*>(this);
      this_->index(val * (this_->num_frames()-1));
   }
}

#endif
