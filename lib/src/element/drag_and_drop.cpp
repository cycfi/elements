/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

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
         ctx.view.refresh(ctx);
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
         auto lw = 3 / ctx.view.scale();
         cnv.stroke_style(get_theme().indicator_color);
         cnv.line_width(lw);
         cnv.add_rect(bounds.inset(lw/2));
         cnv.stroke();
      }
   }

   element* drop_box_base::hit_test(context const& ctx, point p, bool leaf, bool /*control*/)
   {
      return proxy_base::hit_test(ctx, p, leaf, false); // accept non-control subjects
   }

   bool drop_box_base::drop(context const& ctx, drop_info const& info)
   {
      base_type::drop(ctx, info);
      bool r = on_drop(info);
      ctx.view.refresh(ctx);
      return r;
   }

   drop_inserter_element::drop_inserter_element(std::initializer_list<std::string> mime_types_)
    : base_type{mime_types_}
   {}

   void drop_inserter_element::draw(context const& ctx)
   {
      proxy_base::draw(ctx);
      if (is_tracking())
      {
         if (auto c = find_subject<composite_base*>(this))
         {
            in_context_do(ctx, *c, [&](context const& cctx)
            {
               auto draw_insertion_line =
                  [&cctx](float left, float right, float pos)
                  {
                     auto &cnv = cctx.canvas;
                     cnv.stroke_style(get_theme().indicator_color);
                     cnv.line_width(2.0);
                     cnv.move_to({left, pos});
                     cnv.line_to({right, pos});
                     cnv.stroke();
                  };

               point cursor_pos = ctx.cursor_pos();
               if (c->size())
               {
                  auto hit_info = c->hit_element(cctx, cursor_pos, false);
                  rect bounds;
                  float pos;
                  if (hit_info.element_ptr)
                  {
                     bounds = hit_info.bounds;
                     bool before = cursor_pos.y < (bounds.top + (bounds.height()/2));
                     pos = before? bounds.top : bounds.bottom;
                     _insertion_pos = before? hit_info.index : hit_info.index+1;
                  }
                  else
                  {
                     auto top_bounds = c->bounds_of(cctx, 0);
                     auto bottom_bounds = c->bounds_of(cctx, c->size()-1);
                     bounds = cctx.bounds;
                     bool before = cursor_pos.y < top_bounds.top;
                     pos = before? top_bounds.top : bottom_bounds.bottom;
                     _insertion_pos = before? 0 : c->size();
                  }
                  draw_insertion_line(bounds.left, bounds.right, pos);
               }
               else
               {
                  _insertion_pos = 0;
                  draw_insertion_line(cctx.bounds.left, cctx.bounds.right, cctx.bounds.top);
               }
            });
         }
      }
   }

   void drop_inserter_element::track_drop(context const& ctx, drop_info const& info, cursor_tracking status)
   {
      base_type::track_drop(ctx, info, status);
      if (is_tracking())
      {
         static constexpr auto offset = 20;
         rect r = {info.where.x-offset, info.where.y-offset, info.where.x+offset, info.where.y+offset};
         scrollable::find(ctx).scroll_into_view(r);
         ctx.view.refresh(ctx);
      }
   }

   bool drop_inserter_element::drop(context const& ctx, drop_info const& info)
   {
      base_type::drop(ctx, info);
      if (_insertion_pos >= 0)
      {
         bool r = on_drop(info, _insertion_pos);
         ctx.view.refresh(ctx);
         _insertion_pos = -1;
         return r;
      }
      return false;
   }

   bool drop_inserter_element::click(context const& ctx, mouse_button btn)
   {
      bool r = base_type::click(ctx, btn);
      if (r && btn.down)
      {
         if (auto s = find_subject<selection_list_element*>(this))
         {
            std::vector<std::size_t> indices = s->get_selection();
            int latest = s->get_select_end();
            if (latest >= 0)
               on_select(indices, latest);
         }
      }
      return r;
   }

   bool drop_inserter_element::key(context const& ctx, key_info k)
   {
      bool r = base_type::key(ctx, k);
      if (!r)
         return r;

      if (auto s = find_subject<selection_list_element*>(this))
      {
         std::vector<std::size_t> indices = s->get_selection();
         int latest = s->get_select_end();
         if (latest >= 0)
            on_select(indices, latest);
      }
      return r;
   }

   void drop_inserter_element::move(indices_type const& indices)
   {
      if (_insertion_pos >= 0)
      {
         if (auto* c = find_subject<list*>(&subject()))
            c->move(_insertion_pos, indices);
         on_move(_insertion_pos, indices);
         if (auto s = find_subject<selection_list_element*>(this))
         {
            for (int i : indices)
            {
               if (i <= _insertion_pos)
                  --_insertion_pos;
            }
            s->update_selection(_insertion_pos, _insertion_pos+indices.size()-1);
         }
      }
   }

   void drop_inserter_element::erase(indices_type const& indices)
   {
      if (auto* c = find_subject<list*>(&subject()))
      {
         c->erase(indices);
         on_erase(indices);
         if (auto s = find_subject<selection_list_element*>(this))
            s->select_none();
      }
   }

   view_limits draggable_element::limits(basic_context const& ctx) const
   {
      auto e_limits = this->subject().limits(ctx);
      return {{e_limits.min.x, e_limits.min.y}, {full_extent, e_limits.max.y}};
   }

   void draggable_element::draw(context const& ctx)
   {
      if (ctx.enabled && is_selected() && is_enabled())
      {
         auto& canvas_ = ctx.canvas;

         canvas_.begin_path();
         canvas_.add_round_rect(ctx.bounds, 2);
         canvas_.fill_style(get_theme().indicator_color.opacity(0.6));
         canvas_.fill();
      }
      proxy_base::draw(ctx);
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
            r.min.x = 32;
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
                  auto* di = find_parent<drop_inserter_element *>(ctx);
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
                  if (auto* di = find_parent<drop_inserter_element*>(ctx))
                  {
                     if (auto* s = find_parent<selection_list_element*>(ctx))
                     {
                        auto indices = s->get_selection();
                        if (indices.size())
                        {
                           auto [p, cctx] = find_composite(ctx);
                           if (p)
                           {
                              for (int i : indices)
                              {
                                 if (i == p->focus_index())
                                 {
                                    // Lose the focus if the element to be deleted is the focus
                                    relinquish_focus(*p, *cctx);
                                    break;
                                 }
                              }
                           }
                           di->erase(std::move(indices));
                           return true;
                        }
                     }
                  }
               }
               break;

            default:
               break;
         }
      }
      return false;
   }

   element* draggable_element::hit_test(context const& ctx, point p, bool /*leaf*/, bool /*control*/)
   {
      if (ctx.enabled && is_enabled() && ctx.bounds.includes(p))
         return this;
      return nullptr;
   }

   void draggable_element::begin_tracking(context const& ctx, tracker_info& track_info)
   {
      using namespace std::chrono_literals;

      track_info.processed = false;
      if (track_info.modifiers & (mod_shift | mod_action))
         return;

      // Process drag
      if (auto *s = find_parent<selection_list_element*>(ctx))
      {
         auto bounds = ctx.bounds;
         if (is_selected())
         {
            std::size_t num_boxes = std::min<std::size_t>(s->get_selection().size(), max_boxes);
            bounds.right += item_offset * num_boxes;
            bounds.bottom += item_offset * num_boxes;
            auto* di = find_parent<drop_inserter_element *>(ctx);
            auto where = ctx.canvas.user_to_device(track_info.current);

            ctx.view.post(250ms,
               [bounds, num_boxes, &view = ctx.view, this, di, where]()
               {
                  if (is_tracking())
                  {
                     _drag_image = share(
                        floating(bounds,
                           drag_image(link(this->subject()), num_boxes)
                        )
                     );

                     view.add(_drag_image);
                     view.refresh();

                     if (di)
                     {
                        payload pl;
                        pl[address_to_string(di)] = {};
                        view.track_drop({pl, where}, cursor_tracking::entering);
                     }
                  }
               }
            );
            track_info.processed = true;
         }
      }
   }

   void draggable_element::keep_tracking(context const& ctx, tracker_info& track_info)
   {
      track_info.processed = false;
      if (track_info.modifiers & (mod_shift | mod_action))
         return;

      if (_drag_image)
      {
         _drag_image->bounds(
            _drag_image->bounds().move_to(track_info.current.x, track_info.current.y)
         );

         if (auto* di = find_parent<drop_inserter_element *>(ctx))
         {
            auto where = ctx.canvas.user_to_device(track_info.current);
            payload pl;
            pl[address_to_string(di)] = {};
            ctx.view.track_drop({pl, where}, cursor_tracking::hovering);
            track_info.processed = true;
         }
         ctx.view.refresh(); // $$$ Don't be lazy $$$
      }
   }

   void draggable_element::end_tracking(context const& ctx, tracker_info& track_info)
   {
      track_info.processed = false;
      if (track_info.modifiers & (mod_shift | mod_action))
         return;

      bool maybe_dragged =
         std::abs(track_info.distance().x) > 10 ||
         std::abs(track_info.distance().y) > 10
         ;

      if (_drag_image)
      {
         ctx.view.remove(_drag_image);
         _drag_image.reset();
         ctx.view.refresh();

         auto* di = find_parent<drop_inserter_element*>(ctx);
         if (di)
         {
            payload pl;
            pl[address_to_string(di)] = {};
            auto where = ctx.canvas.user_to_device(track_info.current);
            ctx.view.track_drop({pl, where}, cursor_tracking::leaving);
         }

         // Did we actually do a drag?
         if (maybe_dragged)
         {
            if (di)
            {
               if (auto *s = find_parent<selection_list_element*>(ctx))
               {
                  auto indices = s->get_selection();
                  if (indices.size())
                  {
                     di->move(std::move(indices));
                     track_info.processed = true;
                  }
               }
            }
            return;
         }
      }
   }
}}

