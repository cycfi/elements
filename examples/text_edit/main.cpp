/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background
auto make_bkd()
{
   return port(image{ "dark-bkd.jpg" });
}

std::string const text =
   "We are in the midst of an intergalatic condensing of beauty that will "
   "clear a path toward the planet itself. The quantum leap of rebirth is "
   "now happening worldwide. It is time to take healing to the next level. "
   "Soon there will be a deepening of chi the likes of which the infinite "
   "has never seen. The universe is approaching a tipping point. This "
   "vision quest never ends. Imagine a condensing of what could be. "
   "We can no longer afford to live with stagnation. Suffering is born "
   "in the gap where stardust has been excluded. You must take a stand "
   "against discontinuity.\n\n"

   "Without complexity, one cannot dream. Stagnation is the antithesis of "
   "life-force. Only a seeker of the galaxy may engender this wellspring of hope."
   "Yes, it is possible to eliminate the things that can destroy us, but not "
   "without wellbeing on our side. Where there is delusion, faith cannot thrive. "
   "You may be ruled by desire without realizing it. Do not let it eliminate "
   "the growth of your journey.\n\n"

   "--New-Age Bullshit Generator"
;

std::string const text2 =
   "To traverse the quest is to become one with it.\n\n"

   "You and I are adventurers of the quantum cycle. The goal of expanding wave "
   "functions is to plant the seeds of non-locality rather than pain. "
   "The complexity of the present time seems to demand a redefining of our "
   "bodies if we are going to survive. "
   "We are at a crossroads of will and greed. Humankind has nothing to lose. "
   "Our conversations with other storytellers have led to an evolving of "
   "hyper-sentient consciousness. "
   "If you have never experienced this flow on a cosmic scale, it can be "
   "difficult to self-actualize. Although you may not realize it, you are "
   "ancient. Have you found your vision quest?\n\n"

   "Imagine a deepening of what could be. We are being called to explore the "
   "galaxy itself as an interface between nature and transformation. This "
   "circuit never ends. Entity, look within and recreate yourself. "
   "Eons from now, we warriors will exist like never before as we are reborn "
   "by the universe. We must change ourselves and empower others. The "
   "wellspring of sharing is now happening worldwide. "
   "You will soon be awakened by a power deep within yourself - a power "
   "that is ethereal, dynamic. Astral projection may be the solution to "
   "what's holding you back from an ecstatic oasis of divinity. As you "
   "reflect, you will enter into infinite freedom that transcends understanding.\n\n"
;

auto make_edit_box()
{
   return
      scroller(
         margin(
            { 20, 20, 20, 20 },
            align_left_top(hsize(800, basic_text_box(text2+text)))
         )
      );
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "TextEdit", "com.cycfi.text-edit");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   view_.content(
      make_edit_box(),
      make_bkd()
   );

   _app.run();
   return 0;

}
