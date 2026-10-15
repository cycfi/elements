/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_SWATCH_GRID_OCTOBER_14_2026)
#define ELEMENTS_SWATCH_GRID_OCTOBER_14_2026

#include <elements/element/proxy.hpp>
#include <elements/element/swatches.hpp>
#include <elements/element/traversal.hpp>
#include <functional>
#include <memory>

namespace cycfi::elements
{
   /**
    * \class basic_swatch_grid
    *
    * \brief
    *    Content cut into equal cells, columns by rows, numbered left to
    *    right, then top to bottom: swatches to pick one from. The content,
    *    the proxy's subject, is any element: an image, a palette drawn by
    *    code, a row of icons.
    *
    *    Over each cell, a highlighter is drawn: an element that finds the
    *    grid with find_parent, as basic_swatches, and asks which cell it is
    *    drawing, and its state. The cell under the pointer is hot, and a
    *    press and a release on a cell pick it. In a popup, a pick closes
    *    the popup, as a menu item does.
    *
    *    The current cell is a value of its own: value(i) sets it, and
    *    on_change says when the user picks another, so a model binder binds
    *    it as it binds any control.
    */
   class basic_swatch_grid : public proxy_base, public basic_swatches
   {
   public:

      using change_function = std::function<void(int)>;

                              basic_swatch_grid(
                                 element_ptr highlighter
                               , int columns, int rows);

      int                     columns() const   { return _columns; }
      int                     rows() const      { return _rows; }
      int                     size() const      { return _columns * _rows; }

      // The current cell. Set as given, clamped; no callback is called.
      int                     value() const override { return _value; }
      void                    value(int i);

      int                     drawing() const override { return _drawing; }
      swatch_state            state(int i) const override;

      change_function         on_change;

      void                    draw(context const& ctx) override;
      element*                hit_test(
                                 context const& ctx, point p
                               , bool leaf, bool control) override;
      bool                    click(
                                 context const& ctx
                               , mouse_button btn) override;
      bool                    cursor(
                                 context const& ctx, point p
                               , cursor_tracking status) override;
      bool                    wants_control() const override { return true; }
      std::string             class_name() const override;

   protected:

      // Draw the highlighter over cell i, at the given bounds
      void                    highlight(context const& ctx, int i, rect bounds);

   private:

      int                     cell_at(context const& ctx, point p) const;
      rect                    cell_bounds(context const& ctx, int i) const;

      element_ptr             _highlighter;
      int                     _columns;
      int                     _rows;
      int                     _value = 0;
      int                     _drawing = 0;
      int                     _hot = -1;
      int                     _pressed = -1;
   };

   /**
    * \class basic_swatch_cell
    *
    * \brief
    *    The current cell of a swatch grid's content alone, scaled to fit
    *    the space it is given, its proportions kept: what was picked from
    *    the grid. It has no size of its own, and takes no input.
    */
   class basic_swatch_cell : public basic_swatch_grid
   {
   public:

      using basic_swatch_grid::basic_swatch_grid;

      swatch_state            state(int i) const override;

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      element*                hit_test(
                                 context const& ctx, point p
                               , bool leaf, bool control) override;
      bool                    wants_control() const override { return false; }
      std::string             class_name() const override;
   };

   template <concepts::Element Content>
   using swatch_grid_t = proxy<remove_cvref_t<Content>, basic_swatch_grid>;

   template <concepts::Element Content>
   using swatch_cell_t = proxy<remove_cvref_t<Content>, basic_swatch_cell>;

   /**
    * \brief
    *    Make a swatch grid: its columns and rows, its content, and the
    *    highlighter drawn over each cell. See swatch_highlighter
    *    (elements/element/style/swatches.hpp).
    */
   template <concepts::Element Content, concepts::Element Highlighter>
   inline swatch_grid_t<Content>
   swatch_grid(int columns, int rows, Content&& content, Highlighter&& h)
   {
      return {
         std::forward<Content>(content)
       , share(std::forward<Highlighter>(h)), columns, rows
      };
   }

   /**
    * \brief
    *    Make a swatch cell: the current cell alone of content cut into
    *    columns and rows, and the highlighter drawn over it.
    */
   template <concepts::Element Content, concepts::Element Highlighter>
   inline swatch_cell_t<Content>
   swatch_cell(int columns, int rows, Content&& content, Highlighter&& h)
   {
      return {
         std::forward<Content>(content)
       , share(std::forward<Highlighter>(h)), columns, rows
      };
   }
}

#endif
