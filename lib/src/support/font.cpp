/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman
   Copyright (c) 2020 Michał Urbański

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/font.hpp>
#include <infra/assert.hpp>

#include <cairo.h>
#include <cairo-ft.h>
#include <fontconfig/fontconfig.h>
#include <infra/filesystem.hpp>
#include <infra/optional.hpp>

#ifndef __APPLE__
# include <ft2build.h>
# include <freetype/ttnameid.h>
# include FT_SFNT_NAMES_H
# include FT_FREETYPE_H
# include FT_GLYPH_H
# include FT_OUTLINE_H
# include FT_BBOX_H
# include FT_TYPE1_TABLES_H
# if defined(ELEMENTS_HOST_UI_LIBRARY_WIN32)
#  include <Windows.h>
#  include "sysinfoapi.h"
#  include "tchar.h"
# endif
#else
# include <cairo-quartz.h>
#endif

#include <map>
#include <mutex>
#include <memory>
#include <sstream>
#include <algorithm>
#include <vector>
#include <utility>
#include <type_traits>

namespace cycfi { namespace elements
{
   namespace
   {
      inline void ltrim(std::string& s)
      {
         s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            [](int ch) { return ch != ' ' && ch != '"'; }
         ));
      }

      inline void rtrim(std::string& s)
      {
         s.erase(std::find_if(s.rbegin(), s.rend(),
            [](int ch) { return ch != ' ' && ch != '"'; }
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

#ifndef __APPLE__
      auto const& cairo_user_data_key()
      {
         static const cairo_user_data_key_t key = {};
         return key;
      }
#endif
      namespace fc
      {
         struct font_config_deleter
         {
            void operator()(FcConfig* ptr) const
            {
               FcConfigDestroy(ptr);
            }
         };

         using font_config_ptr = std::unique_ptr<FcConfig, font_config_deleter>;

         struct load_config_and_fonts_tag {};
         struct load_config_tag {};
         struct load_none_tag {};
         struct no_init_tag {};

         class config
         {
         public:
            config(no_init_tag)
            {
            }

            config(load_none_tag)
            : _config_ptr((FcInit() == FcTrue) ? FcConfigCreate() : nullptr)
            {
            }

            config(load_config_tag)
            : _config_ptr((FcInit() == FcTrue) ? FcInitLoadConfig() : nullptr)
            {
            }

            config(load_config_and_fonts_tag)
            : _config_ptr((FcInit() == FcTrue) ? FcInitLoadConfigAndFonts() : nullptr)
            {
            }

            ~config()
            {
            }

            explicit operator bool() const noexcept
            {
               return _config_ptr != nullptr;
            }

            FcConfig* get() const noexcept
            {
               return _config_ptr.get();
            }

            bool app_font_add_dir(FcChar8 const* path)
            {
               FcBool status = FcConfigAppFontAddDir(_config_ptr.get(), path);
               return status == FcTrue;
            }

         private:
            font_config_ptr _config_ptr = nullptr;
         };

         config& instance()
         {
            static config conf(fc::load_config_and_fonts_tag{});

            if (!conf)
              throw std::runtime_error("Failed to initialize font config library!");

            return conf;
         }

         struct pattern_empty_tag {};
         struct pattern_shallow_copy_tag {};

         class pattern
         {
         public:
            pattern() = default;

            pattern(pattern_empty_tag)
            : _pattern(FcPatternCreate())
            {
            }

            pattern(pattern_shallow_copy_tag, FcPattern& pattern)
            : _pattern(&pattern)
            {
               FcPatternReference(_pattern);
            }

            ~pattern()
            {
               if (_pattern)
                  FcPatternDestroy(_pattern);
            }

            pattern(pattern const& other)
            : pattern(pattern_shallow_copy_tag{}, *other._pattern)
            {
            }

            pattern& operator=(pattern other) noexcept
            {
               swap(*this, other);
               return *this;
            }

            pattern(pattern&& other) noexcept
            : pattern()
            {
               swap(*this, other);
            }

            friend void swap(pattern& lhs, pattern& rhs) noexcept
            {
               std::swap(lhs._pattern, rhs._pattern);
            }

            FcPattern* handle()
            {
               return _pattern;
            }

            optional<int> get_weight() const
            {
               return get_int(FC_WEIGHT);
            }

            optional<int> get_slant() const
            {
               return get_int(FC_SLANT);
            }

            optional<int> get_width() const
            {
               return get_int(FC_WIDTH);
            }

         private:
            optional<int> get_int(char const* object) const
            {
               int x;
               if (FcPatternGetInteger(_pattern, object, 0, &x) == FcResultMatch)
                  return x;
               else
                  return nullopt;
            }

            FcPattern* _pattern = nullptr;
         };

         class object_set
         {
         public:
            object_set() = default;

            template <typename... Args>
            object_set(Args&&... args)
            {
               static_assert((std::is_same_v<std::decay_t<Args>, char const*> && ...));
               _set = FcObjectSetBuild(std::forward<Args>(args)..., nullptr);
            }

            ~object_set()
            {
               if (_set)
                  FcObjectSetDestroy(_set);
            }

            object_set(object_set const& other) = delete;
            object_set& operator=(object_set const& other) = delete;

            object_set(object_set&& other) noexcept
            : object_set()
            {
               swap(*this, other);
            }

            object_set& operator=(object_set&& other) noexcept
            {
               swap(*this, other);
               return *this;
            }

            friend void swap(object_set& lhs, object_set& rhs) noexcept
            {
               std::swap(lhs._set, rhs._set);
            }

            FcObjectSet* handle()
            {
               return _set;
            }

         private:
            FcObjectSet* _set = nullptr;
         };

         struct font_set_deleter
         {
            void operator()(FcFontSet* set)
            {
               if (set)
                  FcFontSetDestroy(set);
            }
         };

         using font_set_ptr = std::unique_ptr<FcFontSet, font_set_deleter>;

         font_set_ptr font_list(FcConfig* c, pattern& p, object_set& os)
         {
            font_set_ptr ptr(FcFontList(c, p.handle(), os.handle()));

            if (!ptr)
               throw std::runtime_error("FcFontList returned null pointer!");

            return ptr;
         }
      } // namespace

      using cairo_font_map_type = std::map<std::string, cairo_font_face_t*>;

      std::pair<cairo_font_map_type&, std::mutex&> get_cairo_font_map()
      {
         static cairo_font_map_type cairo_font_map_;
         static std::mutex cairo_font_map_mutex_;

         struct cleanup
         {
            ~cleanup()
            {
               std::lock_guard<std::mutex> lock(cairo_font_map_mutex_);
               for (auto [key, scaled_font] : cairo_font_map_)
                  cairo_font_face_destroy(scaled_font);
               cairo_font_map_.clear();
            }
         };

         static cleanup cleanup_;
         return { cairo_font_map_, cairo_font_map_mutex_ };
      }

      int map_fc_weight(int w)
      {
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

      struct font_entry
      {
         font_entry(FcPattern* pat, FcChar8 const* full_name, FcChar8 const* file)
         : pattern(fc::pattern_shallow_copy_tag{}, *pat)
         , full_name(reinterpret_cast<char const*>(full_name))
         , file(reinterpret_cast<char const*>(file))
         {
            if (auto w = pattern.get_weight(); w)
               weight = map_fc_weight(*w); // map the weight (normalized 0 to 100)
            else
               weight = font_constants::weight_normal;

            if (auto s = pattern.get_slant(); s)
               slant = (*s * 100) / 110; // normalize 0 to 100
            else
               slant = font_constants::slant_normal;

            if (auto w = pattern.get_width(); w)
               stretch = (*w * 100) / 200; // normalize 0 to 100
            else
               stretch = font_constants::stretch_normal;
         }

         fc::pattern pattern;
         std::string full_name;
         std::string file;
         std::uint8_t weight;
         std::uint8_t slant;
         std::uint8_t stretch;
      };

      using font_map_type = std::map<std::string, std::vector<font_entry>>;
      font_map_type& font_map()
      {
         static font_map_type font_map_;
         return font_map_;
      }

      void init_font_map()
      {
         std::vector<fs::path> paths = font_paths();

#ifdef __APPLE__
         paths.push_back(get_user_fonts_directory());
#else
         if (paths.empty())
            paths.push_back(fs::current_path() / "resources");
#if defined(ELEMENTS_HOST_UI_LIBRARY_WIN32)
         TCHAR windir[MAX_PATH];
         GetWindowsDirectory(windir, MAX_PATH);
         paths.push_back(fs::path(windir) / "fonts");
#endif
#endif
         fc::config& conf = fc::instance();

         for (auto& path : paths)
            conf.app_font_add_dir(reinterpret_cast<FcChar8 const*>(path.generic_string().c_str()));

         fc::pattern pat(fc::pattern_empty_tag{});
         fc::object_set os(FC_FAMILY, FC_FULLNAME, FC_WIDTH, FC_WEIGHT, FC_SLANT, FC_FILE);
         fc::font_set_ptr fs = fc::font_list(conf.get(), pat, os);

         for (int i = 0; i < fs->nfont; ++i)
         {
            FcPattern* font = fs->fonts[i];
            FcChar8 *file, *family, *full_name;
            if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
               FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
               FcPatternGetString(font, FC_FULLNAME, 0, &full_name) == FcResultMatch
            )
            {
               std::string key = reinterpret_cast<char const*>(family);
               trim(key);

               font_map()[key].push_back(font_entry(font, full_name, file));
            }
         }
      }

      font_entry const* match(font_descr descr)
      {
         if (font_map().empty())
            init_font_map();

         std::istringstream str(std::string{ descr._families });
         std::string family;
         while (getline(str, family, ','))
         {
            trim(family);
            if (auto i = font_map().find(family); i != font_map().end())
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
      class free_type_face
      {
      public:
         free_type_face() = default;

         free_type_face(FT_Face face)
         : _face(face) {}

         ~free_type_face()
         {
            if (_face)
               FT_Done_Face(_face);
         }

         free_type_face(free_type_face const& other) = delete;
         free_type_face& operator=(free_type_face const& other) = delete;

         free_type_face(free_type_face&& other) noexcept
         : free_type_face()
         {
            *this = std::move(other);
         }

         free_type_face& operator=(free_type_face&& other) noexcept
         {
            std::swap(_face, other._face);
            return *this;
         }

         FT_Face handle()
         {
            return _face;
         }

         FT_Face release()
         {
            return std::exchange(_face, nullptr);
         }

         explicit operator bool() const
         {
            return _face != nullptr;
         }

      private:
         FT_Face _face = nullptr;
      };

      void destroy_free_type_face(void* face)
      {
         FT_Done_Face(reinterpret_cast<FT_Face>(face));
      }

      class free_type_library
      {
      public:
         free_type_library()
         {
            FT_Error status = FT_Init_FreeType(&_ft_lib);
            CYCFI_ASSERT(status == 0, "Error: failed to initialize free type library.");
         }

         ~free_type_library()
         {
            if (!_ft_lib)
               return;

            FT_Error status = FT_Done_FreeType(_ft_lib);
            CYCFI_ASSERT(status == 0, "Error: failed to destroy free type library.");
         }

         free_type_library(free_type_library const& other) = delete;
         free_type_library& operator=(free_type_library const& other) = delete;

         free_type_library(free_type_library&& other) noexcept
         {
            swap(*this, other);
         }

         free_type_library& operator=(free_type_library&& other) noexcept
         {
            swap(*this, other);
            return *this;
         }

         friend void swap(free_type_library& lhs, free_type_library& rhs) noexcept
         {
            std::swap(lhs._ft_lib, rhs._ft_lib);
         }

         free_type_face load_face(char const* font_path)
         {
            FT_Face ft_face;
            FT_Error ft_status = FT_New_Face(_ft_lib, font_path, 0, &ft_face);

            if (ft_status == 0)
               return free_type_face(ft_face);
            else
               return free_type_face(nullptr);
         }

         [[nodiscard]]
         cairo_font_face_t* load_font(char const* font_path)
         {
            free_type_face ft_face = load_face(font_path);
            if (!ft_face)
               return nullptr;

            cairo_font_face_t* cairo_face = cairo_ft_font_face_create_for_ft_face(ft_face.handle(), 0);
            if (cairo_face == nullptr)
               return nullptr;

            // extend the freetype font face lifetime to cairo's font face lifetime
            cairo_status_t cairo_status = CAIRO_STATUS_SUCCESS;
            cairo_user_data_key_t const& key = cairo_user_data_key();
            if (cairo_font_face_get_user_data(cairo_face, &key) != ft_face.handle())
            {
               cairo_status = cairo_font_face_set_user_data(
                  cairo_face, &key, ft_face.handle(), &destroy_free_type_face);
            }

            if (cairo_status == CAIRO_STATUS_SUCCESS)
            {
               ft_face.release();
            }
            else
            {
               cairo_font_face_destroy(cairo_face);
               return nullptr;
            }

            return cairo_face;
         }

      private:
         FT_Library _ft_lib = nullptr;
      };
#endif
   }

   std::vector<fs::path>& font_paths()
   {
      static std::vector<fs::path> _paths;
      return _paths;
   }

   font::font(font_descr descr)
   {
#ifndef __APPLE__
      static free_type_library ft_lib;
#endif

      auto match_ptr = match(descr);
      if (match_ptr)
      {
         auto [cairo_font_map, cairo_font_map_mutex] = get_cairo_font_map();
         std::lock_guard<std::mutex> lock(cairo_font_map_mutex);
         if (auto it = cairo_font_map.find(match_ptr->full_name); it != cairo_font_map.end())
         {
            _handle = cairo_font_face_reference(it->second);
         }
         else
         {
#ifdef __APPLE__

            auto cfstr = CFStringCreateWithCString(
               kCFAllocatorDefault
             , match_ptr->full_name.c_str()
             , kCFStringEncodingUTF8
            );
            auto cgfont = CGFontCreateWithFontName(cfstr);
            _handle = cairo_quartz_font_face_create_for_cgfont(cgfont);
            if (cgfont)
               CFRelease(cgfont);
            if (cfstr)
               CFRelease(cfstr);
#else
            _handle = ft_lib.load_font(match_ptr->file.c_str());
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

   font::font(font&& rhs) noexcept
   {
      std::swap(_handle, rhs._handle);
   }

   font& font::operator=(font&& rhs) noexcept
   {
      std::swap(_handle, rhs._handle);
      return *this;
   }

   font::~font()
   {
      if (_handle)
         cairo_font_face_destroy(_handle);
   }
}}


