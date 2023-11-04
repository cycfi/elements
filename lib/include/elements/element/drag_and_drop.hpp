/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DRAG_AND_DROP_OCTOBER_11_2023)
#define ELEMENTS_DRAG_AND_DROP_OCTOBER_11_2023

#include <elements/element/proxy.hpp>
#include <functional>
#include <set>

namespace cycfi { namespace elements
{
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
   inline proxy<Subject, drop_box_base>
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
   inline proxy<Subject, drop_inserter_base>
   drop_inserter(Subject&& subject, std::initializer_list<std::string> mime_types)
   {
      return {std::forward<Subject>(subject), mime_types};
   }
}}

#endif
