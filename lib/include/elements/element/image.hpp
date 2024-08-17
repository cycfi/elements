/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_IMAGE_APRIL_24_2016)
#define ELEMENTS_IMAGE_APRIL_24_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/support/canvas.hpp>
#include <elements/support/pixmap.hpp>
#include <infra/filesystem.hpp>
#include <memory>

namespace cycfi::elements
{
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
                              image(fs::path const& path, fit_enum);
                              image(pixmap_ptr pixmap_);

      virtual point           size() const;
      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      virtual rect            source_rect(context const& ctx) const;

      void                    set_image(fs::path const& path, float scale = 1);
      pixmap_ptr              get_image() const { return _pixmap; }

   protected:

      elements::pixmap&       pixmap() const  { return *_pixmap.get(); }

   private:

      pixmap_ptr              _pixmap;
      bool                    _fit = false;
   };

   ////////////////////////////////////////////////////////////////////////////
	// Elements uses gizmos for user interface images such as buttons, frames etc.
   // Basically a gizmo is a resizeable image. The unique feature is its ability
	// to preserve the image near the edges when it is scaled up or down.
   //
   // The trick is by partictioning the image into 9 patches (tiles) much like
   // a tic-tac-toe:
   //
   //       +---+-------+---+
   //       | c |   h   | c |
   //       +---+-------+---+
   //       |   |       |   |
   //       | v |   m   | v |
   //       |   |       |   |
   //       +---+-------+---+
   //       | c |   h   | c |
   //       +---+-------+---+
   //
   // The corner patches, 'c', are drawn into the destination rectangle without resizing.
   // The top and bottom middle patches, 'h', are stretched horizontally to fit the
   // destination rectangle. The left and right middle patches, 'v', are stretched
   // vertically to fit the destination rectangle. Finally, the middle patch, 'm' is
   // stretched both horizontally and vertically to fit the destination rectangle.
   //
   // Variants of the gizmo are the hgizmo and vgizmo both having 3 patches
   // allowing resizing in one dimension (horozontally or vertically) only.
   //
   ////////////////////////////////////////////////////////////////////////////
   class gizmo : public image
   {
   public:
                              gizmo(char const* filename, float scale = 1);
                              gizmo(pixmap_ptr pixmap_);

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
   };

   class hgizmo : public image
   {
   public:
                              hgizmo(char const* filename, float scale = 1);
                              hgizmo(pixmap_ptr pixmap_);

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
   };

   class vgizmo : public image
   {
   public:
                              vgizmo(char const* filename, float scale = 1);
                              vgizmo(pixmap_ptr pixmap_);

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
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
                              basic_sprite(char const* filename, float height, float scale = 1);

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
