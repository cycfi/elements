/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   The clipboard holds a payload: every format put on it comes back, by
   MIME type; asking for types it does not hold leaves them out; setting it
   replaces all it held; and the deprecated text functions still read and
   write its "text/plain".
=============================================================================*/
#include "test_support.hpp"
#include <algorithm>
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   using bytes = std::vector<std::byte>;

   bytes some_bytes()
   {
      bytes data;
      for (int i = 0; i != 256; ++i)
         data.push_back(std::byte(i));     // every value, zeros included
      return data;
   }

   bool holds_type(std::string const& type)
   {
      auto types = clipboard_types();
      return std::find(types.begin(), types.end(), type) != types.end();
   }

   // Whether the clipboard keeps formats other than text: a system
   // clipboard may hold text only where its host does no more yet, or not
   // be settable at all without a focused window (Wayland, as on CI).
   bool clipboard_formats_work()
   {
      set_clipboard({{"application/x-elements-test", std::string{"test"}}});
      return get_clipboard({"application/x-elements-test"}).size() == 1;
   }
}

TEST_CASE("clipboard: a payload of several formats", "[clipboard]")
{
   test_view tv{extent{100, 100}, 1};
   if (!clipboard_formats_work())
      return;

   set_clipboard({
      {"text/plain", std::string{"hello"}},
      {"text/html", std::string{"<b>hello</b>"}},
      {"application/x-elements-test", some_bytes()}
   });

   CHECK(holds_type("text/plain"));
   CHECK(holds_type("text/html"));
   CHECK(holds_type("application/x-elements-test"));
   CHECK(clipboard_contains("text/plain"));
   CHECK(!clipboard_contains("image/png"));

   // Only what is asked for, and only what is there.
   auto p = get_clipboard({"text/plain", "image/png"});
   REQUIRE(p.size() == 1);
   CHECK(get_plain_text(p) == "hello");

   auto all = get_clipboard({
      "text/plain", "text/html", "application/x-elements-test"});
   REQUIRE(all.size() == 3);
   CHECK(std::get<std::string>(all["text/html"]) == "<b>hello</b>");

   // Binary data comes back as it went, whatever form the host gives it.
   auto const& custom = all["application/x-elements-test"];
   if (auto const* b = std::get_if<bytes>(&custom))
      CHECK(*b == some_bytes());
   else
   {
      auto const& s = std::get<std::string>(custom);
      CHECK(bytes(
         reinterpret_cast<std::byte const*>(s.data()),
         reinterpret_cast<std::byte const*>(s.data()) + s.size()) == some_bytes());
   }
}

TEST_CASE("clipboard: setting replaces all it held", "[clipboard]")
{
   test_view tv{extent{100, 100}, 1};
   if (!clipboard_formats_work())
      return;

   set_clipboard({
      {"text/plain", std::string{"first"}},
      {"text/html", std::string{"<i>first</i>"}}
   });
   set_clipboard({{"text/plain", std::string{"second"}}});

   CHECK(!clipboard_contains("text/html"));
   auto p = get_clipboard({"text/plain", "text/html"});
   CHECK(p.size() == 1);
   CHECK(get_plain_text(p) == "second");
}

TEST_CASE("clipboard: nothing asked, nothing given", "[clipboard]")
{
   test_view tv{extent{100, 100}, 1};
   if (!clipboard_works())
      return;

   clipboard_text("text");
   CHECK(get_clipboard({}).empty());
   CHECK(get_clipboard({"image/png"}).empty());
}

TEST_CASE("clipboard: the deprecated text functions", "[clipboard]")
{
   test_view tv{extent{100, 100}, 1};
   if (!clipboard_works())
      return;

#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable: 4996)
#else
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
   clipboard("old way");
   CHECK(clipboard_text() == "old way");
   clipboard_text("new way");
   CHECK(clipboard() == "new way");
#if defined(_MSC_VER)
# pragma warning(pop)
#else
# pragma GCC diagnostic pop
#endif
}

TEST_CASE("payload: plain text", "[clipboard]")
{
   payload p;
   CHECK(!contains_plain_text(p));
   CHECK(get_plain_text(p).empty());

   p["text/plain"] = std::string{"hi"};
   CHECK(contains_plain_text(p));
   CHECK(get_plain_text(p) == "hi");

   // Text is a string; bytes under "text/plain" are not text.
   payload b;
   b["text/plain"] = bytes{std::byte{'h'}, std::byte{'i'}};
   CHECK(!contains_plain_text(b));
   CHECK(get_plain_text(b).empty());
}
