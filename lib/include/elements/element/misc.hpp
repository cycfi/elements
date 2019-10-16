/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MISC_APRIL_11_2016)
#define ELEMENTS_MISC_APRIL_11_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/support/theme.hpp>
#include <functional>
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

      void draw(context const& ctx)
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

      virtual void
      draw(context const& ctx)
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
   // Background Fill
   ////////////////////////////////////////////////////////////////////////////
   struct background_fill : element
   {
                     background_fill(color color_)
                      : _color(color_)
                     {}

      void           draw(context const& ctx);
      color          _color;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Panels
   ////////////////////////////////////////////////////////////////////////////
   class panel : public element
   {
   public:

                     panel(float opacity_ = get_theme().panel_color.alpha)
                      : _opacity(opacity_)
                     {}

      virtual void   draw(context const& ctx);

   private:

      float          _opacity;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Frames
   ////////////////////////////////////////////////////////////////////////////
   struct frame : public element
   {
      virtual void   draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////
   // Headings
   ////////////////////////////////////////////////////////////////////////////
   struct heading : element
   {
                              heading(
                                 std::string_view text
                               , float size_ = 1.0
                              );

                              heading(
                                 std::string_view text
                               , std::string_view font
                               , float size = 1.0
                              );

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

      std::string             text() const                     { return _text; }
      void                    text(std::string const& text)    { _text = text; }

      using element::text;

      std::string             _text;
      std::string             _font;
      float                   _size;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Title Bars
   ////////////////////////////////////////////////////////////////////////////
   class title_bar : public element
   {
   public:

      virtual void            draw(context const& ctx);
   };

   ////////////////////////////////////////////////////////////////////////////
   // Labels
   ////////////////////////////////////////////////////////////////////////////
   class label : public element
   {
   public:
                              label(
                                 std::string_view text
                               , float size = 1.0
                              );

                              label(
                                 std::string_view text
                               , std::string_view font
                               , float size = 1.0
                              );

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

      std::string             text() const                        { return _text; }
      void                    text(std::string_view text)         { _text = text; }

      std::string             font() const                        { return _font; }
      void                    font(std::string_view font_)        { _font = font_; }

      float                   size() const                        { return _size; }
      void                    size(float size_)                   { _size = size_; }

      using element::text;

   private:

      std::string             _text;
      std::string             _font;
      float                   _size;
   };

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

      virtual void            draw(context const& ctx);

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

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);

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

      virtual bool            key(context const& ctx, key_info k);
      virtual bool            is_control() const { return true; }

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
      return on_key(k);
   }
}}

#endif