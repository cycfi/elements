/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_IMAGE_APRIL_24_2016)
#define ELEMENTS_IMAGE_APRIL_24_2016

#include <elements/element/element.hpp>
#include <elements/support/canvas.hpp>
#include <elements/support/pixmap.hpp>
#include <memory>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Images
   ////////////////////////////////////////////////////////////////////////////
   class image : public element
   {
   public:
                              image(char const* filename, float scale = 1);
                              image(pixmap_ptr pixmap_);

      virtual point           size() const;
      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      virtual rect            source_rect(context const& ctx) const;

   protected:

      elements::pixmap&       pixmap() const  { return *_pixmap.get(); }

   private:

      pixmap_ptr              _pixmap;
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

   ////////////////////////////////////////////////////////////////////////////
   // Images used as controls. Various frames are laid out in a single (big)
   // image but only one frame is drawn at any single time. Useful for
   // switches, knobs and basic (sprite) animation.
   //
   // Note on sprite_as_int and sprite_as_double: The tricky thing about
   // sprites is that they can act as both receiver<int> or receiver<double>
   // depending on usage. For example, buttons use it as a receiver<int>
   // where the int value reflects the current frame displayed. On the other
   // hand, dials regard it as a receiver<double>, where the value 0.0 to 1.0
   // reflects its state from 0 to num_frames()-1. Alas, we cannot directly
   // inherit from both because the overridden value() member function will
   // have an ambiguous return type (double or int?). The sprite_as_int and
   // sprite_as_double TMP trick solves this dilemma.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Derived>
   struct sprite_as_int : receiver<int>
   {
      void                    value(int val) override;
      int                     value() const override;
   };

   template <typename Derived>
   struct sprite_as_double : receiver<double>
   {
      void                    value(double val) override;
      double                  value() const override;
   };

   class basic_sprite : public image
   {
   public:
                              basic_sprite(char const* filename, float height, float scale = 1);

      view_limits             limits(basic_context const& ctx) const override;

      std::size_t             num_frames() const;
      std::size_t             index() const              { return _index; }
      void                    index(std::size_t index_);
      point                   size() const override;

      rect                    source_rect(context const& ctx) const override;

   private:

      size_t                  _index;
      float                   _height;
   };

   struct sprite : basic_sprite, sprite_as_int<sprite>, sprite_as_double<sprite>
   {
      using basic_sprite::basic_sprite;
   };

   template <typename Derived>
   int sprite_as_int<Derived>::value() const
   {
      auto this_ = static_cast<Derived const*>(this);
      return this_->index();
   }

   template <typename Derived>
   void sprite_as_int<Derived>::value(int val)
   {
      auto this_ = static_cast<Derived*>(this);
      this_->index(val);
   }

   template <typename Derived>
   double sprite_as_double<Derived>::value() const
   {
      auto this_ = static_cast<Derived const*>(this);
      return this_->index() / this_->num_frames()-1;
   }

   template <typename Derived>
   void sprite_as_double<Derived>::value(double val)
   {
      auto this_ = static_cast<Derived*>(this);
      this_->index(val * (this_->num_frames()-1));
   }
}}

#endif
