/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_STYLE_GRID_LINES_APRIL_11_2016)
#define ELEMENTS_STYLE_GRID_LINES_APRIL_11_2016

#include <elements/element/element.hpp>

namespace cycfi::elements
{
   /** \struct db_scale
    *
    * \brief
    *    The decibel scale of a console fader, mapping decibels to a
    *    control's travel and back.
    *
    *    A fader is not linear in decibels. It gives the range around unity
    *    the room to be read and played, and compresses the quiet end, where
    *    a decibel matters less. The taper here is a console's: ten decibels
    *    take the same travel from +10 down to -10, half that from -10 to
    *    -40, and a quarter of it below, reaching silence at -70.\n\n
    *
    *    `position` takes decibels to travel, 0 at the bottom of the scale
    *    and 1 at the top; `value` takes travel back to decibels. Both are
    *    monotonic over the scale's range, so a control may be driven either
    *    way.\n\n
    *
    *    A scale whose minimum reaches to or below -70 dB has silence at the
    *    bottom, which a fader marks with the infinity sign rather than a
    *    number. See `db_scale_reaches_silence`. The default range runs from
    *    -144 dB, below the noise floor of 24 bit audio, up to +10 dB.
    *
    * @code
    *    db_scale scale{-144, 10};
    *    auto p = scale.position(0);   // where unity sits on the control
    * @endcode
    */
   struct db_scale
   {
      /**
       * \brief
       *    Construct a scale over a decibel range.
       *
       * \param min
       *    The decibels at the bottom of the control's travel.
       *
       * \param max
       *    The decibels at the top.
       */
                        db_scale(double min = -144, double max = 10)
                         : _min(min), _max(max) {}

      /**
       * \brief
       *    The travel, 0 to 1, at which a given number of decibels sits.
       */
      double            position(double db) const;

      /**
       * \brief
       *    The decibels at a given travel, 0 to 1. The inverse of
       *    `position`.
       */
      double            value(double position) const;

      /**
       * \brief
       *    The decibels at the bottom of the scale.
       */
      double            min() const { return _min; }

      /**
       * \brief
       *    The decibels at the top of the scale.
       */
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

   /** \class vgrid_lines_db
    *
    * \brief
    *    Vertical grid lines on a decibel scale, with the top of the element
    *    at the scale's maximum.
    *
    *    Major lines fall on the decibels a console fader is marked at, see
    *    `db_marks`, and a minor line sits halfway between each pair of
    *    them. All are placed through the `db_scale` taper, so they line up
    *    with a control using the same scale.
    */
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

   /** \class hgrid_lines_db
    *
    * \brief
    *    Horizontal grid lines on a decibel scale. As `vgrid_lines_db`, laid
    *    out along the other axis.
    */
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

   /**
    * \brief
    *    The decibels a fader is marked at, for grids, ticks and labels, as
    *    on a console: unity in the middle, wide steps near it, tens below.
    *    A minor mark sits halfway between neighbours. The array is
    *    terminated by a value below any scale.
    */
   constexpr double db_marks[] =
      {10, 5, 0, -5, -10, -20, -30, -40, -50, -60, -1000};

   /**
    * \brief
    *    Write the label for a mark: the magnitude, without a sign, and the
    *    infinity sign where the scale reaches silence.
    *
    * \returns
    *    The number of characters written, as `snprintf` reports it.
    */
   int db_mark_label(double db, char* text, std::size_t size);

   /**
    * \brief
    *    The lowest decibel mark that carries a label.
    */
   constexpr double db_lowest_mark = -60;

   /**
    * \brief
    *    Whether a scale reaches past the lowest labelled mark, down to what
    *    a fader marks as silence.
    */
   inline bool db_scale_reaches_silence(db_scale const& scale)
   {
      return scale.min() < db_lowest_mark;
   }
}

#endif
