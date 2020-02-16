#include <elements.hpp>
#include <fontconfig/fontconfig.h>
#include <iostream>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

auto samples = basic(
   [](context const& ctx)
   {
/*
      FcConfig* config = FcInitLoadConfigAndFonts();
      //make pattern from font name
      FcPattern* pat = FcNameParse((const FcChar8*)"Open Sans");
      FcConfigSubstitute(config, pat, FcMatchPattern);
      FcDefaultSubstitute(pat);
      char* fontFile; //this is what we'd return if this was a function
      // find the font
      FcResult result;
      FcPattern* font = FcFontMatch(config, pat, &result);
      if (font)
      {
         FcChar8* file = NULL;
         if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
         {
            //we found the font, now print it.
            //This might be a fallback font
            fontFile = (char*)file;
            printf("%s\n",fontFile);
         }
      }
      FcPatternDestroy(pat);
*/


      FcConfig* config = FcInitLoadConfigAndFonts();
      FcPattern* pat = FcPatternCreate();
      FcObjectSet* os = FcObjectSetBuild (FC_FAMILY, FC_FULLNAME, FC_WIDTH, FC_STYLE, FC_WEIGHT, FC_SLANT, FC_LANG, FC_FILE, (char *) 0);
      FcFontSet* fs = FcFontList(config, pat, os);

      printf("Total matching fonts: %d\n", fs->nfont);
      for (int i=0; fs && i < fs->nfont; ++i) {
         FcPattern* font = fs->fonts[i];
         FcChar8 *file, *style, *family, *fullname;
         int weight, slant, width;
         if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
            FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
            FcPatternGetString(font, FC_FULLNAME, 0, &fullname) == FcResultMatch &&
            FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch &&
            FcPatternGetInteger(font, FC_WEIGHT, 0, &weight) == FcResultMatch &&
            FcPatternGetInteger(font, FC_SLANT, 0, &slant) == FcResultMatch &&
            FcPatternGetInteger(font, FC_WIDTH, 0, &width) == FcResultMatch
         )
         {
            std::cout
               << "Filename: \"" << file << '"'
               << "(family: \"" << family << '"'
               << " fullname: \"" << fullname << '"'
               << " style: \"" << style << '"'
               << " weight:" << weight
               << " slant:" << slant
               << " width:" << width
               << std::endl
               ;

            // printf("Filename: %s (family \"%s\", style \"%s\")\n", file, family, style);
         }
      }
      if (fs) FcFontSetDestroy(fs);



/*
      FcConfig* config = FcInitLoadConfigAndFonts();

      // configure the search pattern,
      FcPattern* pat = FcNameParse((const FcChar8*)"Open Sans");
      FcConfigSubstitute(config, pat, FcMatchPattern);
      FcDefaultSubstitute(pat);

      // find the font
      FcResult res;
      FcPattern* font = FcFontMatch(config, pat, &res);
      if (font)
      {
         FcChar8 *file, *style, *family;
         if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
            FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
            FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch)
         {
            printf("Filename: %s (family \"%s\", style \"%s\")\n", file, family, style);
         }


         // FcChar8* file = NULL;
         // if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
         // {
         //    // save the file to another std::string
         //    fontFile = (char*)file;
         // }
         FcPatternDestroy(font);
      }

      FcPatternDestroy(pat);
*/



/*
      FcPattern *pat;
      FcFontSet *fs;
      FcObjectSet *os;
      FcChar8 *s, *file;
      FcConfig *config;
      FcBool result;
      int i;

      result = FcInit();
      config = FcConfigGetCurrent();
      FcConfigSetRescanInterval(config, 0);

      // show the fonts (debugging)
      pat = FcPatternCreate();
      os = FcObjectSetBuild (FC_FAMILY, FC_STYLE, FC_LANG, (char *) 0);
      fs = FcFontList(config, pat, os);
      printf("Total fonts: %d", fs->nfont);
      for (i=0; fs && i < fs->nfont; i++) {
         FcPattern *font = fs->fonts[i];//FcFontSetFont(fs, i);
         FcPatternPrint(font);
         s = FcNameUnparse(font);
         if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
            printf("Filename: %s", file);
         }
         printf("Font: %s", s);
         free(s);
      }
      if (fs) FcFontSetDestroy(fs);
*/

/*

      auto& cnv = ctx.canvas;
      auto descr = font_descr_x{"Open Sans" }.size(24);
      auto descr_c = font_descr_x{"Open Sans Condensed" }.size(24);

      text_layout_x layout_{cnv };
      layout_.font(descr);

      cnv.fill_style(get_theme().label_font_color);

      layout_.text("Regular");
      cnv.fill({ 20, 20 }, layout_);

      layout_.text("Bold");
      layout_.font(descr.weight(font_descr_x::bold));
      cnv.fill({ 150, 20 }, layout_);

      layout_.text("Light");
      layout_.font(descr.weight(font_descr_x::light));
      cnv.fill({ 240, 20 }, layout_);

      layout_.text("Italic");
      layout_.font(descr.style(font_descr_x::italic));
      cnv.fill({ 330, 20 }, layout_);

       layout_.text("Condensed");
       layout_.font(descr_c.stretch(font_descr_x::condensed));
       cnv.fill({ 420, 20 }, layout_);

      layout_.text("Outline");
      layout_.font(descr);
      cnv.stroke_style(get_theme().label_font_color);
      cnv.line_width(0.5);
      cnv.stroke({ 20, 60 }, layout_);

      layout_.text("AV Kerning");
      cnv.fill({ 20, 100 }, layout_);

      cnv.font("Open Sans", 32);
      cnv.fill_style(colors::blue);
      cnv.fill_text({ 20, 160 }, "AV Kerning");
*/
   }
);

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
         share(samples),
         share(background)    // Replace background with your main element,
                              // or keep it and add another layer on top of it.
      }
   );

   _app.run();
   return 0;
}
