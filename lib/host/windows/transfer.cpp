/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include "transfer.hpp"
#include <shlobj.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>

namespace cycfi::elements
{
   // UTF8 conversion utils defined in base_view.cpp
   std::string utf8_encode(std::wstring const& wstr);
   std::wstring utf8_decode(std::string const& str);
}

namespace cycfi::elements::transfer
{
   namespace
   {
      UINT html_format()
      {
         static UINT const format = RegisterClipboardFormatW(L"HTML Format");
         return format;
      }

      UINT png_format()
      {
         static UINT const format = RegisterClipboardFormatW(L"PNG");
         return format;
      }

      std::wstring format_name(UINT format)
      {
         wchar_t name[256];
         int n = GetClipboardFormatNameW(format, name, 256);
         return std::wstring(name, n > 0 ? n : 0);
      }

      // Text of a null-terminated UTF-16 buffer, which may be shorter than
      // the memory holding it.
      std::string from_utf16(std::string const& bytes)
      {
         std::wstring text(
            reinterpret_cast<wchar_t const*>(bytes.data()), bytes.size() / sizeof(wchar_t));
         text.resize(std::wcslen(text.c_str()));
         return utf8_encode(text);
      }

      std::string to_utf16(std::string const& text)
      {
         std::wstring w = utf8_decode(text);
         return std::string(
            reinterpret_cast<char const*>(w.c_str()), (w.size() + 1) * sizeof(wchar_t));
      }

      std::string from_ansi(std::string const& bytes)
      {
         std::string text(bytes.c_str());     // up to the null
         if (text.empty())
            return {};
         int n = MultiByteToWideChar(CP_ACP, 0, text.data(), int(text.size()), nullptr, 0);
         std::wstring w(n, 0);
         MultiByteToWideChar(CP_ACP, 0, text.data(), int(text.size()), &w[0], n);
         return utf8_encode(w);
      }

      // CF_HDROP: a DROPFILES header, then the paths, each null-terminated,
      // and an empty one to end them. In a payload, file:// before each
      // path, one a line.
      std::string from_hdrop(std::string const& bytes)
      {
         if (bytes.size() < sizeof(DROPFILES))
            return {};
         DROPFILES header;
         std::memcpy(&header, bytes.data(), sizeof header);
         if (header.pFiles >= bytes.size())
            return {};

         std::string paths;
         auto add = [&](std::string const& path)
         {
            if (!paths.empty())
               paths += "\n";
            paths += "file://" + path;
         };

         char const* p = bytes.data() + header.pFiles;
         char const* end = bytes.data() + bytes.size();
         if (header.fWide)
         {
            for (;;)
            {
               std::wstring path;
               while (p + 1 < end && (p[0] || p[1]))
               {
                  wchar_t c;
                  std::memcpy(&c, p, sizeof c);
                  path += c;
                  p += sizeof c;
               }
               p += sizeof(wchar_t);
               if (path.empty() || p > end)
                  break;
               add(utf8_encode(path));
            }
         }
         else
         {
            for (;;)
            {
               std::string path;
               while (p < end && *p)
                  path += *p++;
               ++p;
               if (path.empty() || p > end)
                  break;
               add(from_ansi(path));
            }
         }
         return paths;
      }

      // Empty if the uri-list names no files.
      std::string to_hdrop(std::string const& uri_list)
      {
         // The paths are UTF-8: converted here, not through fs::path,
         // which takes a narrow string in the ANSI code page.
         std::wstring files;
         std::size_t i = 0;
         while (i < uri_list.size())
         {
            std::size_t e = uri_list.find('\n', i);
            if (e == std::string::npos)
               e = uri_list.size();
            std::string line = uri_list.substr(i, e - i);
            i = e + 1;
            if (line.substr(0, 7) == "file://" && line.size() > 7)
            {
               files += utf8_decode(line.substr(7));
               files += L'\0';
            }
         }
         if (files.empty())
            return {};
         files += L'\0';

         DROPFILES header = {};
         header.pFiles = sizeof header;
         header.fWide = TRUE;
         std::string bytes(reinterpret_cast<char const*>(&header), sizeof header);
         bytes.append(reinterpret_cast<char const*>(files.data()), files.size() * sizeof(wchar_t));
         return bytes;
      }

      // CF_HTML: a header of offsets into the UTF-8 that follows, which
      // marks the fragment copied within a document. In a payload, the
      // fragment.
      std::string from_cf_html(std::string const& bytes)
      {
         std::string html(bytes.c_str());     // up to the null
         auto offset = [&](char const* key) -> long
         {
            auto i = html.find(key);
            if (i == std::string::npos)
               return -1;
            return std::strtol(html.c_str() + i + std::strlen(key), nullptr, 10);
         };
         long start = offset("StartFragment:");
         long end = offset("EndFragment:");
         if (start < 0 || end < start || std::size_t(end) > html.size())
            return html;
         return html.substr(start, end - start);
      }

      std::string to_cf_html(std::string const& fragment)
      {
         char const* header =
            "Version:0.9\r\n"
            "StartHTML:%010zu\r\n"
            "EndHTML:%010zu\r\n"
            "StartFragment:%010zu\r\n"
            "EndFragment:%010zu\r\n";
         std::string const pre = "<html><body>\r\n<!--StartFragment-->";
         std::string const post = "<!--EndFragment-->\r\n</body></html>";

         // The header's length does not depend on the numbers in it: each
         // is written in ten digits.
         char buf[256];
         std::size_t header_size = std::snprintf(buf, sizeof buf, header,
            std::size_t(0), std::size_t(0), std::size_t(0), std::size_t(0));
         std::size_t start_html = header_size;
         std::size_t start_fragment = start_html + pre.size();
         std::size_t end_fragment = start_fragment + fragment.size();
         std::size_t end_html = end_fragment + post.size();
         std::snprintf(buf, sizeof buf, header,
            start_html, end_html, start_fragment, end_fragment);
         return std::string(buf) + pre + fragment + post + '\0';
      }

      std::string as_bytes(payload_data const& data)
      {
         if (auto const* s = std::get_if<std::string>(&data))
            return *s;
         auto const& b = std::get<std::vector<std::byte>>(data);
         return std::string(reinterpret_cast<char const*>(b.data()), b.size());
      }

      payload_data from_bytes(UINT format, std::string const& mime, std::string const& bytes)
      {
         if (format == CF_UNICODETEXT)
            return from_utf16(bytes);
         if (format == CF_TEXT || format == CF_OEMTEXT)
            return from_ansi(bytes);
         if (format == CF_HDROP)
            return from_hdrop(bytes);
         if (format == html_format())
            return from_cf_html(bytes);
         if (mime.substr(0, 5) == "text/")
            return std::string(bytes.c_str(), std::strlen(bytes.c_str()));
         auto const* p = reinterpret_cast<std::byte const*>(bytes.data());
         return std::vector<std::byte>(p, p + bytes.size());
      }

      // The formats a MIME type may be read from, best first.
      std::vector<UINT> formats(std::string const& mime)
      {
         if (mime == "text/plain")
            return {CF_UNICODETEXT, CF_TEXT};
         return {format(mime)};
      }
   }

   std::string mime_type(UINT format)
   {
      switch (format)
      {
         case CF_UNICODETEXT:
         case CF_TEXT:
         case CF_OEMTEXT:
            return "text/plain";
         case CF_HDROP:
            return "text/uri-list";
      }
      if (format == html_format())
         return "text/html";
      if (format == png_format())
         return "image/png";
      std::string name = utf8_encode(format_name(format));
      if (name.find('/') != std::string::npos)
         return name;
      return {};
   }

   std::vector<std::string> mime_types_of(std::vector<UINT> const& formats)
   {
      std::vector<std::string> types;
      for (auto f : formats)
      {
         auto mime = mime_type(f);
         if (!mime.empty() && std::find(types.begin(), types.end(), mime) == types.end())
            types.push_back(mime);
      }
      return types;
   }

   UINT format(std::string const& mime)
   {
      if (mime == "text/plain")
         return CF_UNICODETEXT;
      if (mime == "text/uri-list")
         return CF_HDROP;
      if (mime == "text/html")
         return html_format();
      if (mime == "image/png")
         return png_format();
      return RegisterClipboardFormatW(utf8_decode(mime).c_str());
   }

   payload read(
      std::vector<UINT> const& available, mime_types const& types
    , std::function<std::optional<std::string>(UINT format)> const& bytes)
   {
      payload result;
      for (auto const& mime : types)
      {
         for (auto f : formats(mime))
         {
            if (std::find(available.begin(), available.end(), f) == available.end())
               continue;
            if (auto b = bytes(f))
            {
               result[mime] = from_bytes(f, mime, *b);
               break;
            }
         }
      }
      return result;
   }

   std::string to_bytes(UINT format, std::string const& mime, payload_data const& data)
   {
      auto bytes = as_bytes(data);
      if (format == CF_UNICODETEXT)
         return to_utf16(bytes);
      if (format == CF_HDROP)
         return to_hdrop(bytes);
      if (format == html_format())
         return to_cf_html(bytes);
      if (mime.substr(0, 5) == "text/")
         return bytes + '\0';
      return bytes;
   }

   std::optional<std::string> global_bytes(HGLOBAL global)
   {
      if (!global)
         return std::nullopt;
      auto const* p = static_cast<char const*>(GlobalLock(global));
      if (!p)
         return std::nullopt;
      std::string bytes(p, GlobalSize(global));
      GlobalUnlock(global);
      return bytes;
   }

   HGLOBAL make_global(std::string const& bytes)
   {
      HGLOBAL global = GlobalAlloc(GMEM_MOVEABLE, std::max<std::size_t>(bytes.size(), 1));
      if (!global)
         return nullptr;
      if (void* p = GlobalLock(global))
      {
         std::memcpy(p, bytes.data(), bytes.size());
         GlobalUnlock(global);
         return global;
      }
      GlobalFree(global);
      return nullptr;
   }
}
