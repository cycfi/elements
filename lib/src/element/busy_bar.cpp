#include <elements/element/busy_bar.hpp>
#include <elements/support/draw_utils.hpp>
#include <elements/support/theme.hpp>
#include <elements/view.hpp>

namespace cycfi::elements
{
   // Implement the start method as per the patch
   void busy_bar_base::start(double val)
   {
      _start = clamp(val, 0.0, 1.0);
   }

   // Implement foreground_bounds to include the start value
   rect busy_bar_base::foreground_bounds(context const& ctx) const
   {
      auto bounds = ctx.bounds;

      if (bounds.width() > bounds.height())
         bounds.width(bounds.width() * value());
      else
         bounds.height(bounds.height() * (1.0 - value()));

      // Adjust bounds based on the start value
      if (bounds.width() > bounds.height())
         bounds.left = bounds.left + (bounds.width() * start());
      else
         bounds.bottom = bounds.bottom + (bounds.height() * start());

      return bounds;
   }

   void busy_bar_base::animate(view& view_, duration time)
   {
      _time = time;
      animate(view_);
   }

   void busy_bar_base::animate(view& view_)
   {
      if(_time == _time.zero()) {
         _status = -0.2;
         value(0.0);
         start(0.0);
         view_.refresh();
         return;
      }
      _status += 0.01;
      start(_status);
      value(_status + 0.2);
      if(_status >= 1.0) {
         _status = -0.2;
      }
      view_.refresh();
      view_.post(std::chrono::duration_cast<std::chrono::milliseconds>(_time), [&view_, this]() {
         animate(view_);
      });
   }
}
