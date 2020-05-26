/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_IO_CONTEXT_MAY_27_2020)
#define ELEMENTS_IO_CONTEXT_MAY_27_2020

#include <vector>
#include <functional>
#include <chrono>

namespace cycfi { namespace elements
{

class io_context
{
public:
   template <typename F>
   inline void post(F f)
   {
      post(std::chrono::seconds(0), std::move(f));
   }

   template <typename T, typename F>
   inline void post(T duration, F f)
   {
      io_handler handler;
      handler._timeout = std::chrono::duration<double>(duration).count();
      handler._function = std::move(f);
      _handlers.push_back(std::move(handler));
   }

   void poll();

private:
   struct io_handler
   {
      double _timeout {};
      std::function<void()> _function;
   };

   std::vector<io_handler> _handlers;
   std::chrono::steady_clock::time_point _last_poll;
   bool _last_poll_valid = false;
};

}}

#endif
