/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The tutorials show code from the examples. Antora cannot include those
   files, so the pages hold copies: each C++ block of a tutorial, with its
   callouts removed, must appear in one of the examples the page names.
   A line of "..." in a block marks code left out.
=============================================================================*/
#include <infra/catch.hpp>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace
{
   std::string read(fs::path const& p)
   {
      std::ifstream in{p};
      std::stringstream ss;
      ss << in.rdbuf();
      return ss.str();
   }

   std::string rtrim(std::string s)
   {
      while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r'))
         s.pop_back();
      return s;
   }

   // The C++ blocks of a page, each split into the runs of lines between
   // its "..." lines.
   std::vector<std::string> chunks_of(std::string const& page)
   {
      static std::regex const callout{R"(\s*<\d+>$)"};
      std::vector<std::string> chunks;
      std::istringstream in{page};
      std::string line, chunk;
      bool in_block = false;
      auto flush = [&]
      {
         if (!chunk.empty())
            chunks.push_back(chunk);
         chunk.clear();
      };
      while (std::getline(in, line))
      {
         if (!in_block)
         {
            in_block = (line == "```c++");
            continue;
         }
         if (line == "```")
         {
            flush();
            in_block = false;
            continue;
         }
         line = rtrim(std::regex_replace(line, callout, ""));
         if (line.find_first_not_of(' ') != std::string::npos
            && line.substr(line.find_first_not_of(' ')) == "...")
            flush();
         else
            chunk += line + '\n';
      }
      return chunks;
   }

   // The sources of every example the page names as `examples/name`.
   std::vector<std::string> sources_of(std::string const& page, fs::path const& examples)
   {
      static std::regex const named{R"(`examples/([a-z_]+)`)"};
      std::vector<std::string> sources;
      for (std::sregex_iterator i{page.begin(), page.end(), named}, end; i != end; ++i)
      {
         auto dir = examples / (*i)[1].str();
         if (!fs::is_directory(dir))
            continue;
         for (auto const& e : fs::recursive_directory_iterator{dir})
         {
            auto ext = e.path().extension();
            if (ext == ".cpp" || ext == ".hpp")
            {
               std::string s;
               std::istringstream in{read(e.path())};
               for (std::string line; std::getline(in, line);)
                  s += rtrim(line) + '\n';
               sources.push_back(s);
            }
         }
      }
      return sources;
   }
}

TEST_CASE("tutorials: code blocks match the examples", "[tutorials_page]")
{
   fs::path const examples{EXAMPLES_PATH};
   fs::path const pages = examples / ".." / "docs" / "modules" / "ROOT" / "pages" / "tutorials";
   REQUIRE(fs::is_directory(pages));

   for (auto const& e : fs::directory_iterator{pages})
   {
      if (e.path().extension() != ".adoc")
         continue;
      auto const page = read(e.path());
      auto const chunks = chunks_of(page);
      if (chunks.empty())
         continue;
      auto const sources = sources_of(page, examples);
      for (auto const& chunk : chunks)
      {
         bool found = false;
         for (auto const& src : sources)
            found = found || src.find(chunk) != std::string::npos;
         INFO(e.path().filename().string() << ":\n" << chunk);
         CHECK(found);
      }
   }
}
