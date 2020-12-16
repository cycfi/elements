/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TEXT_APRIL_17_2016)
#define ELEMENTS_TEXT_APRIL_17_2016

#include <elements/support/glyphs.hpp>
#include <elements/support/theme.hpp>
#include <elements/element/element.hpp>

#include <infra/string_view.hpp>
#include <string>
#include <vector>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // text_reader and text_writer mixins
   ////////////////////////////////////////////////////////////////////////////
   class text_reader
   {
   public:

      virtual                    ~text_reader() = default;
      virtual std::string const& get_text() const = 0;
      char const*                c_str() const { return get_text().c_str(); }
   };

   class text_writer
   {
   public:

      virtual                    ~text_writer() = default;
      virtual void               set_text(string_view text) = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Static Text Box
   ////////////////////////////////////////////////////////////////////////////
   class static_text_box
    : public element
    , public text_reader
    , public text_writer
    , public receiver<std::string>
   {
   public:
                              static_text_box(
                                 std::string text
                               , font font_        = get_theme().text_box_font
                               , float size        = get_theme().text_box_font_size
                               , color color_      = get_theme().text_box_font_color
                              );

                              static_text_box(static_text_box&& rhs) = default;

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      void                    draw(context const& ctx) override;

      std::string const&      get_text() const override            { return _text; }
      void                    set_text(string_view text) override;

      std::string const&      value() const override           { return _text; }
      void                    value(string_view val) override;

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
                                 std::string text
                               , font font_        = get_theme().text_box_font
                               , float size        = get_theme().text_box_font_size
                              );
                              ~basic_text_box();
                              basic_text_box(basic_text_box&& rhs) = default;

      void                    draw(context const& ctx) override;
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    wants_focus() const override;
      void                    begin_focus() override;
      void                    end_focus() override;
      bool                    wants_control() const override;

      bool                    text(context const& ctx, text_info info) override;
      void                    set_text(string_view text) override;

      using element::focus;
      using static_text_box::get_text;

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
      virtual void            delete_(bool forward);
      virtual void            cut(view& v, int start, int end);
      virtual void            copy(view& v, int start, int end);
      virtual void            paste(view& v, int start, int end);

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

      using basic_text_box::get_text;

      using text_function = std::function<void(string_view text)>;

                              basic_input_box(
                                 std::string placeholder = ""
                               , font font_        = get_theme().text_box_font
                               , float size        = get_theme().text_box_font_size
                              )
                               : basic_text_box("", font_, size)
                               , _placeholder(std::move(placeholder))
                              {}

                              basic_input_box(basic_input_box&& rhs) = default;

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      bool                    text(context const& ctx, text_info info) override;
      bool                    key(context const& ctx, key_info k) override;
      void                    delete_(bool forward) override;

      bool                    click(context const& ctx, mouse_button btn) override;
      void                    begin_focus() override;
      void                    end_focus() override;

      text_function           on_text;
      text_function           on_enter;

   private:

      void                    paste(view& v, int start, int end) override;

      std::string             _placeholder;
      bool                    _first_focus;
   };
}}

#endif
