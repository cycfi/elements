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

For the purpose of this document, we will use these terms and limit
expressions:

limits
: The limits of an element

limits.min
: The minimum limits of an element

limits.min.x
: The minimum horizontal limits of an element

limits.min.y
: The minimum vertical limits of an element

limits.max
: The maximum limits of an element

limits.max.x
: The maximum horizontal limits of an element

limits.max.y
: The maximum vertical limits of an element

horizontal limits
: (limits.min.x, limits.max.x)

vertical limits
: (limits.min.y, limits.max.y)

minimum limits
: (limits.min.x, limits.min.y)

maximum limits
: (limits.max.x, limits.max.y)

minimum horizontal limits
: limits.min.x

maximum horizontal limits
: limits.max.x

minimum vertical limits
: limits.min.y

maximum vertical limits
: limits.max.y

By default, an element has full limits: it is infinitely resizable:

```c++
constexpr view_limits full_limits = { { 0.0, 0.0 }, { full_extent, full_extent } };
```

An element with `full_limits` can be resized from an empty point (zero x and
y size) up to the full extent of the screen, and beyond (if possible).

> :point_right: `full_extent` is actually an implementation defined huge
number near the *maximum limits* of the coordinate's data type (also
implementation defined).

* An element has a fixed horizontal size if this expression is true:
  `limits.min.x == limits.max.x`.

* An element has a fixed vertical size if this expression is true:
  `limits.min.y == limits.max.y`.

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

Size elements override the *limits* of an enclosed element. There is a
comprehensive list of size related elements in Elements that can be used for
various purposes. This section catalogues all the available size elements.

### limit

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/limit.png">

Overrides the *limits* of an element.

```c++
limit(limits, subject)
```

Effects:
1. The *limits* of `subject` will be set to the specified `limits`
   constrained by the natural *limits* of the `subject` (the natural *limits*
   of the element will not be violated).

### fixed_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/fixed_size.png">

Fixes the size of an enclosed element (`subject`):

```c++
fixed_size({ width, height }, subject)
```

Effects:
1. `subject` will be laid out with a fixed `width` and `height`, constrained
   by the natural *limits* of the `subject` (the natural *limits* of the
   element will not be violated).

### hsize

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/hsize.png">

Fixes the horizontal size of an enclosed element (`subject`):

```c++
hsize(width, subject)
```

Effects:
1. `subject` will be laid out with a fixed `width`, constrained by the
   natural *horizontal limits* of the `subject` (the natural *horizontal
   limits* of the element will not be violated).
3. The natural *vertical limits* of `subject` will not be affected.

### vsize

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/vsize.png">

Fixes the *vertical limits* of an enclosed element (`subject`):

```c++
hsize(height, subject)
```

Effects:
1. `subject` will be laid out with a fixed `height`, constrained by the
   natural *vertical limits* of the `subject` (the natural *vertical limits*
   of the element will not be violated).
3. The natural *horizontal limits* of `subject` will not be affected.

### min_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/min_size.png">

Overrides the *minimum limits* of an enclosed element (`subject`):

```c++
min_size({ width, height }, subject)
```

Effects:
1. The *minimum limits* of `subject` will be set to the specified `width` and
   `height`, constrained by the natural *minimum limits* of the `subject`.
2. the natural *minimum limits* of the element will not be violated.

### hmin_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/hmin_size.png">

Overrides the *minimum horizontal limits* of an enclosed element (`subject`):

```c++
hmin_size(width, subject)
```

Effects:
1. The *minimum horizontal limits* of `subject` will be set to the specified
   `width` constrained by the natural *horizontal minimum limits* of the
   `subject`.
2. The natural *horizontal minimum limits* of the element will not be
   violated.

### vmin_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/vmin_size.png">

Overrides the *minimum vertical limits* of an enclosed element (`subject`):

```c++
vmin_size(height, subject)
```

Effects:
1. The *minimum vertical limits* of `subject` will be set to the specified `height`
   constrained by the natural vertical *minimum limits* of the `subject`.
2. The natural vertical *minimum limits* of the element will not be violated.

### max_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/max_size.png">

Overrides the *maximum limits* of an enclosed element (`subject`):

```c++
max_size({ width, height }, subject)
```

Effects:
1. The *maximum limits* of `subject` will be set to the specified `width` and
   `height`, constrained by the natural *maximum limits* of the `subject`.
2. The natural *maximum limits* of the element will not be violated.

### hmax_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/hmax_size.png">

Overrides the *maximum horizontal limits* of an enclosed element (`subject`):

```c++
hmax_size(width, subject)
```

Effects:
1. The *maximum horizontal limits* of `subject` will be set to the specified
   `width` constrained by the natural *maximum horizontal limits* of the
   `subject`.
2. The natural *maximum horizontal limits* of the element will not be
   violated.

### vmax_size

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/vmax_size.png">

Overrides the *maximum vertical limits* of an enclosed element (`subject`):

```c++
vmax_size(height, subject)
```

Effects:
1. The *maximum vertical limits* of `subject` will be set to the specified
   `height` constrained by the natural *maximum vertical limits* of the
   `subject`
2. The natural *maximum vertical limits* of the element will not be violated.

## Stretch Elements

Resizable elements are elements with *minimum limits* that are less than the
*maximum limits* in either x or y dimensions or both. Resizable elements can
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
2. The stretch value has no effect to elements with fixed horizontal size.

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
2. The stretch value has no effect to elements with fixed *vertical limits*.

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
2. The `subject` will assume its *minimum horizontal limits*.
3. $$$ TODO How about vertical effects? $$$

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
2. The `subject` will assume its *minimum vertical limits*.

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
2. The element's *limits* is overridden to account for the additional space.
3. The `margin` does not violate the natural *limits* of the subject.
   `margin` will respect the subject's min-max constraints and resizability.

### left_margin

<img width="40%" height="40%" src="{{ site.url }}/elements/assets/images/left_margin.png">

Adds a margin to the left of an enclosed element (`subject`):

```c++
left_margin(left, subject)
```

Effects:
1. Space is added to the left of the subject with the given parameter.
2. The element's *limits* is overridden to account for the additional space.
3. The `left_margin` does not violate the natural *limits* of the subject.
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
2. The element's *limits* is overridden to account for the additional space.
3. The `right_margin` does not violate the natural *limits* of the subject.
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
2. The element's *limits* is overridden to account for the additional space.
3. The `top_margin` does not violate the natural *limits* of the subject.
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
2. The element's *limits* is overridden to account for the additional space.
3. The `bottom_margin` does not violate the natural *limits* of the subject.
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
2. The element's *limits* is overridden to account for the additional space.
3. The `hmargin` (and variants) does not violate the natural *limits* of the
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
2. The element's *limits* is overridden to account for the additional space.
3. The `vmargin` (and variants) does not violate the natural *limits* of the
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
2. The element's *limits* is overridden to account for the additional space.
3. The `left_top_margin` (and variant) does not violate the natural *limits*
   of the subject. `left_top_margin` (and variant) will respect the subject's
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
2. The element's *limits* is overridden to account for the additional space.
3. The `left_bottom_margin` (and variant) does not violate the natural
   *limits* of the subject. `left_bottom_margin` (and variant) will respect
   the subject's min-max constraints and resizability.

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
2. The element's *limits* is overridden to account for the additional space.
3. The `right_top_margin` (and variant) does not violate the natural *limits*
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
2. The element's *limits* is overridden to account for the additional space.
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
   view, constrained to the subject's natural *limits*.
2. The floating element does not violate the natural *limits* of the subject.

## Tiles and Grids

Tiles are the most useful layout elements, seconded by Grids. Tiles are used
everywhere for composing hierarchical elements in rows and columns, typical
to all GUIs. Grids are similar to tiles, but grids have fixed sizes while
tiles allow elements to fluidly adjust depending on available space. Tiles
are best used for composing UI elements while grids are best for composing
tables.

### Horizontal Tiles and Grids

Horizontal Tiles and Grids are composites that lay out one or more child
elements in a row, respecting the *horizontal limits* requirements of each
element (`limits.min.x` and `limits.max.x`).

<img width="60%" height="60%" src="{{ site.url }}/elements/assets/images/htile.png">

### hgrid

Build a horizontal grid with a fixed number of elements:

```c++
hgrid(coords, e1, e2, e3... eN)
```

`e1` to `eN` are the child elements. `coords` is an external container of
horizontal coordinates, which can either be a plain array of type `float[N]`,
`std::array<float, N>`, where N is the number of items.

Example:

```c++
static float coords[] = { 50, 100, 150, 200 };
//...
hgrid(coords, item1, item2, item3, item4)
```

Requirements:
1. The number of supplied coordinates and elements should match, otherwise,
   compiler error (no matching function for call to 'hgrid').
2. The coordinates assume the first element's relative coordinate to be 0 (it
   is at the leftmost position in the row). The relative coordinate of the
   second element is at index 0, the third at index 1, and so on. The last
   coordinate is the total and final width of the grid.

Effects:
1. Elements `e1` to `eN` are held in a `std::array<element_ptr, N>` managed
   by the horizontal grid element, where `N` is the number of elements.
2. The elements are laid out in a single row, next to each other.
3. The elements are positioned using the supplied coordinates (see
   requirement 2 above).
4. The grid's *minimum vertical limits* is computed as the minimum of the
   child element's *minimum vertical limits*.
5. The grid's *maximum vertical limits* is computed as the maximum of the
   child element's *maximum vertical limits*.
6. The final computed minimum limit is clamped to ensure it is not greater
   than the computed maximum limit. Likewise the computed maximum limit is
   clamped to ensure it is not less than the computed minimum limit.
7. The supplied (horizontal) and computed (vertical) coordinates may violate
   the layout limits of any of the elements.
   1. If the allocated size of a child element is lower than the element's
      *minimum limits* in either dimension, the element will be cropped.
   2. If a child element's *maximum limits* in either dimension is exceeded,
      the element will be aligned to the top-left.

### vgrid_composite

### htile

<img width="60%" height="60%" src="{{ site.url }}/elements/assets/images/htile-stretch2.png">

### Vertical Tiles and Grids

Vertical Tiles and Grids are composites that lay out one or more child
elements in a column, respecting the *vertical limits* requirements of each
element (`limits.min.y` and `limits.max.y`).

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
as possible following each child's *maximum horizontal limits*. Once a row is
full, move to the next row and do the same until the end of the row is
filled. Repeat the procedure until all the elements are laid out. The height
of each row is determined by the *maximum vertical limits* of all the
elements to be laid out in that row. The following graphic depicts a
simplified layout scenario for child elements `a` to `r`.

<img width="60%" height="60%" src="{{ site.url }}/elements/assets/images/flow.png">

The child elements arranged in a `flow` composite are automatically re-flowed
(re-layout) when the view size changes.

### flow_composite

To have elements laid out using `flow`, you need to make a `flow_composite`:

```c++
flow_composite c;
```

The `flow_composite` is basically a `std::vector<element_ptr>` that the
client uses to manage the composite's elements. The lifetime of the
container, `c` is the client's responsibility. You use `flow_composite` just
as you would a `std::vector`, such as `push_back` a child element, `child`.
Just keep in mind that we are dealing with `element_ptr` items:

```c++
c.push_back(share(child));
```

> :point_right: `share` turns an element object into an `element_ptr` held by
> the `std::vector<element_ptr>` in `flow_composite`.

### flow

Once we have a `flow_composite`, we can place its contents in a `flow`
element:

```c++
flow(c)
```

where `c` is the `flow_composite` we populated prior.

-------------------------------------------------------------------------------

*Copyright (c) 2014-2020 Joel de Guzman. All rights reserved.*
*Distributed under the [MIT License](https://opensource.org/licenses/MIT)*