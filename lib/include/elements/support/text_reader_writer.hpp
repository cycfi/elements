/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TEXT_READER_WRITER_APRIL_17_2016)
#define ELEMENTS_TEXT_READER_WRITER_APRIL_17_2016

#include <string_view>

namespace cycfi::elements
{
   /**
    * @brief
    *    This class represents a text reader for UTF-8 encoded text.
    *
    *    It defines the interface for classes that are capable of reading
    *    UTF-8 text. Derived classes must implement the `get_text()` function.
    */
   class text_reader_u8
   {
   public:

      virtual                       ~text_reader_u8() = default;
      virtual std::string_view      get_text() const = 0;
   };

   /**
    * @brief
    *    This class defines an interface for writing UTF-8 encoded text.
    *
    *    It serves as a base class for writing or updating UTF-8 text text.
    *    Derived classes must implement the `set_text(std::u8string_view
    *    text)` function.
    */
   class text_writer_u8
   {
   public:

      virtual                       ~text_writer_u8() = default;
      virtual void                  set_text(std::string_view text) = 0;
   };

   /**
    * @brief
    *    This class provides an interface for reading UTF-32 encoded text.
    *
    *    It serves as a base class for classes that are capable of reading
    *    text encoded in UTF-32. Derived classes must implement the
    *    `get_text()` function.
    */
   class text_reader_u32
   {
   public:

      virtual                       ~text_reader_u32() = default;
      virtual std::u32string_view   get_text() const = 0;
   };

   /**
    * @brief
    *    This class serves as a base class for writing UTF-32 encoded text.
    *
    *    Designed to be used as an interface for classes that need to write
    *    UTF-32 encoded text. Derived classes must implement the
    *    `set_text(std::u32string_view text)` function.
    */
   class text_writer_u32
   {
   public:

      virtual                       ~text_writer_u32() = default;
      virtual void                  set_text(std::u32string_view text) = 0;
   };
}

#endif
