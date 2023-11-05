/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DRAG_AND_DROP_OCTOBER_11_2023)
#define ELEMENTS_DRAG_AND_DROP_OCTOBER_11_2023

#include <elements/element/proxy.hpp>
#include <elements/element/tracker.hpp>
#include <elements/element/floating.hpp>
#include <functional>
#include <set>

namespace cycfi { namespace elements
{
   class composite_base;   // forward declaration

   class drop_base : public proxy_base
   {
   public:

      using base_type = proxy_base;
      using mime_types = std::set<std::string>;

                              drop_base(std::initializer_list<std::string> mime_types_);
      bool                    wants_control() const override;
      void                    track_drop(context const& ctx, drop_info const& info, cursor_tracking status) override;
      bool                    drop(context const& ctx, drop_info const& info) override;

      bool                    is_tracking() const     { return _is_tracking; }
      mime_types const&       get_mime_types() const  { return _mime_types; }

   private:

      bool                    _is_tracking = false;
      mime_types              _mime_types;
   };

   class drop_box_base : public drop_base
   {
   public:

      using base_type = drop_base;
      using on_drop_function = std::function<bool(drop_info const& info)>;

                              drop_box_base(std::initializer_list<std::string> mime_types_);
      void                    draw(context const& ctx) override;
      bool                    drop(context const& ctx, drop_info const& info) override;

      on_drop_function        on_drop = [](drop_info const& /*info*/){ return false; };
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, drop_box_base>
   drop_box(Subject&& subject, std::initializer_list<std::string> mime_types)
   {
      return {std::forward<Subject>(subject), mime_types};
   }

   class drop_inserter_base : public drop_base
   {
   public:

      using base_type = drop_base;
      using on_drop_function = std::function<bool(drop_info const& info, std::size_t ix)>;

                              drop_inserter_base(std::initializer_list<std::string> mime_types_);
      void                    draw(context const& ctx) override;
      void                    track_drop(context const& ctx, drop_info const& info, cursor_tracking status) override;
      bool                    drop(context const& ctx, drop_info const& info) override;

      on_drop_function        on_drop = [](drop_info const& /*info*/, std::size_t /*ix*/){ return false; };

   public:

      int                     _insertion_pos = -1;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, drop_inserter_base>
   drop_inserter(Subject&& subject, std::initializer_list<std::string> mime_types)
   {
      return {std::forward<Subject>(subject), mime_types};
   }

   class draggable_element : public tracker<proxy_base>
   {
   public:

      view_limits             limits(basic_context const& ctx) const override;
      void                    draw(context const& ctx) override;
      element*                hit_test(context const& ctx, point p, bool leaf = false) override;

      bool                    is_selected() const;
      void                    select(bool state);

   private:

      void                    begin_tracking(context const& ctx, tracker_info& track_info) override;
      void                    keep_tracking(context const& ctx, tracker_info& track_info) override;
      void                    end_tracking(context const& ctx, tracker_info& track_info) override;

      using drag_image_ptr = std::shared_ptr<floating_element>;

      bool                    _selected = false;
      drag_image_ptr          _drag_image;
   };

   template <typename Subject>
   inline proxy<remove_cvref_t<Subject>, draggable_element>
   draggable(Subject&& subject)
   {
      return {std::forward<Subject>(subject)};
   }

   inline bool draggable_element::is_selected() const
   {
      return _selected;
   }

   inline void draggable_element::select(bool state)
   {
      _selected = state;
   }
}}

#endif
