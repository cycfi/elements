/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_LABEL_APRIL_17_2016)
#define PHOTON_GUI_LIB_WIDGET_LABEL_APRIL_17_2016

#include <photon/widget/widget.hpp>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Labels
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class label_widget : public widget
   {
   public:
                        label_widget(std::string const& text, double size, bool bold);

      virtual rect      limits() const;
      virtual void      draw(layout_info const& l);

   private:

      std::string       text;
      double            size;
      bool              bold;
   };

   inline widget_ptr label()
   {
      return widget_ptr{ new label_widget{} };
   }
}

#endif