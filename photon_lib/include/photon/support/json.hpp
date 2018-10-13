/*=============================================================================
   Copyright (c) 2016-2018 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_PHOTON_GUI_LIB_JSON_DECEMBER_5_2017)
#define CYCFI_PHOTON_GUI_LIB_JSON_DECEMBER_5_2017

#include <type_traits>
#include <utility>
#include <cctype>
#include <vector>
#include <string>
#include <array>
#include <functional>
#include <unordered_map>
#include <ostream>

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/is_view.hpp>
#include <boost/fusion/adapted/array.hpp>
#include <boost/fusion/adapted/std_array.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>

#include <photon/support/exception.hpp>

namespace cycfi { namespace photon { namespace json
{
   namespace x3 = boost::spirit::x3;
   namespace fusion = boost::fusion;

   struct malformed_json_string : exception { using exception::exception; };

   ////////////////////////////////////////////////////////////////////////////
   // json string class
   ////////////////////////////////////////////////////////////////////////////
   template <typename Iter>
   class string
   {
   public:

      string()
      {}

      string(Iter first, Iter last)
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

      Iter _first;
      Iter _last;
   };

   template <typename Iter>
   std::ostream& operator<<(std::ostream& out, string<Iter> str)
   {
      for (auto ch : str)
         out << ch;
      return out;
   }

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

      // Note 1.0: We pass pointer attributes (internally) to bypass the
      // container logic in X3.
      template <typename Iter, typename Ctx, typename Attr>
      typename std::enable_if<std::is_pointer<Attr>::value, bool>::type
      parse_impl(Iter& first, Iter last, Ctx& context, Attr& attr) const;

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
      bool parse_impl(Iter& first, Iter last, Ctx& context, string<Iter>& attr) const;

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
   // json printer class
   ////////////////////////////////////////////////////////////////////////////
   class printer : public x3::parser<printer>
   {
   public:

      struct attribute {};
      typedef attribute attribute_type;
      static bool const has_attribute = true;

      printer(std::ostream& out, unsigned indent = 2)
       : _out(out)
       , _indent(indent)
       , _current_indent(0)
      {}

      // Main print entry point
      template <typename Attr>
      void print(Attr& attr)
      {
         print_impl(attr);
      }

   private:

      void endl()
      {
         _out << std::endl;
         for (int i = 0; i != _current_indent; ++i)
            _out << ' ';
      }

      void indent() { _current_indent += _indent; }
      void dedent() { _current_indent -= _indent; }

      // Floating point numbers
      template <typename Attr>
      typename std::enable_if<std::is_floating_point<Attr>::value>::type
      print_impl(Attr attr);

      // Integers
      template <typename Attr>
      typename std::enable_if<std::is_integral<Attr>::value>::type
      print_impl(Attr attr);

      // Boolean
      void print_impl(bool attr);

      // String
      template <typename Iter>
      void print_impl(string<Iter> attr);
      void print_impl(std::string const& attr);

      template <typename Iter>
      void print_impl(Iter f, Iter l);

      // Object
      template <typename Attr>
      typename std::enable_if<is_adapted_struct<Attr>::value>::type
      print_impl(Attr const& attr);

      // Array
      template <typename Attr>
      void print_impl(std::vector<Attr> const& attr);

      template <typename Attr, std::size_t N>
      void print_impl(std::array<Attr, N> const& attr);

      template <typename Attr, std::size_t N>
      void print_impl(Attr const (&attr)[N]);

      template <typename C>
      void print_container(C const& c);

      friend struct print_object_helper;

      std::ostream& _out;
      unsigned _indent;
      unsigned _current_indent;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Implementation
   ////////////////////////////////////////////////////////////////////////////
   template <typename Iter>
   inline bool string<Iter>::extract(std::string& result) const
   {
      using uchar = boost::uint32_t;   // a unicode code point
      Iter i = _first; ++i;            // skip the initial '"'
      Iter last = _last; --last;       // skip the final '"'

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

   template <typename Iter>
   inline string<Iter>::operator std::string() const
   {
      std::string result;
      if (!extract(result))
         throw malformed_json_string{"Error: Malformed JSON string"};
      return result;
   }

   template <typename Iter, typename Ctx, typename Attr>
   inline typename std::enable_if<std::is_pointer<Attr>::value, bool>::type
   parser::parse_impl(Iter& first, Iter last, Ctx& context, Attr& attr) const
   {
      return parse_impl(first, last, context, *attr);
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
   parser::parse_impl(Iter& first, Iter last, Ctx& context, string<Iter>& attr) const
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
      json::string<Iter> s;
      if (parse_impl(first, last, context, s))
         return s.extract(attr);
      return false;
   }

   template <typename Iter, typename Ctx, typename Attr>
   inline typename std::enable_if<is_adapted_struct<Attr>::value, bool>::type
   parser::parse_impl(Iter& first, Iter last, Ctx& context, Attr& attr) const
   {
      static auto g = '{' >> -(pair_parser{} % ',') >> '}';

      // Note 1.1: p is a pointer. We pass pointer attributes (internally)
      // to bypass the container logic in X3.
      auto p = &attr;
      return g.parse(first, last, context, x3::unused, p);
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
            auto p = &fusion::at_c<I>(attr);
            return (':' >> parser{}).parse(first, last, context, x3::unused, p);
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
      // Note 1.2: attr is actually a pointer. We pass pointer attributes
      // (internally) to bypass the container logic in X3 (see Note 1.0
      // and 1.1 above).

      using attr_type = typename std::remove_reference<decltype(*attr)>::type;
      using dispatch_function = std::function<bool(Iter&, Iter, Ctx&, attr_type&)>;
      using map = std::unordered_map<std::string, dispatch_function>;
      static constexpr auto size = fusion::result_of::size<attr_type>::value;
      static auto lookup = detail::fill_map<size-1, attr_type>::call(map{});

      // Parse the key
      auto i = first;
      std::string name;
      if (!json::parser{}.parse(i, last, context, x3::unused, name))
         return false;

      // Lookup the key
      auto pos = lookup.find(name);
      if (pos == lookup.end())
         return false;

      // Parse the value
      if (!pos->second(i, last, context, *attr))
         return false;

      first = i;
      return true;
   }

   template <typename Attr>
   typename std::enable_if<std::is_floating_point<Attr>::value>::type
   inline printer::print_impl(Attr attr)
   {
      _out << attr;
   }

   template <typename Attr>
   typename std::enable_if<std::is_integral<Attr>::value>::type
   inline printer::print_impl(Attr attr)
   {
      _out << attr;
   }

   inline void
   printer::print_impl(bool attr)
   {
      _out << (attr? "true" : "false");
   }

   template <typename Iter>
   inline void
   printer::print_impl(string<Iter> attr)
   {
      _out << attr;
   }

   template <typename Iter>
   inline void
   printer::print_impl(Iter f, Iter l)
   {
      _out << '"';
      for (auto i = f; i != l; ++i)
      {
         auto c = *i;
         switch (c)
         {
            case '"':   _out << "\\\"";   break;
            case '\\':  _out << "\\\\";   break;
            case '/':   _out << "\\/";    break;
            case '\b':  _out << "\\b";    break;
            case '\f':  _out << "\\f";    break;
            case '\n':  _out << "\\n";    break;
            case '\r':  _out << "\\r";    break;
            case '\t':  _out << "\\t";    break;
            default:    _out << x3::to_utf8(c);
         }
      }

      _out << '"';
   }

   inline void
   printer::print_impl(std::string const& attr)
   {
      typedef boost::u8_to_u32_iterator<std::string::const_iterator> iter_t;
      iter_t f = attr.begin();
      iter_t l = attr.end();
      print_impl(f, l);
   }

   struct print_object_helper
   {
      template <int I, typename Struct>
      static void print_member(printer& pr, Struct const& attr)
      {
         auto name = fusion::extension::struct_member_name<Struct, I>::call();
         pr.print_impl(name, name+std::strlen(name));
         pr._out << " : ";
         pr.print(fusion::at_c<I>(attr));
      }

      template <int I, typename Struct>
      struct print_struct
      {
         static void call(printer& pr, Struct const& attr, bool last = true)
         {
            print_struct<I-1, Struct>::call(pr, attr, false);
            print_member<I>(pr, attr);
            if (!last)
            {
               pr._out << ',';
               pr.endl();
            }
         }
      };

      template <typename Struct>
      struct print_struct<0, Struct>
      {
         static void call(printer& pr, Struct const& attr, bool last = true)
         {
            print_member<0>(pr, attr);
            if (!last)
            {
               pr._out << ',';
               pr.endl();
            }
         }
      };
   };

   template <typename Attr>
   inline typename std::enable_if<is_adapted_struct<Attr>::value>::type
   printer::print_impl(Attr const& attr)
   {
      static constexpr auto size = fusion::result_of::size<Attr>::value;
      _out << '{';
      {
         indent();
         endl();
         print_object_helper::print_struct<size-1, Attr>::call(*this, attr);
         dedent();
      }
      endl();
      _out << '}';
   }

   template <typename C>
   inline void printer::print_container(C const& c)
   {
      _out << '[';
      {
         indent();
         endl();
         for (auto const& e : c)
         {
            print_impl(e);
            if (&e != &c.back())
            {
               _out << ',';
               endl();
            }
         }
         dedent();
      }
      endl();
      _out << ']';
   }

   template <typename Attr>
   inline void printer::print_impl(std::vector<Attr> const& attr)
   {
      print_container(attr);
   }

   template <typename Attr, std::size_t N>
   inline void printer::print_impl(std::array<Attr, N> const& attr)
   {
      print_container(attr);
   }

   template <typename Attr, std::size_t N>
   inline void printer::print_impl(Attr const (&attr)[N])
   {
      print_container(attr);
   }
}}}

namespace boost { namespace spirit { namespace x3 { namespace traits
{
   namespace json = cycfi::photon::json;

   template <typename Attribute>
   struct is_substitute<json::pair_parser::attribute, Attribute
    , typename std::enable_if<
         json::is_adapted_struct<typename std::remove_pointer<Attribute>::type>::value
      >::type>
   : mpl::true_ {};

   template <typename CharT, typename Traits, typename Allocator>
   struct is_substitute<json::parser::attribute, std::basic_string<CharT, Traits, Allocator>>
   : mpl::true_ {};

}}}}

#endif
