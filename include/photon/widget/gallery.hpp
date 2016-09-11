/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016)
#define PHOTON_GUI_LIB_WIDGET_GALLERY_JUNE_5_2016

#include <photon/support/theme.hpp>
#include <photon/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Frames
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class frame : public widget
   {
   public:

      virtual void   draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Labels
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class label : public widget
   {
   public:

                              label(std::string const& text, float size_ = 1.0)
                               : _text(text)
                               , _size(size_)
                              {}

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

      std::string             text() const                     { return _text; }
      void                    text(std::string const& text)    { _text = text; }

      using widget::text;

   private:

      std::string       _text;
      float             _size;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Grid Lines
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class vgrid_lines : public widget
   {
   public:

                              vgrid_lines(float major_divisions, float minor_divisions)
                               : _major_divisions(major_divisions)
                               , _minor_divisions(minor_divisions)
                              {}

      virtual void            draw(context const& ctx);

   private:

      float                   _major_divisions;
      float                   _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Groups
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Heading, typename Content>
   inline auto make_group(
      Heading&&   heading,
      Content&&   content,
      bool        center_heading = true
   )
   {
      auto align_ = center_heading? 0.5 : 0;
      return
        layer(
            align_top(halign(align_, margin({ 10, 4, 10, 4 }, heading))),
            std::forward<Content>(content),
            frame{}
        );
   }

   template <typename Content>
   inline auto group(
      std::string    title,
      Content&&      content,
      float          label_size = 1.0,
      bool           center_heading = true
   )
   {
      return make_group(
         left_top_margin({ 10, 10 }, label(title, label_size)),
         std::forward<Content>(content), center_heading
      );
   }

   template <typename Heading, typename Content>
   inline auto make_unframed_group(
      Heading&&   heading,
      Content&&   content,
      bool        center_heading = true
   )
   {
      auto align_ = center_heading? 0.5 : 0;
      return
        layer(
            align_top(halign(align_, margin({ 10, 4, 10, 4 }, heading))),
            std::forward<Content>(content)
        );
   }

   template <typename Content>
   inline auto unframed_group(
      std::string    title,
      Content&&      content,
      float          label_size = 1.0,
      bool           center_heading = true
   )
   {
      return make_unframed_group(
         left_top_margin({ 10, 10 }, label(title, label_size)),
         std::forward<Content>(content), center_heading
      );
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Captions
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Content>
   inline auto caption(Content&& content, std::string const& title, float size = 1.0)
   {
      return
         vtile(
            std::forward<Content>(content),
            align_center(top_margin(5.0, label(title, size)))
         );
   }
}

#endif