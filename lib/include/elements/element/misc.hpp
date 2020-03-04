/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MISC_APRIL_11_2016)
#define ELEMENTS_MISC_APRIL_11_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/text.hpp>
#include <elements/support/theme.hpp>
#include <elements/support/font.hpp>
#include <functional>
#include <string>
#include <string_view>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Box: A simple colored box.
   ////////////////////////////////////////////////////////////////////////////
   struct box_element : element
   {
      box_element(color color_)
       : _color(color_)
      {}

      void draw(context const& ctx) override
      {
         auto& cnv = ctx.canvas;
         cnv.fill_style(_color);
         cnv.fill_rect(ctx.bounds);
      }

      color _color;
   };

   inline auto box(color color_)
   {
      return box_element{ color_ };
   }

   ////////////////////////////////////////////////////////////////////////////
   // RBox: A simple colored rounded-box.
   ////////////////////////////////////////////////////////////////////////////
   struct rbox_element : element
   {
      rbox_element(color color_, float radius = 4)
       : _color(color_)
       , _radius(radius)
      {}

      void draw(context const& ctx) override
      {
         auto& cnv = ctx.canvas;
         cnv.begin_path();
         cnv.round_rect(ctx.bounds, _radius);
         cnv.fill_style(_color);
         cnv.fill();
      }

      color _color;
      float _radius;
   };

   inline auto rbox(color color_, float radius = 4)
   {
      return rbox_element{ color_, radius };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Basic Element
   //
   // The basic element takes in a function that draws something
   ////////////////////////////////////////////////////////////////////////////
   template <typename F>
   class basic_element : public element
   {
   public:

      basic_element(F f)
       : f(f)
      {}

      void
      draw(context const& ctx) override
      {
         f(ctx);
      }

   private:

      F f;
   };

   template <typename F>
   inline basic_element<F> basic(F f)
   {
      return { f };
   }

   ////////////////////////////////////////////////////////////////////////////
   // Panels
   ////////////////////////////////////////////////////////////////////////////
   class panel : public element
   {
   public:

                     panel(float opacity_ = get_theme().panel_color.alpha)
                      : _opacity(opacity_)
                     {}

      void           draw(context const& ctx) override;

   private:

      float          _opacity;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Frames
   ////////////////////////////////////////////////////////////////////////////
   struct frame : public element
   {
      void           draw(context const& ctx) override;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Basic label
   ////////////////////////////////////////////////////////////////////////////
   struct default_label : element, read_only_text_base
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
   struct basic_label_base : Base, virtual text_base
   {
      using text_type = std::string const&;
      using remove_gen = basic_label_base<Base>;

                              basic_label_base(std::string text)
                               : _text(text)
                              {}

      virtual text_type       get_text() const                  { return _text; }
      virtual void            set_text(std::string_view text)   { _text = text; }

   private:

      std::string             _text;
   };

   template <typename Base>
   struct label_with_font : Base
   {
      using font_type = elements::font const&;
      using remove_gen = label_with_font<Base>;

                              label_with_font(Base const& base, font font_)
                               : Base(base), _font(font_)
                              {}

      virtual font_type       font() const { return _font; }

   private:

      elements::font         _font;
   };

   template <typename Base>
   struct label_with_font_size : Base
   {
      using remove_gen = label_with_font_size<Base>;

                              label_with_font_size(Base const& base, float size)
                               : Base(base), _size(size)
                              {}

      virtual float           font_size() const { return _size; }

   private:

      float                   _size;
   };

   template <typename Base>
   struct label_with_font_color : Base
   {
      using remove_gen = label_with_font_color<Base>;

                              label_with_font_color(Base const& base, color color_)
                               : Base(base), _color(color_)
                              {}

      virtual color           font_color() const { return _color; }

   private:

      color                   _color;
   };

   template <typename Base>
   struct label_with_text_align : Base
   {
      using remove_gen = label_with_text_align<Base>;

                              label_with_text_align(Base const& base, int align)
                               : Base(base), _align(align)
                              {}

      virtual int             text_align() const { return _align; }

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

      virtual font_type       font() const;
      virtual float           font_size() const;
      virtual color           font_color() const;
      virtual int             text_align() const;
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

   ////////////////////////////////////////////////////////////////////////////
   // Title Bars
   ////////////////////////////////////////////////////////////////////////////
   class title_bar : public element
   {
   public:

      void                    draw(context const& ctx) override;
   };

   inline void title_bar::draw(context const& ctx)
   {
      draw_box_vgradient(ctx.canvas, ctx.bounds, 4.0);
   }

   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines
   ////////////////////////////////////////////////////////////////////////////
   class vgrid_lines : public element
   {
   public:

                              vgrid_lines(float major_divisions, float minor_divisions)
                               : _major_divisions(major_divisions)
                               , _minor_divisions(minor_divisions)
                              {}

      void                    draw(context const& ctx) override;

   private:

      float                   _major_divisions;
      float                   _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Icons
   ////////////////////////////////////////////////////////////////////////////
   struct icon : element
   {
                              icon(std::uint32_t code_, float size_ = 1.0);

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;

      std::uint32_t           _code;
      float                   _size;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Key Intercept
   ////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   struct key_intercept_element : public proxy<Subject>
   {
      using base_type = proxy<Subject>;

                              key_intercept_element(Subject&& subject)
                               : base_type(std::forward<Subject>(subject))
                              {}

      bool                    key(context const& ctx, key_info k) override;
      bool                    wants_control() const override { return true; }
      bool                    wants_focus() const override { return true; }

      using key_function = std::function<bool(key_info k)>;

      key_function            on_key = [](auto){ return false; };
   };

   template <typename Subject>
   inline key_intercept_element<Subject>
   key_intercept(Subject&& subject)
   {
      return { std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline bool key_intercept_element<Subject>::key(context const& ctx, key_info k)
   {
      if (on_key(k))
         return true;
      return this->subject().key(ctx, k);
   }
}}

#endif
