/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_MARGIN_APRIL_11_2016)
#define ELEMENTS_MARGIN_APRIL_11_2016

#include <elements/element/proxy.hpp>
#include <elements/support/context.hpp>
#include <infra/support.hpp>
#include <memory>

namespace cycfi::elements
{
   ////////////////////////////////////////////////////////////////////////////
   // Margins
   ////////////////////////////////////////////////////////////////////////////
   namespace concepts
   {
      template <typename T>
      concept Rect = requires(T r)
      {
         { r.left } -> std::convertible_to<float>;
         { r.right } -> std::convertible_to<float>;
         { r.top } -> std::convertible_to<float>;
         { r.bottom } -> std::convertible_to<float>;
      };
   }
   /**
    * \brief
    *    A class that wraps a Subject with a rectangular margin.
    *
    *    The margin_element class extends the behaviour of the Subject by
    *    applying margins around it.
    *
    * \tparam Rect
    *    A type which fulfills the Rect concept, representing the margin to
    *    be applied.
    *
    * \tparam Subject
    *    The element that will be wrapped with a margin.
    */
   template <concepts::Rect Rect, concepts::Element Subject>
   class margin_element : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;

                              margin_element(Rect margin_, Subject subject);
      view_limits             limits(basic_context const& ctx) const override;
      void                    prepare_subject(context& ctx) override;

   private:

      Rect                    _margin;
   };

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    Constructs a margin_element object.
    *
    * \param margin_
    *    The margins to apply around the subject. This parameter is of type
    *    Rect and specifies the left, right, top, and bottom margins.
    *
    * \param subject
    *    The subject element that the margins will be applied to.
    *
    * \tparam Rect
    *    A type which fulfills the Rect concept, representing the margin to
    *    be applied. It must provide properties for left, right, top, and
    *    bottom margins.
    *
    * \tparam Subject
    *    The element that will be wrapped with a margin.
    */
   template <concepts::Rect Rect, concepts::Element Subject>
   inline margin_element<Rect, Subject>::margin_element(Rect margin_, Subject subject)
    : base_type(std::move(subject))
    , _margin(margin_)
   {}

   template <concepts::Rect Rect, concepts::Element Subject>
   inline view_limits margin_element<Rect, Subject>::limits(basic_context const& ctx) const
   {
      auto r = this->subject().limits(ctx);

      r.min.x += _margin.left + _margin.right;
      r.max.x += _margin.left + _margin.right;
      r.min.y += _margin.top + _margin.bottom;
      r.max.y += _margin.top + _margin.bottom;

      clamp_max(r.max.x, full_extent);
      clamp_max(r.max.y, full_extent);
      return r;
   }

   template <concepts::Rect Rect, concepts::Element Subject>
   inline void margin_element<Rect, Subject>::prepare_subject(context& ctx)
   {
      ctx.bounds.top += _margin.top;
      ctx.bounds.left += _margin.left;
      ctx.bounds.bottom -= _margin.bottom;
      ctx.bounds.right -= _margin.right;
   }

   /**
    * \struct static_empty_rect
    *
    * \brief
    *    A struct representing an empty rectangle with static dimensions.
    *
    *    This struct defines a rectangle with all sides (left, right, top,
    *    and bottom) set to 0. It is useful as a default or placeholder value
    *    where no actual margin is intended.
    */
   struct static_empty_rect
   {
      static constexpr int left = 0;
      static constexpr int right = 0;
      static constexpr int top = 0;
      static constexpr int bottom = 0;
   };

   /**
    * \brief
    *    Creates a full margin with specified margins and subject.
    *
    * \param margin_
    *    The margins to apply around the subject. This is a rect structure
    *    specifying the left, right, top, and bottom margins.
    *
    * \param subject
    *    The subject to be wrapped with the specified margins.
    *
    * \return
    *    A margin_element object that wraps the given subject with the
    *    specified margins.
    *
    * \tparam Subject
    *    The type of the subject being wrapped.
    */
   template <concepts::Element Subject>
   inline margin_element<rect, remove_cvref_t<Subject>>
   margin(rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   /**
    * \struct margin_left_rect
    *
    * \brief
    *    A struct derived from static_empty_rect, designed to represent a
    *    left margin.
    *
    *    This struct specializes static_empty_rect by providing a non-zero
    *    left margin. It is useful for cases where only a left margin is
    *    needed, leaving the other sides (right, top, and bottom) as zero,
    *    inheriting from static_empty_rect.
    *
    * \param left
    *    The left margin.
    */
   struct margin_left_rect : static_empty_rect
   {
      margin_left_rect(float left) : left(left) {}
      float left;
   };

   /**
    * \brief
    *    Creates a margin_element with a left margin and a subject.
    *
    * \param margin_
    *    The left margin to apply around the subject. This is a
    *    margin_left_rect structure specifying the width of the left margin.
    *
    * \param subject
    *    The subject to be wrapped with the specified left margin.
    *
    * \return
    *    A margin_element object that wraps the given subject with the
    *    specified left margin.
    *
    * \tparam Subject
    *    The type of the subject being wrapped.
    */
   template <concepts::Element Subject>
   inline margin_element<margin_left_rect, remove_cvref_t<Subject>>
   margin_left(margin_left_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_left(...) instead.")]]
   inline margin_element<margin_left_rect, remove_cvref_t<Subject>>
   left_margin(margin_left_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   /**
    * \struct margin_right_rect
    *
    * \brief
    *    A struct derived from static_empty_rect, designed to represent a
    *    right margin.
    *
    *    This struct specializes static_empty_rect by providing a non-zero
    *    right margin. It is useful for cases where only a right margin is
    *    needed, leaving the other sides (left, top, and bottom) as zero,
    *    inheriting from static_empty_rect.
    *
    * \param right
    *    The right margin.
    */
   struct margin_right_rect : static_empty_rect
   {
      margin_right_rect(float right) : right(right) {}
      float right;
   };

   /**
    * \brief
    *    Creates a margin_element with a right margin and a subject.
    *
    * \param margin_
    *    The right margin to apply around the subject. This is a
    *    margin_right_rect structure specifying the width of the right
    *    margin.
    *
    * \param subject
    *    The subject to be wrapped with the specified right margin.
    *
    * \return
    *    A margin_element object that wraps the given subject with the
    *    specified right margin.
    *
    * \tparam Subject
    *    The type of the subject being wrapped.
    */
   template <concepts::Element Subject>
   inline margin_element<margin_right_rect, remove_cvref_t<Subject>>
   margin_right(margin_right_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_right(...) instead.")]]
   inline margin_element<margin_right_rect, remove_cvref_t<Subject>>
   right_margin(margin_right_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   /**
    * \struct margin_top_rect
    *
    * \brief
    *    A struct derived from static_empty_rect, designed to represent a top
    *    margin.
    *
    *    This struct specializes static_empty_rect by providing a non-zero
    *    top margin. It is useful for cases where only a top margin is
    *    needed, leaving the other sides (left, right, and bottom) as zero,
    *    inheriting from static_empty_rect.
    *
    * \param top
    *    The top margin.
    */
   struct margin_top_rect : static_empty_rect
   {
      margin_top_rect(float top) : top(top) {}
      float top;
   };

   /**
    * \brief
    *    Creates a margin_element with a top margin and a subject.
    *
    * \param margin_
    *    The top margin to apply around the subject. This is a
    *    margin_top_rect structure specifying the height of the top margin.
    *
    * \param subject
    *    The subject to be wrapped with the specified top margin.
    *
    * \return
    *    A margin_element object that wraps the given subject with the specified
    *    top margin.
    *
    * \tparam Subject
    *    The type of the subject being wrapped.
    */
   template <concepts::Element Subject>
   inline margin_element<margin_top_rect, remove_cvref_t<Subject>>
   margin_top(margin_top_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_top(...) instead.")]]
   inline margin_element<margin_top_rect, remove_cvref_t<Subject>>
   top_margin(margin_top_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   /**
    * \struct margin_bottom_rect
    *
    * \brief
    *    A struct derived from static_empty_rect, designed to represent a
    *    bottom margin.
    *
    *    This struct specializes static_empty_rect by providing a non-zero
    *    bottom margin. It is useful for cases where only a bottom margin is
    *    needed, leaving the other sides (left, right, and top) as zero,
    *    inheriting from static_empty_rect.
    *
    * \param bottom
    *    The bottom margin.
    */
   struct margin_bottom_rect : static_empty_rect
   {
      margin_bottom_rect(float bottom) : bottom(bottom) {}
      float bottom;
   };

   /**
    * \brief
    *    Creates a margin_element with a bottom margin and a subject.
    *
    * \param margin_
    *    The bottom margin to apply around the subject. This is a
    *    margin_bottom_rect structure specifying the height of the bottom
    *    margin.
    *
    * \param subject
    *    The subject to be wrapped with the specified bottom margin.
    *
    * \return
    *    A margin_element object that wraps the given subject with the
    *    specified bottom margin.
    *
    * \tparam Subject
    *    The type of the subject being wrapped.
    */
   template <concepts::Element Subject>
   inline margin_element<margin_bottom_rect, remove_cvref_t<Subject>>
   margin_bottom(margin_bottom_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_bottom(...) instead.")]]
   inline margin_element<margin_bottom_rect, remove_cvref_t<Subject>>
   bottom_margin(margin_bottom_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   /**
    * \struct margin_left_top_rect
    *
    * \brief
    *    A struct derived from static_empty_rect, designed to represent left
    *    and top margins.
    *
    *    This struct specializes static_empty_rect by providing non-zero left
    *    and top margins. It is useful for cases where only left and top
    *    margins are needed, leaving the other sides (right and bottom) as
    *    zero, inheriting from static_empty_rect.
    *
    * \param left
    *    The left margin.
    *
    * \param top
    *    The top margin.
    */
   struct margin_left_top_rect : static_empty_rect
   {
      margin_left_top_rect(float left, float top) : left(left), top(top) {}
      float left;
      float top;
   };

   /**
    * \brief
    *    Creates a margin_element with left and top margins and a subject.
    *
    * \param margin_
    *    The left and top margins to apply around the subject. This is a
    *    margin_left_top_rect structure specifying the width of the left
    *    margin and the height of the top margin.
    *
    * \param subject
    *    The subject to be wrapped with the specified left and top margins.
    *    This can be any element that satisfies the Element concept.
    *
    * \return
    *    A margin_element object that wraps the given subject with the
    *    specified left and top margins.
    *
    * \tparam Subject
    *    The type of the subject being wrapped.
    */
   template <concepts::Element Subject>
   inline margin_element<margin_left_top_rect, remove_cvref_t<Subject>>
   margin_left_top(margin_left_top_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_left_top(...) instead.")]]
   inline margin_element<margin_left_top_rect, remove_cvref_t<Subject>>
   left_top_margin(margin_left_top_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   /**
    * \struct hmargin_rect
    *
    * \brief
    *    A struct derived from static_empty_rect, designed to represent
    *    horizontal margins.
    *
    *    This struct specializes static_empty_rect by providing non-zero left
    *    and right margins. It is useful for cases where only horizontal
    *    margins are needed, leaving the top and bottom sides as zero,
    *    inheriting from static_empty_rect.
    *
    * \param left
    *    The left margin.
    *
    * \param right
    *    The right margin.
    *
    * \note
    *    There are two constructors available:
    *    - One that takes separate values for left and right margins.
    *    - Another that takes a single value and applies it to both left and
    *      right margins.
    */
   struct hmargin_rect : static_empty_rect
   {
      hmargin_rect(float left, float right) : left(left), right(right) {}
      hmargin_rect(float left_right) : left(left_right), right(left_right) {}
      float left;
      float right;
   };

   /**
    * \brief
    *    Creates a margin_element with horizontal margins and a subject. Same
    *    as margin_left_right.
    *
    * \param margin_
    *    The horizontal margins to apply around the subject. This is an
    *    hmargin_rect structure specifying the width of the left and right
    *    margins.
    *
    * \param subject
    *    The subject to be wrapped with the specified horizontal margins.
    *
    * \return
    *    A margin_element object that wraps the given subject with the
    *    specified horizontal margins.
    *
    * \tparam Subject
    *    The type of the subject being wrapped.
    */
   template <concepts::Element Subject>
   inline margin_element<hmargin_rect, remove_cvref_t<Subject>>
   hmargin(hmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Creates a margin_element with horizontal margins and a subject. Same
    *    as hmargin.
    *
    * \param margin_
    *    The horizontal margins to apply around the subject. This is an
    *    hmargin_rect structure specifying the width of the left and right
    *    margins.
    *
    * \param subject
    *    The subject to be wrapped with the specified horizontal margins.
    *
    * \return
    *    A margin_element object that wraps the given subject with the
    *    specified horizontal margins.
    *
    * \tparam Subject
    *    The type of the subject being wrapped.
    */
   template <concepts::Element Subject>
   inline margin_element<hmargin_rect, remove_cvref_t<Subject>>
   margin_left_right(hmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_left_right(...) instead.")]]
   inline margin_element<hmargin_rect, remove_cvref_t<Subject>>
   left_right_margin(hmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use hmargin(...) instead.")]]
   inline margin_element<hmargin_rect, remove_cvref_t<Subject>>
   xside_margin(hmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   /**
    * \struct vmargin_rect
    *
    * \brief
    *    A struct derived from static_empty_rect, designed to represent
    *    vertical margins.
    *
    *    This struct specializes static_empty_rect by providing non-zero top
    *    and bottom margins. It is useful for cases where only vertical
    *    margins are needed, leaving the left and right sides as zero,
    *    inheriting from static_empty_rect.
    *
    * \param top
    *    The top margin.
    *
    * \param bottom
    *    The bottom margin.
    *
    * \note
    *    There are two constructors available:
    *    - One that takes separate values for top and bottom margins.
    *    - Another that takes a single value and applies it to both top and
    *      bottom margins.
    */
   struct vmargin_rect : static_empty_rect
   {
      vmargin_rect(float top, float bottom) : top(top), bottom(bottom) {}
      vmargin_rect(float top_bottom) : top(top_bottom), bottom(top_bottom) {}
      float top;
      float bottom;
   };

   /**
    * \brief
    *    Creates a margin_element with vertical margins and a subject.
    *
    * \param margin_
    *    The vertical margins to apply around the subject. This is a
    *    vmargin_rect structure specifying the height of the top and bottom
    *    margins.
    *
    * \param subject
    *    The subject to be wrapped with the specified vertical margins.
    *
    * \return
    *    A margin_element object that wraps the given subject with the specified
    *    vertical margins.
    *
    * \tparam Subject
    *    The type of the subject being wrapped.
    */
   template <concepts::Element Subject>
   inline margin_element<vmargin_rect, remove_cvref_t<Subject>>
   vmargin(vmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   /**
    * \brief
    *    Creates a margin_element with vertical margins and a subject.
    *
    * \param margin_
    *    The vertical margins to apply around the subject. This is a
    *    vmargin_rect structure specifying the height of the top and bottom
    *    margins.
    *
    * \param subject
    *    The subject to be wrapped with the specified vertical margins.
    *
    * \return
    *    A margin_element object that wraps the given subject with the specified
    *    vertical margins.
    *
    * \tparam Subject
    *    The type of the subject being wrapped.
    */
   template <concepts::Element Subject>
   inline margin_element<vmargin_rect, remove_cvref_t<Subject>>
   margin_top_bottom(vmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use margin_top_bottom(...) instead.")]]
   inline margin_element<vmargin_rect, remove_cvref_t<Subject>>
   top_bottom_margin(vmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }

   template <concepts::Element Subject>
   [[deprecated("Use vmargin(...) instead.")]]
   inline margin_element<vmargin_rect, remove_cvref_t<Subject>>
   yside_margin(vmargin_rect margin_, Subject&& subject)
   {
      return {margin_, std::forward<Subject>(subject)};
   }
}

#endif
