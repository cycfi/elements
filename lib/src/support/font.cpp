/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/font.hpp>
#include <map>
#include <mutex>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cairo.h>

#include <fontconfig/fontconfig.h>

#ifdef CAIRO_HAS_QUARTZ_FONT
#include <cairo-quartz.h>
#endif

namespace cycfi { namespace elements
{
   namespace
   {
      inline void ltrim(std::string& s)
      {
         s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            [](int ch)
            {
               return !std::isspace(ch);
            }
         ));
      }

      inline void rtrim(std::string& s)
      {
         s.erase(std::find_if(s.rbegin(), s.rend(),
            [](int ch)
            {
               return !std::isspace(ch);
            }
         ).base(), s.end());
      }

      std::map<std::string, cairo_font_face_t*> cairo_font_map;
      std::mutex cairo_font_map_mutex;

      struct cleanup
      {
         ~cleanup()
         {
            std::lock_guard<std::mutex> lock(cairo_font_map_mutex);
            for (auto [key, scaled_font] : cairo_font_map)
               cairo_font_face_destroy(scaled_font);
            cairo_font_map.clear();
         }
      };

      cleanup cleanup_;

      struct font_entry
      {
         std::string full_name;
         std::string file;
         uint8_t     weight   = font_descr::normal;
         uint8_t     slant    = font_descr::style_normal;
         uint8_t     stretch  = font_descr::stretch_normal;
      };

      std::map<std::string, std::vector<font_entry>> font_map;

      void init_font_map()
      {
         FcConfig*      config = FcInitLoadConfigAndFonts();
         FcPattern*     pat = FcPatternCreate();
         FcObjectSet*   os = FcObjectSetBuild(
                                 FC_FAMILY, FC_FULLNAME, FC_WIDTH, FC_WEIGHT
                               , FC_SLANT, FC_FILE, nullptr);
         FcFontSet*     fs = FcFontList(config, pat, os);

         for (int i=0; fs && i < fs->nfont; ++i)
         {
            FcPattern* font = fs->fonts[i];
            FcChar8 *file, *family, *full_name;
            if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
               FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
               FcPatternGetString(font, FC_FULLNAME, 0, &full_name) == FcResultMatch
            )
            {
               font_entry entry;
               entry.full_name = (const char*) full_name;
               entry.file = (const char*) file;

               int weight;
               if (FcPatternGetInteger(font, FC_WEIGHT, 0, &weight) == FcResultMatch)
                  entry.weight = (weight * 105) / 210; // normalize 0 to 100

               int slant;
               if (FcPatternGetInteger(font, FC_SLANT, 0, &slant) == FcResultMatch)
                  entry.slant = (slant * 100) / 110; // normalize 0 to 100

               int width;
               if (FcPatternGetInteger(font, FC_WIDTH, 0, &width) == FcResultMatch)
                  entry.stretch = (width * 100) / 200; // normalize 0 to 100

               std::string key = (const char*) family;
               ltrim(key);
               rtrim(key);

               if (auto i = font_map.find(key); i != font_map.end())
               {
                  i->second.push_back(entry);
               }
               else
               {
                  font_map[key] = {};
                  font_map[key].push_back(entry);
               }
            }
         }
         if (fs)
            FcFontSetDestroy(fs);
      }

      font_entry const* match(font_descr descr)
      {
         if (font_map.empty())
            init_font_map();

         std::istringstream str(std::string{ descr.families });
         std::string family;
         while (getline(str, family, ','))
         {
            ltrim(family);
            rtrim(family);

            if (auto i = font_map.find(family); i != font_map.end())
            {
               int min = 10000;
               std::vector<font_entry>::const_iterator best_match = i->second.end();
               for (auto j = i->second.begin(); j != i->second.end(); ++j)
               {
                  auto const& item = *j;
                  auto diff =
                     std::abs(int(descr.weight) - int(item.weight)) +
                     std::abs(int(descr.slant) - int(item.slant)) +
                     std::abs(int(descr.stretch) - int(item.stretch))
                     ;
                  if (diff < min)
                  {
                     min = diff;
                     best_match = j;
                  }
               }
               if (best_match != i->second.end())
                  return &*best_match;
            }
         }
         return nullptr;
      }
   }

   font::font(font_descr descr)
   {
      auto match_ptr = match(descr);
   }

   font::font(char const* face)
   {
      std::lock_guard<std::mutex> lock(cairo_font_map_mutex);
      if (auto it = cairo_font_map.find(face); it != cairo_font_map.end())
      {
         _handle = cairo_font_face_reference(it->second);
      }
      else
      {
#ifdef CAIRO_HAS_QUARTZ_FONT
         _handle = cairo_quartz_font_face_create_for_cgfont(
            CGFontCreateWithFontName(
               CFStringCreateWithCString(kCFAllocatorDefault, face, kCFStringEncodingUTF8)
            )
         );
#else
         // $$$ TODO: use a non-toy font selection facility for windows and linux $$$
         _handle = cairo_toy_font_face_create(face, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
#endif
         cairo_font_map[face] = cairo_font_face_reference(_handle);
      }
   }

   font::font(font const& rhs)
   {
      _handle = cairo_font_face_reference(rhs._handle);
   }

   font& font::operator=(font const& rhs)
   {
      if (&rhs != this)
         _handle = cairo_font_face_reference(rhs._handle);
      return *this;
   }

   font::~font()
   {
      cairo_font_face_destroy(_handle);
   }
}}


