/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_VIEW_AUGUST_15_2016)
#define PHOTON_GUI_LIB_VIEW_AUGUST_15_2016

#include <photon/host.hpp>
#include <photon/support/rect.hpp>
#include <photon/support/canvas.hpp>
#include <photon/support/theme.hpp>
#include <photon/element/element.hpp>
#include <photon/element/layer.hpp>
#include <functional>
#include <memory>

namespace photon
{
   struct context;

   class view : public base_view
   {
   public:
                           view(host_view* h);
                           view(view const&) = delete;
      view&                operator=(view const&) = delete;

      virtual void         draw(cairo_t* ctx, rect area) override;
      virtual void         click(mouse_button btn) override;
      virtual void         drag(mouse_button btn) override;
      virtual void         cursor(point p, cursor_tracking status) override;
      virtual void         scroll(point dir, point p) override;
      virtual void         key(key_info const& k) override;
      virtual void         text(text_info const& info) override;
      virtual void         focus(focus_request r) override;

      using base_view::refresh;

      void                 refresh(element& element);
      void                 refresh(context const& ctx);
      rect                 dirty() const { return _dirty; }

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

   private:

      bool                 set_limits();

      rect                 _dirty;
      rect                 _current_bounds;
      view_limits          _current_limits;

      using undo_stack_type = std::stack<undo_redo_task>;

      undo_stack_type      _undo_stack;
      undo_stack_type      _redo_stack;
   };
}

#endif
