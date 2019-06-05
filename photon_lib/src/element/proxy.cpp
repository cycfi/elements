/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/element/proxy.hpp>
#include <photon/support/context.hpp>
#include <photon/view.hpp>

namespace cycfi { namespace photon
{
   ////////////////////////////////////////////////////////////////////////////
   // proxy class implementation
   ////////////////////////////////////////////////////////////////////////////
   view_limits proxy_base::limits(basic_context const& ctx) const
   {
      return subject().limits(ctx);
   }

   view_span proxy_base::span() const
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

   void proxy_base::refresh(context const& ctx, element& element)
   {
      if (&element == this)
      {
         ctx.view.refresh(ctx);
      }
      else
      {
         context sctx { ctx, &subject(), ctx.bounds };
         prepare_subject(sctx);
         subject().refresh(sctx, element);
         restore_subject(sctx);
      }
   }

   void proxy_base::prepare_subject(context& ctx)
   {
   }

   void proxy_base::prepare_subject(context& ctx, point& p)
   {
      prepare_subject(ctx);
   }

   void proxy_base::restore_subject(context& ctx)
   {
   }

   element* proxy_base::click(context const& ctx, mouse_button btn)
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

   bool proxy_base::focus(focus_request r)
   {
      return subject().focus(r);
   }

   element const* proxy_base::focus() const
   {
      return subject().focus();
   }

   element* proxy_base::focus()
   {
      return subject().focus();
   }

   bool proxy_base::is_control() const
   {
      return subject().is_control();
   }

   void proxy_base::value(bool val)
   {
      subject().value(val);
   }

   void proxy_base::value(int val)
   {
      subject().value(val);
   }

   void proxy_base::value(double val)
   {
      subject().value(val);
   }

   void proxy_base::value(std::string val)
   {
      subject().value(val);
   }
}}
