/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_ALIGN_APRIL_14_2016)
#define ELEMENTS_ALIGN_APRIL_14_2016

#include <elements/element/proxy.hpp>
#include <elements/support/context.hpp>
#include <infra/support.hpp>
#include <memory>

namespace cycfi::elements
{
   /**
    *  \class align_element_base
    *
    *  \brief
    *    A base class for aligned elements.
    *
    *    `align_element_base` is a proxy class used as a base class for
    *    alignment elements.
    */
   class align_element_base : public proxy_base
   {
   public:
                     align_element_base(float align)
                      : _align(align)
                     {}

      float          align() const;
      void           align(float align_);

   private:

      float          _align;
   };

   /**
    *  \brief
    *    Get the current alignment value of the element.
    *
    * \return
    *    The current alignment value of the element.
    */
   inline float align_element_base::align() const
   {
      return _align;
   }

   /**
    *  \brief
    *    Set the alignment value of the element.
    *
    * \param
    *    align_ The new alignment value for the element.
    */
   inline void align_element_base::align(float align_)
   {
      _align = align_;
   }

   /**
    * \class halign_element
    *
    * \brief
    *    A template class to handle horizontal alignment for elements.
    *
    * \tparam
    *    Subject Base type, must meet the `concepts::Element` concept,.
    */
   template <concepts::Element Subject>
   class halign_element : public proxy<Subject, align_element_base>
   {
   public:

      using base_type = proxy<Subject, align_element_base>;

                              halign_element(float align, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;
   };

   /**
    * \brief
    *    Aligns the enclosed element (subject) in the x-axis.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param align
    *    The alignment value from 0.0 (left) to 1.0 (right).
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *    An `halign_element` subject with the specified alignment from 0.0 to 1.0.
    */
   template <concepts::Element Subject>
   inline halign_element<remove_cvref_t<Subject>>
   halign(float align, Subject&& subject)
   {
      return {align, std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Left-aligns the enclosed element (subject).
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *    An `halign_element` with a 0.0 alignment.
    */
   template <concepts::Element Subject>
   inline halign_element<remove_cvref_t<Subject>>
   align_left(Subject&& subject)
   {
      return {0.0, std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Center-aligns the enclosed element (subject).
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *    An `halign_element` with a 0.5 alignment.
    */
   template <concepts::Element Subject>
   inline halign_element<remove_cvref_t<Subject>>
   align_center(Subject&& subject)
   {
      return {0.5, std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Right-aligns the enclosed element (subject).
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *    An `halign_element` with a 1.0 alignment.
    */
   template <concepts::Element Subject>
   inline halign_element<remove_cvref_t<Subject>>
   align_right(Subject&& subject)
   {
      return {1.0, std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Constructs an `halign_element` with a specified alignment value and
    *    a subject.
    *
    * \tparam Subject
    *    Subject type, must meet the `concepts::Element` concept.
    *
    * \param align
    *    The initial alignment value.
    *
    * \param subject
    *    The subject.
    */
   template <concepts::Element Subject>
   inline halign_element<Subject>::halign_element(float align, Subject subject)
    : base_type(std::move(subject), align)
   {}

   template <concepts::Element Subject>
   inline view_limits halign_element<Subject>::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      return {{e_limits.min.x, e_limits.min.y}, {full_extent, e_limits.max.y}};
   }

   template <concepts::Element Subject>
   inline void halign_element<Subject>::prepare_subject(context& ctx)
   {
      view_limits    e_limits          = this->subject().limits(ctx);
      float          elem_width        = e_limits.min.x;
      float          available_width   = ctx.bounds.width();

      if (available_width > elem_width)
         elem_width = std::min(available_width, e_limits.max.x);

      ctx.bounds.left += (available_width - elem_width) * this->align();
      ctx.bounds.width(elem_width);
   }

   /**
    * \class valign_element
    *
    * \brief
    *    A template class to handle vertical alignment for elements.
    *
    * \tparam
    *    Subject Base type, must meet the `concepts::Element` concept,.
    */
   template <concepts::Element Subject>
   class valign_element : public proxy<Subject, align_element_base>
   {
   public:

      using base_type = proxy<Subject, align_element_base>;

                              valign_element(float align, Subject subject);

      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;
   };

   /**
    * \brief
    *    Aligns the enclosed element (subject) in the y-axis.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param align
    *    The alignment value from 0.0 (top) to 1.0 (bottom).
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *    A `valign_element` subject with the specified alignment from 0.0 to 1.0.
    */
   template <concepts::Element Subject>
   inline valign_element<remove_cvref_t<Subject>>
   valign(float align, Subject&& subject)
   {
      return {align, std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Top-aligns the enclosed element (subject).
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *    An `valign_element` with a 0.0 alignment.
    */
   template <concepts::Element Subject>
   inline valign_element<remove_cvref_t<Subject>>
   align_top(Subject&& subject)
   {
      return {0.0, std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Middle-aligns the enclosed element (subject).
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *    An `valign_element` with a 0.5 alignment.
    */
   template <concepts::Element Subject>
   inline valign_element<remove_cvref_t<Subject>>
   align_middle(Subject&& subject)
   {
      return {0.5, std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Bottom-aligns the enclosed element (subject).
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *    An `valign_element` with a 1.0 alignment.
    */
   template <concepts::Element Subject>
   inline valign_element<remove_cvref_t<Subject>>
   align_bottom(Subject&& subject)
   {
      return {1.0, std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Constructs a `valign_element` with a specified alignment value and
    *    a subject.
    *
    * \tparam Subject
    *    Subject type, must meet the `concepts::Element` concept.
    *
    * \param align
    *    The initial alignment value.
    *
    * \param subject
    *    The subject.
    */
   template <concepts::Element Subject>
   valign_element<Subject>::valign_element(float align, Subject subject)
    : base_type(std::move(subject), align)
   {}

   template <concepts::Element Subject>
   inline view_limits valign_element<Subject>::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      return {{e_limits.min.x, e_limits.min.y}, {e_limits.max.x, full_extent}};
   }

   template <concepts::Element Subject>
   inline void valign_element<Subject>::prepare_subject(context& ctx)
   {
      auto  e_limits          = this->subject().limits(ctx);
      float elem_height       = e_limits.min.y;
      float available_height  = ctx.bounds.height();

      if (available_height > elem_height)
         elem_height = std::min(available_height, e_limits.max.y);

      ctx.bounds.top += (available_height - elem_height) * this->align();
      ctx.bounds.height(elem_height);
   }

   /**
    * \brief Create an element that is aligned to left and top.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *   Equivalent to align_left(align_top(subject));
    */
   template <concepts::Element Subject>
   inline auto align_left_top(Subject&& subject)
   {
      return align_left(align_top(std::forward<Subject>(subject)));
   }

   /**
    * \brief Create an element that is aligned to center and top.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *   Equivalent to align_center(align_top(subject));
    */
   template <concepts::Element Subject>
   inline auto align_center_top(Subject&& subject)
   {
      return align_center(align_top(std::forward<Subject>(subject)));
   }

   /**
    * \brief Create an element that is aligned to right and top.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *   Equivalent to align_right(align_top(subject));
    */
   template <concepts::Element Subject>
   inline auto align_right_top(Subject&& subject)
   {
      return align_right(align_top(std::forward<Subject>(subject)));
   }

   /**
    * \brief Create an element that is aligned to left and middle.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *   Equivalent to align_left(align_middle(subject));
    */
   template <concepts::Element Subject>
   inline auto align_left_middle(Subject&& subject)
   {
      return align_left(align_middle(std::forward<Subject>(subject)));
   }

   /**
    * \brief Create an element that is aligned to center and middle.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *   Equivalent to align_center(align_middle(subject));
    */
   template <concepts::Element Subject>
   inline auto align_center_middle(Subject&& subject)
   {
      return align_center(align_middle(std::forward<Subject>(subject)));
   }

   /**
    * \brief Create an element that is aligned to right and middle.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *   Equivalent to align_right(align_middle(subject));
    */
   template <concepts::Element Subject>
   inline auto align_right_middle(Subject&& subject)
   {
      return align_right(align_middle(std::forward<Subject>(subject)));
   }

   /**
    * \brief Create an element that is aligned to left and bottom.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *   Equivalent to align_left(align_bottom(subject));
    */
   template <concepts::Element Subject>
   inline auto align_left_bottom(Subject&& subject)
   {
      return align_left(align_bottom(std::forward<Subject>(subject)));
   }

   /**
    * \brief Create an element that is aligned to center and bottom.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *   Equivalent to align_center(align_bottom(subject));
    */
   template <concepts::Element Subject>
   inline auto align_center_bottom(Subject&& subject)
   {
      return align_center(align_bottom(std::forward<Subject>(subject)));
   }

   /**
    * \brief Create an element that is aligned to right and bottom.
    *
    * \tparam Subject
    *    The subject type, must meet the `concepts::Element` concept.
    *
    * \param subject
    *    The subject to be aligned.
    *
    * \return
    *   Equivalent to align_right(align_bottom(subject));
    */
   template <concepts::Element Subject>
   inline auto align_right_bottom(Subject&& subject)
   {
      return align_right(align_bottom(std::forward<Subject>(subject)));
   }
}

#endif
