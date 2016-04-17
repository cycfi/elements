/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SIZE_APRIL_14_2016)
#define PHOTON_GUI_LIB_WIDGET_FIX_APRIL_14_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Fixed sizing widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class size_widget : public proxy
   {
   public:

      size_widget(point size, std::shared_ptr<widget> subject)
       : proxy(subject)
       , _size(size)
      {}

      ~size_widget() {}

      virtual rect   limits(theme const& th) const;
      virtual void   subject_bounds(theme const& th, rect& b);

   private:

      point          _size;
   };

   inline widget_ptr hsize(point size, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new size_widget{ size, subject } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   class hsize_widget : public proxy
   {
   public:

      hsize_widget(double width, std::shared_ptr<widget> subject)
       : proxy(subject)
       , _width(width)
      {}

      ~hsize_widget() {}

      virtual rect   limits(theme const& th) const;
      virtual void   subject_bounds(theme const& th, rect& b);

   private:

      double         _width;
   };

   inline widget_ptr hsize(double width, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new hsize_widget{ width, subject } };
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   class vsize_widget : public proxy
   {
   public:

      vsize_widget(double height, std::shared_ptr<widget> subject)
       : proxy(subject)
       , _height(height)
      {}

      ~vsize_widget() {}

      virtual rect   limits(theme const& th) const;
      virtual void   subject_bounds(theme const& th, rect& b);

   private:

      double         _height;
   };

   inline widget_ptr vsize(double height, std::shared_ptr<widget> subject)
   {
      return widget_ptr{ new vsize_widget{ height, subject } };
   }
}

#endif