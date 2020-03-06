# Layout

## Table of Contents
* [Limits](#limits)
* [Size Elements](#size-elements)
* [Stretch Elements](#stretch-elements)
* [Scale Element](#scale-element)
* [Align Elements](#align-elements)
* [Margin Elements](#margin-elements)
* [Floating](#floating-element)
* [Tiles and Grids](#tiles-and-grids)
* [Layers](#layers)
* [Flow](#flow-element)

-------------------------------------------------------------------------------
Elements provides a comprehensive set of elements for laying out elements the
view.

## Limits

Each element provides information on how it wants to be sized by means of
`min` and `max` limits: a hint that determine its actual extent and how it
will be placed in the view depending on available window space. The element's
`limits` member function determines the minimum and maximum extents of an
element:

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

Size elements override the size limits of an enclosed element. There is a
comprehensive list of size related elements in Elements that can be used for
various purposes. This section catalogues all the available size elements.

### limit

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/limit.png">

Overrides all the limits of an element.

```c++
limit(limits, subject)
```

where `limits` is a struct declared as:

```c++
struct view_limits
{
   point    min;
   point    max;
};
```

Effects:
1. The limits of `subject` will be set to the specified `limits` constrained
   by the natural `limits` of the `subject` (the natural limits of the
   element will not be violated).

### fixed_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/fixed_size.png">

Fixes the size of an enclosed element (`subject`):

```c++
fixed_size({ width, height }, subject)
```

Effects:
1. `subject` will be laid out with a fixed `width` and `height`, constrained
   by the natural `limits` of the `subject` (the natural limits of the
   element will not be violated).

### hsize

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/hsize.png">

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

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/vsize.png">

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

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/min_size.png">

Overrides the minimum size of an enclosed element (`subject`):

```c++
min_size({ width, height }, subject)
```

Effects:
1. The minimum size of `subject` will be set to the specified `width` and
   `height`, constrained by the natural minumum `limits` of the `subject`
   (the natural minumum limits of the element will not be violated)

### hmin_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/hmin_size.png">

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

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/vmin_size.png">

Overrides the minimum height of an enclosed element (`subject`):

```c++
vmin_size(height, subject)
```

Effects:
1. The minimum height of `subject` will be set to the specified `height`
   constrained by the natural vertical minimum `limits` of the `subject` (the
   natural vertical minimum limits of the element will not be violated)

### max_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/max_size.png">

Overrides the maximum size of an enclosed element (`subject`):

```c++
max_size({ width, height }, subject)
```

Effects:
1. The maximum size of `subject` will be set to the specified `width` and
   `height`, constrained by the natural maximum `limits` of the `subject`
   (the natural maximum limits of the element will not be violated)


### hmax_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/hmax_size.png">

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

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/vmax_size.png">

Overrides the maximum height of an enclosed element (`subject`):

```c++
vmax_size(height, subject)
```

Effects:
1. The maximum height of `subject` will be set to the specified `height`
   constrained by the natural vertical maximum `limits` of the `subject` (the
   natural vertical maximum limits of the element will not be violated)

## Stretch Elements

Resizable elements are elements with minimum limits that are less than the
maximum limits in either x or y dimensions or both. Resizable elements can
therefore stretch if there is extra available space allocated to it beyond
its minimum limit. When two or more resizable elements are placed side by
side in an [htile](#htile) or [vtile](#vtile), the element's "stretchiness"
determines how much extra space is given to it. The element's "stretchiness"
is determined by the element's `stretch` member function:

```c++
virtual view_stretch stretch() const;
```

where `view_stretch` is a struct with declared as:

```c++
struct view_stretch
{
   float    x;
   float    y;
};
```

The `view_stretch` `x` and `y` members determine how much an element can
stretch in the x or y dimensions. The default is 1.0. A stretchiness value of
2.0 means that the element is able to stretch twice as much compared to its
siblings in an [htile](#htile) or [vtile](#vtile), assuming they have the
default 1.0 stretchiness.

### hstretch

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/hstretch.png">

Overrides the horizontal stretchiness of an an enclosed element (`subject`):

```c++
hstretch(stretch, subject)
```

Effects:
1. The `subject` will assume the given `stretch` value.
2. The stretch value has no effect to elements with fixed horizontal sizes.

For example, the image below shows how three elements are laid out in an
`htile`, with stretch values of `1.0`, `1.0` and `2.0`, respectively:

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/htile-stretch.png">

The element with the `2.0` stretch value stretches twice as much compared to
its siblings.

### vstretch

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/vstretch.png">

Overrides the vertical stretchiness of an an enclosed element (`subject`):

```c++
vstretch(stretch, subject)
```

Effects:
1. The `subject` will assume the given `stretch` value.
2. The stretch value has no effect to elements with fixed vertical sizes.

For example, the image below shows how three elements are laid out in an
`htile`, with stretch values of `0.5`, `1.0` and `1.5`, respectively:

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/vtile-stretch.png">

The element with the `0.5` stretch value stretches half less, while the
element with the `1.5` stretches half more than the default.

## Scale Element

Elements is resolution independent and elements can be scaled up or down to
suit. The default scale is 1.0 (no scale).

### scale

The `scale` element changes the scale of its enclosed element (`subject`):

```c++
scale(scale_, subject)
```

Effects:
1. The `subject` will scaled given the `scale_` value. A value > 1.0 scales
   the element up (zoom in), while a value < 1.0 scales down (zoom out).

## Align Elements

An element can be aligned arbitrarily from 0.0 to 1.0 in either the x or y
dimensions, or both. There is a comprehensive list of align elements in
Elements that can be used for various purposes. This section catalogues all
the available align elements.

### halign

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/halign.png">

Aligns the an enclosed element (`subject`) in the x dimension:

```c++
halign(align, subject)
```

Effects:
1. Given a total allocated space `X`, `subject` will be positioned
   horizontally to `X * align`.
2. The `subject` will assume its minimum horizontal size: `limits().min.x`.

Examples:
1. `align = 0.0`: align `subject` to the left
2. `align = 1.0`: align `subject` to the right
3. `align = 0.5`: align `subject` to the center

### align_left

<img width="30%" height="30%" src="{{ site.url }}/elements/assets/images/align_left.png">

Left-aligns the an enclosed element (`subject`):

```c++
align_left(subject)
```

Effects:
1. Equivalent to `halign(0.0, subject)`

### align_center

<img width="30%" height="30%" src="{{ site.url }}/elements/assets/images/align_center.png">

Center-aligns the an enclosed element (`subject`):

```c++
align_center(subject)
```

Effects:
1. Equivalent to `halign(0.5, subject)`

### align_right

<img width="30%" height="30%" src="{{ site.url }}/elements/assets/images/align_right.png">

Right-aligns the an enclosed element (`subject`):

```c++
align_right(subject)
```

Effects:
1. Equivalent to `halign(1.0, subject)`

### valign

<img width="20%" height="20%" src="{{ site.url }}/elements/assets/images/valign.png">

Aligns the an enclosed element (`subject`) in the y dimension:

```c++
valign(align, subject)
```

Effects:
1. Given a total allocated space `Y`, `subject` will be positioned vertically
   to `Y * align`.
2. The `subject` will assume its minimum vertical size: `limits().min.y`.

Examples:
1. `align = 0.0`: align `subject` to the top
2. `align = 1.0`: align `subject` to the right
3. `align = 0.5`: align `subject` to the middle

### align_top

<img width="15%" height="15%" src="{{ site.url }}/elements/assets/images/align_top.png">

Aligns the an enclosed element (`subject`) to the top:

```c++
align_top(subject)
```

Effects:
1. Equivalent to `valign(0.0, subject)`

### align_middle

<img width="15%" height="15%" src="{{ site.url }}/elements/assets/images/align_middle.png">

Aligns the an enclosed element (`subject`) to the middle:

```c++
align_middle(subject)
```

Effects:
1. Equivalent to `valign(0.5, subject)`

### align_bottom

<img width="15%" height="15%" src="{{ site.url }}/elements/assets/images/align_bottom.png">

Aligns the an enclosed element (`subject`) to the bottom:

```c++
align_bottom(subject)
```

Effects:
1. Equivalent to `valign(1.0, subject)`

### align_left_top

<img width="25%" height="25%" src="{{ site.url }}/elements/assets/images/align_left_top.png">

Aligns the an enclosed element (`subject`) to the left-top:

```c++
align_left_top(subject)
```

Effects:
1. Equivalent to `align_left(align_top(subject))`

### align_center_top

<img width="25%" height="25%" src="{{ site.url }}/elements/assets/images/align_center_top.png">

Aligns the an enclosed element (`subject`) to the center-top:

```c++
align_center_top(subject)
```

Effects:
1. Equivalent to `align_center(align_top(subject))`

### align_right_top

<img width="25%" height="25%" src="{{ site.url }}/elements/assets/images/align_right_top.png">

Aligns the an enclosed element (`subject`) to the right-top:

```c++
align_right_top(subject)
```

Effects:
1. Equivalent to `align_right(align_top(subject))`

### align_left_middle

<img width="25%" height="25%" src="{{ site.url }}/elements/assets/images/align_left_middle.png">

Aligns the an enclosed element (`subject`) to the left-middle:

```c++
align_left_middle(subject)
```

Effects:
1. Equivalent to `align_left(align_middle(subject))`

### align_center_middle

<img width="25%" height="25%" src="{{ site.url }}/elements/assets/images/align_center_middle.png">

Aligns the an enclosed element (`subject`) to the center-middle:

```c++
align_center_middle(subject)
```

Effects:
1. Equivalent to `align_center(align_middle(subject))`

### align_right_middle

<img width="25%" height="25%" src="{{ site.url }}/elements/assets/images/align_right_middle.png">

Aligns the an enclosed element (`subject`) to the right-middle:

```c++
align_right_middle(subject)
```

Effects:
1. Equivalent to `align_right(align_middle(subject))`

### align_left_bottom

<img width="25%" height="25%" src="{{ site.url }}/elements/assets/images/align_left_bottom.png">

Aligns the an enclosed element (`subject`) to the left-bottom:

```c++
align_left_bottom(subject)
```

Effects:
1. Equivalent to `align_left(align_bottom(subject))`

### align_center_bottom

<img width="25%" height="25%" src="{{ site.url }}/elements/assets/images/align_center_bottom.png">

Aligns the an enclosed element (`subject`) to the center-bottom:

```c++
align_center_bottom(subject)
```

Effects:
1. Equivalent to `align_center(align_bottom(subject))`

### align_right_bottom

<img width="25%" height="25%" src="{{ site.url }}/elements/assets/images/align_right_bottom.png">

Aligns the an enclosed element (`subject`) to the right-bottom:

```c++
align_right_bottom(subject)
```

Effects:
1. Equivalent to `align_right(align_bottom(subject))`

## Margin Elements

Margins add some space around elements. There is a comprehensive list of
margin elements in Elements that can be used for various purposes. This
section catalogues all the available margin elements.

### margin

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/margin.png">

Adds a margin all around an enclosed element (`subject`):

```c++
margin({ left, top, right, bottom }, subject)
```

Effects:
1. Space is added to the left, top, right, and bottom of the subject with the
   given parameters.
2. The element's limits is overridden to account for the additional space.
3. The `margin` does not violate the natural limits of the subject. `margin`
   will respect the subject's min-max constraints and resizability.


### left_margin

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/left_margin.png">

Adds a margin to the left of an enclosed element (`subject`):

```c++
left_margin(left, subject)
```

Effects:
1. Space is added to the left of the subject with the given parameter.
2. The element's limits is overridden to account for the additional space.
3. The `left_margin` does not violate the natural limits of the subject.
   `left_margin` will respect the subject's min-max constraints and
   resizability.

### right_margin

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/right_margin.png">

Adds a margin to the right of an enclosed element (`subject`):

```c++
right_margin(right, subject)
```

Effects:
1. Space is added to the right of the subject with the given parameter.
2. The element's limits is overridden to account for the additional space.
3. The `right_margin` does not violate the natural limits of the subject.
   `right_margin` will respect the subject's min-max constraints and
   resizability.

### top_margin

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/top_margin.png">

Adds a margin to the top of an enclosed element (`subject`):

```c++
top_margin(top, subject)
```

Effects:
1. Space is added to the top of the subject with the given parameter.
2. The element's limits is overridden to account for the additional space.
3. The `top_margin` does not violate the natural limits of the subject.
   `top_margin` will respect the subject's min-max constraints and
   resizability.

### bottom_margin

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/bottom_margin.png">

Adds a margin to the bottom of an enclosed element (`subject`):

```c++
bottom_margin(bottom, subject)
```

Effects:
1. Space is added to the to the bottom of the subject with the given
   parameter.
2. The element's limits is overridden to account for the additional space.
3. The `bottom_margin` does not violate the natural limits of the subject.
   `bottom_margin` will respect the subject's min-max constraints and
   resizability.

### hmargin
(same as `left_right_margin`)

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/hmargin.png">

Adds a margin to the left and right sides of an enclosed element (`subject`):

```c++
// variant 1
hmargin({ left, right }, subject)

// variant 2
hmargin(left, right, subject)

// variant 3
left_right_margin({ left, right }, subject)

// variant 4
left_right_margin(left, right, subject)
```

Effects:
1. Space is added to the to the left and right sides of the subject with the
   given parameters.
2. The element's limits is overridden to account for the additional space.
3. The `hmargin` (and variants) does not violate the natural limits of the
   subject. `hmargin` (and variants) will respect the subject's min-max
   constraints and resizability.

### vmargin
(same as `top_bottom_margin`)

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/vmargin.png">

Adds a margin to the top and bottom sides of an enclosed element (`subject`):

```c++
// variant 1
vmargin({ left, right }, subject)

// variant 2
vmargin(left, right, subject)

// variant 3
top_bottom_margin({ left, right }, subject)

// variant 4
top_bottom_margin(left, right, subject)
```

Effects:
1. Space is added to the to the top and bottom sides of the subject with the
   given parameters.
2. The element's limits is overridden to account for the additional space.
3. The `vmargin` (and variants) does not violate the natural limits of the
   subject. `vmargin` (and variants) will respect the subject's min-max
   constraints and resizability.

### left_top_margin

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/left_top_margin.png">

Adds a margin to the left and top sides of an enclosed element (`subject`):

```c++
// variant 1
left_top_margin({ left, top }, subject)

// variant 2
left_top_margin(left, top, subject)
```

Effects:
1. Space is added to the to the left and top sides of the subject with the
   given parameters.
2. The element's limits is overridden to account for the additional space.
3. The `left_top_margin` (and variant) does not violate the natural limits of
   the subject. `left_top_margin` (and variant) will respect the subject's
   min-max constraints and resizability.

### left_bottom_margin

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/left_bottom_margin.png">

Adds a margin to the left and bottom sides of an enclosed element (`subject`):

```c++
// variant 1
left_bottom_margin({ left, bottom }, subject)

// variant 2
left_bottom_margin(left, bottom, subject)
```

Effects:
1. Space is added to the to the left and bottom sides of the subject with the
   given parameters.
2. The element's limits is overridden to account for the additional space.
3. The `left_bottom_margin` (and variant) does not violate the natural limits
   of the subject. `left_bottom_margin` (and variant) will respect the
   subject's min-max constraints and resizability.

### right_top_margin

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/right_top_margin.png">

Adds a margin to the right and top sides of an enclosed element (`subject`):

```c++
// variant 1
right_top_margin({ left, top }, subject)

// variant 2
right_top_margin(left, top, subject)
```

Effects:
1. Space is added to the to the right and top sides of the subject with the
   given parameters.
2. The element's limits is overridden to account for the additional space.
3. The `right_top_margin` (and variant) does not violate the natural limits
   of the subject. `right_top_margin` (and variant) will respect the
   subject's min-max constraints and resizability.

### right_bottom_margin

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/right_top_margin.png">

Adds a margin to the right and bottom sides of an enclosed element (`subject`):

```c++
// variant 1
right_bottom_margin({ right, bottom }, subject)

// variant 2
right_bottom_margin(right, bottom, subject)
```

Effects:
1. Space is added to the to the right and bottom sides of the subject with the
   given parameters.
2. The element's limits is overridden to account for the additional space.
3. The `right_bottom_margin` (and variant) does not violate the natural
   limits of the subject. `right_bottom_margin` (and variant) will respect
   the subject's min-max constraints and resizability.

## Floating

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/floating.png">

A floating element allows arbitrary placement of an enclosed element
(`subject`) in the main view:

```c++
floating({ left, top, right, bottom }, subject)
```

Effects:
1. The element will be placed exactly to the specified position in the main
   view, constrained to the subject's natural limits.
2. The floating element does not violate the natural limits of the subject.

## Tiles and Grids

Tiles are the most useful layout elements, seconded by Grids. Tiles are used
everywhere for composing hierarchical elements in rows and columns. Grids are
similar to tiles, but grids have fixed sizes while tiles allow elements to
fluidly adjust depending on available space. Tiles are best used for
composing UI elements while grids are best for composing tables.

### Horizontal Tiles and Grids

Horizontal Tiles and Grids are composites that lay out one or more child
elements in a row, respecting the horizontal size requirements of each
element (`limits().min.x` and `limits().max.x`).

<img width="60%" height="60%" src="{{ site.url }}/elements/assets/images/htile.png">

### hgrid

### htile

<img width="60%" height="60%" src="{{ site.url }}/elements/assets/images/htile-stretch2.png">

### Vertical Tiles and Grids

Vertical Tiles and Grids are composites that lay out one or more child
elements in a column, respecting the vertical size requirements of each
element (`limits().min.y` and `limits().max.y`).

<img width="60%" height="60%" src="{{ site.url }}/elements/assets/images/vtile.png">

### vgrid

### vtile

<img width="60%" height="60%" src="{{ site.url }}/elements/assets/images/vtile-stretch2.png">

## Layers

### layer

### deck

## Flow

The flow composite element lays out its children much like the way text is
laid out: lay out each element from left to right, fitting as much elements
as possible following each child's minimum width (`limits().min.x`). Once a
row is full, move to the next row and do the same until the end of the row is
filled. Repeat the procedure until all the elements are laid out. The height
of each row is determined by the maximum height (`limits().max.y`) of all the
elements to be laid out in that row. The following graphic depicts a
simplified layout scenario for child elements `a` to `r`.

<img width="60%" height="60%" src="{{ site.url }}/elements/assets/images/flow.png">

The child elements arranged in a `flow` composite are automatically re-flowed
(re-layout) when the view size changes.

### flowable_container

To have elements laid out using `flow`, you need to make a `flowable_container`:

```c++
c = vector_composite<flowable_container>{};
```

The `vector_composite` creates a `std::vector<element_ptr>` that the client
uses to manage the composite's children elements. The lifetime of the
container, `c` is the client's responsibility.

Here's an example usage:

```c++
vector_composite<flowable_container> _children; // A class member
```

Then `push_back` a child element, `child`:

```c++
_children.push_back(share(child));
```

> :point_right: `share` turns an element object into an `element_ptr` held by
> the `std::vector<element_ptr>` in `flowable_container`.

### flow

Once we have a `flowable_container`, we can place its contents in a `flow`
element:

```c++
flow(c)
```

where `c` is the `flowable_container` we populated prior.

-------------------------------------------------------------------------------

*Copyright (c) 2014-2020 Joel de Guzman. All rights reserved.*
*Distributed under the [MIT License](https://opensource.org/licenses/MIT)*