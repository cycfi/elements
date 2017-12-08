/*=============================================================================
   Copyright (c) 2016-2017 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(PHOTON_GUI_LIB_JSON_DECEMBER_5_2017)
#define PHOTON_GUI_LIB_JSON_DECEMBER_5_2017

#include <type_traits>
#include <utility>
#include <cctype>
#include <vector>
#include <string>
#include <array>

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>

#include <photon/support/exception.hpp>

namespace photon { namespace json
{
   namespace x3 = boost::spirit::x3;

   struct malformed_json_string : exception { using exception::exception; };

   class string
   {
   public:

      string()
       : _first(nullptr), _last(nullptr)
      {}

      string(char const* first, char const* last)
       : _first(first), _last(last)
      {}

      string(string const&) = default;
      string(string&&) = default;

      string& operator=(string const&) = default;
      string& operator=(string&&) = default;

      explicit operator std::string() const;
      bool extract(std::string& s) const;

      char const* begin() const { return _first; }
      char const* end() const { return _last; }

   private:

      char const* _first;
      char const* _last;
   };

   // The JSON parser
   class parser : public x3::parser<parser>
   {
   public:

      struct attribute {};
      typedef attribute attribute_type;
      static bool const has_attribute = true;

      // Main parse entry point
      template <typename Iterator, typename Context, typename Attribute>
      bool parse(Iterator& first, Iterator const& last
       , Context& context, x3::unused_type, Attribute& attr) const
      {
         return parse_impl(first, last, context, attr);
      }

   private:

      parser const& self() const { return *this; }

      // Floating point numbers
      template <typename Iter, typename Context, typename Attribute>
      typename std::enable_if<std::is_floating_point<Attribute>::value, bool>::type
      parse_impl(Iter& first, Iter last, Context& context, Attribute& val) const;

      // Integers
      template <typename Iter, typename Context, typename Attribute>
      typename std::enable_if<std::is_integral<Attribute>::value, bool>::type
      parse_impl(Iter& first, Iter last, Context& context, Attribute& val) const;

      // Boolean
      template <typename Iter, typename Context>
      bool parse_impl(Iter& first, Iter last, Context& context, bool& val) const;

      // String
      template <typename Iter, typename Context>
      bool parse_impl(Iter& first, Iter last, Context& context, string& val) const;

      template <typename Iter, typename Context>
      bool parse_impl(Iter& first, Iter last, Context& context, std::string& val) const;

      // Object
      template <typename Iter, typename Context, typename Attribute>
      typename std::enable_if<
         boost::fusion::traits::is_sequence<Attribute>::value, bool
      >::type
      parse_impl(Iter& first, Iter last, Context& context, Attribute& val) const;

      // Array
      template <typename Iter, typename Context, typename Attribute>
      bool parse_impl(
         Iter& first, Iter last, Context& context
       , std::vector<Attribute>& val) const;

      template <typename Iter, typename Context, typename Attribute, std::size_t N>
      bool parse_impl(
         Iter& first, Iter last, Context& context
       , std::array<Attribute, N>& val) const;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Implementation
   ////////////////////////////////////////////////////////////////////////////

   inline bool string::extract(std::string& result) const
   {
      using uchar = boost::uint32_t;   // a unicode code point
      char const* i = _first + 1;      // skip the initial '"'
      char const* last = _last - 1;    // skip the final '"'

      for (; i < last; ++i)
      {
         auto ch = *i;
         if (std::iscntrl(ch))
            return false;

         if (*i == '\\')
         {
            if (++i == last)
               return false;

            switch (ch = *i)
            {
               case 'b': result += '\b';     break;
               case 'f': result += '\f';     break;
               case 'n': result += '\n';     break;
               case 'r': result += '\r';     break;
               case 't': result += '\t';     break;
               default: result += ch;        break;

               case 'u':
               {
                  x3::uint_parser<uchar, 16, 4, 4> hex4;
                  uchar code_point;
                  auto ii = ++i;
                  bool r = x3::parse(ii, last, hex4, code_point);
                  if (!r)
                     return false;

                  i = ii; // update iterator position
                  using insert_iter = std::back_insert_iterator<std::string>;
                  insert_iter out_iter(result);
                  boost::utf8_output_iterator<insert_iter> utf8_iter(out_iter);
                  *utf8_iter++ = code_point;
               }
            }
         }
         else
         {
            result += ch;
         }
      }
      return true;
   }

   inline string::operator std::string() const
   {
      std::string result;
      if (!extract(result))
         throw malformed_json_string{"Error: Malformed JSON string"};
      return result;
   }

   template <typename Iter, typename Context, typename Attribute>
   inline typename std::enable_if<std::is_floating_point<Attribute>::value, bool>::type
   parser::parse_impl(Iter& first, Iter last, Context& context, Attribute& val) const
   {
      static x3::real_parser<Attribute> p;
      return p.parse(first, last, context, x3::unused, val);
   }

   template <typename Iter, typename Context, typename Attribute>
   inline typename std::enable_if<std::is_integral<Attribute>::value, bool>::type
   parser::parse_impl(Iter& first, Iter last, Context& context, Attribute& val) const
   {
      static x3::int_parser<Attribute> p;
      return p.parse(first, last, context, x3::unused, val);
   }

   template <typename Iter, typename Context>
   inline bool
   parser::parse_impl(Iter& first, Iter last, Context& context, bool& val) const
   {
      return x3::bool_.parse(first, last, context, x3::unused, val);
   }

   template <typename Iter, typename Context>
   inline bool
   parser::parse_impl(Iter& first, Iter last, Context& context, string& val) const
   {
      x3::skip_over(first, last, context);

      // Parse this manually for speed. We do not decode nor validate anything.
      // We simply detect what looks like a double-quoted string which contains
      // any character, including any escaped double quote (i.e. "\\\""). We'll
      // deal with the actual string validation and extraction later.

      static auto double_quote = '"' >> *("\\\"" | (x3::char_ - '"')) >> '"';
      auto iter = first;
      if (double_quote.parse(iter, last, context, x3::unused, x3::unused))
      {
         val = {first, iter};
         first = iter;
         return true;
      }
      return false;
   }

   template <typename Iter, typename Context>
   inline bool
   parser::parse_impl(Iter& first, Iter last, Context& context, std::string& val) const
   {
      json::string s;
      if (parse_impl(first, last, context, s))
         return s.extract(val);
      return false;
   }

   template <typename Iter, typename Context, typename Attribute>
   inline typename std::enable_if<
      boost::fusion::traits::is_sequence<Attribute>::value, bool
   >::type
   parser::parse_impl(Iter& first, Iter last, Context& context, Attribute& val) const
   {
      return false;
   }

   template <typename Iter, typename Context, typename Attribute>
   inline bool
   parser::parse_impl(
      Iter& first, Iter last, Context& context, std::vector<Attribute>& val) const
   {
      static auto g = '[' >> self() >> *(',' >> self()) >> ']';
      return g.parse(first, last, context, x3::unused, val);
   }

   template <typename Iter, typename Context, typename Attribute, std::size_t N>
   inline bool
   parser::parse_impl(
      Iter& first, Iter last, Context& context, std::array<Attribute, N>& val) const
   {
      static auto g = '[' >> self() >> *(',' >> self()) >> ']';
      return g.parse(first, last, context, x3::unused, val);
   }
}}

#endif
