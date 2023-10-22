/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TEXT_UTILS_MAY_22_2016)
#define ELEMENTS_TEXT_UTILS_MAY_22_2016

#include <artist/canvas.hpp>
#include <artist/font.hpp>
#include <string_view>

namespace cycfi::elements
{
   using artist::canvas;
   using artist::rect;
   using artist::point;
   using artist::color;
   using artist::font_descr;

   ////////////////////////////////////////////////////////////////////////////
   void           draw_icon(canvas& cnv, rect bounds, uint32_t code, float size);
   void           draw_icon(canvas& cnv, rect bounds, uint32_t code, float size, color c);
   point          measure_icon(canvas& cnv, uint32_t cp, float size);
   point          measure_text(canvas& cnv, std::string_view text, font_descr font_);

////////////////////////////////////////////////////////////////////////////
   // Helper for converting char8_t[] string literals to char[]
   ////////////////////////////////////////////////////////////////////////////

#if __cplusplus >= 202002L // C++20
   // see https://developercommunity.visualstudio.com/t/C20-String-literal-operator-template-u/1318552
#ifdef __INTELLISENSE__
   consteval const char* operator""_as_char([[maybe_unused]] const char8_t*, [[maybe_unused]] std::size_t)
   {
      return "";
   }
#else
   template<std::size_t N>
   struct char8_t_string_literal
   {
      char8_t buffer[N];
      static constexpr inline std::size_t size = N;

      template<std::size_t... I>
      constexpr char8_t_string_literal(const char8_t(&r)[N], std::index_sequence<I...>)
         : buffer{r[I]...} {}

      constexpr char8_t_string_literal(const char8_t(&r)[N])
         : char8_t_string_literal(r, std::make_index_sequence<N>()) {}
   };

   template<char8_t_string_literal L, std::size_t... I>
   constexpr inline const char as_char_buffer[sizeof...(I)] = {static_cast<char>(L.buffer[I])...};

   template<char8_t_string_literal L, std::size_t... I>
   constexpr auto& make_as_char_buffer(std::index_sequence<I...>)
   {
      return as_char_buffer<L, I...>;
   }

   constexpr char operator ""_as_char(char8_t c)
   {
      return c;
   }

   template<char8_t_string_literal L>
   constexpr auto& operator""_as_char()
   {
      return make_as_char_buffer<L>(std::make_index_sequence<decltype(L)::size>());
   }
#endif
#else
   constexpr const char* operator""_as_char(const char* src, std::size_t)
   {
      return src;
   }
#endif
}

#endif
