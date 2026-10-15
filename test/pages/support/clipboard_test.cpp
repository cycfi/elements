/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/support/clipboard.adoc.
   The clipboard checks run only where the clipboard keeps what is put on
   it (see clipboard_works in test_support.hpp).
=============================================================================*/
#include "test_support.hpp"
#include <algorithm>
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;

namespace
{
   // Whether the clipboard keeps formats other than text.
   bool formats_work()
   {
      set_clipboard({{"application/x-elements-page", std::string{"x"}}});
      return get_clipboard({"application/x-elements-page"}).size() == 1;
   }

   // An element that takes drops, and names what it takes.
   struct takes_text : element
   {
      bool drop(context const&, drop_info const&) override { return true; }
      mime_types drop_types() const override { return {"text/plain"}; }
   };

   // One that takes drops, but names nothing.
   struct names_nothing : element
   {
      bool drop(context const&, drop_info const&) override { return true; }
   };
}

TEST_CASE("clipboard page: copy and paste", "[clipboard_page]")
{
   test_view tv{extent{100, 100}};
   if (!formats_work())
      return;

   // The overview: the text, and the same as HTML. Types under text/
   // come back as strings, others as bytes.
   set_clipboard({
      {"text/plain", std::string{"hello"}},
      {"text/html", std::string{"<b>hello</b>"}}
   });
   auto data = get_clipboard({"text/html", "text/plain"});
   CHECK(data.size() == 2);
   CHECK(std::get<std::string>(data["text/html"]) == "<b>hello</b>");
   CHECK(get_plain_text(data) == "hello");

   set_clipboard({{"application/x-elements-page", std::string{"raw"}}});
   auto raw = get_clipboard({"application/x-elements-page"});
   REQUIRE(raw.size() == 1);
   CHECK(std::holds_alternative<std::vector<std::byte>>(
      raw["application/x-elements-page"]));
   set_clipboard({
      {"text/plain", std::string{"hello"}},
      {"text/html", std::string{"<b>hello</b>"}}
   });

   // Types it does not hold are left out.
   auto png = get_clipboard({"image/png"});
   CHECK(png.empty());

   // The types, with no data fetched.
   auto types = clipboard_types();
   CHECK(std::count(types.begin(), types.end(), "text/html") == 1);
   CHECK(clipboard_contains("text/plain"));
   CHECK(!clipboard_contains("image/png"));

   // Setting it replaces all it held.
   set_clipboard({{"text/plain", std::string{"only"}}});
   CHECK(!clipboard_contains("text/html"));
}

TEST_CASE("clipboard page: the example", "[clipboard_page]")
{
   test_view tv{extent{100, 100}};
   if (!formats_work())
      return;

   using bytes = std::vector<std::byte>;
   bytes own;
   std::string text;
   auto copy = [](std::string as_text, bytes serialized)
   {
      set_clipboard({
         {"text/plain", as_text},
         {"application/x-my-app", serialized}
      });
   };
   auto paste = [&]
   {
      auto data = get_clipboard({"application/x-my-app", "text/plain"});
      if (auto i = data.find("application/x-my-app"); i != data.end())
         own = std::get<bytes>(i->second);
      else if (contains_plain_text(data))
         text = get_plain_text(data);
   };

   auto const serialized = bytes{std::byte{1}, std::byte{0}, std::byte{2}};
   copy("two cells", serialized);
   paste();
   CHECK(own == serialized);

   set_clipboard({{"text/plain", std::string{"from elsewhere"}}});
   paste();
   CHECK(text == "from elsewhere");
}

TEST_CASE("clipboard page: the text functions", "[clipboard_page]")
{
   payload pl;
   CHECK(!contains_plain_text(pl));
   CHECK(get_plain_text(pl).empty());
   pl["text/plain"] = std::string{"text"};
   CHECK(contains_plain_text(pl));
   CHECK(get_plain_text(pl) == "text");

   test_view tv{extent{100, 100}};
   if (!clipboard_works())
      return;
#if defined(__clang__) || defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
   clipboard("old way");
   CHECK(get_plain_text(get_clipboard({"text/plain"})) == "old way");
   set_clipboard({{"text/plain", std::string{"new way"}}});
   CHECK(clipboard() == "new way");
#if defined(__clang__) || defined(__GNUC__)
# pragma GCC diagnostic pop
#endif
}

TEST_CASE("clipboard page: drop types", "[clipboard_page]")
{
   // The view gathers the types from its content and tells the host; an
   // element that names nothing adds nothing.
   test_view tv{extent{100, 100}};
#if defined(ELEMENTS_HOST_UI_LIBRARY_HEADLESS)
   headless::open(tv.view_);
#endif
   tv.view_.content(
      vtile(
         takes_text{},
         names_nothing{},
         drop_box(element{}, {"text/uri-list"})
      ));
   tv.view_.layout();
   tv.draw();
   auto const& told = tv.view_.accepted_drop_types();
   REQUIRE(told);
   CHECK(*told == mime_types{"text/plain", "text/uri-list"});
}
