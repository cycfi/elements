/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>

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

        // TODO SubstructureNotify for detecting child resizing
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

        XSelectInput(display, _window->host, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);
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
        Window root;
        int x, y;
        unsigned int w = 0, h = 0, border_w, depth;
        XGetGeometry(get_display(), _window->host, &root, &x, &y, &w, &h, &border_w, &depth);
        return {static_cast<float>(w), static_cast<float>(h)};
    }

    void window::size(point const& p)
    {
        XResizeWindow(get_display(), _window->host, p.x, p.y);
    }

    void window::limits(view_limits limits_)
    {
        XSizeHints hints = {};
        hints.flags = PMinSize | PMaxSize;
        hints.min_width = limits_.min.x;
        hints.min_height = limits_.min.y;
        hints.max_width = limits_.max.x;
        hints.max_height = limits_.max.y;

        XSetWMNormalHints(get_display(), _window->host, &hints);
    }

    point window::position() const
    {
        Display* display = get_display();
        int screen = DefaultScreen(display);
        int x, y;
        Window child;
        XTranslateCoordinates(display, _window->host, RootWindow(display, screen), 0, 0, &x, &y, &child );
        return {static_cast<float>(x), static_cast<float>(y)};
    }

    void window::position(point const& p)
    {
        (void)p; // TODO
    }
}}
