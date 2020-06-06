---
---
# Context

-------------------------------------------------------------------------------
A majority of element's member functions require a drawing "context". The
`context` class provides information such as the element's `bounds`, a
rectangle that indicates where the element needs to be drawn. The `context`
class inherits from the `basic_context`, a struct which contains a reference
to the `view` &mdash;the main content view inside a window where elements are
placed, and a `canvas`, a drawing surface where the elements are drawn.

<!-- $$$ TODO: links to view and canvas $$$ -->

```c++
struct basic_context
{
   /*...*/

   elements::view&      view;
   elements::canvas&    canvas;
};
```
The `context` class inherits from `basic_context` and additionally includes a
pointer to the "current" element being drawn, a pointer to its parent element
in the elements hierarchy (that which contains it), and the element's `bounds`,
a rectangle that indicates where the element needs to be drawn.

Only one `element` member function takes in a `basic_context` argument,
the `limits` member function (more on that later). Other than that,
all the other `element` member functions take in a `context` argument.
The reason for this is that the at the point where the `limits` is called,
the element's position and parent-child relationships in the elements hierarchy
have not been established yet.

```c++
class context : public basic_context
{
public:

   /*...*/

   element*          element;
   context const*    parent;
   rect              bounds;
};
```

-------------------------------------------------------------------------------

*Copyright (c) 2014-2020 Joel de Guzman. All rights reserved.*
*Distributed under the [MIT License](https://opensource.org/licenses/MIT)*
