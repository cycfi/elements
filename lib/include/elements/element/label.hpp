/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_LABELS_APRIL_11_2016)
#define ELEMENTS_LABELS_APRIL_11_2016

#include <elements/element/element.hpp>
#include <elements/element/text.hpp>
#include <elements/support/theme.hpp>
#include <elements/support/font.hpp>
#include <infra/string_view.hpp>
#include <string>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Basic label
   ////////////////////////////////////////////////////////////////////////////
   struct default_label : element, text_reader
   {
      public:

      using font_type = elements::font const&;
      using remove_gen = default_label;

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      virtual font_type       font() const;
      virtual float           font_size() const;
      virtual color           font_color() const;
      virtual int             text_align() const;
   };

   template <typename Base>
   struct basic_label_base : Base, text_writer
   {
      using text_type = std::string const&;
      using remove_gen = basic_label_base<typename Base::remove_gen>;

                              basic_label_base(std::string text)
                               : _text(std::move(text))
                              {}

      text_type               get_text() const override           { return _text; }
      void                    set_text(string_view text) override { _text = std::string(text); }

   private:

      std::string             _text;
   };

   template <typename Base>
   struct label_with_font : Base
   {
      using font_type = elements::font const&;
      using remove_gen = label_with_font<typename Base::remove_gen>;

                              label_with_font(Base base, elements::font font_)
                               : Base(std::move(base)), _font(std::move(font_))
                              {}

      font_type               font() const override { return _font; }

   private:

      elements::font         _font;
   };

   template <typename Base>
   struct label_with_font_size : Base
   {
      using remove_gen = label_with_font_size<typename Base::remove_gen>;

                              label_with_font_size(Base base, float size)
                               : Base(std::move(base)), _size(size)
                              {}

      float                   font_size() const override { return _size; }

   private:

      float                   _size;
   };

   template <typename Base>
   struct label_with_font_color : Base
   {
      using remove_gen = label_with_font_color<typename Base::remove_gen>;

                              label_with_font_color(Base base, color color_)
                               : Base(std::move(base)), _color(color_)
                              {}

      color                   font_color() const override { return _color; }

   private:

      color                   _color;
   };

   template <typename Base>
   struct label_with_text_align : Base
   {
      using remove_gen = label_with_text_align<typename Base::remove_gen>;

                              label_with_text_align(Base base, int align)
                               : Base(std::move(base)), _align(align)
                              {}

      int                     text_align() const override { return _align; }

   private:

      int                     _align;
   };

   template <typename Base>
   struct label_gen : Base
   {
      using Base::Base;
      using remove_gen = typename Base::remove_gen;

      using font_type      = elements::font const&;
      using gen_font       = label_gen<label_with_font<remove_gen>>;
      using gen_font_size  = label_gen<label_with_font_size<remove_gen>>;
      using gen_font_color = label_gen<label_with_font_color<remove_gen>>;
      using gen_text_align = label_gen<label_with_text_align<remove_gen>>;

      gen_font                font(font_type font_) const;
      gen_font_size           font_size(float size) const;
      gen_font_size           relative_font_size(float size) const;
      gen_font_color          font_color(color color_) const;
      gen_text_align          text_align(int align) const;
   };

   using basic_label = basic_label_base<default_label>;
   using label = label_gen<basic_label>;

   inline default_label::font_type default_label::font() const
   {
      return get_theme().label_font;
   }

   inline float default_label::font_size() const
   {
      return get_theme().label_font_size;
   }

   inline color default_label::font_color() const
   {
      return get_theme().label_font_color;
   }

   inline int default_label::text_align() const
   {
      return get_theme().label_text_align;
   }

   template <typename Base>
   inline typename label_gen<Base>::gen_font
   label_gen<Base>::font(font_type font_) const
   {
      return { *this, font_ };
   }

   template <typename Base>
   inline typename label_gen<Base>::gen_font_size
   label_gen<Base>::font_size(float size) const
   {
      return { *this, size };
   }

   template <typename Base>
   inline typename label_gen<Base>::gen_font_size
   label_gen<Base>::relative_font_size(float size) const
   {
      return { *this, get_theme().label_font_size * size };
   }

   template <typename Base>
   inline typename label_gen<Base>::gen_font_color
   label_gen<Base>::font_color(color color_) const
   {
      return { *this, color_ };
   }

   template <typename Base>
   inline typename label_gen<Base>::gen_text_align
   label_gen<Base>::text_align(int align) const
   {
      return { *this, align };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Headings
   ////////////////////////////////////////////////////////////////////////////
   struct default_heading : default_label
   {
      using font_type = elements::font const&;
      using remove_gen = default_heading;

      font_type               font() const override;
      float                   font_size() const override;
      color                   font_color() const override;
      int                     text_align() const override;
   };

   using basic_heading = basic_label_base<default_heading>;
   using heading = label_gen<basic_heading>;

   inline default_heading::font_type default_heading::font() const
   {
      return get_theme().heading_font;
   }

   inline float default_heading::font_size() const
   {
      return get_theme().heading_font_size;
   }

   inline color default_heading::font_color() const
   {
      return get_theme().heading_font_color;
   }

   inline int default_heading::text_align() const
   {
      return get_theme().heading_text_align;
   }
}}

#endif
