/*=============================================================================
   Copyright (c) 2016-2026 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]

   undo_anchor through the four ways an element changes identity: copied,
   moved, assigned and destroyed.
=============================================================================*/
#include "test_support.hpp"
#include <elements/support/undo_anchor.hpp>
#include <string>
#include <utility>

using namespace cycfi::elements;

namespace
{
   // An element of sorts, its anchor not first, so the offset matters
   struct holder
   {
      holder(std::string text_) : text(std::move(text_)), anchor(this) {}

      std::string text;
      undo_anchor<holder> anchor;
      int more = 0;
   };

   // The anchor in a base, copied as part of a derived object
   struct big_holder : holder
   {
      big_holder(std::string text_) : holder(std::move(text_)) {}
      double extra = 0;
   };
}

TEST_CASE("undo_anchor: its reference gives its owner")
{
   holder b{"b"};
   auto ref = b.anchor.ref();
   CHECK(*ref == &b);
   CHECK(b.anchor.ref() == ref);                 // the same, while it lasts
}

TEST_CASE("undo_anchor: a copy has a reference of its own")
{
   holder a{"a"};
   auto ref = a.anchor.ref();
   holder c = a;
   CHECK(*c.anchor.ref() == &c);
   CHECK(c.anchor.ref() != ref);
   CHECK(*ref == &a);                            // the original's tasks stay

   big_holder d{"d"};
   auto dref = d.anchor.ref();
   big_holder e = d;
   CHECK(*e.anchor.ref() == static_cast<holder*>(&e));
   CHECK(*dref == static_cast<holder*>(&d));
}

TEST_CASE("undo_anchor: a move takes the reference to the new place")
{
   auto a = std::make_unique<holder>("a");
   auto ref = a->anchor.ref();
   holder m = std::move(*a);
   CHECK(m.anchor.ref() == ref);
   CHECK(*ref == &m);
   CHECK(*a->anchor.ref() == a.get());           // the moved-from, anew
   a.reset();
   CHECK(*ref == &m);                            // unaffected by its going
}

TEST_CASE("undo_anchor: an assignment ends the tasks of what it replaces")
{
   holder a{"a"};
   holder b{"b"};
   auto aref = a.anchor.ref();
   auto bref = b.anchor.ref();

   a = b;                                        // copy assignment
   CHECK(*aref == nullptr);                      // a's old content: ended
   CHECK(*a.anchor.ref() == &a);
   CHECK(*bref == &b);                           // b's own: untouched

   holder c{"c"};
   auto cref = c.anchor.ref();
   auto aref2 = a.anchor.ref();
   a = std::move(c);                             // move assignment
   CHECK(*aref2 == nullptr);
   CHECK(a.anchor.ref() == cref);                // c's tasks follow to a
   CHECK(*cref == &a);
   CHECK(*c.anchor.ref() == &c);

   a = a;                                        // self assignment: kept
   CHECK(*cref == &a);
}

TEST_CASE("undo_anchor: destruction ends the tasks")
{
   auto a = std::make_unique<holder>("a");
   auto ref = a->anchor.ref();
   a.reset();
   CHECK(*ref == nullptr);
}
