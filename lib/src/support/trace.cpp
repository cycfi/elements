/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/support/trace.hpp>
#include <elements/support/log.hpp>
#include <elements/support/context.hpp>
#include <elements/element/element.hpp>

#include <cstdint>
#include <sstream>
#include <string>

namespace cycfi::elements
{
   namespace
   {
      thread_local std::uint64_t the_ev = 0;
      thread_local std::uint32_t the_seq = 0;
      thread_local bool the_leaf_done = false;   // one leaf per gesture (the deepest)

      int depth_of(context const& ctx)
      {
         int d = 0;
         for (auto p = ctx.parent; p; p = p->parent)
            ++d;
         return d;
      }

      struct fields
      {
         bool        bounds = false;
         char const* args = nullptr;
         bool        has_ret = false;
         bool        ret = false;
         int         index = -1;
      };

      void emit(context const& ctx, char const* method, char const* phase, fields f)
      {
         std::ostringstream os;
         os << "{\"ev\":" << the_ev
            << ",\"seq\":" << the_seq++
            << ",\"phase\":\"" << phase << '"'
            << ",\"depth\":" << depth_of(ctx)
            << ",\"node\":\"" << static_cast<void const*>(ctx.element) << '"'
            << ",\"parent\":\""
               << static_cast<void const*>(ctx.parent ? ctx.parent->element : nullptr) << '"'
            << ",\"class\":\"" << (ctx.element ? ctx.element->class_name() : std::string{}) << '"'
            << ",\"method\":\"" << method << '"';
         if (f.bounds)
            os << ",\"bounds\":[" << ctx.bounds.left << ',' << ctx.bounds.top << ','
               << ctx.bounds.right << ',' << ctx.bounds.bottom << ']';
         if (f.args && *f.args)
            os << ",\"args\":" << f.args;   // raw JSON object
         if (f.has_ret)
            os << ",\"ret\":" << (f.ret ? "true" : "false");
         if (f.index >= 0)
            os << ",\"index\":" << f.index;
         os << '}';

         LOG_INFO(logger(log_cat::trace), "{}", os.str());
      }
   }

   void trace_leaf(
      context const& ctx, char const* method, std::string args, bool has_ret, bool ret)
   {
      if (!trace_enabled() || the_leaf_done)
         return;
      the_leaf_done = true;   // deepest emits first; ignore the rest this gesture
      emit(ctx, method, "leaf",
         {.bounds = true, .args = args.c_str(), .has_ret = has_ret, .ret = ret});
   }

   // Cheap runtime gate: trace is off (Critical) by default; enable with
   // CYCFI_LOG=trace=debug. Records are logged at Info, so we skip when the
   // trace logger's level is above Info.
   bool trace_enabled()
   {
      return logger(log_cat::trace)->get_log_level() <= quill::LogLevel::Info;
   }

   void trace_new_gesture(char const* /*method*/)
   {
      if (!trace_enabled())
         return;
      ++the_ev;
      the_seq = 0;
      the_leaf_done = false;
   }

   trace_scope::trace_scope(context const& ctx, char const* method, std::string args)
    : _ctx(ctx), _method(method), _args(std::move(args))
   {
      if (trace_enabled())
         emit(_ctx, _method, "enter", {.bounds = true, .args = _args.c_str()});
   }

   trace_scope::~trace_scope()
   {
      if (trace_enabled())
         emit(_ctx, _method, "exit",
            {.has_ret = _has_ret, .ret = _ret, .index = _child_index});
   }
}
