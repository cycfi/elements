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
#include <functional>
#include <unordered_map>

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/is_view.hpp>
#include <boost/fusion/adapted/array.hpp>
#include <boost/fusion/adapted/std_array.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>

#include <photon/support/exception.hpp>

namespace photon { namespace json
{
   namespace x3 = boost::spirit::x3;
   namespace fusion = boost::fusion;

   struct malformed_json_string : exception { using exception::exception; };

   ////////////////////////////////////////////////////////////////////////////
   // json string class
   ////////////////////////////////////////////////////////////////////////////
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

   template <typename T>
   using is_adapted_struct =
      std::is_same<typename fusion::traits::tag_of<T>::type, fusion::struct_tag>;

   ////////////////////////////////////////////////////////////////////////////
   // json parser class
   ////////////////////////////////////////////////////////////////////////////
   class parser : public x3::parser<parser>
   {
   public:

      struct attribute {};
      typedef attribute attribute_type;
      static bool const has_attribute = true;

      // Main parse entry point
      template <typename Iter, typename Ctx, typename Attr>
      bool parse(Iter& first, Iter const& last
       , Ctx& context, x3::unused_type, Attr& attr) const
      {
         return parse_impl(first, last, context, attr);
      }

   private:

      parser const& self() const { return *this; }

      // Floating point numbers
      template <typename Iter, typename Ctx, typename Attr>
      typename std::enable_if<std::is_floating_point<Attr>::value, bool>::type
      parse_impl(Iter& first, Iter last, Ctx& context, Attr& attr) const;

      // Integers
      template <typename Iter, typename Ctx, typename Attr>
      typename std::enable_if<std::is_integral<Attr>::value, bool>::type
      parse_impl(Iter& first, Iter last, Ctx& context, Attr& attr) const;

      // Boolean
      template <typename Iter, typename Ctx>
      bool parse_impl(Iter& first, Iter last, Ctx& context, bool& attr) const;

      // String
      template <typename Iter, typename Ctx>
      bool parse_impl(Iter& first, Iter last, Ctx& context, string& attr) const;

      template <typename Iter, typename Ctx>
      bool parse_impl(Iter& first, Iter last, Ctx& context, std::string& attr) const;

      // Object
      template <typename Iter, typename Ctx, typename Attr>
      typename std::enable_if<is_adapted_struct<Attr>::value, bool>::type
      parse_impl(Iter& first, Iter last, Ctx& context, Attr& attr) const;

      // Array
      template <typename Iter, typename Ctx, typename Attr>
      bool parse_impl(
         Iter& first, Iter last, Ctx& context
       , std::vector<Attr>& attr) const;

      template <typename Iter, typename Ctx, typename Attr, std::size_t N>
      bool parse_impl(
         Iter& first, Iter last, Ctx& context
       , std::array<Attr, N>& attr) const;

      template <typename Iter, typename Ctx, typename Attr, std::size_t N>
      bool parse_impl(
         Iter& first, Iter last, Ctx& context, Attr (&attr)[N]) const;

      // Fusion iterator range
      template <typename Iter, typename Ctx, typename First, typename Last>
      bool
      parse_impl(Iter& first, Iter last, Ctx& context
       , fusion::iterator_range<First, Last>& attr) const;
   };

   ////////////////////////////////////////////////////////////////////////////
   // json pair parser class
   ////////////////////////////////////////////////////////////////////////////
   struct pair_parser : x3::parser<pair_parser>
   {
   public:

      struct attribute {};
      typedef attribute attribute_type;
      static bool const has_attribute = true;

      template <typename Iter, typename Ctx, typename Attr>
      bool parse(Iter& first, Iter const& last
       , Ctx& context, x3::unused_type, Attr& attr) const;
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

   template <typename Iter, typename Ctx, typename Attr>
   inline typename std::enable_if<std::is_floating_point<Attr>::value, bool>::type
   parser::parse_impl(Iter& first, Iter last, Ctx& context, Attr& attr) const
   {
      static x3::real_parser<Attr> p;
      return p.parse(first, last, context, x3::unused, attr);
   }

   template <typename Iter, typename Ctx, typename Attr>
   inline typename std::enable_if<std::is_integral<Attr>::value, bool>::type
   parser::parse_impl(Iter& first, Iter last, Ctx& context, Attr& attr) const
   {
      static x3::int_parser<Attr> p;
      return p.parse(first, last, context, x3::unused, attr);
   }

   template <typename Iter, typename Ctx>
   inline bool
   parser::parse_impl(Iter& first, Iter last, Ctx& context, bool& attr) const
   {
      return x3::bool_.parse(first, last, context, x3::unused, attr);
   }

   template <typename Iter, typename Ctx>
   inline bool
   parser::parse_impl(Iter& first, Iter last, Ctx& context, string& attr) const
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
         attr = {first, iter};
         first = iter;
         return true;
      }
      return false;
   }

   template <typename Iter, typename Ctx>
   inline bool
   parser::parse_impl(Iter& first, Iter last, Ctx& context, std::string& attr) const
   {
      json::string s;
      if (parse_impl(first, last, context, s))
         return s.extract(attr);
      return false;
   }

   template <typename Iter, typename Ctx, typename Attr>
   inline typename std::enable_if<is_adapted_struct<Attr>::value, bool>::type
   parser::parse_impl(Iter& first, Iter last, Ctx& context, Attr& attr) const
   {
      return false;
   }

   template <typename Iter, typename Ctx, typename First, typename Last>
   bool
   parser::parse_impl(Iter& first, Iter last, Ctx& context
    , fusion::iterator_range<First, Last>& attr) const
   {
      using attr_type = fusion::iterator_range<First, Last>;
      static_assert(fusion::result_of::size<attr_type>::value == 1, "Unexpected size!");
      return self().parse_impl(first, last, context, fusion::at_c<0>(attr));
   }

   template <typename Iter, typename Ctx, typename Attr>
   inline bool
   parser::parse_impl(
      Iter& first, Iter last, Ctx& context, std::vector<Attr>& attr) const
   {
      static auto g = '[' >> -(self() % ',') >> ']';
      return g.parse(first, last, context, x3::unused, attr);
   }

   namespace detail
   {
      template <int N>
      struct gen_sequence
      {
         static auto call()
         {
            return parser{} >> ',' >> gen_sequence<N-1>::call();
         }
      };

      template <>
      struct gen_sequence<1>
      {
         static auto call()
         {
            return parser{};
         }
      };

      template <>
      struct gen_sequence<0>
      {
         static auto call()
         {
            return x3::eps;
         }
      };
   }

   template <typename Iter, typename Ctx, typename Attr, std::size_t N>
   inline bool
   parser::parse_impl(
      Iter& first, Iter last, Ctx& context, std::array<Attr, N>& attr) const
   {
      static auto g = '[' >> detail::gen_sequence<N>::call() >> ']';
      return g.parse(first, last, context, x3::unused, attr);
   }

   template <typename Iter, typename Ctx, typename Attr, std::size_t N>
   inline bool
   parser::parse_impl(
      Iter& first, Iter last, Ctx& context, Attr (&attr)[N]) const
   {
      static auto g = '[' >> detail::gen_sequence<N>::call() >> ']';
      return g.parse(first, last, context, x3::unused, attr);
   }

   namespace detail
   {
      template <int I, typename Struct, typename Map>
      static void add_symbol(Map& map)
      {
         auto name = fusion::extension::struct_member_name<Struct, I>::call();
         auto f = [](auto& first, auto last, auto& context, auto& attr) -> bool
         {
            return (':' >> parser{})
               .parse(first, last, context, fusion::at_c<I>(attr));
         };
         map[name] = f;
      }

      template <int I, typename Struct>
      struct fill_map
      {
         template <typename Map>
         static Map&& call(Map&& map)
         {
            add_symbol<I, Struct>(map);
            return fill_map<I-1, Struct>::call(std::forward<Map>(map));
         }
      };

      template <typename Struct>
      struct fill_map<0, Struct>
      {
         template <typename Map>
         static Map&& call(Map&& map)
         {
            add_symbol<0, Struct>(map);
            return std::forward<Map>(map);
         }
      };
   }

   template <typename Iter, typename Ctx, typename Attr>
   inline bool
   pair_parser::parse(Iter& first, Iter const& last
    , Ctx& context, x3::unused_type, Attr& attr) const
   {
      using dispatch_function = std::function<bool(Iter&, Iter, Ctx&, Attr&)>;
      using map = std::unordered_map<std::string, dispatch_function>;
      static constexpr auto size = fusion::result_of::size<Attr>::value;
      static auto lookup = detail::fill_map<size-1, Attr>::call(map{});

      // Parse the key
      auto i = first;
      std::string name;
      if (!json::parser{}.parse(i, last, context, name))
         return false;

      // Lookup the key
      auto pos = lookup.find(name);
      if (pos == lookup.end())
         return false;

      // Parse the value
      if (!pos->second(i, last, context, attr))
         return false;

      first = i;
      return true;
   }
}}

#endif
