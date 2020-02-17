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
            [](int ch) { return !std::isspace(ch); }
         ));
      }

      inline void rtrim(std::string& s)
      {
         s.erase(std::find_if(s.rbegin(), s.rend(),
            [](int ch) { return !std::isspace(ch); }
         ).base(), s.end());
      }

      inline void trim(std::string& s)
      {
         ltrim(s);
         rtrim(s);
      }

      inline float lerp(float a, float b, float f)
      {
         return (a * (1.0 - f)) + (b * f);
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

      enum
      {
         fc_thin            = 0,
         fc_extralight      = 40,
         fc_light           = 50,
         fc_semilight       = 55,
         fc_book            = 75,
         fc_normal          = 80,
         fc_medium          = 100,
         fc_semibold        = 180,
         fc_bold            = 200,
         fc_extrabold       = 205,
         fc_black           = 210
      };

      int map_fc_weight(int w)
      {
         auto&& map = [](double mina, double maxa, double minb, double maxb, double val)
         {
            return lerp(mina, maxa, (val-minb)/(maxb-minb));
         };

         if (w < fc_extralight)
            return map(font_descr::thin, font_descr::extra_light, fc_thin, fc_extralight, w);
         if (w < fc_light)
            return map(font_descr::extra_light, font_descr::light, fc_extralight, fc_light, w);
         if (w < fc_normal)
            return map(font_descr::light, font_descr::normal, fc_light, fc_normal, w);
         if (w < fc_medium)
            return map(font_descr::normal, font_descr::medium, fc_normal, fc_medium, w);
         if (w < fc_semibold)
            return map(font_descr::medium, font_descr::semi_bold, fc_medium, fc_semibold, w);
         if (w < fc_bold)
            return map(font_descr::semi_bold, font_descr::bold, fc_semibold, fc_bold, w);
         if (w < fc_extrabold)
            return map(font_descr::bold, font_descr::extra_bold, fc_bold, fc_extrabold, w);
         if (w < fc_black)
            return map(font_descr::extra_bold, font_descr::black, fc_extrabold, fc_black, w);
         return map(font_descr::black, 100, fc_black, 220, std::min(w, 220));
      }

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
                  entry.weight = map_fc_weight(weight); // map the weight

               int slant;
               if (FcPatternGetInteger(font, FC_SLANT, 0, &slant) == FcResultMatch)
                  entry.slant = (slant * 100) / 110; // normalize 0 to 100

               int width;
               if (FcPatternGetInteger(font, FC_WIDTH, 0, &width) == FcResultMatch)
                  entry.stretch = (width * 100) / 200; // normalize 0 to 100

               std::string key = (const char*) family;
               trim(key);

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
            trim(family);
            if (auto i = font_map.find(family); i != font_map.end())
            {
               int min = 10000;
               std::vector<font_entry>::const_iterator best_match = i->second.end();
               for (auto j = i->second.begin(); j != i->second.end(); ++j)
               {
                  auto const& item = *j;

                  // Get biased score (lower is better). Give `weight` attribute
                  // the highest bias (1.0), followed by `slant` (0.5) and then
                  // `stretch` (0.25).
                  auto diff =
                     (std::abs(int(descr.weight) - int(item.weight)) * 1.0) +
                     (std::abs(int(descr.slant) - int(item.slant)) * 0.5) +
                     (std::abs(int(descr.stretch) - int(item.stretch)) * 0.25)
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


