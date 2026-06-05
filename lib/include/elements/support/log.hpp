/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(ELEMENTS_LOG_JUNE_02_2026)
#define ELEMENTS_LOG_JUNE_02_2026

#include <quill/Frontend.h>
#include <quill/Logger.h>
#include <quill/LogMacros.h>
#include <string>
#include <string_view>

///////////////////////////////////////////////////////////////////////////////
// Elements event logging (Part 1: always-on host/event diagnostics).
//
// There is intentionally no wrapper around quill's logging API. Call sites use
// quill's LOG_* macros directly with a category logger, e.g.
//
//    #include <elements/support/log.hpp>
//    LOG_INFO(logger(log_cat::view), "resize {}x{}", w, h);
//
// The only Elements-owned code is the one-time setup in log.cpp. Categories are
// just named quill loggers. Levels are tunable at runtime via the CYCFI_LOG
// environment variable, e.g. CYCFI_LOG=view=debug,render=debug,input=off
//
// Performance: the frontend queue is bounded with a drop-on-full policy, so a
// burst of logging never blocks the UI/audio thread -- it drops messages (quill
// counts them) rather than stalling the producer.
///////////////////////////////////////////////////////////////////////////////
namespace cycfi::elements
{
   // Bounded, drop-on-full frontend so logging can never block the caller.
   struct log_frontend_options
   {
      static constexpr quill::QueueType queue_type =
         quill::QueueType::BoundedDropping;
      static constexpr size_t initial_queue_capacity = 256u * 1024u;
      static constexpr uint32_t blocking_queue_retry_interval_ns = 800;
      static constexpr size_t unbounded_queue_max_capacity =
         2ull * 1024u * 1024u * 1024u;
      static constexpr quill::HugePagesPolicy huge_pages_policy =
         quill::HugePagesPolicy::Never;
   };

   using log_frontend = quill::FrontendImpl<log_frontend_options>;
   using logger_type = quill::LoggerImpl<log_frontend_options>;

   enum class log_cat
   {
      app,        // app start/stop, run loop, platform/version banner
      window,     // host window create/destroy/move/resize/focus, attach
      view,       // base_view ctors, attach/detach, size changes, scale/HiDPI
      render,     // backend init, context create/fail, drawable size
      input,      // mouse/key/scroll/drag-drop (off by default)
      resource,   // font activation, search paths, bundle/resource resolution
      layout,     // element layout/limits negotiation (off by default)
      error,      // assertion failures, exceptions, unexpected states
      trace       // element-traversal tracing -> JSON sink (off by default)
   };

   // Idempotent: starts the quill backend, creates the rotating file sink and
   // the per-category loggers, applies default levels + CYCFI_LOG overrides, and
   // emits a startup banner. Safe to call repeatedly (plugin editors reopen);
   // only the first call takes effect.
   //
   // `app_name` names the log directory and files (e.g. "<app_name>.log"). To use
   // a custom name, call this once before any logging; otherwise the default is
   // used. The log directory is per-OS (macOS: ~/Library/Logs/<app_name>;
   // Windows: %LOCALAPPDATA%/<app_name>/Logs; Linux: $XDG_STATE_HOME or
   // ~/.local/state/<app_name>), overridable on any platform via the
   // CYCFI_LOG_DIR environment variable.
   void           log_init(std::string_view app_name = "elements");

   // Accessor for a category's named logger. Calls log_init() on first use.
   logger_type*   logger(log_cat cat);

   // Directory where log files are written (resolved at log_init).
   std::string    log_dir();
}

#endif
