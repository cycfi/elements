/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_IMAGE_APRIL_24_2016)
#define PHOTON_GUI_LIB_WIDGET_IMAGE_APRIL_24_2016

#include <photon/widget/widget.hpp>
#include <photon/canvas.hpp>
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

                           image(image_ptr img_);

                           image(image&&) = default;
                           image(image const&) = default;
                           image& operator=(image&&) = default;
                           image& operator=(image const&) = default;

      virtual point        size(context const& ctx) const;
      virtual void         draw(context const& ctx);

   protected:

      image_ptr const&     get_image() const  { return _img; }

   private:

      image_ptr            _img;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Images used as controls. Various frames are laid out in a single (big)
   // image but only one frame is drawn at any single time. Useful for switches,
   // knobs and basic (sprite) animation.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class sprite : public image
   {
   public:
                           sprite(image_ptr img_, point size);

      virtual rect         limits(basic_context const& ctx) const;
      virtual void         draw(context const& ctx);

      virtual point        size(context const& ctx) const;
      std::size_t          index() const              { return _index; }
      void                 index(std::size_t index_)  { _index = index_; }

   private:

      point                _size;
      std::size_t          _index;
   };
}

#endif