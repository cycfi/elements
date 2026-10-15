/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/dock.hpp>
#include <elements/element/child_window.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>
#include <algorithm>
#include <cmath>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // dock_element
   ////////////////////////////////////////////////////////////////////////////
   dock_element::dock_element(element_ptr content)
    : _content{std::move(content)}
    , _middle{std::make_shared<hsplit_composite>()}
    , _left{std::make_shared<vsplit_composite>()}
    , _right{std::make_shared<vsplit_composite>()}
    , _top{std::make_shared<hsplit_composite>()}
    , _bottom{std::make_shared<hsplit_composite>()}
   {
      rebuild();
   }

   namespace
   {
      template <typename Side>
      bool erase_from(Side& side, element_ptr const& pane)
      {
         auto i = std::find(side.begin(), side.end(), pane);
         if (i == side.end())
            return false;
         side.erase(i);
         return true;
      }

      template <typename Side>
      bool holds(Side const& side, element_ptr const& pane)
      {
         return std::find(side.begin(), side.end(), pane) != side.end();
      }
   }

   void dock_element::add(element_ptr pane, dock_side side)
   {
      if (!pane || side_of(pane))
         return;
      switch (side)
      {
         case dock_side::left:   _left->push_back(pane); break;
         case dock_side::right:  _right->push_back(pane); break;
         case dock_side::top:    _top->push_back(pane); break;
         case dock_side::bottom: _bottom->push_back(pane); break;
      }
      rebuild();
   }

   bool dock_element::remove(element_ptr pane)
   {
      bool found = erase_from(*_left, pane) || erase_from(*_right, pane)
         || erase_from(*_top, pane) || erase_from(*_bottom, pane);
      if (found)
         rebuild();
      return found;
   }

   std::optional<dock_side> dock_element::side_of(element_ptr pane) const
   {
      if (holds(*_left, pane))
         return dock_side::left;
      if (holds(*_right, pane))
         return dock_side::right;
      if (holds(*_top, pane))
         return dock_side::top;
      if (holds(*_bottom, pane))
         return dock_side::bottom;
      return std::nullopt;
   }

   // The splits, rebuilt from what is docked. A split whose number of
   // children changes is shared out again (see split_element).
   void dock_element::rebuild()
   {
      _middle->clear();
      if (!_left->empty())
         _middle->push_back(_left);
      _middle->push_back(_content);
      if (!_right->empty())
         _middle->push_back(_right);

      clear();
      if (!_top->empty())
         push_back(_top);
      push_back(_middle);
      if (!_bottom->empty())
         push_back(_bottom);

      // Each docked side starts at a quarter of the dock, the content
      // takes the rest; the panes on one side share it evenly.
      auto side_shares = [](bool before, bool after)
      {
         std::vector<float> s;
         if (before)
            s.push_back(0.25f);
         s.push_back(1.0f - (before? 0.25f : 0) - (after? 0.25f : 0));
         if (after)
            s.push_back(0.25f);
         return s;
      };
      for (auto* s : {_left.get(), _right.get()})
         s->shares({});
      for (auto* s : {_top.get(), _bottom.get()})
         s->shares({});
      _middle->shares(side_shares(!_left->empty(), !_right->empty()));
      shares(side_shares(!_top->empty(), !_bottom->empty()));
   }

   void dock_element::preview(std::optional<dock_side> side)
   {
      _preview = side;
   }

   void dock_element::draw(context const& ctx)
   {
      vsplit_composite::draw(ctx);
      if (!_preview)
         return;

      // A third of the dock along the side, at most.
      auto r = ctx.bounds;
      auto const w = r.width() / 3;
      auto const h = r.height() / 3;
      switch (*_preview)
      {
         case dock_side::left:   r.right = r.left + w; break;
         case dock_side::right:  r.left = r.right - w; break;
         case dock_side::top:    r.bottom = r.top + h; break;
         case dock_side::bottom: r.top = r.bottom - h; break;
      }
      auto& cnv = ctx.canvas;
      cnv.fill_style(get_theme().indicator_hilite_color.opacity(0.25));
      cnv.add_rect(r);
      cnv.fill();
   }

   std::string dock_element::class_name() const
   {
      return "dock";
   }

   ////////////////////////////////////////////////////////////////////////////
   // dockable_base
   ////////////////////////////////////////////////////////////////////////////
   bool dockable_base::is_docked() const
   {
      auto d = _dock.lock();
      return d && d->side_of(
         std::const_pointer_cast<element>(shared_from_this()));
   }

   // Whether p is on a movable handle (a title) in the content.
   bool dockable_base::on_handle(context const& ctx, point p)
   {
      context sctx{ctx, &subject(), ctx.bounds};
      prepare_subject(sctx, p);
      auto e = subject().hit_test(sctx, p, true, true);
      restore_subject(sctx);
      return dynamic_cast<movable_base*>(e) != nullptr;
   }

   rect dockable_base::dock_bounds(context const& ctx) const
   {
      rect r;
      if (auto d = _dock.lock())
         ctx.view.in_context_do(*d,
            [&](context const& dctx) { r = dctx.bounds; });
      return r;
   }

   bool dockable_base::click(context const& ctx, mouse_button btn)
   {
      if (btn.down)
      {
         // Remember the window this content floats in, to undock into it.
         if (auto fl = find_parent<floating_element*>(ctx))
            _window = std::static_pointer_cast<floating_element>(
               fl->shared_from_this());
         _dragging = on_handle(ctx, btn.pos);
         _start = btn.pos;
         _side.reset();
      }

      auto r = proxy_base::click(ctx, btn);

      if (!btn.down && _dragging)
      {
         _dragging = false;
         auto d = _dock.lock();
         if (d && _side && _window && ctx.view.is_open(_window))
         {
            // Dock: out of the view's layers, into the dock.
            d->preview(std::nullopt);
            ctx.view.remove(_window);
            d->add(shared_from_this(), *_side);
            ctx.view.layout();
         }
         _side.reset();
      }
      return r;
   }

   void dockable_base::drag(context const& ctx, mouse_button btn)
   {
      proxy_base::drag(ctx, btn);
      if (!_dragging)
         return;

      auto d = _dock.lock();
      if (!d)
         return;

      if (is_docked())
      {
         // Undock, once the title is dragged out by more than gravity.
         auto const moved = std::hypot(btn.pos.x - _start.x, btn.pos.y - _start.y);
         if (moved <= gravity)
            return;
         _dragging = false;

         // Into its window, or, for a pane that was docked from the start,
         // a plain resizable one of the size it had in the dock.
         if (!_window)
         {
            _window = share(child_window(ctx.bounds,
               resizable(hold(shared_from_this()))));
         }
         d->remove(shared_from_this());

         // The window opens with its title under the pointer, as it was
         // grabbed in the dock.
         auto const off = point{_start.x - ctx.bounds.left
            , _start.y - ctx.bounds.top};
         _window->bounds(
            _window->bounds().move_to(btn.pos.x - off.x, btn.pos.y - off.y));
         ctx.view.add(_window);
         ctx.view.layout();

         // The drag goes on, in the window: press its title where the
         // pointer is, if the button is still down by then.
         auto& view_ = ctx.view;
         view_.post(
            [&view_]
            {
               auto b = view_.current_button();
               if (b.down)
                  view_.click(b);
            });
         return;
      }

      // Floating: pull an edge near the dock's to it, and show where the
      // window would dock.
      auto fl = find_parent<floating_element*>(ctx);
      if (!fl)
         return;
      auto const db = dock_bounds(ctx);
      auto b = fl->bounds();
      std::optional<dock_side> side;
      float best = gravity;
      auto consider = [&](float gap, dock_side s)
      {
         if (std::abs(gap) <= best)
         {
            best = std::abs(gap);
            side = s;
         }
      };
      // An edge of the window, or the pointer, near an edge of the dock.
      consider(b.left - db.left, dock_side::left);
      consider(db.right - b.right, dock_side::right);
      consider(b.top - db.top, dock_side::top);
      consider(db.bottom - b.bottom, dock_side::bottom);
      consider(btn.pos.x - db.left, dock_side::left);
      consider(db.right - btn.pos.x, dock_side::right);
      consider(btn.pos.y - db.top, dock_side::top);
      consider(db.bottom - btn.pos.y, dock_side::bottom);

      if (side)
      {
         switch (*side)
         {
            case dock_side::left:   b = b.move(db.left - b.left, 0); break;
            case dock_side::right:  b = b.move(db.right - b.right, 0); break;
            case dock_side::top:    b = b.move(0, db.top - b.top); break;
            case dock_side::bottom: b = b.move(0, db.bottom - b.bottom); break;
         }
         fl->bounds(b);
      }
      if (side != _side)
      {
         _side = side;
         d->preview(side);
      }
      ctx.view.refresh();
   }

   std::string dockable_base::class_name() const
   {
      return "dockable";
   }
}
