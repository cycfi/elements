/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <photon/support/json.hpp>

namespace json = photon::json;
namespace x3 = boost::spirit::x3;

template <typename T>
void test_parser(json::parser const& jp, char const* in, T n)
{
   char const* f = in;
   char const* l = f + std::strlen(f);

   decltype(n) attr;
   bool r = x3::parse(f, l, jp, attr);
   BOOST_TEST(r);
   BOOST_TEST(attr == n);
};

void test_string1(json::parser const& jp, char const* in)
{
   char const* f = in;
   char const* l = f + std::strlen(f);

   json::string s;
   bool r = x3::parse(f, l, jp, s);
   BOOST_TEST(r);
   BOOST_TEST(s.begin() == in);
   BOOST_TEST(s.end() == l);
};

void test_string2(json::parser const& jp, char const* in, std::string s)
{
   char const* f = in;
   char const* l = f + std::strlen(f);

   std::string attr;
   bool r = x3::parse(f, l, jp, attr);
   BOOST_TEST(r);
   BOOST_TEST(attr == s);
};

template <typename Container>
void test_array(json::parser const& jp, char const* in, Container const& c)
{
   char const* f = in;
   char const* l = f + std::strlen(f);

   Container attr;
   bool r = x3::phrase_parse(f, l, jp, x3::space, attr);
   BOOST_TEST(r);
   BOOST_TEST(attr == c);
};

void test_json()
{
   json::parser jp;

   {
      test_parser(jp, "1234", 1234);
      test_parser(jp, "1234.45", 1234.45);
      test_parser(jp, "true", true);
      test_parser(jp, "false", false);
   }

   {
      test_string1(jp, "\"This is my string\"");
      test_string1(jp, "\"This is \\\"my\\\" string\"");
   }

   {
      test_string2(jp, "\"This is my string\"", "This is my string");
      test_string2(jp, "\"This is \\\"my\\\" string\"", "This is \"my\" string");
      test_string2(jp, "\"Sosa did fine.\\u263A\"", u8"Sosa did fine.\u263A");
      test_string2(jp, "\"Snowman: \\u2603\"", u8"Snowman: \u2603");
   }

   {
      std::vector<int> c = {1, 2, 3, 4};
      test_array(jp, "[1, 2, 3, 4]", c);
   }

   {
      std::vector<std::string> c = {"a", "b", "c", "d"};
      test_array(jp, "[\"a\", \"b\", \"c\", \"d\"]", c);
   }
}

int main(int argc, const char* argv[])
{
   test_json();
   return boost::report_errors();
}
