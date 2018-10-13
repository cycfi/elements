/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <photon/support/json.hpp>
#include <boost/fusion/include/equal_to.hpp>
#include <sstream>

namespace json = cycfi::photon::json;
namespace x3 = boost::spirit::x3;
namespace fusion = boost::fusion;

template <typename T>
void test_parser(json::parser const& jp, char const* in, T n)
{
   char const* f = in;
   char const* l = f + std::strlen(f);

   T attr;
   bool r = x3::parse(f, l, jp, attr);
   BOOST_TEST(r);
   BOOST_TEST(attr == n);
};

void test_string1(json::parser const& jp, char const* in)
{
   char const* f = in;
   char const* l = f + std::strlen(f);

   json::string<char const*> s;
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

void test_string3(json::parser const& jp, std::string in, std::string s)
{
   std::string::const_iterator f = in.begin();
   std::string::const_iterator l = in.end();

   std::string attr;
   bool r = x3::parse(f, l, jp, attr);
   BOOST_TEST(r);
   BOOST_TEST(attr == s);
};

template <typename C>
bool same(C const& a, C const& b)
{
   if (boost::size(a) != boost::size(b))
      return false;
   for (std::size_t i = 0; i != boost::size(a); ++i)
      if (a[i] != b[i])
         return false;
   return true;
}

template <typename Container>
void test_array(json::parser const& jp, char const* in, Container const& c)
{
   char const* f = in;
   char const* l = f + std::strlen(f);

   Container attr;
   bool r = x3::phrase_parse(f, l, jp, x3::space, attr);
   BOOST_TEST(r);
   BOOST_TEST(same(attr, c));
};

struct foo
{
   int i;
   double d;
   std::string s;
};

struct bar
{
   int ii;
   double dd;
   std::vector<int> vv;
   foo ff;
};

template <typename T>
void test_object(json::parser const& jp, char const* in, T const& obj)
{
   char const* f = in;
   char const* l = f + std::strlen(f);

   T attr;
   bool r = x3::phrase_parse(f, l, jp, x3::space, attr);
   BOOST_TEST(r);
   BOOST_TEST(attr == obj);
};

BOOST_FUSION_ADAPT_STRUCT(
   foo,
   (int, i)
   (double, d)
   (std::string, s)
)

BOOST_FUSION_ADAPT_STRUCT(
   bar,
   (int, ii)
   (double, dd)
   (std::vector<int>, vv)
   (foo, ff)
)

using fusion::operator==;

void test_json()
{
   json::parser jp;

   // ints and bools
   {
      test_parser(jp, "1234", 1234);
      test_parser(jp, "1234.45", 1234.45);
      test_parser(jp, "true", true);
      test_parser(jp, "false", false);
   }

   // strings
   {
      test_string1(jp, "\"This is my string\"");
      test_string1(jp, "\"This is \\\"my\\\" string\"");

      test_string2(jp, "\"This is my string\"", "This is my string");
      test_string2(jp, "\"This is \\\"my\\\" string\"", "This is \"my\" string");
      test_string2(jp, "\"Sosa did fine.\\u263A\"", u8"Sosa did fine.\u263A");
      test_string2(jp, "\"Snowman: \\u2603\"", u8"Snowman: \u2603");

      test_string3(jp, "\"This is my string\"", "This is my string");
   }

   // int vector
   {
      std::vector<int> c = {1, 2, 3, 4};
      test_array(jp, "[1, 2, 3, 4]", c);

      // empty vector
      std::vector<int> c2;
      test_array(jp, "[]", c2);
   }

   // int array
   {
      std::array<int, 4> c = {{1, 2, 3, 4}};
      test_array(jp, "[1, 2, 3, 4]", c);

      int c2[4] = {1, 2, 3, 4};
      test_array(jp, "[1, 2, 3, 4]", c2);

      // empty array
      std::array<int, 0> c3;
      test_array(jp, "[]", c3);
   }

   // double vector
   {
      std::vector<double> c = {1.1, 2.2, 3.3, 4.4};
      test_array(jp, "[1.1, 2.2, 3.3, 4.4]", c);
   }

   // string vector
   {
      std::vector<std::string> c = {"a", "b", "c", "d"};
      test_array(jp, "[\"a\", \"b\", \"c\", \"d\"]", c);
   }

   // struct
   {
      foo obj = {1, 2.2, "hey!"};
      bar obj2 = {8, 9.9, {1, 2, 3, 4}, obj};

      {
         char const* in = R"(
            {
               "i" : 1,
               "d" : 2.2,
               "s" : "hey!"
            }
         )";

         test_object(jp, in, obj);
      }

      {
         char const* in = R"(
            {
               "ii" : 8,
               "dd" : 9.9,
               "vv" : [1, 2, 3, 4],
               "ff" :
               {
                  "i" : 1,
                  "d" : 2.2,
                  "s" : "hey!"
               }
            }
         )";

         test_object(jp, in, obj2);
      }

      // Round trip
      {
         std::stringstream ss;
         json::printer pr{ss, 3};
         pr.print(obj2);
         test_object(jp, ss.str().c_str(), obj2);

         // Just for fun!
         std::cout << ss.str() << std::endl;
      }
   }
}

int main(int argc, const char* argv[])
{
   test_json();
   return boost::report_errors();
}
