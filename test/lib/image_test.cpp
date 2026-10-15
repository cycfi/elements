/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   An image, and an image grid, report a picture that cannot be loaded or
   used to the error handler, then throw; replacing a picture with one that
   fails keeps the old one; a null picture is rejected, not followed.
=============================================================================*/
#include "test_support.hpp"
#include <elements/support/error_handler.hpp>
#include <string>

using namespace cycfi::elements;

namespace
{
   // Counts the reports while it lives.
   struct reports
   {
      reports()
       : saved{error_handler::get().on_resource_error}
      {
         error_handler::get().on_resource_error =
            [this](error_id id, std::string_view) { if (id == error_id::image_load_failed) ++count; };
      }

      ~reports() { error_handler::get().on_resource_error = saved; }

      decltype(error_handler::on_resource_error) saved;
      int count = 0;
   };

   image_ptr picture() { return std::make_shared<cycfi::artist::image>(10.0f, 10.0f); }
}

TEST_CASE("image: a missing file given to set_image is reported", "[image]")
{
   image a{picture()};
   auto before = a.get_image();
   reports r;
   CHECK_THROWS(a.set_image(cycfi::fs::path{"no/such/file.png"}));
   CHECK(r.count == 1);
   CHECK(a.get_image() == before);               // the old picture kept
}

TEST_CASE("image: a null picture is rejected", "[image]")
{
   reports r;
   CHECK_THROWS(image{image_ptr{}});
   CHECK(r.count == 1);

   image a{picture()};
   auto before = a.get_image();
   CHECK_THROWS(a.set_image(image_ptr{}));
   CHECK(r.count == 2);
   CHECK(a.get_image() == before);               // the old picture kept
}

TEST_CASE("image grid: a missing file is reported", "[image]")
{
   reports r;
   CHECK_THROWS(image_grid_cell(cycfi::fs::path{"no/such/file.png"}, 2, 2, element{}));
   CHECK(r.count == 1);
}
