# Layout

## Table of Contents
* [Limits](#limits)
* [Size Elements](#size-elements)
* [Stretch Elements](#stretch-elements)
* [Align Elements](#align-elements)
* [Margin Elements](#margin-elements)
* [Floating Elements](#floating-element)
* [Flow Element](#flow-element)
* [Tile Elements](#tile-elements)
* [Grid Elements](#grid-elements)
* [Layers Elements](#layers-elements)

-------------------------------------------------------------------------------
Elements provides a comprehensive set of elements for laying out elements the
view.

## Limits

Each element provides a `min` and `max` limits: a hint that determine its
actual extent and how it will be placed in the view depending on available
window space. The element's `limits` member function determines the minimum
and maximum extents of an element:

```c++
view_limits limits(basic_context const& ctx) const;
```

See [Context](context) for information about `basic_context`. `view_limits`
is a struct that gives us the `min` and `max` information:

```c++
struct view_limits
{
   point    min;
   point    max;
};
```

By default, an element has full limits: it is infinitely resizable:

```c++
constexpr view_limits full_limits = { { 0.0, 0.0 }, { full_extent, full_extent } };
```

An element with `full_limits` can be resized from an empty point (zero x and
y size) up to the full extent of the screen, and beyond (if possible).

> :point_right: `full_extent` is actually an implementation defined huge
number near the maximum limits of the coordinate's data type (also
implementation defined).

An element has fixed size in either the `x` or `y` dimension, or both, if its
`limits` has either `min.x == max.x` or `min.y == max.y`, or both.

Examples:

```c++
{ { 100, 100 }, { 100, 100 } }; // Fixed size
```

```c++
{ { 100, 100 }, { 0, full_extent } }; // Fixed width, flexible height
```

```c++
{ { 100, 100 }, { 100, 200 } }; // Fixed width, semi-flexible height (100 to 200)
```

## Size Elements

Size elements override the size limits of an enclosed element.

### limit

![limit]({{ site.url }}/elements/assets/images/limit.png)

Overrides all the limits of an element.

```c++
limit(limits, subject)
```

Effects:
1. The limits of `subject` will be set to the specified `limits` constrained
   by the natural `limits` of the `subject` (the natural limits of the
   element will not be violated).

### fixed_size

![fixed_size]({{ site.url }}/elements/assets/images/fixed_size.png)

Fixes the size of an enclosed element (`subject`):

```c++
fixed_size({ width, height }, subject)
```

Effects:
1. `subject` will be laid out with a fixed `width` and `height`, constrained
   by the natural `limits` of the `subject` (the natural limits of the
   element will not be violated).

### hsize

![hsize]({{ site.url }}/elements/assets/images/hsize.png)

Fixes the horizontal size of an enclosed element (`subject`):

```c++
hsize(width, subject)
```

Effects:
1. `subject` will be laid out with a fixed `width`, constrained by the
   natural horizontal `limits` of the `subject` (the natural horizontal
   limits of the element will not be violated).
3. The natural vertical size of `subject` will not be affected.

### vsize

![vsize]({{ site.url }}/elements/assets/images/vsize.png)

Fixes the vertical size of an enclosed element (`subject`):

```c++
hsize(height, subject)
```

Effects:
1. `subject` will be laid out with a fixed `height`, constrained by the
   natural vertical `limits` of the `subject` (the natural vertical limits of
   the element will not be violated).
3. The natural horizontal size of `subject` will not be affected.

### min_size

![min_size]({{ site.url }}/elements/assets/images/min_size.png)

Overrides the minimum size of an enclosed element (`subject`):

```c++
min_size({ width, height }, subject)
```

Effects:
1. The minimum size of `subject` will be set to the specified `width` and
   `height`, constrained by the natural minumum `limits` of the `subject`
   (the natural minumum limits of the element will not be violated)

### hmin_size

![hmin_size]({{ site.url }}/elements/assets/images/hmin_size.png)

Overrides the minimum width of an enclosed element (`subject`):

```c++
hmin_size(width, subject)
```

Effects:
1. The minimum width of `subject` will be set to the specified `width`
   constrained by the natural horizontal minimum `limits` of the `subject`
   (the natural horizontal minimum limits of the element will not be
   violated)

### vmin_size

![vmin_size]({{ site.url }}/elements/assets/images/vmin_size.png)

Overrides the minimum height of an enclosed element (`subject`):

```c++
vmin_size(height, subject)
```

Effects:
1. The minimum height of `subject` will be set to the specified `height`
   constrained by the natural vertical minimum `limits` of the `subject` (the
   natural vertical minimum limits of the element will not be violated)

### max_size

![max_size]({{ site.url }}/elements/assets/images/max_size.png)

Overrides the maximum size of an enclosed element (`subject`):

```c++
max_size({ width, height }, subject)
```

Effects:
1. The maximum size of `subject` will be set to the specified `width` and
   `height`, constrained by the natural maximum `limits` of the `subject`
   (the natural maximum limits of the element will not be violated)


### hmax_size

![hmax_size]({{ site.url }}/elements/assets/images/hmax_size.png)

Overrides the maximum width of an enclosed element (`subject`):

```c++
hmax_size(width, subject)
```

Effects:
1. The maximum width of `subject` will be set to the specified `width`
   constrained by the natural horizontal maximum `limits` of the `subject`
   (the natural horizontal maximum limits of the element will not be
   violated)

### vmax_size

![vmax_size]({{ site.url }}/elements/assets/images/vmax_size.png)

Overrides the maximum height of an enclosed element (`subject`):

```c++
vmax_size(height, subject)
```

Effects:
1. The maximum height of `subject` will be set to the specified `height`
   constrained by the natural vertical maximum `limits` of the `subject` (the
   natural vertical maximum limits of the element will not be violated)

## Stretch Elements

### hstretch

### vstretch

### scale

## Align Elements

### halign

### align_left

### align_center

### align_right

### valign

### align_top

### align_middle

### align_bottom

### align_left_top

### align_center_top

### align_right_top

### align_left_middle

### align_center_middle

### align_right_middle

### align_left_bottom

### align_center_bottom

### align_right_bottom

## Margin Elements

### margin

### left_margin

### right_margin

### top_margin

### bottom_margin

### xside_margin

### yside_margin

## Floating Element

## Flow Element

## Tile Elements

### htile

### vtile

## Grid Elements

### hgrid

### vgrid

## Layers Elements

### layer

### deck

-------------------------------------------------------------------------------

*Copyright (c) 2014-2020 Joel de Guzman. All rights reserved.*
*Distributed under the [MIT License](https://opensource.org/licenses/MIT)*