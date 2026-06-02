/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_PAYLOAD_OCTOBER_31_2023)
#define ELEMENTS_PAYLOAD_OCTOBER_31_2023

#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <variant>
#include <cstddef>
#include <infra/filesystem.hpp>

namespace cycfi::elements
{
   /**
    * \typedef payload_data
    *
    * \brief
    *    Type alias representing data contained within a payload.
    *
    *    The `payload_data` type alias represents the two possible types of
    *    data contained within a payload: textual data (as `std::string`) and
    *    binary data (as `std::vector<std::byte>`).
    */
   using payload_data = std::variant<std::string, std::vector<std::byte>>;

   /**
    * \struct payload
    *
    * \brief
    *    A container for transferring information, inheriting from `std::map`.
    *
    *    Payload is a container for transferring information. Payload is a
    *    std::map with std::string keys that define the format of the data
    *    using MIME types (e.g., "text/plain"). The actual data is a
    *    std::variant<std::string, std::vector<std::byte>>. std::string for
    *    text and std::vector<std::byte> for everything else.
    */
   struct payload : std::map<std::string, payload_data>
   {
      using base_type = std::map<std::string, payload_data>;
      using base_type::base_type;
      using base_type::operator=;
   };

   bool                    contains_filepaths(payload const& data);
   std::vector<fs::path>   get_filepaths(payload const& data);
}

#endif

