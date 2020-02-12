/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if defined(__linux__) || defined(_WIN32)

#include <elements/support/canvas.hpp>
#include <cairo-ft.h>
#include <ft2build.h>
#include <freetype/ttnameid.h>
#include FT_SFNT_NAMES_H
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BBOX_H
#include FT_TYPE1_TABLES_H

namespace cycfi { namespace elements
{
   std::map<std::string, cairo_font_face_t*> canvas::_fonts;
   static std::map<FT_Face, cairo_font_face_t*> font_faces;

   struct font_deleter
   {
      ~font_deleter()
      {
         for (auto& p : font_faces)
         {
            FT_Done_Face(p.first);
            cairo_font_face_destroy(p.second);
         }
      }
   };

   static font_deleter font_deleter_;

   void activate_font(
      std::map<std::string, cairo_font_face_t*>& fonts
    , FT_Library& value
    , fs::path font_path)
   {
      FT_Face face;
      FT_Error status = FT_New_Face(value, font_path.string().c_str(), 0, &face);
      if (status != 0)
         return;
      cairo_font_face_t* ct = cairo_ft_font_face_create_for_ft_face(face, 0);

      auto const count = FT_Get_Sfnt_Name_Count(face);
      for (FT_UInt i = 0; i < count; ++i)
      {
         FT_SfntName name;
         FT_Get_Sfnt_Name(face, i, &name);
         if (name.name_id == TT_NAME_ID_FULL_NAME)
         {
            font_faces[face] = ct;
            fonts[std::string((char const*)name.string, name.string_len)] = ct;
            break;
         }
      }
   }

   void canvas::load_fonts(fs::path resource_path)
   {
      FT_Library value;
      FT_Error status = FT_Init_FreeType(&value);
      if (status != 0)
         return;

      // Load the user fonts from the Resource folder. Normally this is automatically
      // done on application startup, but for plugins, we need to explicitly load
      // the user fonts ourself.
      for (fs::directory_iterator it{ resource_path }; it != fs::directory_iterator{}; ++it)
         if (it->path().extension() == ".ttf")
            activate_font(_fonts, value, it->path());
   }
}}

#endif
