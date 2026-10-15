/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DOCK_OCTOBER_10_2026)
#define ELEMENTS_DOCK_OCTOBER_10_2026

#include <elements/element/split.hpp>
#include <elements/element/floating.hpp>
#include <memory>
#include <optional>

namespace cycfi::elements
{
   enum class dock_side { left, top, right, bottom };

   ////////////////////////////////////////////////////////////////////////////
   // A dock: content, with panes docked along its four sides. Panes on one
   // side share it; every boundary, between panes and between the sides and
   // the content, is a split boundary the user drags. Docking or undocking
   // a pane shares its side out again.
   ////////////////////////////////////////////////////////////////////////////
   class dock_element : public vsplit_composite
   {
   public:

      explicit                dock_element(element_ptr content);

      void                    add(element_ptr pane, dock_side side);
      bool                    remove(element_ptr pane);
      std::optional<dock_side> side_of(element_ptr pane) const;

      // Show where a pane would dock; nullopt hides it.
      void                    preview(std::optional<dock_side> side);
      std::optional<dock_side> preview() const { return _preview; }

      void                    draw(context const& ctx) override;
      std::string             class_name() const override;

   private:

      void                    rebuild();

      element_ptr             _content;
      std::shared_ptr<hsplit_composite> _middle;
      std::shared_ptr<vsplit_composite> _left, _right;
      std::shared_ptr<hsplit_composite> _top, _bottom;
      std::optional<dock_side> _preview;
   };

   template <concepts::Element Content>
   inline dock_element dock(Content&& content)
   {
      return dock_element{share(std::forward<Content>(content))};
   }

   ////////////////////////////////////////////////////////////////////////////
   // Makes a child window dockable. A drag of its title (a movable handle)
   // that brings an edge of the window within `gravity` of the same edge of
   // the dock snaps the window to it and shows where it would dock; releasing
   // there docks the content. Dragging the title of a docked pane out by
   // more than `gravity` undocks it into its window, where the pointer is.
   ////////////////////////////////////////////////////////////////////////////
   class dockable_base : public proxy_base
   {
   public:

      explicit             dockable_base(std::shared_ptr<dock_element> dock)
                            : _dock{std::move(dock)}
                           {}

      bool                 click(context const& ctx, mouse_button btn) override;
      void                 drag(context const& ctx, mouse_button btn) override;
      std::string          class_name() const override;

      bool                 is_docked() const;

      // How near an edge must come to the dock's to be pulled to it.
      float                gravity = 16;

   private:

      bool                 on_handle(context const& ctx, point p);
      rect                 dock_bounds(context const& ctx) const;

      std::weak_ptr<dock_element>   _dock;
      std::shared_ptr<floating_element> _window;
      bool                 _dragging = false;
      point                _start;
      std::optional<dock_side> _side;
   };

   template <concepts::Element Content>
   inline proxy<remove_cvref_t<Content>, dockable_base>
   dockable(std::shared_ptr<dock_element> dock, Content&& content)
   {
      return {std::forward<Content>(content), std::move(dock)};
   }
}

#endif
