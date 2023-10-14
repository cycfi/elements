/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_POINT_APRIL_11_2016)
#define ELEMENTS_POINT_APRIL_11_2016

#include <cstdint>

namespace cycfi { namespace elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Colors
   ////////////////////////////////////////////////////////////////////////////
   struct color
   {
      constexpr color() = default;

      constexpr color(float red, float green, float blue, float alpha = 1.0f)
       : red(red), green(green), blue(blue), alpha(alpha)
      {}

      constexpr color opacity(float alpha_) const;
      constexpr color level(float amount) const;

      float red   = 0.0f;
      float green = 0.0f;
      float blue  = 0.0f;
      float alpha = 0.0f;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   constexpr color rgb(std::uint32_t rgb)
   {
      return {
         ((rgb >> 16) & 0xff) / 255.0f
       , ((rgb >> 8) & 0xff) / 255.0f
       , (rgb & 0xff) / 255.0f
      };
   }

   constexpr color rgba(std::uint32_t rgba)
   {
      return {
         ((rgba >> 24) & 0xff) / 255.0f
       , ((rgba >> 16) & 0xff) / 255.0f
       , ((rgba >> 8) & 0xff) / 255.0f
       , (rgba & 0xff) / 255.0f
      };
   }

   constexpr color rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b)
   {
      return {
         r / 255.0f
       , g / 255.0f
       , b / 255.0f
      };
   }

   constexpr color rgba(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
   {
      return {
         r / 255.0f
       , g / 255.0f
       , b / 255.0f
       , a / 255.0f
      };
   }

   constexpr bool operator==(color a, color b)
   {
      return
         (a.alpha == b.alpha) &&
         (a.red == b.red) &&
         (a.green == b.green) &&
         (a.blue == b.blue)
         ;
   }

   constexpr bool operator!=(color a, color b)
   {
      return !(a == b);
   }

   constexpr color color::opacity(float alpha_) const
   {
      return { red, green, blue, alpha_ };
   }

   constexpr color color::level(float amount) const
   {
      color r = *this;
      r.red    *= amount;
      r.green  *= amount;
      r.blue   *= amount;
      return r;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Common colors
   ////////////////////////////////////////////////////////////////////////////
   namespace colors
   {
      constexpr color alice_blue             = rgb(240, 248, 255);
      constexpr color antique_white          = rgb(250, 235, 215);
      constexpr color aquamarine             = rgb(50, 191, 193);
      constexpr color azure                  = rgb(240, 255, 255);
      constexpr color beige                  = rgb(245, 245, 220);
      constexpr color bisque                 = rgb(255, 228, 196);
      constexpr color black                  = rgb(0, 0, 0);
      constexpr color blanched_almond        = rgb(255, 235, 205);
      constexpr color blue                   = rgb(0, 0, 255);
      constexpr color blue_violet            = rgb(138, 43, 226);
      constexpr color brown                  = rgb(165, 42, 42);
      constexpr color burly_wood             = rgb(222, 184, 135);
      constexpr color cadet_blue             = rgb(95, 146, 158);
      constexpr color kchartreuse            = rgb(127, 255, 0);
      constexpr color chocolate              = rgb(210, 105, 30);
      constexpr color coral                  = rgb(255, 114, 86);
      constexpr color cornflower_blue        = rgb(34, 34, 152);
      constexpr color corn_silk              = rgb(255, 248, 220);
      constexpr color cyan                   = rgb(0, 255, 255);
      constexpr color dark_goldenrod         = rgb(184, 134, 11);
      constexpr color dark_green             = rgb(0, 86, 45);
      constexpr color dark_khaki             = rgb(189, 183, 107);
      constexpr color dark_olive_green       = rgb(85, 86, 47);
      constexpr color dark_orange            = rgb(255, 140, 0);
      constexpr color dark_orchid            = rgb(139, 32, 139);
      constexpr color dark_salmon            = rgb(233, 150, 122);
      constexpr color dark_sea_green         = rgb(143, 188, 143);
      constexpr color dark_slate_blue        = rgb(56, 75, 102);
      constexpr color dark_slate_gray        = rgb(47, 79, 79);
      constexpr color dark_turquoise         = rgb(0, 166, 166);
      constexpr color dark_violet            = rgb(148, 0, 211);
      constexpr color deep_pink              = rgb(255, 20, 147);
      constexpr color deep_sky_blue          = rgb(0, 191, 255);
      constexpr color dim_gray               = rgb(84, 84, 84);
      constexpr color dodger_blue            = rgb(30, 144, 255);
      constexpr color firebrick              = rgb(142, 35, 35);
      constexpr color floral_white           = rgb(255, 250, 240);
      constexpr color forest_green           = rgb(80, 159, 105);
      constexpr color gains_boro             = rgb(220, 220, 220);
      constexpr color ghost_white            = rgb(248, 248, 255);
      constexpr color gold                   = rgb(218, 170, 0);
      constexpr color goldenrod              = rgb(239, 223, 132);
      constexpr color green                  = rgb(0, 255, 0);
      constexpr color green_yellow           = rgb(173, 255, 47);
      constexpr color honeydew               = rgb(240, 255, 240);
      constexpr color hot_pink               = rgb(255, 105, 180);
      constexpr color indian_red             = rgb(107, 57, 57);
      constexpr color ivory                  = rgb(255, 255, 240);
      constexpr color khaki                  = rgb(179, 179, 126);
      constexpr color lavender               = rgb(230, 230, 250);
      constexpr color lavender_blush         = rgb(255, 240, 245);
      constexpr color lawn_green             = rgb(124, 252, 0);
      constexpr color lemon_chiffon          = rgb(255, 250, 205);
      constexpr color light_blue             = rgb(176, 226, 255);
      constexpr color light_coral            = rgb(240, 128, 128);
      constexpr color light_cyan             = rgb(224, 255, 255);
      constexpr color light_goldenrod        = rgb(238, 221, 130);
      constexpr color light_goldenrod_yellow = rgb(250, 250, 210);
      constexpr color light_gray             = rgb(168, 168, 168);
      constexpr color light_pink             = rgb(255, 182, 193);
      constexpr color light_salmon           = rgb(255, 160, 122);
      constexpr color light_sea_green        = rgb(32, 178, 170);
      constexpr color light_sky_blue         = rgb(135, 206, 250);
      constexpr color light_slate_blue       = rgb(132, 112, 255);
      constexpr color light_slate_gray       = rgb(119, 136, 153);
      constexpr color light_steel_blue       = rgb(124, 152, 211);
      constexpr color light_yellow           = rgb(255, 255, 224);
      constexpr color lime_green             = rgb(0, 175, 20);
      constexpr color linen                  = rgb(250, 240, 230);
      constexpr color magenta                = rgb(255, 0, 255);
      constexpr color maroon                 = rgb(143, 0, 82);
      constexpr color medium_aquamarine      = rgb(0, 147, 143);
      constexpr color medium_blue            = rgb(50, 50, 204);
      constexpr color medium_forest_green    = rgb(50, 129, 75);
      constexpr color medium_goldenrod       = rgb(209, 193, 102);
      constexpr color medium_orchid          = rgb(189, 82, 189);
      constexpr color medium_purple          = rgb(147, 112, 219);
      constexpr color medium_sea_green       = rgb(52, 119, 102);
      constexpr color medium_slate_blue      = rgb(106, 106, 141);
      constexpr color medium_spring_green    = rgb(35, 142, 35);
      constexpr color medium_turquoise       = rgb(0, 210, 210);
      constexpr color medium_violet_red      = rgb(213, 32, 121);
      constexpr color midnight_blue          = rgb(47, 47, 100);
      constexpr color mint_cream             = rgb(245, 255, 250);
      constexpr color misty_rose             = rgb(255, 228, 225);
      constexpr color moccasin               = rgb(255, 228, 181);
      constexpr color navajo_white           = rgb(255, 222, 173);
      constexpr color navy                   = rgb(35, 35, 117);
      constexpr color navy_blue              = rgb(35, 35, 117);
      constexpr color old_lace               = rgb(253, 245, 230);
      constexpr color olive_drab             = rgb(107, 142, 35);
      constexpr color orange                 = rgb(255, 135, 0);
      constexpr color orange_red             = rgb(255, 69, 0);
      constexpr color orchid                 = rgb(239, 132, 239);
      constexpr color pale_goldenrod         = rgb(238, 232, 170);
      constexpr color pale_green             = rgb(115, 222, 120);
      constexpr color pale_turquoise         = rgb(175, 238, 238);
      constexpr color pale_violet_red        = rgb(219, 112, 147);
      constexpr color papaya_whip            = rgb(255, 239, 213);
      constexpr color peach_puff             = rgb(255, 218, 185);
      constexpr color peru                   = rgb(205, 133, 63);
      constexpr color pink                   = rgb(255, 181, 197);
      constexpr color plum                   = rgb(197, 72, 155);
      constexpr color powder_blue            = rgb(176, 224, 230);
      constexpr color purple                 = rgb(160, 32, 240);
      constexpr color red                    = rgb(255, 0, 0);
      constexpr color rosy_brown             = rgb(188, 143, 143);
      constexpr color royal_blue             = rgb(65, 105, 225);
      constexpr color saddle_brown           = rgb(139, 69, 19);
      constexpr color salmon                 = rgb(233, 150, 122);
      constexpr color sandy_brown            = rgb(244, 164, 96);
      constexpr color sea_green              = rgb(82, 149, 132);
      constexpr color sea_shell              = rgb(255, 245, 238);
      constexpr color sienna                 = rgb(150, 82, 45);
      constexpr color sky_blue               = rgb(114, 159, 255);
      constexpr color slate_blue             = rgb(126, 136, 171);
      constexpr color slate_gray             = rgb(112, 128, 144);
      constexpr color snow                   = rgb(255, 250, 250);
      constexpr color spring_green           = rgb(65, 172, 65);
      constexpr color steel_blue             = rgb(84, 112, 170);
      constexpr color tan                    = rgb(222, 184, 135);
      constexpr color thistle                = rgb(216, 191, 216);
      constexpr color tomato                 = rgb(255, 99, 71);
      constexpr color transparent            = rgb(0, 0, 1);
      constexpr color turquoise              = rgb(25, 204, 223);
      constexpr color violet                 = rgb(156, 62, 206);
      constexpr color violet_red             = rgb(243, 62, 150);
      constexpr color wheat                  = rgb(245, 222, 179);
      constexpr color white                  = rgb(255, 255, 255);
      constexpr color white_smoke            = rgb(245, 245, 245);
      constexpr color yellow                 = rgb(255, 255, 0);
      constexpr color yellow_green           = rgb(50, 216, 56);

      // greys
      constexpr color gray[] =
      {
         rgb(0, 0, 0),
         rgb(3, 3, 3),
         rgb(5, 5, 5),
         rgb(8, 8, 8),
         rgb(10, 10, 10),
         rgb(13, 13, 13),
         rgb(15, 15, 15),
         rgb(18, 18, 18),
         rgb(20, 20, 20),
         rgb(23, 23, 23),
         rgb(26, 26, 26),
         rgb(28, 28, 28),
         rgb(31, 31, 31),
         rgb(33, 33, 33),
         rgb(36, 36, 36),
         rgb(38, 38, 38),
         rgb(41, 41, 41),
         rgb(43, 43, 43),
         rgb(46, 46, 46),
         rgb(48, 48, 48),
         rgb(51, 51, 51),
         rgb(54, 54, 54),
         rgb(56, 56, 56),
         rgb(59, 59, 59),
         rgb(61, 61, 61),
         rgb(64, 64, 64),
         rgb(66, 66, 66),
         rgb(69, 69, 69),
         rgb(71, 71, 71),
         rgb(74, 74, 74),
         rgb(77, 77, 77),
         rgb(79, 79, 79),
         rgb(82, 82, 82),
         rgb(84, 84, 84),
         rgb(87, 87, 87),
         rgb(89, 89, 89),
         rgb(92, 92, 92),
         rgb(94, 94, 94),
         rgb(97, 97, 97),
         rgb(99, 99, 99),
         rgb(102, 102, 102),
         rgb(105, 105, 105),
         rgb(107, 107, 107),
         rgb(110, 110, 110),
         rgb(112, 112, 112),
         rgb(115, 115, 115),
         rgb(117, 117, 117),
         rgb(120, 120, 120),
         rgb(122, 122, 122),
         rgb(125, 125, 125),
         rgb(127, 127, 127),
         rgb(130, 130, 130),
         rgb(133, 133, 133),
         rgb(135, 135, 135),
         rgb(138, 138, 138),
         rgb(140, 140, 140),
         rgb(143, 143, 143),
         rgb(145, 145, 145),
         rgb(148, 148, 148),
         rgb(150, 150, 150),
         rgb(153, 153, 153),
         rgb(156, 156, 156),
         rgb(158, 158, 158),
         rgb(161, 161, 161),
         rgb(163, 163, 163),
         rgb(166, 166, 166),
         rgb(168, 168, 168),
         rgb(171, 171, 171),
         rgb(173, 173, 173),
         rgb(176, 176, 176),
         rgb(179, 179, 179),
         rgb(181, 181, 181),
         rgb(184, 184, 184),
         rgb(186, 186, 186),
         rgb(189, 189, 189),
         rgb(191, 191, 191),
         rgb(194, 194, 194),
         rgb(196, 196, 196),
         rgb(199, 199, 199),
         rgb(201, 201, 201),
         rgb(204, 204, 204),
         rgb(207, 207, 207),
         rgb(209, 209, 209),
         rgb(212, 212, 212),
         rgb(214, 214, 214),
         rgb(217, 217, 217),
         rgb(219, 219, 219),
         rgb(222, 222, 222),
         rgb(224, 224, 224),
         rgb(227, 227, 227),
         rgb(229, 229, 229),
         rgb(232, 232, 232),
         rgb(235, 235, 235),
         rgb(237, 237, 237),
         rgb(240, 240, 240),
         rgb(242, 242, 242),
         rgb(245, 245, 245),
         rgb(247, 247, 247),
         rgb(250, 250, 250),
         rgb(252, 252, 252),
         rgb(255, 255, 255),
      };
   }
 }}

 #endif
