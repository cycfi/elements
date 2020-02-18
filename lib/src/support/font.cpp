/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/font.hpp>
#include <infra/assert.hpp>

#include <cairo.h>
#include <cairo-ft.h>
#include <fontconfig/fontconfig.h>
#include <boost/filesystem.hpp>

#ifndef __APPLE__
# include <ft2build.h>
# include <freetype/ttnameid.h>
# include FT_SFNT_NAMES_H
# include FT_FREETYPE_H
# include FT_GLYPH_H
# include FT_OUTLINE_H
# include FT_BBOX_H
# include FT_TYPE1_TABLES_H
#else
# include <cairo-quartz.h>
#endif

#include <map>
#include <mutex>
#include <sstream>
#include <algorithm>
#include <vector>

namespace cycfi { namespace elements
{
   namespace fs = boost::filesystem;
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
         std::string    full_name;
         std::string    file;
         uint8_t        weight   = font_constants::weight_normal;
         uint8_t        slant    = font_constants::slant_normal;
         uint8_t        stretch  = font_constants::stretch_normal;
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

         namespace fc = font_constants;

         if (w < fc_extralight)
            return map(fc::thin, fc::extra_light, fc_thin, fc_extralight, w);
         if (w < fc_light)
            return map(fc::extra_light, fc::light, fc_extralight, fc_light, w);
         if (w < fc_normal)
            return map(fc::light, fc::weight_normal, fc_light, fc_normal, w);
         if (w < fc_medium)
            return map(fc::weight_normal, fc::medium, fc_normal, fc_medium, w);
         if (w < fc_semibold)
            return map(fc::medium, fc::semi_bold, fc_medium, fc_semibold, w);
         if (w < fc_bold)
            return map(fc::semi_bold, fc::bold, fc_semibold, fc_bold, w);
         if (w < fc_extrabold)
            return map(fc::bold, fc::extra_bold, fc_bold, fc_extrabold, w);
         if (w < fc_black)
            return map(fc::extra_bold, fc::black, fc_extrabold, fc_black, w);
         return map(fc::black, 100, fc_black, 220, std::min(w, 220));
      }

      void init_font_map()
      {
         FcConfig*      config = FcInitLoadConfigAndFonts();

#ifdef __APPLE__
         auto resources_path = fs::current_path().parent_path() / "Resources";
#else
         auto resources_path = fs::current_path() / "resources";
#endif
         FcConfigAppFontAddDir(config, (FcChar8 const*)resources_path.c_str());
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
                  entry.weight = map_fc_weight(weight); // map the weight (normalized 0 to 100)

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

         std::istringstream str(std::string{ descr._families });
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

                  // Get biased score (lower is better). Give `slant` attribute
                  // the highest bias (3.0), followed by `weight` (1.0) and then
                  // `stretch` (0.25).
                  auto diff =
                     (std::abs(int(descr._weight) - int(item.weight)) * 1.0) +
                     (std::abs(int(descr._slant) - int(item.slant)) * 3.0) +
                     (std::abs(int(descr._stretch) - int(item.stretch)) * 0.25)
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

#ifndef __APPLE__
      struct free_type_font_loader
      {
         free_type_font_loader()
         {
            FT_Error status = FT_Init_FreeType(&ft_lib_value);
            CYCFI_ASSERT(status == 0, "Error: failed to initialize free type library.");
         }

         cairo_font_face_t* load(char const* font_path)
         {
            FT_Face face;
            FT_Error status = FT_New_Face(ft_lib_value, font_path, 0, &face);
            if (status != 0)
               return nullptr;
            return cairo_ft_font_face_create_for_ft_face(face, 0);
         }

         FT_Library ft_lib_value;
      };
#endif
   }

   font::font(font_descr descr)
   {
#ifndef __APPLE__
      static free_type_font_loader font_loader;
#endif

      auto match_ptr = match(descr);
      if (match_ptr)
      {
         std::lock_guard<std::mutex> lock(cairo_font_map_mutex);
         if (auto it = cairo_font_map.find(match_ptr->full_name); it != cairo_font_map.end())
         {
            _handle = cairo_font_face_reference(it->second);
         }
         else
         {
#ifdef __APPLE__
            _handle = cairo_quartz_font_face_create_for_cgfont(
               CGFontCreateWithFontName(
                  CFStringCreateWithCString(
                     kCFAllocatorDefault
                   , match_ptr->full_name.c_str()
                   , kCFStringEncodingUTF8
                  )
               )
            );
#else
            _handle = font_loader.load(match_ptr->file.c_str());
#endif

            if (_handle)
               cairo_font_map[match_ptr->full_name] = cairo_font_face_reference(_handle);
         }
      }
      else
      {
         _handle = nullptr;
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
      if (_handle)
         cairo_font_face_destroy(_handle);
   }
}}


