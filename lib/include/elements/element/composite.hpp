/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_COMPOSITE_APRIL_10_2016)
#define ELEMENTS_COMPOSITE_APRIL_10_2016

#include <elements/element/element.hpp>
#include <elements/element/proxy.hpp>
#include <elements/support/context.hpp>

#include <vector>
#include <array>
#include <set>

namespace cycfi::elements
{
   /**
    * \brief
    *    The `storage` class provides an abstraction for a storage for
    *    elements.
    *
    *    The `storage` class is an interface for generic storage of elements,
    *    with member functions for checking the size of the storage, if the
    *    storage is empty or not, and for accessing elements at a given
    *    index.
    */
   class storage
   {
   public:

      virtual                 ~storage() = default;

      virtual std::size_t     size() const = 0;
      bool                    empty() const { return size() == 0; }
      virtual element&        at(std::size_t ix) const = 0;
   };

   // Forward declaration
   class context;

   /**
    * \brief
    *    The `composite_base` class, derived from `element` and `storage`
    *    classes, serves as the base class for composites which are elements
    *    that contain other elements.
    */
   class composite_base : public element, public storage
   {
   public:

   // Display

      view_limits             limits(basic_context const& ctx) const override = 0;
      element*                hit_test(context const& ctx, point p, bool leaf, bool control) override;
      void                    draw(context const& ctx) override;
      void                    layout(context const& ctx) override = 0;
      void                    refresh(context const& ctx, element& element, int outward = 0) override;
      void                    in_context_do(context const& ctx, element& e, context_function f) override;

      using element::refresh;

   // Control

      bool                    wants_control() const override;
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;
      bool                    key(context const& ctx, key_info k) override;
      bool                    text(context const& ctx, text_info info) override;
      bool                    cursor(context const& ctx, point p, cursor_tracking status) override;
      bool                    scroll(context const& ctx, point dir, point p) override;
      void                    enable(bool state = true) override;
      bool                    is_enabled() const override;

      bool                    wants_focus() const override;
      void                    begin_focus(focus_request req) override;
      bool                    end_focus() override;
      element const*          focus() const override;
      element*                focus() override;
      int                     focus_index() const;

      void                    track_drop(context const& ctx, drop_info const& info, cursor_tracking status) override;
      bool                    drop(context const& ctx, drop_info const& info) override;

   // Composite

      using weak_element_ptr = std::weak_ptr<elements::element>;

      struct hit_info
      {
         element*             element_ptr = nullptr;
         element*             leaf_element_ptr = nullptr;
         rect                 bounds = rect{};
         int                  index = -1;
      };

      virtual hit_info        hit_element(context const& ctx, point p, bool control) const;
      virtual rect            bounds_of(context const& ctx, std::size_t index) const = 0;
      virtual bool            reverse_index() const { return false; }

      using for_each_callback =
         std::function<bool(element& e, std::size_t ix, rect const& r)>;

      virtual void            for_each_visible(
                                 context const& ctx
                               , for_each_callback f
                               , bool reverse = false
                              ) const;

      friend void             relinquish_focus(composite_base& c, context const& ctx);
      void                    focus(std::size_t index);

      virtual void            reset();

   private:

      bool                    new_focus(context const& ctx, int index, focus_request req);

      int                     _focus = -1;
      int                     _saved_focus = -1;
      int                     _click_tracking = -1;
      int                     _cursor_tracking = -1;
      std::set<int>           _cursor_hovering;
      bool                    _enabled = true;
   };

   void relinquish_focus(composite_base& c, context const& ctx);

   namespace concepts
   {
      template <typename T>
      concept Container = requires (T a, std::size_t ix)
      {
         { a.size() } -> std::same_as<std::size_t>;
         { a.empty() } -> std::convertible_to<bool>;
         { a[ix] } -> std::convertible_to<element_ptr>;
      };

      template <typename T>
      concept Composite = std::is_base_of_v<storage, std::decay_t<T>>;
   }

   /**
    * \brief
    *    The `composite` class template is a high-level composite object that
    *    inherits from a `Base` class and a `Container` class, conforming to
    *    the `Composite` and `Container` concepts respectively.
    *
    *    It offers unification of both `Base` (a composite type that is
    *    derived from the `storage` class) and `Container` (typically a std
    *    container of element pointers, e.g. std::vector<element_ptr>).
    */
   template <concepts::Container Container, concepts::Composite Base>
   class composite : public Base, public Container
   {
   public:

      using base_type = Base;
      using container_type = Container;
      using Base::Base;
      using Container::Container;
      using Container::operator=;

      std::size_t             size() const override;
      element&                at(std::size_t ix) const override;

      using Container::empty;
   };

   /**
    * \brief
    *    `array_composite` is a specific instantiation of the `composite`
    *    class template, designed to work with a fixed-size array of
    *    `element_ptr`.
    *
    *    This forms a composite object with a base type derived from a
    *    `composite_base` class (conforming to the `Composite` concept) and a
    *    storage of type `std::array<element_ptr, N>`. `N` denotes the size
    *    of the array.
    *
    *    Thus, `array_composite` represents a composite of elements that is
    *    fixed in size at compile time.
    */
   template <size_t N, concepts::Composite Base>
   using array_composite = composite<std::array<element_ptr, N>, Base>;

   /**
    * \brief
    *    `vector_composite` is another specific instantiation of the
    *    `composite` class template, designed to work with a
    *    dynamically-sized `std::vector` of `element_ptr`.
    *
    *    It forms a composite object with a base type derived from a
    *    `composite_base` class (conforming to the `Composite` concept) and a
    *    storage of type `std::vector<element_ptr>`.
    *
    *    Unlike `array_composite`, `vector_composite` allows for dynamic
    *    resizing, representing a composite of elements that can grow or
    *    shrink at runtime.
    */
   template <concepts::Composite Base>
   using vector_composite = composite<std::vector<element_ptr>, Base>;

   /**
    * \brief
    *    The `range_composite` class template is a specific kind of
    *    `Composite` that operates on a subset of a given `storage`.
    *
    *    This class is specialized for situations where only a specific range
    *    within an existing storage needs to constitute the composite
    *    instead of utilizing the entire storage.
    *
    *    The `range_composite` class is initialized with an existing
    *    `storage`, along with `first` and `last` indices that define the
    *    range of interest within the storage.
    *
    *    It overrides the `size()` and `at()` member functions to align with
    *    the specified subset of the storage rather than operating on the
    *    full storage. This is useful in scenarios where one needs to deal
    *    with composites that represent a portion of an existing storage.
    */
   template <concepts::Composite Base>
   class range_composite : public Base
   {
   public:
                              range_composite(
                                 storage&     container_
                               , std::size_t    first
                               , std::size_t    last
                              )
                               : _first(first)
                               , _last(last)
                               , _container(container_)
                              {}

      std::size_t             size() const override;
      element&                at(std::size_t ix) const override;

   private:

      std::size_t             _first;
      std::size_t             _last;
      storage&              _container;
   };

   //--------------------------------------------------------------------------
   // Inlines
   //--------------------------------------------------------------------------

   /**
    * \brief
    *    This `size()` function for the `composite` class template uses the
    *    `size()` member function from the `Container` class template.
    *
    *    The function will return the size of the storage being accessed.
    */
   template <concepts::Container Container, concepts::Composite Base>
   inline std::size_t composite<Container, Base>::size() const
   {
      return Container::size();
   }

   /**
    * \brief
    *    This `at()` function for the `composite` class template accesses the
    *    element at the specified index `ix` in the storage.
    *
    *    The function uses the `get()` method to retrieve the pointer from
    *    `shared_ptr` and dereferences it to obtain the element itself.
    */
   template <concepts::Container Container, concepts::Composite Base>
   inline element& composite<Container, Base>::at(std::size_t ix) const
   {
      return *(*this)[ix].get();
   }

   /**
    * \brief
    *    This `size()` function for the `range_composite` class template
    *    returns the size of the range from `_first` to `_last`.
    *
    *    The function calculates the size by subtracting `_first` from
    *    `_last`.
    */
   template <concepts::Composite Base>
   inline std::size_t range_composite<Base>::size() const
   {
      return _last - _first;
   }

   /**
    * \brief
    *    This `at()` function for the `range_composite` class template
    *    accesses the element at the specified index `ix` within the given
    *    range in the storage.
    *
    *    The function adjusts the index by taking into account the `_first`
    *    position in the range.
    */
   template <concepts::Composite Base>
   inline element& range_composite<Base>::at(std::size_t ix) const
   {
      return _container.at(_first + ix);
   }

   /**
    * \brief
    *    This `focus_index()` member function of `composite_base` class
    *    returns the focus index `_focus`.
    */
   inline int composite_base::focus_index() const
   {
      return _focus;
   }

   /**
    * \brief
    *    This `is_enabled()` member function of `composite_base` class
    *    returns the state of `_enabled` variable.
    */
   inline bool composite_base::is_enabled() const
   {
      return _enabled;
   }

   /**
    * \brief
    *    This `is_enabled()` function for `element` checks whether the
    *    element `e` is enabled. This function is declared in context.hpp to
    *    avoid cyclic dependencies.
    *
    *    The function calls `is_enabled()` member function of `element`
    *    class.
    */
   inline bool is_enabled(element const& e)
   {
      return e.is_enabled();
   }

   /**
    * \brief
    *    This `enable()` function for `composite_base` class sets the state
    *    of `_enabled` variable.
    *
    *    The function allows to enable or disable the `composite_base` object
    *    depending on the boolean value `state`.
    */
   inline void composite_base::enable(bool state)
   {
      _enabled = state;
   }
}

#endif
