/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <elements/support/io_context.hpp>

namespace cycfi { namespace elements
{

void io_context::poll()
{
    const auto now = std::chrono::steady_clock::now();
    const double time_delta = std::chrono::duration<double>(now - _last_poll).count();
    _last_poll = now;

    if (!_last_poll_valid)
    {
       _last_poll_valid = true;
       return;
    }

    for (auto& handler : _handlers)
       handler._timeout -= time_delta;

    bool any_handled = false;
    const std::size_t num_handlers = _handlers.size();

    for (std::size_t i = 0; i < num_handlers; ++i)
    {
       auto& handler = _handlers[i];
       if (handler._timeout < 0)
       {
          handler._function();
          any_handled = true;
       }
    }

    if (any_handled)
    {
       size_t count = 0;
       for (size_t i = 0, n = _handlers.size(); i < n; ++i)
       {
          if (_handlers[i]._timeout >= 0)
             _handlers[count++] = std::move(_handlers[i]);
       }
       _handlers.resize(count);
    }
}

}}
