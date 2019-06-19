###############################################################################
#  Copyright (c) 2016-2019 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
set(PHOTON_APP_PROJECT "SpriteSlidersAndKnobs")
set(PHOTON_APP_TITLE "Sprite Sliders And Knobs")
set(PHOTON_APP_COPYRIGHT "Copyright (c) 2016-2019 Joel de Guzman")
set(PHOTON_APP_ID "com.cycfi.sprite_sliders_and_knobs")
set(PHOTON_APP_VERSION "1.0")

set(PHOTON_APP_RESOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/knob_sprites_white_128x128.png
   ${CMAKE_CURRENT_SOURCE_DIR}/resources/slider-white.png
)

include("../../CMakeMain.txt")