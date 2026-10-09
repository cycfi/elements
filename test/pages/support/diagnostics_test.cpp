/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/support/diagnostics.adoc.
   The suite's log goes to the build's results directory and view is at
   debug: test_main.cpp sets CYCFI_LOG_DIR and CYCFI_LOG before anything
   logs, since log_init reads them once.
=============================================================================*/
#include "test_support.hpp"
#include <elements/support/log.hpp>
#include <elements/support/trace.hpp>
#include <elements/support/perf.hpp>
#include <elements/support/error_handler.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   std::string const log_root = std::string{RESULTS_PATH} + "logs";

   std::string slurp(std::string const& path)
   {
      std::ifstream in{path};
      std::stringstream ss;
      ss << in.rdbuf();
      return ss.str();
   }
}

TEST_CASE("diagnostics page: the log", "[diagnostics_page]")
{
   // The directory is the override. The name is the default, "elements":
   // the suite logged before this case could name it.
   CHECK(log_dir() == log_root);

   // The page's example. view is at debug through CYCFI_LOG, so the debug
   // line is kept; input is off, so its line is not.
   LOG_INFO(logger(log_cat::view), "resize {}x{}", 640, 480);
   LOG_DEBUG(logger(log_cat::view), "scale {}", 2.0);
   LOG_INFO(logger(log_cat::input), "click at {}", 10);

   // Default levels: warning for most categories, so an info line on app
   // is dropped; error always on.
   LOG_INFO(logger(log_cat::app), "not kept");
   LOG_ERROR(logger(log_cat::error), "kept");

   log_shutdown();                     // flushes what the backend holds
   auto text = slurp(log_root + "/elements.log");
   CHECK(text.find("elements logging started") != std::string::npos);
   CHECK(text.find("resize 640x480") != std::string::npos);
   CHECK(text.find("scale 2") != std::string::npos);
   CHECK(text.find("click at 10") == std::string::npos);
   CHECK(text.find("not kept") == std::string::npos);
   CHECK(text.find("kept") != std::string::npos);

   // The trace file sits beside it, empty: tracing is off by default.
   CHECK(!trace_enabled());
   CHECK(slurp(log_root + "/elements-trace.jsonl").empty());
}

TEST_CASE("diagnostics page: error handlers", "[diagnostics_page]")
{
   // The page's example: an application takes over resource errors.
   auto& eh = error_handler::get();
   auto saved = eh.on_resource_error;

   error_id got_id{};
   std::string got_msg;
   eh.on_resource_error = [&](error_id id, std::string_view msg)
   {
      got_id = id;
      got_msg = std::string{msg};
   };

   // A missing image file reports image_load_failed, then throws.
   CHECK_THROWS_AS(image{"no_such_file.png"}, std::runtime_error);
   CHECK(got_id == error_id::image_load_failed);
   CHECK(got_msg.find("no_such_file.png") != std::string::npos);

   eh.on_resource_error = saved;
}

TEST_CASE("diagnostics page: perf is off without ELEMENTS_PERF", "[diagnostics_page]")
{
   CHECK(!perf::enabled());
   perf::record(1.0);               // and record does nothing
}
