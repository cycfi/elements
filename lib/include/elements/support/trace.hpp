/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(ELEMENTS_TRACE_JUNE_03_2026)
#define ELEMENTS_TRACE_JUNE_03_2026

#include <string>

namespace cycfi::elements
{
   class context;

   // Element-traversal tracing. Always compiled; gated at runtime on the `trace`
   // log category (off by default, enable with CYCFI_LOG=trace=debug). When
   // disabled, trace_scope early-outs on a cheap level check.

   void trace_new_gesture(char const* method);

   // Cheap runtime check: is tracing currently enabled? Use it to guard any
   // non-trivial work done only to produce a trace record.
   bool trace_enabled();

   // Emit a single terminal "leaf" record for the actual control that handled
   // the event (resolved by composite hit-testing), since leaf controls don't
   // self-trace. `has_ret`/`ret` carry the call's return value (omit for void
   // calls like drag).
   void trace_leaf(
      context const& ctx, char const* method, std::string args,
      bool has_ret, bool ret);

   class trace_scope
   {
   public:
                     trace_scope(
                        context const& ctx, char const* method, std::string args = {});
                     ~trace_scope();

      // The call's return value (e.g. click's bool). Leave unset for void calls
      // (drag) -- the record then carries no "ret".
      void           returns(bool r) { _ret = r; _has_ret = true; }
      void           child_index(int i) { _child_index = i; }

   private:
      context const& _ctx;
      char const*    _method;
      std::string    _args;
      bool           _ret = false;
      bool           _has_ret = false;
      int            _child_index = -1;   // index of the child this node routed to
   };
}

#endif
