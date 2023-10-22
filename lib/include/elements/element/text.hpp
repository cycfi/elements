/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TEXT_APRIL_17_2016)
#define ELEMENTS_TEXT_APRIL_17_2016

#include <elements/element/element.hpp>
#include <elements/support/theme.hpp>
#include <elements/support/receiver.hpp>
#include <artist/text_layout.hpp>

#include <infra/string_view.hpp>
#include <string>
#include <vector>
#include <set>

namespace cycfi::elements
{
   using artist::font_descr;
   using artist::font;

   ////////////////////////////////////////////////////////////////////////////
   // text_reader and text_writer mixins
   ////////////////////////////////////////////////////////////////////////////
   class text_reader_u8
   {
   public:

      virtual                       ~text_reader_u8() = default;
      virtual std::string_view      get_text() const = 0;
   };

   class text_writer
   {
   public:

      virtual                       ~text_writer() = default;
      virtual void                  set_text(string_view text) = 0;
   };

   class text_reader_u32
   {
   public:

      virtual                       ~text_reader_u32() = default;
      virtual std::u32string_view   get_text() const = 0;
   };

   class text_writer_u32
   {
   public:

      virtual                       ~text_writer_u32() = default;
      virtual void                  set_text(std::u32string_view text) = 0;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Static Text Box
   ////////////////////////////////////////////////////////////////////////////
   class static_text_box
    : public element
    , public text_reader_u32
    , public text_writer_u32
    , public receiver<std::u32string_view>
   {
   public:

      using text_layout_const = artist::text_layout const;

                              static_text_box(
                                 std::string_view text
                               , font_descr font_  = get_theme().text_box_font
                               , color color_      = get_theme().text_box_font_color
                              );

                              static_text_box(static_text_box&& rhs) = default;

      view_limits             limits(basic_context const& ctx) const override;
      void                    layout(context const& ctx) override;
      void                    draw(context const& ctx) override;

      std::u32string_view     get_text() const override           { return _layout.text(); }
      void                    set_text(std::u32string_view text) override;
      void                    set_text(std::string_view text);
      font const&             get_font() const { return _font; }

      std::u32string_view     value() const override              { return _layout.text(); }
      void                    value(std::u32string_view val) override;

      std::size_t             insert(std::size_t pos, std::string_view text);
      std::size_t             replace(std::size_t pos, std::size_t len, std::string_view text);
      void                    erase(std::size_t pos, std::size_t len);
      text_layout_const&      get_layout() const { return _layout; }

      point                   current_size() const { return _current_size; };

   private:

      void                    sync() const;

      class font              _font;
      artist::text_layout     _layout;
      color                   _color;
      point                   _current_size = {-1, -1};
   };

   ////////////////////////////////////////////////////////////////////////////
   // Editable Text Box
   ////////////////////////////////////////////////////////////////////////////
   class basic_text_box : public static_text_box
   {
   public:

      using static_text_box::set_text;

                              basic_text_box(
                                 std::string_view text
                               , font_descr font_ = get_theme().text_box_font
                              );
                              ~basic_text_box();
                              basic_text_box(basic_text_box&& rhs) = default;

      void                    draw(context const& ctx) override;
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    wants_focus() const override;
      void                    begin_focus(focus_request req = restore_previous) override;
      void                    end_focus() override;
      bool                    wants_control() const override;

      bool                    text(context const& ctx, text_info info) override;
      void                    set_text(std::u32string_view text) override;

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
      virtual bool            word_break(int index) const;
      virtual bool            line_break(int index) const;

   protected:

      void                    scroll_into_view(context const& ctx, bool save_x);
      virtual void            delete_(bool forward);
      virtual void            cut(view& v, int start, int end);
      virtual void            copy(view& v, int start, int end);
      virtual void            paste(view& v, int start, int end);

   private:

      struct caret_metrics
      {
         char32_t const*      str;           // The start of the string
         point                pos;           // Position where glyph is drawn
         rect                 caret;         // Caret bounds
         float                line_height;   // Line height
      };

      char32_t const*         caret_position(context const& ctx, point p);
      caret_metrics           caret_info(context const& ctx, char32_t const* s);

      struct state_saver;
      using state_saver_f = std::function<void()>;
      using state_saver_ptr = std::shared_ptr<state_saver>;
      using state_saver_set = std::set<state_saver_ptr>;

      state_saver_f           capture_state();

      int                     _select_start;
      int                     _select_end;
      float                   _current_x;
      state_saver_f           _typing_state;
      bool                    _is_focus : 1;
      bool                    _show_caret : 1;
      bool                    _caret_started : 1;
      state_saver_set         _state_savers;
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
                               , font_descr font_ = get_theme().text_box_font
                              )
                               : basic_text_box("", font_)
                               , _placeholder(std::move(placeholder))
                              {}

                              basic_input_box(basic_input_box&& rhs) = default;

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      bool                    text(context const& ctx, text_info info) override;
      bool                    key(context const& ctx, key_info k) override;
      void                    delete_(bool forward) override;

      bool                    click(context const& ctx, mouse_button btn) override;
      void                    begin_focus(focus_request req = restore_previous) override;
      void                    end_focus() override;

      text_function           on_text;
      text_function           on_enter;

   private:

      void                    paste(view& v, int start, int end) override;

      std::string             _placeholder;
      bool                    _first_focus;
   };
}

#endif
