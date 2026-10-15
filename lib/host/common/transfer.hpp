/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   Data transfer between a payload and the formats other applications use:
   what the clipboard and a drop hold, by name. Shared by the hosts whose
   native formats are named by MIME type (X11, Wayland, GTK), each of which
   moves the bytes its own way.

   In a payload, text is a std::string of UTF-8, and "text/uri-list" is one
   file:// URI per line, separated by '\n', each path as the file system has
   it (see get_filepaths). On the wire, text comes under several names, and
   a uri-list is RFC 2483's: CRLF line ends, '#' comment lines, and paths
   percent-encoded.
=============================================================================*/
#if !defined(ELEMENTS_HOST_TRANSFER_OCTOBER_10_2026)
#define ELEMENTS_HOST_TRANSFER_OCTOBER_10_2026

#include <elements/support/payload.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cycfi::elements::transfer
{
   ////////////////////////////////////////////////////////////////////////////
   // Names
   ////////////////////////////////////////////////////////////////////////////

   // The names "text/plain" goes by, best first: the UTF-8 ones, then plain
   // text of unstated encoding, then X11's Latin-1 STRING. Not X11's TEXT,
   // whose reply may come in any encoding.
   inline std::vector<std::string> const& text_names()
   {
      static std::vector<std::string> const names = {
         "text/plain;charset=utf-8",
         "UTF8_STRING",
         "text/plain",
         "STRING"
      };
      return names;
   }

   inline bool is_latin1_name(std::string_view name)
   {
      return name == "STRING";
   }

   // The payload's MIME type for a native name, or nothing if the name is
   // not data (X11's TARGETS, TIMESTAMP and the like) or not a type we can
   // take as is (text in an encoding other than UTF-8).
   inline std::optional<std::string> mime_type(std::string_view name)
   {
      for (auto const& text : text_names())
      {
         if (name == text)
            return std::string{"text/plain"};
      }
      if (name.find('/') == std::string_view::npos)
         return std::nullopt;        // TARGETS, MULTIPLE, SAVE_TARGETS...
      if (name.substr(0, 11) == "text/plain;")
         return std::nullopt;        // another charset
      return std::string{name};
   }

   // The native names to offer a payload's MIME type under.
   inline std::vector<std::string> native_names(std::string_view mime)
   {
      if (mime == "text/plain")
         return text_names();
      return {std::string{mime}};
   }

   // The MIME types of a list of native names, each once, in their order.
   inline std::vector<std::string> mime_types_of(std::vector<std::string> const& names)
   {
      std::vector<std::string> types;
      for (auto const& name : names)
      {
         if (auto mime = mime_type(name))
         {
            bool seen = false;
            for (auto const& t : types)
               seen = seen || t == *mime;
            if (!seen)
               types.push_back(*mime);
         }
      }
      return types;
   }

   // Of the native names on offer, the best one to ask for the MIME type
   // under, or nothing if none of them carries it.
   inline std::optional<std::string>
   best_name(std::string_view mime, std::vector<std::string> const& offered)
   {
      for (auto const& name : native_names(mime))
      {
         for (auto const& o : offered)
         {
            if (o == name)
               return name;
         }
      }
      return std::nullopt;
   }

   // Of the MIME types a drag offers, those the view takes drops of: all
   // of them if it was never told (see base_view::accept_drop_types).
   inline std::vector<std::string> accepted_types(
      std::vector<std::string> const& offered, std::optional<mime_types> const& accepted)
   {
      if (!accepted)
         return offered;
      std::vector<std::string> types;
      for (auto const& type : offered)
      {
         if (accepted->count(type))
            types.push_back(type);
      }
      return types;
   }

   ////////////////////////////////////////////////////////////////////////////
   // text/uri-list
   ////////////////////////////////////////////////////////////////////////////
   inline std::string percent_decode(std::string_view s)
   {
      auto hex = [](char c) -> int
      {
         if (c >= '0' && c <= '9') return c - '0';
         if (c >= 'a' && c <= 'f') return c - 'a' + 10;
         if (c >= 'A' && c <= 'F') return c - 'A' + 10;
         return -1;
      };
      std::string out;
      out.reserve(s.size());
      for (std::size_t i = 0; i < s.size(); ++i)
      {
         if (s[i] == '%' && i + 2 < s.size())
         {
            int hi = hex(s[i+1]), lo = hex(s[i+2]);
            if (hi >= 0 && lo >= 0)
            {
               out += char((hi << 4) | lo);
               i += 2;
               continue;
            }
         }
         out += s[i];
      }
      return out;
   }

   // Percent-encode all but the characters a URI path may hold as they are.
   inline std::string percent_encode_path(std::string_view s)
   {
      static char const digits[] = "0123456789ABCDEF";
      std::string out;
      out.reserve(s.size());
      for (char c : s)
      {
         auto u = static_cast<unsigned char>(c);
         bool plain =
               (u >= 'a' && u <= 'z') || (u >= 'A' && u <= 'Z')
            || (u >= '0' && u <= '9')
            || u == '/' || u == '-' || u == '_' || u == '.' || u == '~';
         if (plain)
         {
            out += c;
         }
         else
         {
            out += '%';
            out += digits[u >> 4];
            out += digits[u & 0xf];
         }
      }
      return out;
   }

   // A uri-list off the wire, in the payload's form.
   inline std::string uri_list_from_wire(std::string_view in)
   {
      std::string out;
      std::size_t i = 0;
      while (i < in.size())
      {
         std::size_t e = in.find('\n', i);
         std::string_view line =
            in.substr(i, e == std::string_view::npos ? std::string_view::npos : e - i);
         i = (e == std::string_view::npos) ? in.size() : e + 1;
         while (!line.empty() &&
                (line.back() == '\r' || line.back() == ' ' || line.back() == '\t'
                 || line.back() == '\0'))
            line.remove_suffix(1);
         if (line.empty() || line.front() == '#')
            continue;
         if (!out.empty())
            out += '\n';
         // A file's path is decoded, as the file system has it. Any other
         // URI stays as it is: decoding would change what it means.
         // "file://localhost/x" is a local file all the same: "file:///x".
         std::string_view const localhost = "file://localhost/";
         if (line.substr(0, localhost.size()) == localhost)
         {
            out += "file://";
            line.remove_prefix(localhost.size() - 1);
            out += percent_decode(line);
         }
         else if (line.substr(0, 7) == "file://")
         {
            out += percent_decode(line);
         }
         else
         {
            out += line;
         }
      }
      return out;
   }

   // A uri-list in the payload's form, for the wire.
   inline std::string uri_list_to_wire(std::string_view in)
   {
      std::string out;
      std::size_t i = 0;
      while (i < in.size())
      {
         std::size_t e = in.find('\n', i);
         std::string_view line =
            in.substr(i, e == std::string_view::npos ? std::string_view::npos : e - i);
         i = (e == std::string_view::npos) ? in.size() : e + 1;
         if (line.empty())
            continue;
         if (line.substr(0, 7) == "file://")
            out += "file://" + percent_encode_path(line.substr(7));
         else
            out += line;            // another scheme: already a URI
         out += "\r\n";
      }
      return out;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Data
   ////////////////////////////////////////////////////////////////////////////
   inline std::string latin1_to_utf8(std::string_view s)
   {
      std::string out;
      out.reserve(s.size());
      for (char c : s)
      {
         auto u = static_cast<unsigned char>(c);
         if (u < 0x80)
         {
            out += c;
         }
         else
         {
            out += char(0xC0 | (u >> 6));
            out += char(0x80 | (u & 0x3F));
         }
      }
      return out;
   }

   // Bytes received under a native name, as the payload holds them: text
   // types as a string, everything else as bytes.
   inline payload_data from_wire(std::string_view name, std::string_view mime, std::string_view bytes)
   {
      if (mime == "text/plain")
      {
         // Some sources end text with a NUL; it is not part of the text.
         while (!bytes.empty() && bytes.back() == '\0')
            bytes.remove_suffix(1);
         if (is_latin1_name(name))
            return latin1_to_utf8(bytes);
         return std::string{bytes};
      }
      if (mime == "text/uri-list")
         return uri_list_from_wire(bytes);
      if (mime.substr(0, 5) == "text/")
         return std::string{bytes};
      auto const* p = reinterpret_cast<std::byte const*>(bytes.data());
      return std::vector<std::byte>(p, p + bytes.size());
   }

   // A payload entry, as the bytes to send under a native name.
   inline std::string to_wire(std::string_view name, std::string_view mime, payload_data const& data)
   {
      std::string bytes;
      if (auto const* s = std::get_if<std::string>(&data))
         bytes = *s;
      else
      {
         auto const& b = std::get<std::vector<std::byte>>(data);
         bytes.assign(reinterpret_cast<char const*>(b.data()), b.size());
      }

      if (mime == "text/uri-list")
         return uri_list_to_wire(bytes);
      if (mime == "text/plain" && is_latin1_name(name))
      {
         // UTF-8 to Latin-1, with '?' for what Latin-1 does not have.
         std::string out;
         for (std::size_t i = 0; i < bytes.size(); ++i)
         {
            auto u = static_cast<unsigned char>(bytes[i]);
            if (u < 0x80)
               out += char(u);
            else if ((u & 0xE0) == 0xC0 && i + 1 < bytes.size() && u <= 0xC3)
               out += char(((u & 0x1F) << 6) | (static_cast<unsigned char>(bytes[++i]) & 0x3F));
            else
            {
               out += '?';
               while (i + 1 < bytes.size() && (static_cast<unsigned char>(bytes[i+1]) & 0xC0) == 0x80)
                  ++i;
            }
         }
         return out;
      }
      return bytes;
   }

   // An entry the host set itself, as a round trip through the system
   // clipboard would return it: text types as a string, the rest as bytes.
   inline payload_data as_received(std::string_view mime, payload_data const& data)
   {
      return from_wire(mime, mime, to_wire(mime, mime, data));
   }
}

#endif
