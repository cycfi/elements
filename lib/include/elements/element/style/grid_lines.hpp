/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_GRID_LINES_APRIL_11_2016)
#define ELEMENTS_STYLE_GRID_LINES_APRIL_11_2016

#include <elements/element/element.hpp>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // The decibel scale of a console fader: 10 dB takes the same travel
   // from +10 down to -10, half that from -10 to -40, a quarter below, to
   // silence at -70. Maps decibels to a control's travel, 0 to 1, and
   // back. The range is in decibels; a minimum at or below -70 dB is
   // silence, the infinity mark of a fader.
   ////////////////////////////////////////////////////////////////////////////
   struct db_scale
   {
                        db_scale(double min = -144, double max = 10)
                         : _min(min), _max(max) {}

      double            position(double db) const;
      double            value(double position) const;
      double            min() const { return _min; }
      double            max() const { return _max; }

   private:

      double            _min, _max;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines - Vertical
   ////////////////////////////////////////////////////////////////////////////
   class vgrid_lines : public element
   {
   public:

                     vgrid_lines(float major_divisions, float minor_divisions)
                      : _major_divisions(major_divisions)
                      , _minor_divisions(minor_divisions)
                     {}

      void           draw(context const& ctx) override;
      std::string    class_name() const override { return "vgrid_lines"; }

   private:

      float          _major_divisions;
      float          _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines - Horizontal
   ////////////////////////////////////////////////////////////////////////////
   class hgrid_lines : public element
   {
   public:

                     hgrid_lines(float major_divisions, float minor_divisions)
                      : _major_divisions(major_divisions)
                      , _minor_divisions(minor_divisions)
                     {}

      void           draw(context const& ctx) override;
      std::string    class_name() const override { return "hgrid_lines"; }

   private:

      float          _major_divisions;
      float          _minor_divisions;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Grid Lines on a decibel scale. Major lines at the fader's marks,
   // minor lines halfway between them, all placed through the db_scale
   // taper. Vertical has the top at the scale's maximum.
   ////////////////////////////////////////////////////////////////////////////
   class vgrid_lines_db : public element
   {
   public:

                     vgrid_lines_db(db_scale scale = {})
                      : _scale(scale)
                     {}

      void           draw(context const& ctx) override;
      std::string    class_name() const override { return "vgrid_lines_db"; }

   private:

      db_scale       _scale;
   };

   class hgrid_lines_db : public element
   {
   public:

                     hgrid_lines_db(db_scale scale = {})
                      : _scale(scale)
                     {}

      void           draw(context const& ctx) override;
      std::string    class_name() const override { return "hgrid_lines_db"; }

   private:

      db_scale       _scale;
   };

   // The decibels a fader is marked at, for grids, ticks and labels, as on
   // a console: unity in the middle, wide steps near it, tens below. A
   // minor mark sits halfway between neighbours. Terminated by a value
   // below any scale.
   constexpr double db_marks[] =
      {10, 5, 0, -5, -10, -20, -30, -40, -50, -60, -1000};

   // Labels are magnitudes, the infinity sign for silence.
   int db_mark_label(double db, char* text, std::size_t size);

   // The lowest labelled mark, and whether a scale reaches past it, down
   // to what a fader marks as silence.
   constexpr double db_lowest_mark = -60;

   inline bool db_scale_reaches_silence(db_scale const& scale)
   {
      return scale.min() < db_lowest_mark;
   }
}

#endif
