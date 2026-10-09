/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Asserts the claims of docs/modules/ROOT/pages/support/payload.adoc.
=============================================================================*/
#include "test_support.hpp"
#include <string>
#include <vector>

using namespace cycfi::elements;
using namespace cycfi::elements::test;
namespace fs = cycfi::fs;

TEST_CASE("payload page: a map of MIME type to data", "[payload_page]")
{
   // The page's example.
   payload pl;
   pl["text/plain"] = std::string{"hello"};
   pl["application/octet-stream"] = std::vector<std::byte>{std::byte{1}, std::byte{2}};

   CHECK(pl.size() == 2);
   CHECK(std::get<std::string>(pl["text/plain"]) == "hello");
   CHECK(std::get<std::vector<std::byte>>(pl["application/octet-stream"]).size() == 2);
   CHECK(pl.find("text/html") == pl.end());

   // A payload is a std::map: the usual lookups apply.
   CHECK(pl.count("text/plain") == 1);
}

TEST_CASE("payload page: file paths", "[payload_page]")
{
   // The hosts put dropped files in text/uri-list: one file:// URI per
   // line, separated by '\n', the path as the file system has it.
   payload files;
   files["text/uri-list"] =
      std::string{"file:///tmp/a.wav\nfile:///Users/joel/b.wav"};

   CHECK(contains_filepaths(files));
   auto paths = get_filepaths(files);
   REQUIRE(paths.size() == 2);
   CHECK(paths[0] == fs::path{"/tmp/a.wav"});
   CHECK(paths[1] == fs::path{"/Users/joel/b.wav"});

   // A single file, no newline.
   payload one;
   one["text/uri-list"] = std::string{"file:///tmp/a.wav"};
   CHECK(contains_filepaths(one));
   CHECK(get_filepaths(one).size() == 1);

   // No uri-list, or one that does not start with file://: not file paths.
   payload text;
   text["text/plain"] = std::string{"file:///tmp/a.wav"};
   CHECK(!contains_filepaths(text));
   CHECK(get_filepaths(text).empty());

   payload web;
   web["text/uri-list"] = std::string{"https://cycfi.com/"};
   CHECK(!contains_filepaths(web));
   CHECK(get_filepaths(web).empty());

   // A line that is not a file URI is skipped, not returned.
   payload mixed;
   mixed["text/uri-list"] =
      std::string{"file:///tmp/a.wav\nhttps://cycfi.com/\nfile:///tmp/c.wav"};
   CHECK(get_filepaths(mixed).size() == 2);
}

TEST_CASE("payload page: a drop box", "[payload_page]")
{
   // The page's second example: a drop box that takes files.
   std::vector<fs::path> got;
   auto box_ = share(drop_box(
      align_center_middle(label("Drop files here")), {"text/uri-list"}));
   box_->on_drop = [&got](drop_info const& info)
   {
      if (!contains_filepaths(info.data))
         return false;
      got = get_filepaths(info.data);
      return true;
   };

   test_view tv{extent{200, 100}, 1};
   tv.view_.content(hold(box_));
   tv.view_.layout();
   tv.draw();

   // As a host does: the drag enters and hovers, then drops. The view
   // routes the drop to the element the drag was over.
   drop_info info;
   info.data["text/uri-list"] = std::string{"file:///tmp/a.wav"};
   info.where = {100, 50};
   tv.view_.track_drop(info, cursor_tracking::entering);
   CHECK(box_->is_tracking());
   CHECK(tv.view_.drop(info));
   CHECK(!box_->is_tracking());
   REQUIRE(got.size() == 1);
   CHECK(got[0] == fs::path{"/tmp/a.wav"});

   // A payload without one of the box's MIME types is not tracked.
   drop_info other;
   other.data["text/plain"] = std::string{"hello"};
   other.where = {100, 50};
   tv.view_.track_drop(other, cursor_tracking::entering);
   CHECK(!box_->is_tracking());
}
