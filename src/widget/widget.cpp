/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/widget.hpp>
#include <photon/window.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // widget class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////

   rect widget::limits(basic_context const& ctx) const
   {
      return full_limits;
   }

   widget* widget::hit_test(context const& ctx, point const& p)
   {
      return (ctx.bounds.includes(p)) ? this : 0;
   }

   void widget::draw(context const& ctx)
   {
   }

   void widget::layout(context const& ctx)
   {
   }

   widget* widget::click(context const& ctx, mouse_button btn)
   {
      return 0;
   }

   void widget::drag(context const& ctx, mouse_button btn)
   {
   }

   bool widget::key(context const& ctx, key_info const& k)
   {
      return false;
   }

   bool widget::text(context const& ctx, text_info const& info)
   {
      return false;
   }

   bool widget::cursor(context const& ctx, point const& p)
   {
      return false;
   }

   bool widget::scroll(context const& ctx, point const& p)
   {
      return false;
   }

   bool widget::focus(focus_request r)
   {
      return r == focus_request::end_focus;
   }

   widget const* widget::focus() const
   {
      return this;
   }

   widget* widget::focus()
   {
      return this;
   }

   bool widget::is_control() const
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // proxy class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect proxy_base::limits(basic_context const& ctx) const
   {
      return subject().limits(ctx);
   }

   widget* proxy_base::hit_test(context const& ctx, point const& p)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      return subject().hit_test(sctx, p);
   }

   void proxy_base::draw(context const& ctx)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      subject().draw(sctx);
   }

   void proxy_base::layout(context const& ctx)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      subject().layout(sctx);
   }

   void proxy_base::prepare_subject(context& ctx)
   {
   }

   widget* proxy_base::click(context const& ctx, mouse_button btn)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      return subject().click(sctx, btn);
   }

   void proxy_base::drag(context const& ctx, mouse_button btn)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      subject().drag(sctx, btn);
   }

   bool proxy_base::key(context const& ctx, key_info const& k)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      return subject().key(sctx, k);
   }

   bool proxy_base::text(context const& ctx, text_info const& info)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      return subject().text(sctx, info);
   }

   bool proxy_base::cursor(context const& ctx, point const& p)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      return subject().cursor(sctx, p);
   }

   bool proxy_base::scroll(context const& ctx, point const& p)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      return subject().scroll(sctx, p);
   }

   bool proxy_base::focus(focus_request r)
   {
      return subject().focus(r);
   }

   widget const* proxy_base::focus() const
   {
      return subject().focus();
   }

   widget* proxy_base::focus()
   {
      return subject().focus();
   }

   bool proxy_base::is_control() const
   {
      return subject().is_control();
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // composite_base class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////
   namespace
   {
      rect window_bounds(window const& w)
      {
         auto size = w.size();
         return rect{ 0, 0, size.x, size.y };
      }
   }

   widget* composite_base::hit_test(context const& ctx, point const& p)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, p);
         if (info.element)
         {
            context ectx{ ctx, info.element, info.bounds };
            return info.element->hit_test(ectx, p);
         }
      }
      return 0;
   }

   void composite_base::draw(context const& ctx)
   {
      auto w_bounds = window_bounds(ctx.window);
      for (std::size_t i = 0; i < size(); ++i)
      {
         rect bounds = bounds_of(ctx, i);
         if (intersects(bounds, w_bounds))
         {
            auto elem = (*this)[i];
            context ectx{ ctx, elem.get(), bounds };
            elem->draw(ectx);
         }
      }
   }

   widget* composite_base::click(context const& ctx, mouse_button btn)
   {
      point p = ctx.cursor_pos();

      if (!empty())
      {
         hit_info info = hit_element(ctx, p);

         if (info.element && focus(focus_request::wants_focus))
         {
            if (_focus != info.index)
            {
               // end the previous focus
               if (_focus != -1)
                  (*this)[_focus]->focus(focus_request::end_focus);

               // start a new focus
               _focus = info.index;
               if (_focus != -1)
                  (*this)[_focus]->focus(focus_request::begin_focus);
            }
         }

         if (info.element)
         {
            _drag_tracking = info.index;
            context ectx{ ctx, info.element, info.bounds };
            if (info.element->click(ectx, btn))
               return info.element;
         }
      }
      _drag_tracking = -1;
      return 0;
   }

   void composite_base::drag(context const& ctx, mouse_button btn)
   {
      if (_drag_tracking != -1)
      {
         rect bounds = bounds_of(ctx, _drag_tracking);
         widget* element = (*this)[_drag_tracking].get();
         context ectx{ ctx, element, bounds };
         element->drag(ectx, btn);
      }
   }

   bool composite_base::key(context const& ctx, key_info const& k)
   {
      if (_focus != -1)
      {
         rect bounds = bounds_of(ctx, _focus);
         widget* focus_ptr = (*this)[_focus].get();
         context ectx{ ctx, focus_ptr, bounds };
         return focus_ptr->key(ectx, k);
      };

      return false;
   }

   bool composite_base::text(context const& ctx, text_info const& info)
   {
      if (_focus != -1)
      {
         rect bounds = bounds_of(ctx, _focus);
         widget* focus_ptr = (*this)[_focus].get();
         context ectx{ ctx, focus_ptr, bounds };
         return focus_ptr->text(ectx, info);
      };

      return false;
   }

   bool composite_base::cursor(context const& ctx, point const& p)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, p);
         if (info.element && photon::intersects(info.bounds, window_bounds(ctx.window)))
         {
            context ectx{ ctx, info.element, info.bounds };
            return info.element->cursor(ectx, p);
         }
      }
      return false;
   }

   bool composite_base::scroll(context const& ctx, point const& p)
   {
      if (!empty())
      {
         hit_info info = hit_element(ctx, ctx.cursor_pos());
         if (info.element && photon::intersects(info.bounds, window_bounds(ctx.window)))
         {
            context ectx{ ctx, info.element, info.bounds };
            return info.element->scroll(ectx, p);
         }
      }
      return false;
   }

   bool composite_base::focus(focus_request r)
   {
      switch (r) {

         case focus_request::wants_focus:
            for (auto element : *this)
               if (element->focus(focus_request::wants_focus))
                  return true;
            return false;

         case focus_request::begin_focus:
            if (_focus != -1)
               (*this)[_focus]->focus(focus_request::begin_focus);
            return true;

         case focus_request::end_focus:
            if (_focus != -1)
               (*this)[_focus]->focus(focus_request::end_focus);
            return true;
      }

      return false;
   }

   widget const* composite_base::focus() const
   {
      return (empty() || (_focus == -1))? 0 : (*this)[_focus].get();
   }

   widget* composite_base::focus()
   {
      return (empty() || (_focus == -1))? 0 : (*this)[_focus].get();
   }

   void composite_base::focus(std::size_t index)
   {
      if (index < size())
         _focus = int(index);
   }

   composite_base::hit_info composite_base::hit_element(context const& ctx, point const& p) const
   {
      for (std::size_t i = 0; i < size(); ++i)
      {
         widget_ptr e = (*this)[i];
         if (e->is_control())
         {
            rect bounds = bounds_of(ctx, i);
            if (bounds.includes(p))
               return hit_info{ e.get(), bounds, int(i) };
         }
      }
      return hit_info{ 0, rect{}, -1 };
   }

   bool composite_base::is_control() const
   {
      for (auto const& e : *this)
         if (e->is_control())
            return true;
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   // scrollable class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////
   scrollable::scrollable_context scrollable::find(context const& ctx_)
   {
      auto const* ctx = &ctx_;
      while (ctx && ctx->widget)
      {
         auto* sp = dynamic_cast<scrollable*>(ctx->widget);
         if (sp)
            return { ctx, sp };
         else
            ctx = ctx->parent;
      }
      return { 0, 0 };
   }
}
