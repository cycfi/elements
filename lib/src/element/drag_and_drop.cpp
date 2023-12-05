/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/element/drag_and_drop.hpp>
#include <elements/element/traversal.hpp>
#include <elements/element/list.hpp>
#include <elements/element/port.hpp>
#include <elements/support/context.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>

namespace cycfi { namespace elements
{
   drop_base::drop_base(std::initializer_list<std::string> mime_types_)
    : _mime_types{mime_types_}
   {}

   bool drop_base::wants_control() const
   {
      return true;
   }

   namespace
   {
      std::string address_to_string(void* p)
      {
         return std::string(reinterpret_cast<char const*>(&p), sizeof(void*));
      }
   }

   void drop_base::prepare_subject(context& ctx)
   {
      proxy_base::prepare_subject(ctx);
      std::string id = address_to_string(this);
      if (_mime_types.find(id) == _mime_types.end())
         _mime_types.insert(id);
   }

   void drop_base::track_drop(context const& ctx, drop_info const& info, cursor_tracking status)
   {
      // Return early if none of registered mime types is in the `drop_info`
      if (!std::any_of(_mime_types.begin(), _mime_types.end(),
         [&](auto const& item)
         {
            return info.data.find(item) != info.data.end();
         }))
         return;

      auto new_is_tracking = status != cursor_tracking::leaving;
      if (new_is_tracking != _is_tracking)
      {
         _is_tracking = new_is_tracking;
         ctx.view.refresh(ctx.bounds);
      }
   }

   bool drop_base::drop(context const& /*ctx*/, drop_info const& /*info*/)
   {
      _is_tracking = false;
      return false;
   }

   drop_box_base::drop_box_base(std::initializer_list<std::string> mime_types_)
    : base_type{mime_types_}
   {}

   void drop_box_base::draw(context const& ctx)
   {
      proxy_base::draw(ctx);
      if (is_tracking())
      {
         auto& cnv = ctx.canvas;
         auto& bounds = ctx.bounds;
         cnv.stroke_style(get_theme().indicator_hilite_color.opacity(0.5));
         cnv.line_width(2.0);
         cnv.add_rect(bounds);
         cnv.stroke();
      }
   }

   bool drop_box_base::drop(context const& ctx, drop_info const& info)
   {
      base_type::drop(ctx, info);
      bool r = on_drop(info);
      ctx.view.refresh(ctx.bounds);
      return r;
   }

   drop_inserter_base::drop_inserter_base(std::initializer_list<std::string> mime_types_)
    : base_type{mime_types_}
   {}

   void drop_inserter_base::draw(context const& ctx)
   {
      proxy_base::draw(ctx);
      if (is_tracking())
      {
         if (auto c = find_subject<composite_base*>(this))
         {
            in_context_do(ctx, *c, [&](context const& cctx)
            {
               point cursor_pos = ctx.cursor_pos();
               if (c->size())
               {
                  auto hit_info = c->hit_element(cctx, cursor_pos, false);
                  if (hit_info.element_ptr)
                  {
                     rect const& bounds = hit_info.bounds;
                     bool before = cursor_pos.y < (bounds.top + (bounds.height()/2));
                     float pos = before? bounds.top : bounds.bottom;
                     _insertion_pos = before? hit_info.index : hit_info.index+1;

                     auto &cnv = cctx.canvas;
                     cnv.stroke_style(get_theme().indicator_hilite_color.opacity(0.5));
                     cnv.line_width(2.0);
                     cnv.move_to({bounds.left, pos});
                     cnv.line_to({bounds.right, pos});
                     cnv.stroke();
                  }
               }
               else
               {
                  _insertion_pos = 0;
                  auto &cnv = cctx.canvas;
                  cnv.stroke_style(get_theme().indicator_hilite_color.opacity(0.5));
                  cnv.line_width(2.0);
                  cnv.move_to({cctx.bounds.left, cctx.bounds.top});
                  cnv.line_to({cctx.bounds.right, cctx.bounds.top});
                  cnv.stroke();
               }
            });
         }
      }
   }

   void drop_inserter_base::track_drop(context const& ctx, drop_info const& info, cursor_tracking status)
   {
      base_type::track_drop(ctx, info, status);
      if (is_tracking())
      {
         static constexpr auto offset = 20;
         rect r = {info.where.x-offset, info.where.y-offset, info.where.x+offset, info.where.y+offset};
         scrollable::find(ctx).scroll_into_view(r);
         ctx.view.refresh(ctx.bounds);
      }
   }

   bool drop_inserter_base::drop(context const& ctx, drop_info const& info)
   {
      base_type::drop(ctx, info);
      if (_insertion_pos >= 0)
      {
         bool r = on_drop(info, _insertion_pos);
         ctx.view.refresh(ctx.bounds);
         _insertion_pos = -1;
         return r;
      }
      return false;
   }

   void drop_inserter_base::move(indices_type const& indices)
   {
      if (_insertion_pos >= 0)
      {
         if (auto* c = find_subject<list*>(&subject()))
            c->move(_insertion_pos, indices);
         on_move(_insertion_pos, indices);
      }
   }

   void drop_inserter_base::erase(indices_type const& indices)
   {
      if (auto* c = find_subject<list*>(&subject()))
         c->erase(indices);
      on_erase(indices);
   }

   view_limits draggable_element::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      return {{e_limits.min.x, e_limits.min.y}, {full_extent, e_limits.max.y}};
   }

   void draggable_element::draw(context const& ctx)
   {
      if (is_selected() && is_enabled())
      {
         auto& canvas_ = ctx.canvas;

         canvas_.begin_path();
         canvas_.add_round_rect(ctx.bounds, 2);
         canvas_.fill_style(get_theme().indicator_color.opacity(0.6));
         canvas_.fill();
      }
      if (is_enabled())
      {
         proxy_base::draw(ctx);
      }
      else
      {
         auto r = override_theme(
            &theme::label_font_color
          , get_theme().inactive_font_color
         );
         proxy_base::draw(ctx);
      }
   }

   namespace
   {
      constexpr auto item_offset = 10;
      constexpr auto max_boxes = 20;

      class drag_image_element : public proxy_base
      {
      public:

         drag_image_element(std::size_t num_boxes)
          : _num_boxes{num_boxes}
         {}

         view_limits limits(basic_context const& ctx) const override
         {
            auto r = this->subject().limits(ctx);
            r.min.x += item_offset * _num_boxes;
            r.max.x += item_offset * _num_boxes;
            r.min.y += item_offset * _num_boxes;
            r.max.y += item_offset * _num_boxes;
            clamp_max(r.max.x, full_extent);
            clamp_max(r.max.y, full_extent);
            return r;
         }

         void prepare_subject(context& ctx) override
         {
            ctx.bounds.bottom -= item_offset * _num_boxes;
            ctx.bounds.right -= item_offset * _num_boxes;
         }

         void draw(context const& ctx) override
         {
            auto& canvas_ = ctx.canvas;
            auto bounds = ctx.bounds.inset(-8, -2);
            bounds.right -= item_offset * _num_boxes;
            bounds.bottom -= item_offset * _num_boxes;
            float opacity = 0.6;
            for (std::size_t i = 0; i != _num_boxes; ++i)
            {
               canvas_.begin_path();
               canvas_.add_round_rect(bounds, 4);
               canvas_.fill_style(get_theme().indicator_color.opacity(opacity));
               canvas_.fill();
               opacity *= 0.6;
               bounds = bounds.move(+10, +10);
            }
            proxy_base::draw(ctx);
         }

      private:

         std::size_t _num_boxes = 0;
      };

      template <typename Subject>
      inline proxy<remove_cvref_t<Subject>, drag_image_element>
      drag_image(Subject&& subject, std::size_t num_boxes)
      {
         return {std::forward<Subject>(subject), num_boxes};
      }

      std::size_t count_selected(composite_base const& c)
      {
         std::size_t n = 0;
         for (std::size_t i = 0; i != c.size(); ++i)
         {
            if (auto e = find_element<draggable_element*>(&c.at(i)))
            {
               if (e->is_selected())
                  ++n;
            }
         }
         return n;
      }

      void unselect_all(composite_base const& c)
      {
         for (std::size_t i = 0; i != c.size(); ++i)
         {
            if (auto e = find_element<draggable_element*>(&c.at(i)))
               e->select(false);
         }
      }

      int find_first_selected(composite_base const& c)
      {
         for (std::size_t i = 0; i != c.size(); ++i)
         {
            if (auto e = find_element<draggable_element*>(&c.at(i)))
            {
               if (e->is_selected())
                  return i;
            }
         }
         return -1;
      }

      int find_last_selected(composite_base const& c)
      {
         for (int i = c.size()-1; i >= 0; --i)
         {
            if (auto e = find_element<draggable_element*>(&c.at(i)))
            {
               if (e->is_selected())
                  return i;
            }
         }
         return -1;
      }

      void shift_select(composite_base const& c)
      {
         if (int first = find_first_selected(c); first >= 0)
         {
            if (int last = find_last_selected(c); last >= 0)
            {
               // Select all from first to last
               for (int i = first; i != last+1; ++i)
               {
                  if (auto e = find_element<draggable_element*>(&c.at(i)))
                     e->select(true);
               }
            }
         }
      }

      std::vector<std::size_t> collect_selected(composite_base const& c)
      {
         std::vector<std::size_t> indices;
         for (std::size_t i = 0; i != c.size(); ++i)
         {
            if (auto e = find_element<draggable_element*>(&c.at(i)))
            {
               if (e->is_selected())
                  indices.push_back(i);
            }
         }
         return indices;
      }

      void select_all(composite_base const& c)
      {
         for (std::size_t i = 0; i != c.size(); ++i)
         {
            if (auto e = find_element<draggable_element*>(&c.at(i)))
               e->select(true);
         }
      }
   }

   bool draggable_element::key(context const& ctx, key_info k)
   {
      if (k.action == key_action::press || k.action == key_action::repeat)
      {
         switch (k.key)
         {
            case key_code::escape:
               {
                  ctx.view.remove(_drag_image);
                  _drag_image.reset();
                  escape_tracking(ctx);
                  auto* di = find_parent<drop_inserter_base*>(ctx);
                  if (di)
                  {
                     payload pl;
                     pl[address_to_string(di)] = {};
                     ctx.view.track_drop({pl, ctx.cursor_pos()}, cursor_tracking::leaving);
                  }
               }
               break;

            case key_code::backspace:
            case key_code::_delete:
               {
                  if (auto* di = find_parent<drop_inserter_base*>(ctx))
                  {
                     auto [c, cctx] = find_composite(ctx);
                     if (c)
                     {
                        auto indices = collect_selected(*c);
                        if (indices.size())
                        {
                           di->erase(std::move(indices));
                           return true;
                        }
                     }
                  }
               }
               break;

            case key_code::a:
               if (k.modifiers & mod_action)
               {
                  auto [c, cctx] = find_composite(ctx);
                  if (c)
                  {
                     select_all(*c);
                     ctx.view.refresh();
                     return true;
                  }
               }
               break;

            default:
               break;
         }
      }
      return false;
   }

   element* draggable_element::hit_test(context const& ctx, point p, bool /*leaf*/)
   {
      if (is_enabled() && ctx.bounds.includes(p))
         return this;
      return nullptr;
   }

   void draggable_element::begin_tracking(context const& ctx, tracker_info& track_info)
   {
      if (track_info.modifiers & mod_shift)
      {
         // Process shift-select
         select(true);
         auto [c, cctx] = find_composite(ctx);
         if (c)
         {
            shift_select(*c);
            cctx->view.refresh(*cctx);
         }
      }
      else if (track_info.modifiers & mod_action)
      {
         // Process action-select
         select(!is_selected());
         ctx.view.refresh(ctx);
      }
      else
      {
         // Process drag
         auto [c, cctx] = find_composite(ctx);
         if (c)
         {
            auto bounds = ctx.bounds;
            auto limits = this->subject().limits(ctx);
            bounds.width(limits.min.x);
            if (is_selected())
            {
               std::size_t num_boxes = std::min<std::size_t>(count_selected(*c), max_boxes);
               bounds.right += item_offset * num_boxes;
               bounds.bottom += item_offset * num_boxes;

               _drag_image = share(
                  floating(bounds,
                     drag_image(link(this->subject()), num_boxes)
                  )
               );
               ctx.view.add(_drag_image);
               ctx.view.refresh();

               if (auto* di = find_parent<drop_inserter_base*>(ctx))
               {
                  payload pl;
                  pl[address_to_string(di)] = {};
                  ctx.view.track_drop({pl, track_info.current}, cursor_tracking::entering);
               }
            }
         }
      }
   }

   void draggable_element::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      if (_drag_image)
      {
         _drag_image->bounds(
            _drag_image->bounds().move_to(track_info.current.x, track_info.current.y)
         );

         if (auto* di = find_parent<drop_inserter_base*>(ctx))
         {
            payload pl;
            pl[address_to_string(di)] = {};
            ctx.view.track_drop({pl, track_info.current}, cursor_tracking::hovering);
         }
      }
      ctx.view.refresh();
   }

   void draggable_element::end_tracking(context const& ctx, tracker_info& track_info)
   {
      bool maybe_dragged =
         std::abs(track_info.distance().x) > 10 ||
         std::abs(track_info.distance().y) > 10
         ;

      if (_drag_image)
      {
         ctx.view.remove(_drag_image);
         _drag_image.reset();
         ctx.view.refresh();

         auto* di = find_parent<drop_inserter_base*>(ctx);
         if (di)
         {
            payload pl;
            pl[address_to_string(di)] = {};
            ctx.view.track_drop({pl, track_info.current}, cursor_tracking::leaving);
         }

         // Did we actually do a drag?
         if (maybe_dragged)
         {
            if (di)
            {
               auto [c, cctx] = find_composite(ctx);
               if (c)
               {
                  auto indices = collect_selected(*c);
                  if (indices.size())
                     di->move(std::move(indices));
               }
            }
            return;
         }
      }

      // If this happens, the user hit escape while dragging
      if (is_selected() && maybe_dragged)
         return;

      if ((track_info.modifiers & (mod_shift | mod_action)) == 0)
      {
         auto [c, cctx] = find_composite(ctx);
         if (c)
         {
            unselect_all(*c);
            cctx->view.refresh(*cctx);
         }
         select(true);
         ctx.view.refresh(ctx);
      }
   }
}}

