# Concepts

## Table of Contents
* [Flyweight](flyweight)
* [App and Window](#app-and-window)
* [Background](#background)
* [Aligns and Sizes](#aligns-and-sizes)
* [Labels, Margins and Layers](#labels-margins-and-layers)

-------------------------------------------------------------------------------

## Flyweight

Elements, is very lightweight and extremely modular. You compose very
fine-grained, flyweight “elements” to form deep element hierarchies using a
declarative interface with heavy emphasis on reuse. Specific examples should
make it clear.

Let's start with a basic window with a dark grey background. Actually, this
is the same as the 'empty' example provided in the ‘examples’ directory.

```c++
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

int main(int argc, const char* argv[])
{
   app _app(argc, argv);
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      {
         share(background)
      }
   );

   _app.run();
   return 0;
```

## App and Window

Here, we create an `app` and a `window`, and set the window's `on_close` to
`stop` or quit the application. `_win.on_close` is called when the window is
closed by the user, thereby quitting the application.

Like any other GUI libraries or frameworks, the `app` manages the main event
loop, while the `window` manages application windows. However, take note that
the `app` and the `window` classes are optional. There are certain
situations, like e.g. building plugins or incorporating Elements in another
framework such as QT, where you do not want Elements to have manage the event
loop or windows. The important class of interest here is the `view`. In these
cases, you want to embed the `view` directly. You can directly construct a
`view` given a platform window (or widget), as its main `content` view or
child window (or widget).

The `view` does not know anything about drawing or user interaction. Instead,
it relies on its `content` to do these. Without `content`, it is an empty
shell. And so this is where it starts to get interesting. The content of the
`view` typically contains multiple layers, much like typical graphics
applications. In this example, we only have one layer: the `background`.

## Background

```c++
auto background = box(bkd_color);
```

It is a simple, infinitely resizable `box` with the specified color:
`rgba(35, 35, 37, 255)`. It will resize automatically when the window is
resized. We set it as the view's content:

```c++
view_.content(
   {
      share(background)
   }
);
```

Running this example, you get:

![concepts_a]({{ site.url }}/elements/assets/images/concepts_a.png){: width="60%" }

## Aligns and Sizes

Now let's add a blue round rectangle. We've seen how to create a box: the
background using `box`, passing in a color. A rounded box is created using
`rbox`, passing in the color and the radius:

```c++
auto blue_rbox = rbox(colors::navy_blue, 15);
```

We can actually use that already, as-is, by placing it in another layer in
the view:

```c++
view_.content(
   {
      share(blue_rbox),
      share(background)
   }
);
```

But like the `box`, the `rbox` is infinitely resizable and will hog the
entire window. What we want is to give it a specific size and center it in
the window. Elements are very lightweight. Most elements do not even have a
size, nor know their position in the view.

So we give it a 100x50 size by wrapping it in the `fixed_size` element:

```c++
auto blue_rbox =
   fixed_size(
      { 100, 50 },
      rbox(colors::navy_blue, 15)
   );
```

And then we align it centered (in the x axis) and middle (in the Y axis)
using the `align_center_middle` element:

```c++
auto blue_rbox =
   align_center_middle(
      fixed_size(
         { 100, 50 },
         rbox(colors::navy_blue, 15)
      )
   );
```

Without the alignment element, the main window would have been constrained to
a fixed 100x50 size.

But now, we are seeing how fine-grained elements are composed. The `rbox` is
placed inside the `fixed_size` element which is then placed inside a
`align_center_middle` element.

Let's run this example:

![concepts_b]({{ site.url }}/elements/assets/images/concepts_b.png){: width="60%" }

## Labels, Margins and Layers

Our goal this time is to place some text inside the blue box. The element we
will use is the `label`. Most elements do not have a size, but as an
exception, labels (simple text elements) do. So, instead of fixing the size
of the box using `fixed_size`, we instead place the label alongside the box
in a `layer` element, but add some margin. Here's how yo do it:

```c++
auto blue_rbox =
   align_center_middle(
      layer(
         margin(
            { 20, 20, 20, 20 },
            label("Hey There!")
         ),
         rbox(colors::blue, 15)
      )
   );
```

If it's not obvious yet, let's spell it out for you.

Step 1: We make a label:

```c++
label("Hey There!")
```

Step 2: We add a 20px margin around it:

```c++
margin(
   { 20, 20, 20, 20 },
   label("Hey There!")
)
```

Step 3: We make a blue rounded box:

```c++
rbox(colors::blue, 15)
```

Step 4: We place the label and the blue rounded box as siblings in a `layer`
element:

```c++
layer(
   margin(
      { 20, 20, 20, 20 },
      label("Hey There!")
   ),
   rbox(colors::blue, 15)
)
```

Step 4: Then finally, we place it in the `align_center_middle` element.

Tip: If the hierarchy gets a bit too deep, use the c++ `auto` to name
sub-parts of the element composition.

The `layer` element is a composite element that holds zero or more 'child'
elements. Some of you might have noticed that we already mentioned the
`layer` element before when we were discussing the view's content in the [App
and Window](#app-and-window) section: "The content of the `view` typically
contains multiple layers, much like typical graphics applications". Well,
actually, this is the same `layer` thing here. The view's main content is a
`layer` element.

-------------------------------------------------------------------------------

*Copyright (c) 2014-2020 Joel de Guzman. All rights reserved.*
*Distributed under the [MIT License](https://opensource.org/licenses/MIT)*