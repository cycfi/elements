/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)

   Data transfer between a payload and Windows clipboard formats: what the
   clipboard and a drop (an IDataObject) hold. Both give the bytes of a
   format; these map the formats to MIME types and the bytes to the
   payload's data, both ways.
=============================================================================*/
#if !defined(ELEMENTS_WIN32_TRANSFER_OCTOBER_10_2026)
#define ELEMENTS_WIN32_TRANSFER_OCTOBER_10_2026

#include <windows.h>
#include <elements/support/payload.hpp>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace cycfi::elements::transfer
{
   // The MIME type of a clipboard format, or empty if it has none.
   std::string mime_type(UINT format);

   // The MIME types of a list of formats, each once, in their order.
   std::vector<std::string> mime_types_of(std::vector<UINT> const& formats);

   // The format to put a MIME type on the clipboard in.
   UINT format(std::string const& mime);

   // Read each of the MIME types asked for that are among the formats
   // available, through `bytes`, which gives the bytes of a format.
   payload read(
      std::vector<UINT> const& available, mime_types const& types
    , std::function<std::optional<std::string>(UINT format)> const& bytes);

   // A payload entry as the bytes of its format.
   std::string to_bytes(UINT format, std::string const& mime, payload_data const& data);

   // The bytes of global memory, and global memory holding bytes.
   std::optional<std::string> global_bytes(HGLOBAL global);
   HGLOBAL make_global(std::string const& bytes);
}

#endif
