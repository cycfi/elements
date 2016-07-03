/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/proxy.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // proxy class implementation
   ////////////////////////////////////////////////////////////////////////////////////////////////
   rect proxy_base::limits(basic_context const& ctx) const
   {
      return subject().limits(ctx);
   }

   widget* proxy_base::hit_test(context const& ctx, point p)
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

   bool proxy_base::cursor(context const& ctx, point p)
   {
      context sctx { ctx, &subject(), ctx.bounds };
      prepare_subject(sctx);
      return subject().cursor(sctx, p);
   }

   bool proxy_base::scroll(context const& ctx, point p)
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
   
   void proxy_base::idle(basic_context const& ctx)
   {
      subject().idle(ctx);
   }
}
