/*=============================================================================
   Copyright (c) 2016-2019 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <photon/view.hpp>
#include <photon/window.hpp>
#include <photon/support/context.hpp>
// #include <chrono>
// #include <thread>
// #include <unordered_map>

 namespace cycfi { namespace photon
 {
//    class idle_tasks
//    {
//    public:

//       using task = view::task;
//       using task_list = std::unordered_map<std::string, task>;

//       idle_tasks()
//        : _loop([this]{ run(); })
//       {
//       }

//       ~idle_tasks()
//       {
//          _running = false;
//          _loop.join();
//       }

//       void add(std::string name, task const& f)
//       {
//          std::lock_guard<std::mutex> guard(_mutex);
//          _tasks[name] = f;
//       }

//       void remove(std::string name)
//       {
//          std::lock_guard<std::mutex> guard(_mutex);
//          _tasks.erase(name);
//       }

//    private:

//       void run()
//       {
//          using namespace std::chrono;
//          auto start = high_resolution_clock::now();
//          milliseconds const period(1000);
//          while (_running)
//          {
//             std::this_thread::sleep_for(milliseconds(10));
//             auto now = high_resolution_clock::now();
//             auto elapsed = duration_cast<milliseconds>(now - start);
//             if (period <= elapsed)
//             {
//                start = now;
//                std::lock_guard<std::mutex> guard(_mutex);
//                for (auto const& task : _tasks)
//                   task.second();
//             }
//          }
//       }

//       task_list      _tasks;
//       std::thread    _loop;
//       bool           _running = true;
//       std::mutex     _mutex;
//    };

   view::view(host_window h)
    : base_view(h)
   //  , _idle_tasks(std::make_unique<idle_tasks>())
   {
   }

   view::view(window& win)
    : base_view(win.host())
   //  , _idle_tasks(std::make_unique<idle_tasks>())
   {
      on_change_limits = [&win](view_limits limits_)
      {
         win.limits(limits_);
      };
   }

   view::~view()
   {
   }

   void view::set_limits()
   {
      if (_content.empty())
         return;

      auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
      auto context_ = cairo_create(surface_);
      canvas cnv{ *context_ };
      bool resized = false;

      // Update the limits and constrain the window size to the limits
      basic_context bctx{ *this, cnv };
      auto limits_ = _content.limits(bctx);
      if (limits_.min != _current_limits.min || limits_.max != _current_limits.max)
      {
         _current_limits = limits_;
         if (on_change_limits)
            on_change_limits(limits_);
      }

      cairo_surface_destroy(surface_);
      cairo_destroy(context_);
   }

   void view::draw(cairo_t* context_, rect dirty_)
   {
      if (_content.empty())
         return;

      _dirty = dirty_;

      // Update the limits and constrain the window size to the limits
      set_limits();

      canvas cnv{ *context_ };
      auto size_ = size();
      rect subj_bounds = { 0, 0, size_.x, size_.y };
      context ctx{ *this, cnv, &_content, subj_bounds };

      // layout the subject only if the window bounds changes
      if (subj_bounds != _current_bounds)
      {
         _current_bounds = subj_bounds;
         _content.layout(ctx);
      }

      // draw the subject
      _content.draw(ctx);
   }

   namespace
   {
      template <typename F, typename This>
      void call(F f, This& self, rect _current_bounds)
      {
         auto surface_ = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
         auto context_ = cairo_create(surface_);
         canvas cnv{ *context_ };
         context ctx { self, cnv, &self.content(), _current_bounds };

         f(ctx, self.content());

         cairo_surface_destroy(surface_);
         cairo_destroy(context_);
      }
   }

   void view::refresh(element& element)
   {
      call(
         [&element](auto const& ctx, auto& _content) { _content.refresh(ctx, element); },
         *this, _current_bounds
      );
   }

   void view::refresh(context const& ctx)
   {
      refresh(ctx.bounds);
   }

   void view::click(mouse_button btn)
   {
      if (_content.empty())
         return;

      call(
         [btn](auto const& ctx, auto& _content) { _content.click(ctx, btn); },
         *this, _current_bounds
      );
   }

   void view::drag(mouse_button btn)
   {
      if (_content.empty())
         return;

      call(
         [btn](auto const& ctx, auto& _content) { _content.drag(ctx, btn); },
         *this, _current_bounds
      );
   }

   void view::cursor(point p, cursor_tracking status)
   {
      if (_content.empty())
         return;

      call(
         [p, status](auto const& ctx, auto& _content)
         {
            if (!_content.cursor(ctx, p, status))
               set_cursor(cursor_type::arrow);
         },
         *this, _current_bounds
      );
   }

   void view::scroll(point dir, point p)
   {
      if (_content.empty())
         return;

      call(
         [dir, p](auto const& ctx, auto& _content) { _content.scroll(ctx, dir, p); },
         *this, _current_bounds
      );
   }

   void view::key(key_info const& k)
   {
      if (_content.empty())
         return;

      call(
         [k](auto const& ctx, auto& _content) { _content.key(ctx, k); },
         *this, _current_bounds
      );
   }

   void view::text(text_info const& info)
   {
      if (_content.empty())
         return;

      call(
         [info](auto const& ctx, auto& _content) { _content.text(ctx, info); },
         *this, _current_bounds
      );
   }

   void view::add_undo(undo_redo_task f)
   {
      _undo_stack.push(f);
      if (has_redo())
      {
         // clear the redo stack
         undo_stack_type empty{};
         _redo_stack.swap(empty);
      }
   }

   bool view::undo()
   {
      if (has_undo())
      {
         auto t = _undo_stack.top();
         _undo_stack.pop();
         _redo_stack.push(t);
         t.undo();  // execute undo function
         return true;
      }
      return false;
   }

   bool view::redo()
   {
      if (has_redo())
      {
         auto t = _redo_stack.top();
         _undo_stack.push(t);
         _redo_stack.pop();
         t.redo();  // execute redo function
         return true;
      }
      return false;
   }

   void view::focus(focus_request r)
   {
      if (_content.empty())
         return;

      _content.focus(r);
      refresh();
   }

   void view::content(layers_type&& layers)
   {
      _content = std::forward<layers_type>(layers);
      set_limits();
   }

   void view::tick()
   {
      using namespace std::chrono;
      auto now = high_resolution_clock::now();
      for (auto& task : _tasks)
      {
         if (now >= (task.second.start + task.second.period))
         {
            task.second.start = now;
            task.second.f();
         }
      }
   }

   void view::add_task(void *id, milliseconds period, task const &f)
   {
      using namespace std::chrono;
      _tasks[id] = { f, period, high_resolution_clock::now() };
   }

   void view::remove_task(void *id)
   {
      _tasks.erase(id);
   }
}}
