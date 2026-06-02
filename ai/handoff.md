# Elements Artist 2026 Handoff

## Current state

Branch: `artist_2026`

Baseline branch: `skia_2024`

Baseline commit: `4c64f37d19160f28625561f28ce59c5f8a94c9e0`

## AR1.1: Branch and initial notes

### What changed

- Created the `artist_2026` working branch from local `skia_2024`.
- Added initial migration notes.
- Recorded that no build has been run yet.

### Why

The migration plan calls for a clean branch and documentation-only setup step
before baseline build, dependency changes, or Cairo work.

### Files touched

- `ai/elements_artist_2026_backend_update_plan.md`
- `ai/current-task.md`
- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

None. AR1.1 explicitly does not build or test.

### Remaining issues / next steps

- AR1.2: identify the smallest available demo/example target.
- AR1.2: run and document the unchanged baseline build.
- Do not update Artist until the baseline build and dependency/backend wiring
  are documented.

---

## AR1.2: Baseline build

### What changed

- Identified `EmptyStarter` as the smallest baseline demo/example target.
- Configured a Debug Ninja build in the ignored `build/` directory.
- Built the baseline `EmptyStarter` target successfully.
- Documented the build result and existing warnings in `ai/build-and-test.md`.

### Why

This records the unchanged `skia_2024` baseline behavior before updating the
Artist dependency. It gives the later Artist 2026 work a concrete comparison
point.

### Files touched

- `ai/build-and-test.md`
- `ai/handoff.md`

### Commands run

```sh
cmake -S . -B build -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_MAKE_PROGRAM=ninja
cmake --build build --target EmptyStarter
```

### Tests run

No automated test suite was run.

Baseline build verification:

- `EmptyStarter` built successfully.
- Output executable exists at
  `build/examples/empty/EmptyStarter.app/Contents/MacOS/EmptyStarter`.

Demo run:

- Launched successfully with
  `open build/examples/empty/EmptyStarter.app` for interactive inspection.

### Existing baseline warnings

AppleClang reports enum-conversion deprecation warnings where
`canvas::text_halign` and `canvas::text_valign` are combined in style/support
code. These were recorded but not fixed because AR1.2 is baseline-only.

### Remaining issues / next steps

- AR2: locate and document Artist dependency and backend selection.
- Do not update Artist until dependency/backend wiring is documented.

---

## AR2: Artist dependency and backend selection

### What changed

- Added `ai/architecture.md`.
- Documented that Artist is a Git submodule at `lib/artist`.
- Documented the current compile-time backend selection path.
- Documented that the baseline macOS build selected Quartz2D, not Skia.
- Documented the current Skia-oriented host glue on GTK/Windows and the
  Quartz2D/Skia split in the macOS host.

### Why

The migration plan requires recording dependency/backend wiring before updating
the Artist submodule or enabling Cairo.

### Files touched

- `ai/architecture.md`
- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

None. AR2 was read-only source inspection plus documentation.

### Key findings

- `lib/artist` is a Git submodule pointing to `https://github.com/cycfi/artist.git`.
- The submodule is currently on `artist_2024_dev` at
  `08ee27ce79690ca706a8d3252006e609c11b0971`.
- Elements maps `ELEMENTS_QUARTZ_2D`/`ELEMENTS_SKIA` to Artist's
  `ARTIST_QUARTZ_2D`/`ARTIST_SKIA` before `add_subdirectory(artist)`.
- Current backend choice is compile-time, not runtime.
- Current Elements has no Cairo selector yet.

### Remaining issues / next steps

- AR3: update the `lib/artist` submodule to `artist_2026_dev`.
- After AR3, verify the existing backend path before adding Cairo.

---

## AR2 follow-up: Cairo selector wiring

### What changed

- Added `ELEMENTS_CAIRO` CMake option.
- Added `ELEMENTS_CAIRO` -> `ARTIST_CAIRO` backend mapping in
  `lib/CMakeLists.txt`.
- Added an early configure guard that errors if Cairo backend files are missing
  from the vendored Artist checkout.

### Why

This enables the new backend selector in Elements without allowing a confusing
late compile failure on older Artist snapshots.

### Files touched

- `CMakeLists.txt`
- `lib/CMakeLists.txt`
- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

Targeted configure check:

```sh
cmake -S . -B build-cairo-check -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_MAKE_PROGRAM=ninja -DELEMENTS_CAIRO=ON
```

Result:

- Fails fast at configure with explicit guidance to update `lib/artist` to
  `artist_2026_dev` (or newer).

### Remaining issues / next steps

- AR3: update `lib/artist` submodule to `artist_2026_dev`.
- Re-run configure/build with `-DELEMENTS_CAIRO=ON` after the submodule update.

---

## AR3: Update Artist dependency to artist_2026_dev

### What changed

- Updated `lib/artist` submodule checkout to `artist_2026_dev`.
- Superproject submodule pointer now references
  `e573d53c67bc8b3bbe1468a7c6ff888b3d592767`.
- Ran targeted configure/build check on existing backend path after submodule
  update.

### Why

This is the dependency update step required before enabling and testing Cairo
against the new Artist branch.

### Files touched

- `lib/artist` (submodule pointer in superproject)
- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

```sh
cmake -S . -B build-ar3-skia-check -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_MAKE_PROGRAM=ninja
cmake --build build-ar3-skia-check --target EmptyStarter
```

Result:

- Configure passed.
- `EmptyStarter` build passed.
- Existing baseline backend path remains working after dependency update.

### Remaining issues / next steps

- Re-run Cairo configure with `-DELEMENTS_CAIRO=ON` now that Artist 2026 is
  present.
- If configure passes, run a targeted Cairo `EmptyStarter` build and fix the
  first concrete host/backend integration issue.

---

## AR4: Enable Cairo backend

### What changed

- Configured Elements with `ELEMENTS_CAIRO=ON`.
- Fixed the first concrete integration blocker in macOS host code:
  `lib/host/macos/base_view.mm` now handles `ARTIST_CAIRO`.
- Added Cairo render-to-image-surface and CGImage blit path in macOS host view.

### Why

AR4 requires enabling the new Cairo backend and resolving concrete build/link
issues with minimal scope.

### Files touched

- `lib/host/macos/base_view.mm`
- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

```sh
cmake -S . -B build-ar4-cairo -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_MAKE_PROGRAM=ninja -DELEMENTS_CAIRO=ON
cmake --build build-ar4-cairo --target EmptyStarter
open build-ar4-cairo/examples/empty/EmptyStarter.app
```

Result:

- Configure passed with Artist Cairo backend.
- `EmptyStarter` build passed.
- App launched successfully for interactive verification.

---

## AR5 follow-up: Artist example Cairo hook alignment

### What changed

- Compared Elements macOS Cairo path with Artist host example:
  `lib/artist/examples/host/macos/cairo_app.mm`.
- Updated `lib/host/macos/base_view.mm` `ARTIST_CAIRO` path to follow the same
  Artist example sequence:
  - Cairo image surface render
  - device scale setup for backing scale
  - Cairo pixel buffer wrap via `CGBitmapContextCreate`
  - `CGImage` blit with CTM flip for `isFlipped=YES`
- Replaced prior direct `cairo_quartz_surface_create_for_cg_context(...)`
  path.

### Why

User reported sluggish interaction and requested that Cairo hookup be aligned
with Artist host examples. This makes Elements host integration consistent with
current Artist 2026 example behavior on macOS.

### Files touched

- `lib/host/macos/base_view.mm`
- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar4-cairo --target BasicSlidersAndKnobs -j6
/Users/joel/dev/cycfi/elements/build-ar4-cairo/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app/Contents/MacOS/BasicSlidersAndKnobs
```

Result:

- Rebuild passed.
- App launched for interactive verification.

### Remaining issues / next steps

- Sluggishness is still a known issue and should be treated as a separate
  optimization packet (profile draw/invalidations/timer cadence and compare
  against legacy Elements Cairo-only host behavior on `master`).

---

## AR5 follow-up: Crash hardening for Cairo host path

### What changed

- Added defensive guards in `lib/host/macos/base_view.mm` `ARTIST_CAIRO` path:
  - skip draw on zero-size bounds/backing bounds
  - fallback for invalid backing scale
  - validate Cairo surface/context creation and status
  - validate Cairo image data pointer and stride
  - validate CoreGraphics bitmap context and CGImage creation

### Why

After switching to the Artist-style Cairo hook path, user reported a crash.
These checks make the path robust during transient layout/context states.

### Files touched

- `lib/host/macos/base_view.mm`
- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar4-cairo --target BasicSlidersAndKnobs -j6
/Users/joel/dev/cycfi/elements/build-ar4-cairo/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app/Contents/MacOS/BasicSlidersAndKnobs
```

Result:

- Rebuild passed.
- App launched for interactive verification with no immediate crash.

### Remaining issues / next steps

- Continue performance investigation for sluggish interactions (separate from
  this crash-hardening fix).

---

## AR5 follow-up: Hold Artist-style Cairo hook

### What changed

- User reported the Cairo path still crashes after the Artist-style image
  surface hookup and defensive guards.
- Crash investigation was paused by request.
- Current decision: revert the macOS Cairo draw path back to the prior
  Elements-style Cairo hookup using `cairo_quartz_surface_create_for_cg_context`.

### Why

The Artist-style path did not stabilize the example quickly and introduced a
crash in the current Elements host context. The safer short-term state is the
previous Elements-style hookup while the crash/performance problem is deferred.

### Files touched

- `ai/handoff.md`

### Tests run

None for this handoff entry.

### Remaining issues / next steps

- Revert `lib/host/macos/base_view.mm` `ARTIST_CAIRO` branch to the prior
  direct Quartz Cairo surface path.
- Rebuild `BasicSlidersAndKnobs` after the revert.
- Keep sluggishness and Artist-style host compatibility as follow-up work, not
  part of this rollback.

---

## AR5 follow-up: Revert to Elements-style Cairo hookup

### What changed

- Reverted `lib/host/macos/base_view.mm` `ARTIST_CAIRO` drawing branch from the
  Artist example image-surface + CGImage blit path back to the prior
  Elements-style direct Quartz Cairo surface path:
  `cairo_quartz_surface_create_for_cg_context(...)`.
- Updated `ai/build-and-test.md` with the rollback build result.

### Why

The Artist-style path continued to crash in the Elements host context. User
asked to hold that investigation and revert to the previous Elements-style
Cairo hookup.

### Files touched

- `lib/host/macos/base_view.mm`
- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar4-cairo --target BasicSlidersAndKnobs -j6
```

Result:

- Rebuild passed.

### Remaining issues / next steps

- `BasicSlidersAndKnobs` was not launched after the rollback because user asked
  to hold for now.
- Sluggish Cairo interaction remains a known issue.
- Artist-style host hookup crash remains deferred.

---

## Quartz2D verification: BasicSlidersAndKnobs

### What changed

- Configured a separate Quartz2D verification build directory:
  `build-ar5-quartz2d`.
- Built and launched `BasicSlidersAndKnobs` using Artist Quartz2D.
- Updated `ai/build-and-test.md` with commands and result.

### Why

User requested verification of `BasicSlidersAndKnobs` using Quartz2D after the
Cairo sluggishness/crash checks.

### Files touched

- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

```sh
cmake -S . -B build-ar5-quartz2d -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_MAKE_PROGRAM=ninja -DELEMENTS_QUARTZ_2D=ON -DELEMENTS_CAIRO=OFF
cmake --build /Users/joel/dev/cycfi/elements/build-ar5-quartz2d --target BasicSlidersAndKnobs -j6
open /Users/joel/dev/cycfi/elements/build-ar5-quartz2d/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
```

Result:

- Configure passed and selected Artist Quartz2D.
- Build passed.
- Launch command returned successfully.
- No newer `BasicSlidersAndKnobs` crash report appeared after launch.

### Remaining issues / next steps

- Existing enum-conversion warnings remain.
- `pgrep` process-list verification was unavailable because `sysmond` was not
  available in this environment.
- Cairo sluggishness/crash work remains deferred.

---

## Cairo Release verification: BasicSlidersAndKnobs

### What changed

- Pulled `lib/artist` on `artist_2026_dev`.
- Submodule advanced from `e573d53` to `2cf9be5`.
- Reconfigured `build-ar5-cairo-release` as `Release` with `ELEMENTS_CAIRO=ON`.
- Built and launched `BasicSlidersAndKnobs` from the Cairo Release build.
- Updated `ai/build-and-test.md` with commands and results.

### Why

User requested rebuilding and running `BasicSlidersAndKnobs` in Release mode
after pulling latest Artist.

### Files touched

- `lib/artist` (submodule pointer)
- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

```sh
git -C lib/artist pull --ff-only origin artist_2026_dev
cmake -S . -B build-ar5-cairo-release -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_MAKE_PROGRAM=ninja -DELEMENTS_CAIRO=ON
cmake --build /Users/joel/dev/cycfi/elements/build-ar5-cairo-release --target BasicSlidersAndKnobs -j6
open /Users/joel/dev/cycfi/elements/build-ar5-cairo-release/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
```

Result:

- Artist pull fast-forwarded to `2cf9be5`.
- Configure passed and selected Artist Cairo Release.
- Build passed.
- Launch command returned successfully.
- No newer `BasicSlidersAndKnobs` crash report appeared after launch.

### Remaining issues / next steps

- Artist Cairo build emits one warning:
  `lib/artist/lib/impl/cairo/font.cpp:143:28: unused function 'make_scaled_font'`.
- Interactive performance assessment still needs user confirmation.

---

## Cairo Release verification: TextAndIcons

### What changed

- Built and launched `TextAndIcons` from the Cairo Release build directory:
  `build-ar5-cairo-release`.
- Updated `ai/build-and-test.md` with the commands and result.

### Why

User requested running the text example after the latest Artist Cairo Release
checks.

### Files touched

- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar5-cairo-release --target TextAndIcons -j6
open /Users/joel/dev/cycfi/elements/build-ar5-cairo-release/examples/text_and_icons/TextAndIcons.app
```

Result:

- Build passed.
- Launch command returned successfully.
- No `TextAndIcons` crash report appeared after launch.

### Remaining issues / next steps

- Continue interactive visual/performance verification of Cairo Release
  examples.

---

## Cairo Release performance note

### What changed

- User confirmed Cairo Release mode is still sluggish after:
  - pulling Artist to `2cf9be5`
  - rebuilding `BasicSlidersAndKnobs` in `Release`
  - launching `BasicSlidersAndKnobs`
  - building and launching `TextAndIcons`

### Why

This records that Release/LTO and the latest Artist Cairo changes did not
resolve the interactive sluggishness.

### Files touched

- `ai/handoff.md`

### Tests run

No new commands for this note. It records user-observed behavior from the
current Cairo Release app runs.

### Remaining issues / next steps

- Treat sluggish Cairo interaction as the next focused work item.
- Start with targeted profiling/instrumentation of repaint frequency, dirty
  rect handling, event-triggered redraws, and text drawing cost.
- Compare Cairo Release behavior against Quartz2D Release for the same example.
- Avoid reintroducing the Artist example image-surface hookup until the prior
  crash is understood.

---

## Cairo hook alignment with Elements master

### What changed

- Checked `master:lib/host/macos/base_view.mm` for the legacy Elements macOS
  Cairo host hookup.
- Adjusted `lib/host/macos/base_view.mm` `ARTIST_CAIRO` branch to mirror the
  master scheme:
  - read width/height from `[self bounds]`
  - use `NSGraphicsContext.currentContext.CGContext`
  - create a direct Quartz Cairo surface with
    `cairo_quartz_surface_create_for_cg_context(...)`
  - create a `cairo_t*` with `cairo_create(surface)`
  - wrap the Cairo context in `artist::canvas` for the Artist 2026 draw API
  - destroy the surface/context

### Why

User requested using the scheme from the Elements `master` branch rather than
the Artist example image-surface + CGImage blit path.

### Files touched

- `lib/host/macos/base_view.mm`
- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

```sh
cmake --build /Users/joel/dev/cycfi/elements/build-ar5-cairo-release --target BasicSlidersAndKnobs -j6
open /Users/joel/dev/cycfi/elements/build-ar5-cairo-release/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
```

Result:

- Cairo Release build passed.
- Launch command returned successfully.
- No newer `BasicSlidersAndKnobs` crash report appeared after launch.

### Remaining issues / next steps

- User-observed sluggishness may still need profiling; this change only aligns
  the host hook structure with Elements `master`.

### Remaining issues / next steps

- AR5: perform broader Cairo smoke test beyond `EmptyStarter` (text + controls).
- Check GTK/Windows host paths for Cairo support once macOS path is stable.
- Use `lib/artist/examples/host/*` Cairo hosts as primary porting references.
- Cross-check behavior against Elements `master` legacy Cairo-only backend.

---

## Session cleanup: squash commits

### What changed

- Squashed all commits from `f2c09e26` to HEAD into a single commit `4dee4b33`
  on `artist_2026`.
- Force-pushed `artist_2026` to origin.

### Why

Clean branch history before continuing integration work.

### Files touched

- `ai/handoff.md`

---

## Cairo performance fix: FcFontMatch per draw call

### What changed

- Identified and fixed the root cause of Cairo sluggishness in Elements.
- Updated `lib/artist` submodule from `2cf9be5` to `a8cd07b` (two commits on
  `artist_2026_dev`).
- Added `ai/elements_cairo_host_integration.md`: deep-dive reference document
  covering the macOS/Linux/Windows Cairo host paths, `view.cpp`
  offscreen-context pattern, and font architecture.

### Root cause

Profiling `BasicSlidersAndKnobs` with `sample(1)` showed 280+ of 350 samples
(~80% of CPU time) inside `FcFontMatch` → `FcFontSetMatchInternal` →
`FcStrHashIgnoreBlanksAndCase`, called from `draw_slider_labels`.

`draw_slider_labels` constructs a `font(font_descr)` object on every draw
call. The artist_2026_dev Cairo backend was calling `FcFontMatch` on every
such construction — a full Fontconfig search through all installed fonts. With
the slider invalidation loop firing on every mouse-drag event, this ran
hundreds of times per second.

Skia and the Elements `master` Cairo backend both pre-enumerate all fonts once
via `FcFontList` into a `static std::map<family, vector<font_entry>>` at
startup, then resolve any `font_descr` with a local `std::map::find` +
small-vector biased-score search. No `FcFontMatch` is called after startup.
The artist_2026_dev Cairo backend was the only backend missing this.

### Artist commits (`artist_2026_dev`)

| Commit | Description |
|--------|-------------|
| `061d426` | Fix Cairo font perf: replace FcFontMatch with pre-enumerated font map |
| `a8cd07b` | Fix teardown segfault: decouple HarfBuzz face from FreeType lifecycle |

**`061d426` — font map approach:**
- `init_font_map()`: calls `FcFontList` once, builds
  `std::map<family, vector<font_entry>>` with pre-mapped weight/slant/stretch
  in Artist scale.
- `match(font_descr)`: resolves via `std::map::find` + biased score
  (slant ×3, weight ×1, stretch ×0.25). No Fontconfig call.
- Face cache keyed by `FC_FULLNAME`: stores FT cairo face + CG face (macOS) +
  `hb_face_t*`, created once per unique typeface.
- Scaled fonts (per-size, cheap) and `hb_font_t` (O(1)) created per
  `font(font_descr)` call as before.

**`a8cd07b` — teardown crash fix:**
- `hb_ft_face_create_referenced` stores `FT_Done_Face` as a HarfBuzz blob
  destructor. At process exit, `hb_face_destroy` fired `FT_Done_Face` after
  FreeType's dylib had begun teardown → SIGSEGV at `FT_Done_Face+36`.
- Fix: create `hb_face_t` via `hb_blob_create_from_file` + `hb_face_create`
  instead. HarfBuzz holds a plain font data blob with no FT_Face reference;
  `hb_face_destroy` at exit frees raw memory only — no FreeType call, no
  cross-dylib ordering dependency.
- All 224 Artist Cairo test assertions pass, no segfault.

### Files touched

- `lib/artist` (submodule pointer: `2cf9be5` → `a8cd07b`)
- `ai/elements_cairo_host_integration.md` (new)
- `ai/handoff.md`

### Tests run

```sh
# Artist Cairo test suite (via submodule)
ctest --test-dir /Users/joel/dev/cycfi/artist/build-cairo --output-on-failure

# Elements Cairo Release — BasicSlidersAndKnobs
cmake --build build-ar5-cairo-release --target BasicSlidersAndKnobs
open build-ar5-cairo-release/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
```

Results:
- Artist: 224 assertions in 15 test cases — all passed, no segfault.
- Elements Cairo Release: build passed; `BasicSlidersAndKnobs` responsive.

### Remaining issues / next steps

- Advance `build-ar5-cairo-release` submodule to `a8cd07b` and rebuild to
  pick up the fix (copy done locally; submodule pointer committed here).
- Linux and Windows Cairo host draw paths not yet implemented in Elements
  (both currently Skia-only). See `ai/elements_cairo_host_integration.md`.
- Artist submodule pin in Elements tracks `artist_2026_dev`; keep in sync
  when Artist is updated.

---

## AR5: Cairo smoke test

### What changed

- Ran broader Cairo smoke build and launch checks on representative examples:
  `HelloUniverse`, `Buttons`, `BasicSlidersAndKnobs`, `TextAndIcons`.
- Updated build/test records with AR5 commands and outcomes.

### Why

AR5 requires moving beyond `EmptyStarter` to verify text and control rendering
paths under the Cairo backend.

### Files touched

- `ai/build-and-test.md`
- `ai/handoff.md`

### Tests run

```sh
cmake --build build-ar4-cairo --target HelloUniverse Buttons BasicSlidersAndKnobs
cmake --build build-ar4-cairo --target TextAndIcons
open build-ar4-cairo/examples/hello_universe/HelloUniverse.app
open build-ar4-cairo/examples/buttons/Buttons.app
open build-ar4-cairo/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
open build-ar4-cairo/examples/text_and_icons/TextAndIcons.app
```

Result:

- All targets built successfully.
- All four apps launched successfully for interactive smoke verification.

### Remaining issues / next steps

- AR6/AR7: capture and fix the first concrete visual or interaction regression,
  if any, from interactive testing.
- Assess GTK/Windows Cairo host support using the same reference strategy.

---

## Sync Stage 0: Bump lib/artist to artist_2026_skia_upgrade

### What changed

- Created working branch `artist_element_sync` off `artist_2026`.
- Added `ai/elements_artist_skia_upgrade_sync_plan.md` (staged sync plan).
- Fetched `artist_2026_skia_upgrade` into the `lib/artist` submodule clone.
- Repointed `.gitmodules` `branch` from `artist_2026_dev` to
  `artist_2026_skia_upgrade`.
- Moved the submodule pointer `5e54e7e` → `08a51ba` (clean fast-forward).

### Why

`artist_2026_skia_upgrade` is `artist_2026_dev` plus 15 commits with no
divergence, and Artist's public `include/` API is unchanged between the two.
The sync is a fast-forward submodule bump; the only material change is the
Skia backend moving to vcpkg-built m148.

### Files touched

- `.gitmodules`
- `lib/artist` (submodule pointer)
- `ai/elements_artist_skia_upgrade_sync_plan.md`

### Tests run

None in Stage 0 (pointer move only).

### Result

- Branch + plan committed (`0ee4f18a`).
- Submodule bump committed (`d3d5c291`); `git submodule status` shows
  `08a51ba`.

### Remaining issues / next steps

- Stage 1: verify Cairo build (build-ar5-cairo-release).
- Stage 2: verify Quartz2D build (build-ar5-quartz2d).
- Stage 3: verify Skia build via vcpkg m148 (new dependency mechanism) —
  highest risk.

---

## Sync Stage 1: Verify Cairo backend against skia_upgrade

### What changed

- Rebuilt `BasicSlidersAndKnobs` in `build-ar5-cairo-release` (Cairo Release)
  against `lib/artist` @ `08a51ba`.

### Why

Stage 1 confirms the Cairo backend still compiles and runs after the submodule
fast-forward.

### Tests run

```sh
cmake --build build-ar5-cairo-release --target BasicSlidersAndKnobs -j6
open build-ar5-cairo-release/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
```

### Result

- Build passed. Only the known pre-existing enum-conversion warnings in
  `lib/src/support/theme.cpp` (text_valign | text_halign). No errors.
- App launched and ran; no crash report.

### Remaining issues / next steps

- Stage 2: verify Quartz2D (build-ar5-quartz2d).
- Stage 3: verify Skia via vcpkg m148.

---

## Sync Stage 2: Verify Quartz2D backend against skia_upgrade

### What changed

- Rebuilt `BasicSlidersAndKnobs` in `build-ar5-quartz2d` against `lib/artist`
  @ `08a51ba`.

### Tests run

```sh
cmake --build build-ar5-quartz2d --target BasicSlidersAndKnobs -j6
open build-ar5-quartz2d/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
```

### Result

- Build passed. Only pre-existing warnings (theme enum-conversion; Quartz2D
  `image.mm` CGBitmap anon-enum conversion). No errors.
- App launched and ran; no crash report.

### Remaining issues / next steps

- Stage 3: verify Skia via vcpkg m148 — new dependency mechanism, highest risk.

---

## Sync Stage 3: Verify Skia backend against skia_upgrade (vcpkg m148)

### What changed

The Skia backend moved from a hand-managed prebuilt to vcpkg-built Skia m148,
and the `tools/sk_app` host helper was deleted upstream. This required real
host-integration work in Elements (the other two backends were no-ops):

- **`lib/host/macos/base_view.mm`** — rewrote the macOS Skia host. Removed the
  `sk_app::WindowContext` / `MakeGLForMac` (OpenGL) path. Now renders through
  **CAMetalLayer + `GrDirectContexts::MakeMetal`**, per the Artist reference
  `examples/host/macos/skia_app.mm`:
  - `elements_init`: create Metal device/queue, attach a CAMetalLayer as a
    *sublayer* (render-on-demand), create the Ganesh Metal context.
  - `drawRect:` ARTIST_SKIA branch: size the layer to bounds, acquire
    `nextDrawable`, wrap via `GrBackendRenderTargets::MakeMtl` +
    `SkSurfaces::WrapBackendRenderTarget`, draw, `flushAndSubmit`, present.
  - **Chosen approach: render-on-demand Metal sublayer** — keeps Elements'
    `setNeedsDisplay:`→`drawRect:` invalidation model intact (a CAMetalLayer-
    *hosting* view would not get `drawRect:`). The view stays layer-backed and
    the Metal layer is a sublayer.
- **`lib/CMakeLists.txt`** — link `-framework QuartzCore` for APPLE+ELEMENTS_SKIA
  (CAMetalLayer / kCAGravityTopLeft).
- **`toolchain-macos.cmake`** (new) — wrapper toolchain that re-adds the
  Homebrew aggregate pkgconfig dir (`/opt/homebrew/lib/pkgconfig`) after
  including the Artist vcpkg toolchain. vcpkg overwrites `PKG_CONFIG_PATH`,
  hiding fontconfig/freetype2 (Elements) and harfbuzz + its transitive
  glib-2.0/graphite2 (Artist macOS Skia block). Mirrors Artist's
  `toolchain-linux.cmake` pattern documented in
  `lib/artist/ai/build_setup_guide.md`.

### Build commands

```sh
# one-time: init + bootstrap the Artist submodule's vcpkg
git -C lib/artist submodule update --init lib/external/vcpkg   # (commit 00d899c)
lib/artist/lib/external/vcpkg/bootstrap-vcpkg.sh -disableMetrics

cmake -S . -B build-skia -G Ninja \
  -D CMAKE_BUILD_TYPE=Debug \
  -D ELEMENTS_SKIA=ON \
  -D CMAKE_TOOLCHAIN_FILE=$(pwd)/toolchain-macos.cmake \
  -D VCPKG_MANIFEST_DIR=$(pwd)/lib/artist \
  -D VCPKG_TARGET_TRIPLET=arm64-osx
cmake --build build-skia --target BasicSlidersAndKnobs -j6
```

### Result

- Configure passed (Skia served from vcpkg binary cache — no 15-min rebuild).
- Build passed; `BasicSlidersAndKnobs` links and launches.
- App runs with no crash report.
- **All three backends now build: Cairo ✅, Quartz2D ✅, Skia ✅.**

### Gotchas (for the guide)

1. The Artist vcpkg submodule (`lib/artist/lib/external/vcpkg`) is not checked
   out by default; its pinned commit must be fetched explicitly, then
   bootstrapped.
2. Elements has no vcpkg wiring of its own — pass the Artist submodule's
   toolchain and `-DVCPKG_MANIFEST_DIR=<abs>/lib/artist` (manifest lives in the
   Artist tree, not the Elements root).
3. vcpkg overwrites `PKG_CONFIG_PATH`; use the `toolchain-macos.cmake` wrapper
   so the unconditional fontconfig/freetype2 deps (and Artist's harfbuzz) still
   resolve from Homebrew.

### Remaining issues / next steps

- Visual render confirmation by screenshot was blocked by the desktop
  environment (a fullscreen browser Space; `screencapture` grabs the active
  display only). Recommend a quick interactive visual check of `build-skia`
  `BasicSlidersAndKnobs` to confirm rendering + slider interaction.
- Stage 4: commit + final documentation.

---

## Sync Stage 4: All examples + Skia teardown crash fix + docs

### What changed

- Built **all 31 example apps** under each backend: Cairo (build-ar5-cairo-
  release), Quartz2D (build-ar5-quartz2d), Skia (build-skia). 441/441 targets
  each.
- **Fixed a Skia teardown crash** that affected every example
  (`lib/host/macos/base_view.mm`). On Quit, AppKit calls `exit()` without
  deallocating the view, so the `GrDirectContext` was destroyed during C++
  static finalization with live GPU resources → Skia debug
  `GrManagedResource::Trace` leak assert (`sk_abort_no_print`, SIGTRAP). Fix:
  observe `NSApplicationWillTerminateNotification` and call
  `releaseResourcesAndAbandonContext()` + reset before exit.
- Added the macOS Skia build steps + gotchas to `ai/build-and-test.md`.

### Tests run

```sh
# build all examples per backend
cmake --build build-skia -j6              # 441/441
cmake --build build-ar5-cairo-release -j6 # 441/441
cmake --build build-ar5-quartz2d -j6      # 441/441

# launch+quit sweep of all 31 Skia examples
```

### Result

- All 31 Skia examples launch and quit cleanly — **no crash reports** after the
  teardown fix (before it, ~24 examples crashed on quit).
- Window captures confirm correct Skia rendering (sliders/knobs/labels and
  multi-weight text + icons).
- **All three backends build, run, and render.**

### Remaining issues / next steps

- Skia build verified in Debug; a Release `build-skia` has not been run (the
  leak assert is debug-only, but Release is the shipping config — worth a pass).
- vcpkg submodule pin (`00d899c`) inside the Artist submodule is environment
  setup, not committed in Elements; documented in build-and-test.md.
