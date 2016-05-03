/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_ALIGN_APRIL_14_2016)
#define PHOTON_GUI_LIB_WIDGET_ALIGN_APRIL_14_2016

#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Alignment widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class halign_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

      halign_widget(double align, Subject&& subject)
       : base_type(std::forward<Subject>(subject))
       , _align(align)
      {}

      halign_widget(double align, Subject const& subject)
       : base_type(subject)
       , _align(align)
      {}

      ~halign_widget() {}

      halign_widget(halign_widget&&) = default;
      halign_widget(halign_widget const&) = default;
      halign_widget& operator=(halign_widget&&) = default;
      halign_widget& operator=(halign_widget const&) = default;

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      double         _align;
   };

   template <typename Subject>
   inline halign_widget<typename std::decay<Subject>::type>
   halign(double align, Subject&& subject)
   {
      return { align, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline rect halign_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect e_limits = this->subject().limits(ctx);
      return rect{ e_limits.left, e_limits.top, full_extent, e_limits.bottom };
   }

   template <typename Subject>
   inline void halign_widget<Subject>::prepare_subject(context& ctx)
   {
      rect     e_limits          = this->subject().limits(ctx);
      double   elem_width        = e_limits.left;
      double   available_width   = ctx.bounds.width();

      if (available_width > elem_width)
         elem_width = std::min(available_width, e_limits.right);

      ctx.bounds.left += (available_width - elem_width) * _align;
      ctx.bounds.width(elem_width);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class valign_widget : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

      valign_widget(double align, Subject&& subject)
       : base_type(std::move(subject))
       , _align(align)
      {}

      valign_widget(double align, Subject const& subject)
       : base_type(subject)
       , _align(align)
      {}

      ~valign_widget() {}

      valign_widget(valign_widget&&) = default;
      valign_widget& operator=(valign_widget&&) = default;

      virtual rect   limits(basic_context const& ctx) const;
      virtual void   prepare_subject(context& ctx);

   private:

      double         _align;
   };

   template <typename Subject>
   inline valign_widget<Subject> valign(double align, Subject&& subject)
   {
      return valign_widget<Subject>{ align, std::move(subject) };
   }

   template <typename Subject>
   inline valign_widget<Subject> valign(double align, Subject const& subject)
   {
      return valign_widget<Subject>{ align, subject };
   }

   template <typename Subject>
   inline rect valign_widget<Subject>::limits(basic_context const& ctx) const
   {
      rect e_limits = this->subject().limits(ctx);
      return rect{ e_limits.left, e_limits.top, full_extent, e_limits.bottom };
   }

   template <typename Subject>
   inline void valign_widget<Subject>::prepare_subject(context& ctx)
   {
      rect     e_limits          = this->subject().limits(ctx);
      double   elem_height       = e_limits.top;
      double   available_height  = ctx.bounds.height();

      if (available_height > elem_height)
         elem_height = std::min(available_height, e_limits.right);

      ctx.bounds.top += (available_height - elem_height) * _align;
      ctx.bounds.height(elem_height);
   }
}

#endif