/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/proxy.hpp>
#include <elements/support/context.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // proxy class implementation
   ////////////////////////////////////////////////////////////////////////////
   view_limits proxy_base::limits(basic_context const& ctx) const
   {
      return subject().limits(ctx);
   }

   view_stretch proxy_base::stretch() const
   {
      return subject().stretch();
   }

   unsigned proxy_base::span() const
   {
      return subject().span();
   }

   element* proxy_base::hit_test(context const& ctx, point p)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx, p);
      auto r = subject().hit_test(sctx, p);
      restore_subject(sctx);
      return r;
   }

   void proxy_base::draw(context const& ctx)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      subject().draw(sctx);
      restore_subject(sctx);
   }

   void proxy_base::layout(context const& ctx)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      subject().layout(sctx);
      restore_subject(sctx);
   }

   void proxy_base::refresh(context const& ctx, element& element, int outward)
   {
      if (&element == this)
      {
         ctx.view.refresh(ctx, outward);
      }
      else
      {
         context sctx { ctx, &subject(), ctx.bounds };
         prepare_subject(sctx);
         subject().refresh(sctx, element, outward);
         restore_subject(sctx);
      }
   }

   void proxy_base::prepare_subject(context& /* ctx */)
   {
   }

   void proxy_base::prepare_subject(context& ctx, point& /* p */)
   {
      prepare_subject(ctx);
   }

   void proxy_base::restore_subject(context& /* ctx */)
   {
   }

   bool proxy_base::click(context const& ctx, mouse_button btn)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx, btn.pos);
      auto r = subject().click(sctx, btn);
      restore_subject(sctx);
      return r;
   }

   void proxy_base::drag(context const& ctx, mouse_button btn)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx, btn.pos);
      subject().drag(sctx, btn);
      restore_subject(sctx);
   }

   bool proxy_base::key(context const& ctx, key_info k)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      auto r = subject().key(sctx, k);
      restore_subject(sctx);
      return r;
   }

   bool proxy_base::text(context const& ctx, text_info info)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      auto r = subject().text(sctx, info);
      restore_subject(sctx);
      return r;
   }

   bool proxy_base::cursor(context const& ctx, point p, cursor_tracking status)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx, p);
      auto r = subject().cursor(sctx, p, status);
      restore_subject(sctx);
      return r;
   }

   bool proxy_base::scroll(context const& ctx, point dir, point p)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx, p);
      auto r = subject().scroll(sctx, dir, p);
      restore_subject(sctx);
      return r;
   }

   bool proxy_base::wants_focus() const
   {
      return subject().wants_focus();
   }

   void proxy_base::begin_focus()
   {
      return subject().begin_focus();
   }

   void proxy_base::end_focus()
   {
      return subject().end_focus();
   }

   element const* proxy_base::focus() const
   {
      return subject().focus();
   }

   element* proxy_base::focus()
   {
      return subject().focus();
   }

   bool proxy_base::wants_control() const
   {
      return subject().wants_control();
   }
}}
