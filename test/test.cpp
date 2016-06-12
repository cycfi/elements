#include <photon/window.hpp>
#include <photon/app.hpp>
#include <photon/widget.hpp>

using namespace photon;

auto box = basic(
   [](context const& ctx)
   {
      auto& c = ctx.canvas();

      c.begin_path();
      c.round_rect(ctx.bounds, 4);
      c.fill_color(colors::gold.opacity(0.8));
      c.fill();
   }
);

auto make_vtile()
{
   auto _box = top_margin(
      { 20 },
      hsize(100, box)
   );

   return margin(
      { 20, 0, 20, 20 },
      vtile(
         halign(0.0, _box),
         halign(0.2, _box),
         halign(0.4, _box),
         halign(0.6, _box),
         halign(0.8, _box),
         halign(1.0, _box)
      )
   );
}

auto make_vtile2()
{
   auto _box = top_margin(
      { 20 },
      box
   );

   return margin(
      { 20, 0, 20, 20 },
      vtile(
         vpercent(1.0, _box),
         vpercent(0.5, _box),
         vpercent(0.5, _box),
         vpercent(0.5, _box),
         vpercent(2.0, _box)
      )
   );
}

auto make_vtile_main()
{
   return htile(
      margin({ 10, 10, 10, 10 }, group("Aligns", make_vtile())),
      margin({ 10, 10, 10, 10 }, group("Percentages", make_vtile2()))
   );
}

auto make_htile()
{
   auto _box = left_margin(
      { 20 },
      vsize(100, box)
   );

   return margin(
      { 0, 20, 20, 20 },
      htile(
         valign(0.0, _box),
         valign(0.2, _box),
         valign(0.4, _box),
         valign(0.6, _box),
         valign(0.8, _box),
         valign(1.0, _box)
      )
   );
}

auto make_htile2()
{
   auto _box = left_margin(
      { 20 },
      box
   );

   return margin(
      { 0, 20, 20, 20 },
      htile(
         hpercent(1.0, _box),
         hpercent(0.5, _box),
         hpercent(0.5, _box),
         hpercent(0.5, _box),
         hpercent(2.0, _box)
      )
   );
}

auto make_htile_main()
{
   return htile(
      margin({ 10, 10, 10, 10 }, group("Aligns", make_htile())),
      margin({ 10, 10, 10, 10 }, group("Percentages", make_htile2()))
   );
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

auto make_basic_text()
{
   auto fr = [](auto el)
   {
      return margin({ 10, 10, 10, 10 }, layer(margin({ 10, 5, 10, 5 }, el), frame{}));
   };

   auto eh = [=](char const* txt)
   {
      return fr(halign(0.5,  heading{ txt }));
   };

   auto el = [=](double align, char const* txt)
   {
      return fr(halign(align,  label{ txt }));
   };

   auto icons =
      margin({ 10, 10, 10, 10 },
         htile(
            align_center(icon{ icons::paperclip,      24 }),
            align_center(icon{ icons::arrowright,     24 }),
            align_center(icon{ icons::barchart,       24 }),
            align_center(icon{ icons::batteryfull,    24 }),
            align_center(icon{ icons::car,            24 }),
            align_center(icon{ icons::glass,          24 })
         )
      );

   auto text_box = margin({ 10, 10, 10, 10 }, static_text_box{ text });

   return
      margin(
         { 10, 10, 10, 10 },
         vtile(
            eh("Photon UI"),
            el(1.0, "Hello, World. I am Photon."),
            el(1.0, "A cross-platform, fine-grained, highly modular C++ GUI library."),
            el(0.0, "I used to be called Pica in the mid 90s."),
            el(0.5, "Now, Joel rewrote my code using modern C++14."),
            margin({ 10, 10, 10, 10 }, group("Icons", icons)),
            margin({ 10, 10, 10, 10 }, group("Text Box", text_box))
         )
      );
}

auto make_dropdown_menu()
{
   auto dropdown  = dropdown_menu("Dropdown");

   auto menu =
      layer(
         vtile(
               menu_item("Menu Item 1"),
               menu_item("Menu Item 2"),
               menu_item("Menu Item 3"),
               menu_item_spacer(),
               menu_item("Menu Item 4"),
               menu_item("Menu Item 5")
         ),
         menu_background{}
      );

   dropdown.menu(menu);

   return dropdown;
}

auto make_buttons(window& main_window)
{
   auto bred      = colors::red.opacity(0.4);
   auto bgreen    = colors::green.level(0.7).opacity(0.4);
   auto bblue     = colors::blue.opacity(0.4);
   auto brblue    = colors::royal_blue.opacity(0.4);

   auto mbutton   = button("Momentary Button");
   auto tbutton   = toggle_button("Toggle Button", bred);
   auto lbutton   = ref(latching_button("Latching Button", bgreen));
   auto reset     = button("Clear Latch", icons::unlock, bblue);
   auto note      = button(icons::gear, "Setup", brblue);

   reset.on_click =
      [lbutton, &main_window](bool) mutable
      {
         lbutton.get().state(0);
         main_window.draw();
      };

   return
      group("Buttons",
         margin({ 20, 20, 20, 20 },
            vtile(
               top_margin(20, make_dropdown_menu()),
               top_margin(20, mbutton),
               top_margin(20, tbutton),
               top_margin(20, lbutton),
               top_margin(20, reset),
               top_margin(20, note)
            )
         )
      );
}

auto make_knobs_and_sliders()
{
   return
      group("Basic Sliders and Knobs",
         margin({ 20, 10, 20, 10 },
            htile(
               hpercent(1.5,
                  vsize(80,
                     vtile(
                        top_margin(20, slider{}),
                        top_margin(10, slider{})
                     )
                  )
               ),
               hpercent(0.3, left_margin(20, knob{})),
               hpercent(0.3, left_margin(20, knob{}))
            )
         )
      );
}

auto make_check_boxes()
{
   return
      group("Check boxes",
         margin({ 10, 10, 10, 20},
            vtile(
               top_margin(10, align_left(check_box("Check Box 1"))),
               top_margin(10, align_left(check_box("Check Box 2"))),
               top_margin(10, align_left(check_box("Check Box 3")))
            )
         )
   );
}

auto make_controls(window& main_window)
{
   auto  column1 =
      margin({ 20, 0, 20, 20 },
         vtile(
            top_margin(20, input_box("Text Input Box")),
            top_margin(20, make_check_boxes()),
            top_margin(20, make_knobs_and_sliders()),
            widget{} // empty space
         )
      );

   auto  column2 =
      margin({ 20, 0, 20, 20 },
         vtile(
            top_margin(20, make_buttons(main_window)),
            widget{} // empty space
         )
      );

   return htile(column1, hpercent(0.5, column2));
}


auto make_view_port(canvas&  canvas_)
{
   auto  space = std::make_shared<canvas::image>(canvas_, "./assets/images/space.jpg");

   auto  img =
      size(
         point{ 1920, 1080 }
       , image(space)
      );

   return scroller(img);
}

auto make_edit_box()
{
   return
      scroller(
         margin(
            { 20, 20, 20, 20 },
            align_left(align_top(basic_text_box(text2+text, 800)))
         )
      );
}

int main()
{
   using namespace photon;

   auto&    my_app = make_app<app>();
   window   main_window("Photon", { 1000, 600 }, color{ 62, 91, 102, 255 }, my_app);
   canvas&  canvas_ = main_window.canvas();


   auto     knob_sprites = std::make_shared<canvas::image>(
               canvas_, "./assets/images/knob_sprites_150x150_darker.png");

   auto     slider_img = std::make_shared<canvas::image>(
               canvas_, "./assets/images/slider.png");

   {
      widget_ptr main_widget;

      auto  m_item1_text = "Vertical Tiles";
      auto  m_item2_text = "Horizontal Tiles";
      auto  m_item3_text = "Static Text and Icons";
      auto  m_item4_text = "Controls";
      auto  m_item5_text = "Viewport and Scrollers";
      auto  m_item6_text = "Edit Text";

      auto  m_item1 = ref(menu_item(m_item1_text));
      auto  m_item2 = ref(menu_item(m_item2_text));
      auto  m_item3 = ref(menu_item(m_item3_text));
      auto  m_item4 = ref(menu_item(m_item4_text));
      auto  m_item5 = ref(menu_item(m_item5_text));
      auto  m_item6 = ref(menu_item(m_item6_text));

      {
         auto menu =
            ref(
               layer(
                  vtile(
                     m_item1,
                     m_item2,
                     m_item3,
                     m_item4,
                     m_item5,
                     m_item6
                  ),
                  menu_background{}
               )
            );

         auto content =
            ref(
               deck(
                  make_vtile_main(),
                  make_htile_main(),
                  make_basic_text(),
                  make_controls(main_window),
                  make_view_port(canvas_),
                  make_edit_box()
               )
            )
         ;

         auto  title = ref(label(m_item1_text));
         content.get().select(5);

         m_item1.get().on_click =
            [title, m_item1_text, content, &main_window]() mutable
            {
               title.get().text(m_item1_text);
               content.get().select(5);
               main_window.draw(true);
            };

         m_item2.get().on_click =
            [title, m_item2_text, content, &main_window]() mutable
            {
               title.get().text(m_item2_text);
               content.get().select(4);
               main_window.draw(true);
            };

         m_item3.get().on_click =
            [title, m_item3_text, content, &main_window]() mutable
            {
               title.get().text(m_item3_text);
               content.get().select(3);
               main_window.draw(true);
            };

         m_item4.get().on_click =
            [title, m_item4_text, content, &main_window]() mutable
            {
               title.get().text(m_item4_text);
               content.get().select(2);
               main_window.draw(true);
            };

         m_item5.get().on_click =
            [title, m_item5_text, content, &main_window]() mutable
            {
               title.get().text(m_item5_text);
               content.get().select(1);
               main_window.draw(true);
            };

         m_item6.get().on_click =
            [title, m_item6_text, content, &main_window]() mutable
            {
               title.get().text(m_item6_text);
               content.get().select(0);
               main_window.draw(true);
            };

         auto  caret = left_margin(5, icon(icons::caretdown));
         auto  dropdown = basic_dropdown_menu(htile(title, caret), htile(title, caret));

         dropdown.menu(menu);

         auto  main_pane = ref(pane(dropdown, content));
         main_widget = new_(margin({ 20, 20, 20, 20 }, main_pane));
      }




      {
         auto p =
               margin(
                  { 20, 20, 20, 20 },
                  layer(
                     margin(
                        { 20, 20, 20, 20 },
                        sprite(knob_sprites, { 150, 150 })
                     ),
                     panel{}
                  )
               )
            ;

         //main_widget = new_(p);
      }

      {
         auto sl = margin(
            { 20, 20, 20, 20 },
            layer(
               margin(
                  { 20, 20, 20, 20 },
                  image_knob{ knob_sprites, 150, 100, true }
               ),
               margin(
                  { 20, 20, 20, 20 },
                  frame{}
               ),
               panel{}
            )
         );

         //main_widget = new_(std::move(sl));
      }

      {
         auto sl = margin(
            { 20, 20, 20, 20 },
            layer(
               margin(
                  { 20, 20, 20, 20 },
                  image_slider{ slider_img, 0.15, 1.5, { 1.0, 1.15 } }
               ),
               panel{}
            )
         );

         //main_widget = new_(std::move(sl));
      }


      main_window.content.elements.push_back(main_widget);

      //main_window.content.elements.push_back(new_(floating({ 50, 50, 100, 100 }, knob{})));

      my_app.run();
   }

   return 0;
}
