/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_VIEW_AUGUST_15_2016)
#define PHOTON_GUI_LIB_VIEW_AUGUST_15_2016

#include <photon/support/rect.hpp>
#include <photon/support/canvas.hpp>
#include <photon/support/theme.hpp>
#include <photon/widget/widget.hpp>
#include <photon/widget/layer.hpp>
#include <functional>
#include <memory>

namespace photon
{
   class view;
}

namespace client
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Main user initialization. This client supplied function initializes the view at
   // construction time. This is the main customization point provided for clients.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   struct init_view
   {
      typedef void (*init_view_function)(photon::view& v);
      init_view(init_view_function f);
   };
}

namespace photon
{
   struct view_impl;
   struct context;

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // The Application. There's one of this per view. The application provides the
   // client specific behavior and builds the view's content. The client typically
   // creates a subclass of this and assigns it to the view's app member.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class application
   {
   public:
                     application(view& view_)
                      : _view(view_)
                     {}

      virtual        ~application() {}

      view&          _view;
   };

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // The View. There's only one of this per window.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   using application_ptr = std::shared_ptr<application>;

   enum class cursor_type
   {
      arrow,
      ibeam,
      cross_hair,
      hand,
      h_resize,
      v_resize
   };

   class view
   {
   public:
                           view();
                           view(view const&) = delete;
      view&                operator=(view const&) = delete;

      void                 draw(rect dirty_);
      void                 refresh(widget& widget);
      point                cursor_pos() const;
      void                 click(mouse_button btn);
      void                 drag(mouse_button btn);
      void                 cursor(point p, cursor_tracking status);
      void                 set_cursor(cursor_type type);
      void                 scroll(point dir, point p);

      void                 key(key_info const& k);
      void                 text(text_info const& info);

      void                 focus(focus_request r);
      bool                 is_focus() const;

      point                size() const;
      void                 size(point size_);
      void                 limits(widget_limits limits_) const;

      void                 refresh();
      void                 refresh(context const& ctx);
      rect                 dirty() const                 { return _dirty; }

      bool                 maintain_aspect() const       { return _maintain_aspect; }
      void                 maintain_aspect(bool flag)    { _maintain_aspect = flag; }

      std::string          clipboard() const;
      void                 clipboard(std::string const& text) const;

      struct undo_redo_task
      {
         std::function<void()> undo;
         std::function<void()> redo;
      };

      void                 add_undo(undo_redo_task t);
      bool                 has_undo() { return !_undo_stack.empty(); }
      bool                 has_redo() { return !_redo_stack.empty(); }
      bool                 undo();
      bool                 redo();

      layer_composite      content;
      application_ptr      app;

   private:

      cairo_t*             setup_context();
      void                 set_limits(basic_context& bctx);

      friend class platform_view_access;

      view_impl*           _impl;
      rect                 _dirty;
      rect                 _current_bounds;
      bool                 _maintain_aspect;

      using undo_stack_type = std::stack<undo_redo_task>;

      undo_stack_type      _undo_stack;
      undo_stack_type      _redo_stack;
   };
}

#endif