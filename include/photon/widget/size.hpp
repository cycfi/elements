/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_SIZE_APRIL_14_2016)
#define PHOTON_GUI_LIB_WIDGET_SIZE_APRIL_14_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Fixed sizing widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class size_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

      size_widget(point size, Subject&& subject)
       : base_type(std::forward<Subject>(subject))
       , _size(size)
      {}

      size_widget(point size, Subject const& subject)
       : base_type(subject)
       , _size(size)
      {}

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      point          _size;
   };

   template <typename Subject>
   inline size_widget<typename std::decay<Subject>::type>
   size(point size, Subject&& subject)
   {
      return { size, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect size_widget<Subject>::limits(basic_context const& ctx) const
   {
      return rect{ _size.x, _size.y, _size.x, _size.y };
   }

   template <typename Subject>
   inline void size_widget<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.right = ctx.bounds.left + _size.x;
      ctx.bounds.bottom = ctx.bounds.top + _size.y;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class hsize_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

      hsize_widget(double width, Subject&& subject)
       : base_type(std::forward<Subject>(subject))
       , _width(width)
      {}
      
      hsize_widget(double width, Subject const& subject)
       : base_type(subject)
       , _width(width)
      {}

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      double         _width;
   };

   template <typename Subject>
   inline hsize_widget<typename std::decay<Subject>::type>
   hsize(double width, Subject&& subject)
   {
      return { width, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect hsize_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect e_limits = this->subject().limits(ctx);
      return rect{ _width, e_limits.top, _width, e_limits.bottom };
   }

   template <typename Subject>
   inline void hsize_widget<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.right = ctx.bounds.left + _width;
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class vsize_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

      vsize_widget(double height, Subject&& subject)
       : base_type(std::forward<Subject>(subject))
       , _height(height)
      {}
      
      vsize_widget(double height, Subject const& subject)
       : base_type(subject)
       , _height(height)
      {}

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      double         _height;
   };

   template <typename Subject>
   inline vsize_widget<typename std::decay<Subject>::type>
   vsize(double height, Subject&& subject)
   {
      return { height, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect vsize_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect e_limits = this->subject().limits(ctx);
      return rect{ e_limits.left, _height, e_limits.right, _height };
   }

   template <typename Subject>
   inline void vsize_widget<Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.bottom = ctx.bounds.top + _height;
   }
}

#endif