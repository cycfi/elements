/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_WIDGET_TEXT_APRIL_17_2016)
#define CYCFI_PHOTON_GUI_LIB_WIDGET_TEXT_APRIL_17_2016

#include <elemental/support/glyphs.hpp>
#include <elemental/support/theme.hpp>
#include <elemental/element/element.hpp>
#include <boost/asio.hpp>
#include <string>
#include <vector>

namespace cycfi { namespace elemental
{
   ////////////////////////////////////////////////////////////////////////////
   // Static Text Box
   ////////////////////////////////////////////////////////////////////////////
   class static_text_box : public element
   {
   public:

      using element::value;

                              static_text_box(
                                 std::string const& text
                               , char const* face  = get_theme().text_box_font
                               , float size        = get_theme().text_box_font_size
                               , color color_      = get_theme().text_box_font_color
                               , int style         = canvas::normal
                              );

                              static_text_box(static_text_box&& rhs) = default;

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            layout(context const& ctx);
      virtual void            draw(context const& ctx);

      std::string const&      text() const                     { return _text; }
      virtual void            text(std::string const& text);
      virtual void            value(std::string val);

      using element::text;

   private:

      void                    sync() const;

   protected:

      std::string             _text;
      mutable master_glyphs   _layout;
      std::vector<glyphs>     _rows;
      color                   _color;
      point                   _current_size = { -1, -1 };
   };

   ////////////////////////////////////////////////////////////////////////////
   // Editable Text Box
   ////////////////////////////////////////////////////////////////////////////
   class basic_text_box : public static_text_box
   {
   public:
                              basic_text_box(
                                 std::string const& text
                               , char const* face  = get_theme().text_box_font
                               , float size        = get_theme().text_box_font_size
                              );
                              ~basic_text_box();
                              basic_text_box(basic_text_box&& rhs) = default;

      virtual void            draw(context const& ctx);
      virtual element*        click(context const& ctx, mouse_button btn);
      virtual void            drag(context const& ctx, mouse_button btn);
      virtual bool            cursor(context const& ctx, point p, cursor_tracking status);
      virtual bool            text(context const& ctx, text_info info);
      virtual void            text(std::string const& text);
      virtual bool            key(context const& ctx, key_info k);
      virtual bool            focus(focus_request r);
      virtual bool            is_control() const;

      using element::focus;
      using static_text_box::text;

      int                     select_start() const    { return _select_start; }
      void                    select_start(int pos);
      int                     select_end() const      { return _select_end; }
      void                    select_end(int pos);
      void                    select_all();
      void                    select_none();

      virtual void            draw_selection(context const& ctx);
      virtual void            draw_caret(context const& ctx);
      virtual bool            word_break(char const* utf8) const;
      virtual bool            line_break(char const* utf8) const;

   protected:

      void                    scroll_into_view(context const& ctx, bool save_x);

   private:

      struct glyph_metrics
      {
         char const* str;           // The start of the utf8 string
         point       pos;           // Position where glyph is drawn
         rect        bounds;        // Glyph bounds
         float       line_height;   // Line height
      };

      char const*             caret_position(context const& ctx, point p);
      glyph_metrics           glyph_info(context const& ctx, char const* s);

      virtual void            delete_();
      virtual void            cut(view& v, int start, int end);
      virtual void            copy(view& v, int start, int end);
      virtual void            paste(view& v, int start, int end);

      struct state_saver;
      using state_saver_f = std::function<void()>;

      state_saver_f           capture_state();

      int                     _select_start;
      int                     _select_end;
      float                   _current_x;
      state_saver_f           _typing_state;
      bool                    _is_focus : 1;
      bool                    _show_caret : 1;
      bool                    _caret_started : 1;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Input Text Box
   ////////////////////////////////////////////////////////////////////////////
   class basic_input_box : public basic_text_box
   {
   public:

      using basic_text_box::text;

      using text_function = std::function<std::string(std::string const& text)>;
      using enter_function = std::function<bool(std::string const& text)>;

                              basic_input_box(std::string const& placeholder)
                               : basic_text_box("")
                               , _placeholder(placeholder)
                              {}

                              basic_input_box(basic_input_box&& rhs) = default;

      virtual view_limits     limits(basic_context const& ctx) const;
      virtual void            draw(context const& ctx);
      virtual bool            text(context const& ctx, text_info info);
      virtual bool            key(context const& ctx, key_info k);

      text_function           on_text;
      enter_function          on_enter;

   private:

      virtual void            paste(view& v, int start, int end);

      std::string             _placeholder;
   };
}}

#endif