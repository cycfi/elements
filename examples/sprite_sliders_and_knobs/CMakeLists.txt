###############################################################################
#  Copyright (c) 2016-2020 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
set(ELEMENTS_APP_PROJECT "SpriteSlidersAndKnobs")
set(ELEMENTS_APP_TITLE "Sprite Sliders And Knobs")
set(ELEMENTS_APP_COPYRIGHT "Copyright (c) 2016-2020 Joel de Guzman")
set(ELEMENTS_APP_ID "com.cycfi.sprite-sliders-and-knobs")
set(ELEMENTS_APP_VERSION "1.0")

set(ELEMENTS_APP_RESOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/knob_sprites_white_128x128.png
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/slider-white.png
)

include("../../CMakeMain.txt")