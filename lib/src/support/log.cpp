/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/support/log.hpp>
#include <quill/Backend.h>
#include <quill/sinks/RotatingFileSink.h>

#include <array>
#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <string>
#include <string_view>

namespace fs = std::filesystem;

namespace cycfi::elements
{
   namespace
   {
      constexpr std::size_t num_categories = 9;
      std::array<logger_type*, num_categories> the_loggers{};
      std::string the_log_dir;

      char const* category_name(log_cat cat)
      {
         switch (cat)
         {
            case log_cat::app:      return "app";
            case log_cat::window:   return "window";
            case log_cat::view:     return "view";
            case log_cat::render:   return "render";
            case log_cat::input:    return "input";
            case log_cat::resource: return "resource";
            case log_cat::layout:   return "layout";
            case log_cat::error:    return "error";
            case log_cat::trace:    return "trace";
         }
         return "?";
      }

      // Default per-category level. Most categories at Warning so the common
      // path stays on quill's cheap level-check; error always on; the verbose
      // categories are effectively off until raised via CYCFI_LOG.
      quill::LogLevel default_level(log_cat cat)
      {
         switch (cat)
         {
            case log_cat::error:    return quill::LogLevel::Error;
            case log_cat::input:    return quill::LogLevel::Critical; // off
            case log_cat::layout:   return quill::LogLevel::Critical; // off
            case log_cat::trace:    return quill::LogLevel::Critical; // off
            default:                return quill::LogLevel::Warning;
         }
      }

      quill::LogLevel parse_level(std::string_view s)
      {
         if (s == "trace")    return quill::LogLevel::TraceL1;
         if (s == "debug")    return quill::LogLevel::Debug;
         if (s == "info")     return quill::LogLevel::Info;
         if (s == "notice")   return quill::LogLevel::Notice;
         if (s == "warn" || s == "warning")
                              return quill::LogLevel::Warning;
         if (s == "error")    return quill::LogLevel::Error;
         if (s == "off")      return quill::LogLevel::None;
         return quill::LogLevel::Warning;
      }

      fs::path resolve_log_dir(std::string const& app_name)
      {
         std::error_code ec;

         // Explicit override wins on any platform (used as-is).
         if (char const* over = std::getenv("CYCFI_LOG_DIR"))
         {
            fs::path dir{over};
            fs::create_directories(dir, ec);
            if (!ec)
               return dir;
         }

         // Per-OS user log location.
         fs::path dir;
#if defined(_WIN32)
         if (char const* base = std::getenv("LOCALAPPDATA"))
            dir = fs::path{base} / app_name / "Logs";
#elif defined(__APPLE__)
         if (char const* home = std::getenv("HOME"))
            dir = fs::path{home} / "Library" / "Logs" / app_name;
#else // Linux / other Unix: XDG state dir
         if (char const* xdg = std::getenv("XDG_STATE_HOME"))
            dir = fs::path{xdg} / app_name;
         else if (char const* home = std::getenv("HOME"))
            dir = fs::path{home} / ".local" / "state" / app_name;
#endif
         if (!dir.empty())
         {
            fs::create_directories(dir, ec);
            if (!ec)
               return dir;
         }

         // Last-resort fallback.
         return fs::temp_directory_path();
      }

      // Parse CYCFI_LOG=cat=level,cat=level,... e.g. view=debug,render=debug,input=off
      void apply_env_overrides()
      {
         char const* env = std::getenv("CYCFI_LOG");
         if (!env)
            return;

         std::string_view spec{env};
         while (!spec.empty())
         {
            auto comma = spec.find(',');
            auto token = spec.substr(0, comma);
            auto eq = token.find('=');
            if (eq != std::string_view::npos)
            {
               auto name = token.substr(0, eq);
               auto level = parse_level(token.substr(eq + 1));
               for (std::size_t i = 0; i < num_categories; ++i)
               {
                  if (name == category_name(static_cast<log_cat>(i)) && the_loggers[i])
                     the_loggers[i]->set_log_level(level);
               }
            }
            if (comma == std::string_view::npos)
               break;
            spec.remove_prefix(comma + 1);
         }
      }
   }

   void log_init(std::string_view app_name)
   {
      static std::once_flag once;
      std::call_once(once, [name = std::string{app_name}]
      {
         quill::Backend::start();

         fs::path dir = resolve_log_dir(name);
         the_log_dir = dir.string();

         quill::RotatingFileSinkConfig cfg;
         cfg.set_rotation_max_file_size(5u * 1024u * 1024u);  // 5 MB
         cfg.set_max_backup_files(3);
         cfg.set_overwrite_rolled_files(true);
         cfg.set_open_mode('a');                              // append across sessions

         auto sink = log_frontend::create_or_get_sink<quill::RotatingFileSink>(
            (dir / (name + ".log")).string(), cfg);

         // The trace category writes raw JSON-lines to its own file (one record
         // per line, no prefix), for offline element-traversal graph rebuilding.
         quill::RotatingFileSinkConfig trace_cfg;
         trace_cfg.set_rotation_max_file_size(10u * 1024u * 1024u);
         trace_cfg.set_max_backup_files(3);
         trace_cfg.set_open_mode('w');
         auto trace_sink = log_frontend::create_or_get_sink<quill::RotatingFileSink>(
            (dir / (name + "-trace.jsonl")).string(), trace_cfg);

         for (std::size_t i = 0; i < num_categories; ++i)
         {
            auto cat = static_cast<log_cat>(i);
            logger_type* lg =
               (cat == log_cat::trace)
                  ? log_frontend::create_or_get_logger(
                       category_name(cat), trace_sink,
                       quill::PatternFormatterOptions{"%(message)"})
                  : log_frontend::create_or_get_logger(category_name(cat), sink);
            lg->set_log_level(default_level(cat));
            the_loggers[i] = lg;
         }

         apply_env_overrides();

         // Startup banner so every captured log self-identifies.
#if defined(ARTIST_SKIA)
         constexpr char const* backend = "skia";
#elif defined(ARTIST_CAIRO)
         constexpr char const* backend = "cairo";
#elif defined(ARTIST_QUARTZ_2D)
         constexpr char const* backend = "quartz2d";
#else
         constexpr char const* backend = "unknown";
#endif
#if defined(NDEBUG)
         constexpr char const* build = "release";
#else
         constexpr char const* build = "debug";
#endif
         LOG_INFO(the_loggers[static_cast<std::size_t>(log_cat::app)],
            "elements logging started: backend={} build={}", backend, build);
      });
   }

   logger_type* logger(log_cat cat)
   {
      log_init();
      return the_loggers[static_cast<std::size_t>(cat)];
   }

   std::string log_dir()
   {
      log_init();
      return the_log_dir;
   }
}
