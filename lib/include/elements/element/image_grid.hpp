/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_IMAGE_GRID_OCTOBER_1_2026)
#define ELEMENTS_IMAGE_GRID_OCTOBER_1_2026

#include <elements/element/image.hpp>
#include <elements/element/proxy.hpp>
#include <elements/element/traversal.hpp>
#include <functional>
#include <memory>

namespace cycfi::elements
{
   /**
    * \class basic_image_grid
    *
    * \brief
    *    An image divided into equal cells, columns by rows, numbered left
    *    to right, then top to bottom: a picture per choice, all in one
    *    image.
    *
    *    The proxy's subject is a cell styler, drawn over each cell shown.
    *    It finds the grid with find_parent, and asks which cell it is
    *    drawing with drawing(), and that cell's state.
    *
    *    The current cell is a value of its own: value(i) sets it, and
    *    on_change says when the user picks another, so a model binder binds
    *    it as it binds any control.
    */
   class basic_image_grid : public proxy_base
   {
   public:

      using change_function = std::function<void(int)>;

      enum cell_state { normal, hot, current };

                              basic_image_grid(
                                 image_ptr img
                               , int columns, int rows);
                              basic_image_grid(
                                 fs::path const& path
                               , int columns, int rows);

      int                     columns() const   { return _columns; }
      int                     rows() const      { return _rows; }
      int                     size() const      { return _columns * _rows; }
      image_ptr               get_image() const { return _image; }

      // The current cell. Set as given, clamped; no callback is called.
      int                     value() const     { return _value; }
      void                    value(int i);

      // For the styler: the cell being drawn, and a cell's state
      int                     drawing() const   { return _drawing; }
      virtual cell_state      state(int i) const;

      change_function         on_change;

      std::string             class_name() const override;

   protected:

      // Draw cell i of the image into a rect, then its styler over it.
      void                    draw_cell(
                                 context const& ctx, int i, rect bounds);
      point                   cell_size() const;

   private:

      image_ptr               _image;
      int                     _columns;
      int                     _rows;
      int                     _value = 0;
      int                     _drawing = 0;
   };

   /**
    * \class basic_image_grid_menu
    *
    * \brief
    *    Every cell of an image grid, laid out as in the image, to pick one
    *    from: the cell under the cursor is hot, and a click picks it. In a
    *    popup, a pick closes the popup, as a menu item does.
    */
   class basic_image_grid_menu : public basic_image_grid
   {
   public:

                              basic_image_grid_menu(
                                 image_ptr img
                               , int columns, int rows, float scale = 1);
                              basic_image_grid_menu(
                                 fs::path const& path
                               , int columns, int rows, float scale = 1);

      cell_state              state(int i) const override;

      view_limits             limits(basic_context const& ctx) const override;
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

   private:

      int                     cell_at(context const& ctx, point p) const;
      rect                    cell_bounds(context const& ctx, int i) const;

      float                   _scale;
      int                     _hot = -1;
      int                     _pressed = -1;
   };

   /**
    * \class basic_image_grid_cell
    *
    * \brief
    *    The current cell of an image grid alone, scaled to fit the space it
    *    is given, its proportions kept: what was picked from the grid.
    */
   class basic_image_grid_cell : public basic_image_grid
   {
   public:

      using basic_image_grid::basic_image_grid;

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      std::string             class_name() const override;
   };

   /**
    * \brief
    *    Make an image grid menu: the image, its columns and rows, the
    *    scale it is drawn at, and the element drawn over each cell. See
    *    image_grid_highlight (elements/element/style/image_grid.hpp).
    */
   template <concepts::Element Styler, typename Image>
   inline proxy<remove_cvref_t<Styler>, basic_image_grid_menu>
   image_grid_menu(
      Image&& img, int columns, int rows, float scale, Styler&& styler)
   {
      return {
         std::forward<Styler>(styler)
       , std::forward<Image>(img), columns, rows, scale
      };
   }

   /**
    * \brief
    *    Make an image grid cell: the image, its columns and rows, and the
    *    element drawn over the cell. See image_regions
    *    (elements/element/style/image_grid.hpp).
    */
   template <concepts::Element Styler, typename Image>
   inline proxy<remove_cvref_t<Styler>, basic_image_grid_cell>
   image_grid_cell(Image&& img, int columns, int rows, Styler&& styler)
   {
      return {
         std::forward<Styler>(styler)
       , std::forward<Image>(img), columns, rows
      };
   }
}

#endif
