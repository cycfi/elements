/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_UNDO_ANCHOR_OCTOBER_14_2026)
#define ELEMENTS_UNDO_ANCHOR_OCTOBER_14_2026

#include <cstddef>
#include <memory>

namespace cycfi::elements
{
   /**
    * \class undo_anchor
    *
    * \brief
    *    What an element's undo tasks hold to reach the element, as a member
    *    of the element: a shared reference that gives the element, or null
    *    once the tasks no longer apply to it. A task finding null returns
    *    false, and the view drops it.
    *
    *    A copy gets a reference of its own: no task recorded before applies
    *    to it. A move takes the reference along, to the element's new
    *    place. An assignment ends the tasks of the content it replaces, and
    *    destruction ends them all. The anchor finds its element from its own
    *    place in it, so an element holding one can default its copy, move
    *    and assignment.
    */
   template <typename T>
   class undo_anchor
   {
   public:

      using ref_type = std::shared_ptr<T*>;

                              explicit undo_anchor(T* owner);
                              undo_anchor(undo_anchor const& rhs);
                              undo_anchor(undo_anchor&& rhs) noexcept;
                              ~undo_anchor();

      undo_anchor&            operator=(undo_anchor const& rhs);
      undo_anchor&            operator=(undo_anchor&& rhs);

      ref_type                ref() const { return _ref; }

   private:

      T*                      owner() const;
      void                    renew();

      std::ptrdiff_t          _offset;
      ref_type                _ref;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inline implementation
   ////////////////////////////////////////////////////////////////////////////
   template <typename T>
   inline undo_anchor<T>::undo_anchor(T* owner)
    : _offset(reinterpret_cast<char const*>(this)
         - reinterpret_cast<char const*>(owner))
    , _ref(std::make_shared<T*>(owner))
   {}

   template <typename T>
   inline undo_anchor<T>::undo_anchor(undo_anchor const& rhs)
    : _offset(rhs._offset)
   {
      renew();
   }

   template <typename T>
   inline undo_anchor<T>::undo_anchor(undo_anchor&& rhs) noexcept
    : _offset(rhs._offset)
    , _ref(std::move(rhs._ref))
   {
      *_ref = owner();
      rhs.renew();
   }

   template <typename T>
   inline undo_anchor<T>::~undo_anchor()
   {
      *_ref = nullptr;
   }

   template <typename T>
   // cppcheck-suppress operatorEqVarError ; _offset is the same in every T
   inline undo_anchor<T>& undo_anchor<T>::operator=(undo_anchor const& rhs)
   {
      if (this != &rhs)
      {
         *_ref = nullptr;
         renew();
      }
      return *this;
   }

   template <typename T>
   // cppcheck-suppress operatorEqVarError ; _offset is the same in every T
   inline undo_anchor<T>& undo_anchor<T>::operator=(undo_anchor&& rhs)
   {
      if (this != &rhs)
      {
         *_ref = nullptr;
         _ref = std::move(rhs._ref);
         *_ref = owner();
         rhs.renew();
      }
      return *this;
   }

   template <typename T>
   inline T* undo_anchor<T>::owner() const
   {
      return reinterpret_cast<T*>(
         const_cast<char*>(reinterpret_cast<char const*>(this)) - _offset);
   }

   template <typename T>
   inline void undo_anchor<T>::renew()
   {
      _ref = std::make_shared<T*>(owner());
   }
}

#endif
