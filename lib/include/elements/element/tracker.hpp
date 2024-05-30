/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_TRACKER_AUGUST_29_2016)
#define ELEMENTS_TRACKER_AUGUST_29_2016

#include <artist/point.hpp>
#include <elements/element/element.hpp>
#include <memory>
#include <type_traits>

namespace cycfi::elements
{
   struct tracker_info
   {
      explicit          tracker_info(point start_, int modifiers_ = 0)
                         : start(start_), modifiers(modifiers_)
                        {}

                        tracker_info(tracker_info const&) = default;
      virtual           ~tracker_info() = default;

      point             distance() const;    // Distance from start state
      point             movement() const;    // Movement from previous state

      point             start;
      point             current = start;
      point             previous = start;
      point             offset = point{0, 0};
      int               modifiers = 0;
      bool              processed = true;
   };

   ////////////////////////////////////////////////////////////////////////////
   // tracker tracks the mouse movement.
   ////////////////////////////////////////////////////////////////////////////
   template <typename Base = element, typename TrackerInfo = tracker_info>
   class tracker : public Base
   {
   public:

      using tracker_info = TrackerInfo;

                              template <typename... T>
                              tracker(T&&... args)
                               : Base(args...)
                              {}

                              tracker(tracker const& rhs);
                              tracker(tracker&& rhs) = default;
      tracker&                operator=(tracker const& rhs);
      tracker&                operator=(tracker&& rhs) = default;

      bool                    wants_control() const override { return true; }
      bool                    click(context const& ctx, mouse_button btn) override;
      void                    drag(context const& ctx, mouse_button btn) override;

      tracker_info*           get_state() { return state.get(); }
      tracker_info const*     get_state() const { return state.get(); }
      bool                    is_tracking() const { return state.get() != nullptr; }

      void                    escape_tracking(context const& ctx);

   protected:

      using tracker_info_ptr = std::unique_ptr<tracker_info>;

      virtual tracker_info_ptr new_state(context const& ctx, point start, int modifiers);
      virtual void             begin_tracking(context const& /*ctx*/, tracker_info& /*track_info*/) {}
      virtual void             keep_tracking(context const& /*ctx*/, tracker_info& /*track_info*/) {}
      virtual void             end_tracking(context const& /*ctx*/, tracker_info& /*track_info*/) {}
      void                     track_scroll(context const& ctx, point dir, point p);

   private:

      tracker_info_ptr         state;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Inlines
   ////////////////////////////////////////////////////////////////////////////
   inline point tracker_info::distance() const
   {
      return {current.x-start.x, current.y-start.y};
   }

   inline point tracker_info::movement() const
   {
      return {current.x-previous.x, current.y-previous.y};
   }

   template <typename Base, typename TrackerInfo>
   tracker<Base, TrackerInfo>::tracker(tracker const& rhs)
    : Base(rhs)
   {}

   template <typename Base, typename TrackerInfo>
   tracker<Base, TrackerInfo>&
   tracker<Base, TrackerInfo>::operator=(tracker const& rhs)
   {
      Base::operator=(rhs);
      state.reset();
      return *this;
   }

   template <typename Base, typename TrackerInfo>
   inline bool
   tracker<Base, TrackerInfo>::click(context const& ctx, mouse_button btn)
   {
      Base::click(ctx, btn);
      bool processed = true;
      if (btn.down)
      {
         state = new_state(ctx, btn.pos, btn.modifiers);
         this->on_tracking(ctx, element::begin_tracking);
         begin_tracking(ctx, *state);
         processed = state->processed;
      }
      else if (state)
      {
         this->on_tracking(ctx, element::end_tracking);
         end_tracking(ctx, *state);
         processed = state->processed;
         state.reset();
      }
      return processed;
   }

   template <typename Base, typename TrackerInfo>
   inline void
   tracker<Base, TrackerInfo>::drag(context const& ctx, mouse_button btn)
   {
      Base::drag(ctx, btn);
      if (state)
      {
         state->previous = state->current;
         state->current = btn.pos;
         state->current = state->current.move(-state->offset.x, -state->offset.y);
         state->modifiers = btn.modifiers;
         keep_tracking(ctx, *state);
         this->on_tracking(ctx, element::while_tracking);
      }
   }

   template <typename Base, typename TrackerInfo>
   inline typename tracker<Base, TrackerInfo>::tracker_info_ptr
   tracker<Base, TrackerInfo>::new_state(context const& /* ctx */, point start, int modifiers)
   {
      return std::make_unique<tracker_info>(start, modifiers);
   }

   template <typename Base, typename TrackerInfo>
   inline void
   tracker<Base, TrackerInfo>::track_scroll(context const& ctx, point /* dir */, point /* p */)
   {
      this->on_tracking(ctx, element::while_tracking);
   }

   template <typename Base, typename TrackerInfo>
   inline void
   tracker<Base, TrackerInfo>::escape_tracking(context const& ctx)
   {
      if (state)
      {
         this->on_tracking(ctx, element::end_tracking);
         end_tracking(ctx, *state);
         state.reset();
      }
   }
}

#endif
