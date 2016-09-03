/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#if !defined(PHOTON_GUI_LIB_WIDGET_TRACKER_AUGUST_29_2016)
#define PHOTON_GUI_LIB_WIDGET_TRACKER_AUGUST_29_2016

#include <photon/support/point.hpp>
#include <photon/widget/widget.hpp>
#include <memory>

namespace photon
{
   ////////////////////////////////////////////////////////////////////////////////////////////////
   // tracker Tracks the mouse movement.
   ////////////////////////////////////////////////////////////////////////////////////////////////
   class tracker : public widget
   {
   public:

      struct info
      {
         explicit info(point start_)
                   : start(start_)
                  {}

                  info(info const&) = default;
         virtual  ~info() {}

         point    start;
         point    current = start;
         point    previous = start;
         point    offset = point{ 0, 0 };
      };

                           tracker()   {}
      virtual              ~tracker()  {}

                           tracker(tracker&& rhs) = default;
      tracker&             operator=(tracker&& rhs) = default;

      virtual widget*      click(context const& ctx, mouse_button btn);
      virtual void         drag(context const& ctx, mouse_button btn);
      virtual bool         is_control() const;

   protected:

      using info_ptr = std::unique_ptr<info>;

      virtual info_ptr     new_state(context const& ctx, point start);
      virtual void         begin_tracking(context const& ctx, info& track_info) = 0;
      virtual void         keep_tracking(context const& ctx, info& track_info) = 0;
      virtual void         end_tracking(context const& ctx, info& track_info) = 0;

   private:

      info_ptr             state;
   };
}

#endif