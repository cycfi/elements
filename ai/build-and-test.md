# Build And Test

## AR1.1: Branch and initial notes

Date: 2026-05-31

## Branch

`artist_2026`

## Source baseline

`skia_2024`

## Baseline commit

`4c64f37d19160f28625561f28ce59c5f8a94c9e0`

## Build status

Not run.

## Notes

AR1.1 is a documentation-only setup step. The baseline build is intentionally
deferred to AR1.2.

---

## AR1.2: Baseline build

Date: 2026-05-31

## Branch

`artist_2026`

## Source baseline

`skia_2024`

## Baseline commit

`4c64f37d19160f28625561f28ce59c5f8a94c9e0`

## Platform

macOS / Darwin arm64

## Compiler

AppleClang 21.0.0.21000101

## Build tools

- CMake 3.28.1
- Ninja 1.11.1

## Configure command

```sh
cmake -S . -B build -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_MAKE_PROGRAM=ninja
```

## Configure result

Passed.

Notes:

- The first configure attempt was blocked by sandbox permissions while CMake
  updated Git submodule metadata under `.git/modules`.
- The same configure command succeeded after approval.
- CMake selected the Cocoa host UI and Artist Quartz2D backend on macOS.

## Smallest demo/example target

`EmptyStarter`

Source path:

```text
examples/empty/main.cpp
```

Output:

```text
build/examples/empty/EmptyStarter.app/Contents/MacOS/EmptyStarter
```

## Build command

```sh
cmake --build build --target EmptyStarter
```

## Build result

Passed.

The target produced an arm64 Mach-O executable at:

```text
build/examples/empty/EmptyStarter.app/Contents/MacOS/EmptyStarter
```

## Warnings

The baseline build emits existing AppleClang enum-conversion deprecation
warnings where `canvas::text_halign` and `canvas::text_valign` are combined:

- `lib/src/element/style/check_box.cpp`
- `lib/src/element/style/button.cpp`
- `lib/src/element/style/radio_button.cpp`
- `lib/src/element/style/slider.cpp`
- `lib/src/support/draw_utils.cpp`
- `lib/src/support/text_utils.cpp`
- `lib/src/support/theme.cpp`

These warnings were not fixed in AR1.2 because they are baseline issues and do
not block the migration.

## Demo/example run result

Launched with:

```sh
open build/examples/empty/EmptyStarter.app
```

Result: launched successfully for interactive inspection.

---

## AR2: Artist dependency and backend selection

Date: 2026-05-31

## Branch

`artist_2026`

## Commands run

Read-only inspection commands only:

```sh
find . -path './build' -prune -o -path './cmake-build-debug' -prune -o -name 'CMakeLists.txt' -o -name '*.cmake' -print
rg -n "artist|Artist|skia|Skia|cairo|Cairo|backend|renderer|ELEMENTS_HOST_UI_LIBRARY|ARTIST_|QUARTZ|Quartz|add_subdirectory|FetchContent|ExternalProject|find_package|target_link_libraries" CMakeLists.txt cmake lib examples .gitmodules
git submodule status lib/artist
git -C lib/artist branch --show-current
```

## Result

Passed.

No build or test was run in AR2. The Artist dependency and current backend
selection wiring are documented in `ai/architecture.md`.

---

## AR2 follow-up: Cairo selector wiring

Date: 2026-05-31

## Branch

`artist_2026`

## Commands run

```sh
cmake -S . -B build-cairo-check -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_MAKE_PROGRAM=ninja -DELEMENTS_CAIRO=ON
```

## Result

Expected configure-time failure with a clear message:

```text
ELEMENTS_CAIRO=ON requires a Cairo-capable Artist checkout.
Update lib/artist to artist_2026_dev (or newer).
```

## Notes

- Added `ELEMENTS_CAIRO` option at top-level CMake.
- Added backend mapping from `ELEMENTS_CAIRO` to `ARTIST_CAIRO`.
- Added fail-fast guard to prevent long compile errors when the vendored Artist
  submodule does not yet include Cairo backend files.

---

## AR3: Update Artist dependency to artist_2026_dev

Date: 2026-05-31

## Branch

`artist_2026`

## Submodule change

Path:

```text
lib/artist
```

Before:

```text
08ee27ce79690ca706a8d3252006e609c11b0971
```

After:

```text
e573d53c67bc8b3bbe1468a7c6ff888b3d592767
```

Branch in submodule after update:

```text
artist_2026_dev
```

## Commands run

```sh
git -C lib/artist fetch origin artist_2026_dev
git -C lib/artist checkout artist_2026_dev
cmake -S . -B build-ar3-skia-check -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_MAKE_PROGRAM=ninja
cmake --build build-ar3-skia-check --target EmptyStarter
```

## Result

Passed.

The existing baseline backend path still configures and builds with the updated
Artist dependency.

Configured backend on this macOS baseline check:

```text
Building Artist lib for MacOS with Quartz2D.
```

Target build result:

```text
EmptyStarter built successfully.
```

## Warnings

The same baseline AppleClang enum-conversion warnings remain in Elements style
and support files. No warning cleanup was done in AR3.

---

## AR4: Enable Cairo backend

Date: 2026-05-31

## Branch

`artist_2026`

## Configure command

```sh
cmake -S . -B build-ar4-cairo -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_MAKE_PROGRAM=ninja -DELEMENTS_CAIRO=ON
```

## Configure result

Passed.

Key configure output:

```text
Building Artist lib with Cairo.
```

## Build command

```sh
cmake --build build-ar4-cairo --target EmptyStarter
```

## Build result

Passed after one host integration fix.

## First concrete issue and fix

Issue:

- macOS host file `lib/host/macos/base_view.mm` only handled
  `ARTIST_QUARTZ_2D` and `ARTIST_SKIA`, and failed with
  `#error Unknown Graphics Backend` when `ARTIST_CAIRO` was active.

Fix:

- Added an `ARTIST_CAIRO` branch in `base_view.mm`.
- Added `<cairo.h>` include for the Cairo path.
- Implemented Cairo draw path using:
  - `cairo_quartz_surface_create_for_cg_context` + `cairo_create`
  - `canvas{cairo_ctx}` for Elements rendering
  - current AppKit `CGContext` as the Cairo target

## Run result

Launched successfully with:

```sh
open build-ar4-cairo/examples/empty/EmptyStarter.app
```

## Notes

- Existing baseline enum-conversion warnings are still present.
- No broad host refactor was done; this is a targeted macOS Cairo enablement
  step for `EmptyStarter`.

---

## AR5: Cairo smoke test

Date: 2026-05-31

## Branch

`artist_2026`

## Build commands

```sh
cmake --build build-ar4-cairo --target HelloUniverse Buttons BasicSlidersAndKnobs
cmake --build build-ar4-cairo --target TextAndIcons
```

## Build result

Passed.

Built and linked successfully:

- `HelloUniverse`
- `Buttons`
- `BasicSlidersAndKnobs`
- `TextAndIcons`

## Launch commands

```sh
open build-ar4-cairo/examples/hello_universe/HelloUniverse.app
open build-ar4-cairo/examples/buttons/Buttons.app
open build-ar4-cairo/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
open build-ar4-cairo/examples/text_and_icons/TextAndIcons.app
```

## Launch result

All four apps launched successfully for interactive smoke verification.

## Coverage summary

- Basic text and image/background: `HelloUniverse`
- Buttons and image-backed controls: `Buttons`
- Slider/knob controls: `BasicSlidersAndKnobs`
- Mixed text and icon rendering: `TextAndIcons`

## Notes

- Existing enum-conversion warnings remain (baseline, not AR5-specific).
- AR5 confirms Cairo path compiles and launches across multiple representative
  example categories on macOS.

---

## AR5 follow-up: Match Artist macOS Cairo host hookup

Date: 2026-05-31

## Branch

`artist_2026`

## Context

User reported sluggish Cairo interaction (notably on `BasicSlidersAndKnobs`) and
requested matching the Artist host example hookup.

## Artist reference checked

```text
lib/artist/examples/host/macos/cairo_app.mm
```

Observed pattern in Artist host example:

- Render into `cairo_image_surface_create(CAIRO_FORMAT_ARGB32, pw, ph)`.
- Apply HiDPI scaling with `cairo_surface_set_device_scale(surface, scale, scale)`.
- Draw via `canvas{cairo_ctx}`.
- Flush and wrap pixel buffer with `CGBitmapContextCreate`.
- Create `CGImage`, flip CTM, then `CGContextDrawImage` to current AppKit context.

## Elements change

Updated `lib/host/macos/base_view.mm` `ARTIST_CAIRO` branch to mirror the same
image-surface + CGImage blit flow, replacing direct
`cairo_quartz_surface_create_for_cg_context(...)` path.

## Build command

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar4-cairo --target BasicSlidersAndKnobs -j6
```

## Build result

Passed.

## Run command

```sh
/Users/joel/dev/cycfi/elements/build-ar4-cairo/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app/Contents/MacOS/BasicSlidersAndKnobs
```

## Run result

Process launched successfully for interactive check.

## Notes

- Hookup now matches Artist example behavior on macOS.
- Sluggishness can still remain and likely needs a separate pass focused on
  invalidation frequency, repaint area, and event/animation scheduling.

---

## AR5 follow-up: Crash hardening in Cairo host path

Date: 2026-05-31

## Branch

`artist_2026`

## Context

After the Artist-style hook alignment, user reported a crash.

## Change

Hardened `ARTIST_CAIRO` draw path in `lib/host/macos/base_view.mm`:

- return early on transient zero-size bounds/backing bounds
- guard invalid `scale`
- guard Cairo surface/context creation and status
- guard Cairo pixel buffer/stride
- guard CoreGraphics bitmap/image creation

## Build command

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar4-cairo --target BasicSlidersAndKnobs -j6
```

## Build result

Passed.

## Run command

```sh
/Users/joel/dev/cycfi/elements/build-ar4-cairo/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app/Contents/MacOS/BasicSlidersAndKnobs
```

## Run result

Process launched successfully for interactive check (no immediate crash in this
run).

---

## AR5 follow-up: Revert to Elements-style Cairo hookup

Date: 2026-05-31

## Branch

`artist_2026`

## Context

User reported the Artist-style Cairo image-surface path still crashes and asked
to hold the investigation. The macOS Cairo draw path was reverted to the prior
Elements-style direct Quartz Cairo surface hookup.

## Change

`lib/host/macos/base_view.mm` `ARTIST_CAIRO` branch now uses:

```text
cairo_quartz_surface_create_for_cg_context(...)
```

instead of the Artist example image-surface + CGImage blit path.

## Build command

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar4-cairo --target BasicSlidersAndKnobs -j6
```

## Build result

Passed.

## Run result

Not run after this rollback. User asked to hold for now.

## Notes

- Cairo backend selector remains enabled.
- The rollback only affects the macOS Cairo draw hookup.
- Known issues deferred: sluggish interaction and crash investigation for the
  Artist-style hook path.

---

## Quartz2D verification: BasicSlidersAndKnobs

Date: 2026-05-31

## Branch

`artist_2026`

## Configure command

```sh
cmake -S . -B build-ar5-quartz2d -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_MAKE_PROGRAM=ninja -DELEMENTS_QUARTZ_2D=ON -DELEMENTS_CAIRO=OFF
```

## Configure result

Passed after rerunning with permission to update nested Git submodule metadata.

Key configure output:

```text
Building Artist lib for MacOS with Quartz2D.
```

## Build command

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar5-quartz2d --target BasicSlidersAndKnobs -j6
```

## Build result

Passed.

Existing enum-conversion warnings remain in Elements style/support files.

## Run command

```sh
open /Users/joel/dev/cycfi/elements/build-ar5-quartz2d/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
```

## Run result

Launch command returned successfully.

No newer `BasicSlidersAndKnobs` diagnostic crash report appeared after launch;
the latest reports remained the earlier Cairo crash reports:

```text
BasicSlidersAndKnobs-2026-05-31-100652.ips
BasicSlidersAndKnobs-2026-05-31-100549.ips
```

Process-list verification with `pgrep` was not available in this environment
because `sysmond` was unavailable.

---

## Cairo Release verification: BasicSlidersAndKnobs

Date: 2026-05-31

## Branch

`artist_2026`

## Artist dependency

Pulled `lib/artist` on `artist_2026_dev`.

Before:

```text
e573d53
```

After:

```text
2cf9be5 Fix text_layout::draw on Quartz: add cairo_move_to before cairo_show_glyphs
```

## Configure command

```sh
cmake -S . -B build-ar5-cairo-release -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_MAKE_PROGRAM=ninja -DELEMENTS_CAIRO=ON
```

## Configure result

Passed after rerunning with permission to update nested Git submodule metadata.

Confirmed cache values:

```text
ARTIST_CAIRO:BOOL=ON
CMAKE_BUILD_TYPE:STRING=Release
ELEMENTS_CAIRO:BOOL=ON
```

Key configure output:

```text
Building Artist lib with Cairo.
Enabling LTO for artist
```

## Build command

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar5-cairo-release --target BasicSlidersAndKnobs -j6
```

## Build result

Passed.

Warning:

```text
lib/artist/lib/impl/cairo/font.cpp:143:28: warning: unused function 'make_scaled_font'
```

## Run command

```sh
open /Users/joel/dev/cycfi/elements/build-ar5-cairo-release/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
```

## Run result

Launch command returned successfully.

No newer `BasicSlidersAndKnobs` diagnostic crash report appeared after launch;
the latest reports remained:

```text
BasicSlidersAndKnobs-2026-05-31-100652.ips
BasicSlidersAndKnobs-2026-05-31-100549.ips
```

## Cairo Release verification: TextAndIcons

Date: 2026-05-31

## Branch

`artist_2026`

## Build command

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar5-cairo-release --target TextAndIcons -j6
```

## Build result

Passed.

## Run command

```sh
open /Users/joel/dev/cycfi/elements/build-ar5-cairo-release/examples/text_and_icons/TextAndIcons.app
```

## Run result

Launch command returned successfully.

No `TextAndIcons` diagnostic crash report appeared after launch.

---

## Cairo hook alignment with Elements master

Date: 2026-05-31

## Branch

`artist_2026`

## Reference checked

```text
git show master:lib/host/macos/base_view.mm
```

The Elements `master` macOS Cairo host path uses:

- view bounds width/height
- `NSGraphicsContext.currentContext.CGContext`
- `cairo_quartz_surface_create_for_cg_context(...)`
- `cairo_create(surface)`
- draw into the Cairo context
- destroy surface/context

## Change

Adjusted `lib/host/macos/base_view.mm` `ARTIST_CAIRO` branch to mirror that
scheme more directly. The only adaptation is wrapping `cairo_t*` in
`artist::canvas` before calling the current Artist 2026 Elements draw path.

## Build command

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar5-cairo-release --target BasicSlidersAndKnobs -j6
```

## Build result

Passed.

## Run command

```sh
open /Users/joel/dev/cycfi/elements/build-ar5-cairo-release/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
```

## Run result

Launch command returned successfully.

No newer `BasicSlidersAndKnobs` diagnostic crash report appeared after launch.
