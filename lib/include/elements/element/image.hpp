/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_IMAGE_APRIL_24_2016)
#define ELEMENTS_IMAGE_APRIL_24_2016

#include <elements/element/element.hpp>
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
    * \class sprite_as_int
    *
    * \brief
    *    A template structure to handle sprite images used as 'int' receivers.
    *
    *    Receives an 'int' value which indicates which frame of sprite image
    *    should be displayed.
    *
    * \tparam Derived
    *    The derived class type.
    */
   template <typename Derived>
   struct sprite_as_int : receiver<int>
   {
      void                    value(int val) override;
      int                     value() const override;
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
    *
    *    Note on sprite_as_int and sprite_as_double: The tricky thing about
    *    sprites is that they can act as both receiver<int> or
    *    receiver<double> depending on usage. For example, buttons use it as
    *    a receiver<int> where the int value reflects the current frame
    *    displayed. On the other hand, dials regard it as a receiver<double>,
    *    where the value 0.0 to 1.0 reflects its state from 0 to
    *    num_frames()-1. Alas, we cannot directly inherit from both because
    *    the overridden value() member function will have an ambiguous return
    *    type (double or int?). The sprite_as_int and sprite_as_double TMP
    *    trick solves this dilemma.
    */
   struct sprite : basic_sprite, sprite_as_int<sprite>, sprite_as_double<sprite>
   {
      using basic_sprite::basic_sprite;
   };

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
    *    Returns the index of the currently displayed frame in sprite_as_int.
    *
    * \tparam Derived
    *    The derived class type.
    *
    * \returns
    *    The index of the currently displayed frame as an integer.
    */
   template <typename Derived>
   int sprite_as_int<Derived>::value() const
   {
      auto this_ = static_cast<Derived const*>(this);
      return this_->index();
   }

   /**
    * \brief
    *    Sets the index of the sprite_as_int to the provided value.
    *
    * \tparam Derived
    *    The derived class type.
    *
    * \param val
    *    The value to set the index to (index of the frame to be displayed).
    */
   template <typename Derived>
   void sprite_as_int<Derived>::value(int val)
   {
      auto this_ = static_cast<Derived*>(this);
      this_->index(val);
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
