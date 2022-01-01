/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <X11/Xlib.h>
#include <X11/Xlib.h>

namespace cycfi { namespace elements
{
    struct host_window
    {
        Window host;
    };

    Display* get_display();
    Window get_window(host_window& h)
    {
        return h.host;
    }

    window::window(std::string const& name, int /*style_*/, rect const& bounds)
    : _window(new host_window)
    {
        Display* display = get_display();
        int screen = DefaultScreen(display);
        int depth  = DefaultDepth(display, screen);
        int border = 1;
        Visual *visual = DefaultVisual(display, screen);
        XSetWindowAttributes attributes;
        attributes.background_pixel = XWhitePixel(display, screen);

        _window->host = XCreateWindow(
            display,
            RootWindow(display, screen),
            bounds.top, bounds.left, bounds.right, bounds.bottom,
            border,
            depth,
            InputOutput,
            visual,
            CWBackPixel,
            &attributes);

        XSelectInput(display, _window->host, ExposureMask | KeyPressMask);
        XMapWindow(display, _window->host);
        XStoreName(display, _window->host, name.c_str());

        Atom wm_delete_window = XInternAtom(get_display(), "WM_DELETE_WINDOW", True);
        XSetWMProtocols(display, _window->host, &wm_delete_window, 1);
        XFlush(display);
    }

    window::~window()
    {
        on_close(); // FIXME: move where make possible to veto the event
        XDestroyWindow(get_display(), _window->host);
        delete _window;
    }

    point window::size() const
    {
        return point(); // TODO
    }

    void window::size(point const& p)
    {
        (void)p; // TODO
    }

    void window::limits(view_limits limits_)
    {
        (void)limits_; // TODO
    }

    point window::position() const
    {
        return point(); // TODO
    }

    void window::position(point const& p)
    {
        (void)p; // TODO
    }
}}
