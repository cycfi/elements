/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_IMAGE_APRIL_24_2016)
#define PHOTON_GUI_LIB_WIDGET_IMAGE_APRIL_24_2016

#include <photon/widget/widget.hpp>
#include <photon/support/canvas.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Images
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class image : public widget
   {
   public:

      using image_ptr = std::shared_ptr<canvas::image>;

                           image(char const* filename);
                           image(image_ptr img_);
                           image(image const&) = default;
                           image& operator=(image const&) = default;

      point                size() const;
      virtual void         draw(context const& ctx);
      virtual rect         source_rect(context const& ctx) const;

   protected:

      canvas::image&       get_image() const  { return *_img.get(); }

   private:

      image_ptr            _img;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
	// Photon uses gizmos for user interface images such as buttons, frames etc.
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
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class gizmo : public image
   {
   public:
                           gizmo(char const* filename);
                           gizmo(image_ptr img_);

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         draw(context const& ctx);
   };

   class hgizmo : public image
   {
   public:
                           hgizmo(char const* filename);
                           hgizmo(image_ptr img_);

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         draw(context const& ctx);
   };

   class vgizmo : public image
   {
   public:
                           vgizmo(char const* filename);
                           vgizmo(image_ptr img_);

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Images used as controls. Various frames are laid out in a single (big)
   // image but only one frame is drawn at any single time. Useful for switches,
   // knobs and basic (sprite) animation.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <size_t width_, size_t height_>
   class sprite : public image
   {
   public:
                           sprite(char const* filename);
                           sprite(image_ptr img_);

      virtual rect         limits(basic_context const& ctx) const;

      std::size_t          index() const              { return _index; }
      void                 index(std::size_t index_)  { _index = index_; }
      constexpr float      width() const              { return width_; }
      constexpr float      height() const             { return height_; }

      virtual rect         source_rect(context const& ctx) const;

   private:

      size_t               _index;
   };

   template <size_t width_, size_t height_>
   inline sprite<width_, height_>::sprite(char const* filename)
    : image(filename)
    , _index(0)
   {}

   template <size_t width_, size_t height_>
   inline sprite<width_, height_>::sprite(image_ptr img_)
    : image(img_)
    , _index(0)
   {}

   template <size_t width_, size_t height_>
   inline rect sprite<width_, height_>::limits(basic_context const& ctx) const
   {
      return { width(), height(), width(), height() };
   }

   template <size_t width_, size_t height_>
   rect sprite<width_, height_>::source_rect(context const& ctx) const
   {
      return rect{ 0, height()*_index, width(), height()*(_index+1) };
   }
}

#endif