/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_ALIGN_APRIL_14_2016)
#define PHOTON_GUI_LIB_WIDGET_ALIGN_APRIL_14_2016

#include <photon/widget/proxy.hpp>
#include <photon/support/context.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // Alignment widgets
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class align_widget_base : public proxy_base
   {
   public:
                     align_widget_base(float align)
                      : _align(align)
                     {}

      float          align() const        { return _align; }
      void           align(float align_)  { _align = align_; }

   private:

      float          _align;
   };

   template <typename Subject>
   class halign_widget : public proxy<Subject, align_widget_base>
   {
   public:

      using base_type = proxy<Subject, align_widget_base>;

                              halign_widget(float align, Subject&& subject);
                              halign_widget(float align, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);
   };

   template <typename Subject>
   inline halign_widget<typename std::decay<Subject>::type>
   halign(float align, Subject&& subject)
   {
      return { align, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline halign_widget<typename std::decay<Subject>::type>
   align_left(Subject&& subject)
   {
      return { 0.0, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline halign_widget<typename std::decay<Subject>::type>
   align_center(Subject&& subject)
   {
      return { 0.5, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline halign_widget<typename std::decay<Subject>::type>
   align_right(Subject&& subject)
   {
      return { 1.0, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline halign_widget<Subject>::halign_widget(float align, Subject&& subject)
    : base_type(std::forward<Subject>(subject), align)
   {}

   template <typename Subject>
   inline halign_widget<Subject>::halign_widget(float align, Subject const& subject)
    : base_type(subject, align)
   {}

   template <typename Subject>
   inline widget_limits halign_widget<Subject>::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      return { { e_limits.min.x, e_limits.min.y }, { full_extent, e_limits.max.y } };
   }

   template <typename Subject>
   inline void halign_widget<Subject>::prepare_subject(context& ctx)
   {
      widget_limits  e_limits          = this->subject().limits(ctx);
      float          elem_width        = e_limits.min.x;
      float          available_width   = ctx.bounds.width();

      if (available_width > elem_width)
         elem_width = std::min(available_width, e_limits.max.x);

      ctx.bounds.left += (available_width - elem_width) * this->align();
      ctx.bounds.width(elem_width);
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////
   template <typename Subject>
   class valign_widget : public proxy<Subject, align_widget_base>
   {
   public:

      using base_type = proxy<Subject, align_widget_base>;

                              valign_widget(float align, Subject&& subject);
                              valign_widget(float align, Subject const& subject);

      virtual widget_limits   limits(basic_context const& ctx) const;
      virtual void            prepare_subject(context& ctx);
   };

   template <typename Subject>
   inline valign_widget<typename std::decay<Subject>::type>
   valign(float align, Subject&& subject)
   {
      return { align, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline valign_widget<typename std::decay<Subject>::type>
   align_top(Subject&& subject)
   {
      return { 0.0, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline valign_widget<typename std::decay<Subject>::type>
   align_middle(Subject&& subject)
   {
      return { 0.5, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   inline valign_widget<typename std::decay<Subject>::type>
   align_bottom(Subject&& subject)
   {
      return { 1.0, std::forward<Subject>(subject) };
   }

   template <typename Subject>
   valign_widget<Subject>::valign_widget(float align, Subject&& subject)
    : base_type(std::move(subject), align)
   {}

   template <typename Subject>
   valign_widget<Subject>::valign_widget(float align, Subject const& subject)
    : base_type(subject, align)
   {}

   template <typename Subject>
   inline widget_limits valign_widget<Subject>::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      return { { e_limits.min.x, e_limits.min.y }, { e_limits.max.x, full_extent } };
   }

   template <typename Subject>
   inline void valign_widget<Subject>::prepare_subject(context& ctx)
   {
      auto  e_limits          = this->subject().limits(ctx);
      float elem_height       = e_limits.min.y;
      float available_height  = ctx.bounds.height();

      if (available_height > elem_height)
         elem_height = std::min(available_height, e_limits.max.y);

      ctx.bounds.top += (available_height - elem_height) * this->align();
      ctx.bounds.height(elem_height);
   }
}

#endif