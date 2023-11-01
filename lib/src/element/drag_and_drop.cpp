/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/drag_and_drop.hpp>
#include <elements/support/context.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   drop_box_base::drop_box_base(std::initializer_list<std::string> mime_types_)
    : _mime_types{mime_types_}
   {}

   void drop_box_base::draw(context const& ctx)
   {
      proxy_base::draw(ctx);
      if (_is_tracking)
      {
         auto& cnv = ctx.canvas;
         auto& bounds = ctx.bounds;
         cnv.stroke_style(get_theme().indicator_hilite_color.opacity(0.5));
         cnv.line_width(2.0);
         cnv.add_rect(bounds);
         cnv.stroke();
      }
   }

   bool drop_box_base::wants_control() const
   {
      return true;
   }

   void drop_box_base::track_drop(context const& ctx, drop_info const& info, cursor_tracking status)
   {
      // Return early if none of registered mime types is in the `drop_info`
      if (!std::any_of(_mime_types.begin(), _mime_types.end(),
         [&](auto const& item)
         {
            return info.data.contains(item);
         }))
         return;

      auto new_is_tracking = status != cursor_tracking::leaving;
      if (new_is_tracking != _is_tracking)
      {
         _is_tracking = new_is_tracking;
         ctx.view.refresh(ctx.bounds);
      }
   }

   bool drop_box_base::drop(context const& ctx, drop_info const& info)
   {
      bool r = on_drop(info);
      _is_tracking = false;
      ctx.view.refresh(ctx.bounds);
      return r;
   }
}}

