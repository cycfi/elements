/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   infra's UTF-8 conversions, as the text elements use them: decoding never
   reads past its input and never throws, an invalid or truncated sequence
   decodes as U+FFFD, and encoding never produces invalid UTF-8.
=============================================================================*/
#include "test_support.hpp"
#include <infra/utf8_utils.hpp>
#include <string>

using cycfi::to_utf8;
using cycfi::to_utf32;
using cycfi::is_valid_utf8;

TEST_CASE("utf8: valid text round trips", "[utf8]")
{
   std::u32string const text = U"aé中\U0001F600́ z";
   CHECK(to_utf32(to_utf8(text)) == text);
   CHECK(is_valid_utf8(to_utf8(text)));
   CHECK(to_utf32("") == U"");
   CHECK(is_valid_utf8(""));
}

TEST_CASE("utf8: invalid and truncated sequences", "[utf8]")
{
   // Each case decodes within its bytes, without throwing.
   struct { std::string in; std::u32string out; } const cases[] = {
      {"\xff", U"�"},                              // never valid
      {"a\xfe" "b", U"a�b"},
      {"\xF0\x9F", U"�"},                          // cut short at the end
      {"\xF0\x9F" "a", U"�a"},                     // cut short by a new start
      {"\xE4\xB8", U"�"},
      {"\x80", U"�"},                              // a lone continuation byte
      {"\xC0\xAF", U"��"},                    // overlong
      {"\xED\xA0\x80", U"���"},          // an encoded surrogate
   };
   for (auto const& c : cases)
   {
      INFO("input of " << c.in.size() << " bytes");
      // Copy into a buffer of exactly that size, so reading past it is
      // caught by the address sanitizer.
      std::string exact = c.in;
      exact.shrink_to_fit();
      std::u32string out;
      CHECK_NOTHROW(out = to_utf32(exact));
      CHECK(out == c.out);
      CHECK(!is_valid_utf8(exact));
   }
}

TEST_CASE("utf8: encoding never produces invalid UTF-8", "[utf8]")
{
   for (char32_t cp : {char32_t(0xD800), char32_t(0xDFFF), char32_t(0x110000), char32_t(0xFFFFFFFF)})
   {
      auto s = cycfi::codepoint_to_utf8(cp);
      CHECK(is_valid_utf8(s));
      CHECK(to_utf32(s) == U"�");
   }
   CHECK(cycfi::codepoint_to_utf8(0x10FFFF) == "\xF4\x8F\xBF\xBF");
}
