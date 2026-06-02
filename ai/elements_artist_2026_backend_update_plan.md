# Cycfi Elements Artist 2026 Backend Update Plan

## Objective

Update Cycfi Elements from the `skia_2024` baseline to use the latest Cycfi Artist rendering library from the `artist_2026_dev` branch.

This is not a broad rendering API migration.

Important clarification:

The `skia_2024` Elements code already uses an older Artist-derived rendering library. The `artist_2026_dev` rendering library is expected to remain API-compatible with the Elements code in `skia_2024`.

The main addition in `artist_2026_dev` is the new Cairo backend.

Therefore the migration should be handled as:

```text
Elements skia_2024 code
→ update Artist dependency to artist_2026_dev
→ verify existing Skia backend still builds
→ enable and test the new Cairo backend
```

Do not design a compatibility shim unless real API incompatibilities are discovered.

---

## Branching Instruction

Create a new working branch named `artist_2026` based on `skia_2024`.

```sh
git checkout skia_2024
git checkout -b artist_2026
```

All work should happen on:

```text
artist_2026
```

Do not modify `skia_2024` directly.

Treat `skia_2024` as the baseline/reference branch.

Do not run `git pull` unless explicitly instructed. Use the current local
`skia_2024` checkout as the baseline unless the user asks to update it.

---

## High-level Strategy

Keep the migration simple.

The expected path is:

```text
create branch
→ baseline build
→ update Artist dependency
→ build existing Skia path
→ enable Cairo backend
→ fix only actual build/rendering issues
→ test Elements examples/widgets
→ document results
```

Avoid speculative architecture work.

Avoid rewriting rendering abstractions.

Avoid creating an adapter or compatibility layer unless the compiler or runtime tests prove one is necessary.

---

## Repository Context

Baseline Elements branch:

```text
skia_2024
```

Working Elements branch:

```text
artist_2026
```

Target Artist branch:

```text
artist_2026_dev
```

Project notes should be kept in:

```text
ai/current-task.md
ai/architecture.md
ai/build-and-test.md
ai/decisions.md
ai/handoff.md
```

The `ai/` planning files are project artifacts for this migration and should be
tracked on the `artist_2026` branch.

Optional notes, only if needed:

```text
ai/api-incompatibilities.md
ai/cairo-backend-notes.md
ai/widget-parity.md
```

Operating rules for AI coding agents:

- Use short, focused sessions.
- Scope work to specific files or subsystems.
- Do not perform a broad rewrite.
- Do not change public Elements APIs unless a real incompatibility requires it.
- Do not create a compatibility shim preemptively.
- First verify the expected API compatibility.
- Run targeted builds/tests.
- Stop after one clean diff.
- Update `ai/handoff.md` after each useful step.
- Use existing ignored build directories where possible.
- Do not commit generated build files, logs, binaries, screenshots, or CMake
  cache files unless explicitly requested.

One clean diff means one logical change: either docs-only setup, dependency
update, build-option update, or one targeted fix. Do not combine these unless
required to make the build meaningful.

---

# Stage 0: Create Working Branch and Initial Notes

## Goal

Create `artist_2026` from `skia_2024`, then add the initial migration notes.
Do not build yet.

## Tasks

1. Check out the baseline branch:

```sh
git checkout skia_2024
```

2. Create the working branch:

```sh
git checkout -b artist_2026
```

3. Confirm current branch and commit:

```sh
git status
git branch --show-current
git rev-parse HEAD
```

4. Create or update initial AI notes:

```text
ai/current-task.md
ai/build-and-test.md
ai/handoff.md
```

5. Record that the build has not been run yet.

## `ai/current-task.md` should say

```md
# Current Task

Update Cycfi Elements from the `skia_2024` baseline to use the latest Cycfi Artist rendering library from `artist_2026_dev`.

## Working branch

`artist_2026`

## Baseline branch

`skia_2024`

## Important clarification

The Artist API is expected to be compatible with the existing Elements `skia_2024` code. The main new work is enabling and testing the new Cairo backend.

## Do not do yet

- Do not rewrite rendering abstractions.
- Do not create a compatibility shim unless real API incompatibilities are found.
- Do not change public Elements APIs.
- Do not remove the existing Skia path.
```

## Deliverable

Documentation-only diff:

```text
ai/current-task.md
ai/build-and-test.md
ai/handoff.md
```

---

# Stage 1: Baseline Build of `skia_2024` Code on `artist_2026`

## Goal

Confirm that the inherited `skia_2024` code still builds on the new `artist_2026` working branch before updating the Artist dependency.

## Tasks

1. Identify the smallest available demo/example target and record its target
   name/path. Use this same target for later smoke tests unless it is unsuitable.
2. Build the project using the existing documented process.
3. Run the smallest available demo/example if practical.
4. Record:
   - compiler version
   - platform
   - build command
   - build result
   - smallest demo/example target
   - demo/example result
   - any existing warnings or failures

## Deliverable

Update:

```text
ai/build-and-test.md
ai/handoff.md
```

Suggested `ai/build-and-test.md` section:

```md
# Baseline Build

## Branch

artist_2026

## Source baseline

skia_2024

## Build command

...

## Result

...

## Notes

...
```

## Rule

Do not fix unrelated baseline issues unless they prevent further progress.
Do not update the Artist dependency until this baseline build result and the
current dependency wiring are documented.

---

# Stage 2: Locate Artist Dependency and Backend Selection

## Goal

Find exactly how Elements currently includes Artist/Skia and how backend selection is configured.

## Search Commands

```sh
find . -name "CMakeLists.txt" -o -name "*.cmake"
rg "artist|Artist|skia|Skia|cairo|Cairo|backend|renderer"
rg "add_subdirectory|FetchContent|ExternalProject|find_package|target_link_libraries"
```

## Questions to Answer

1. Is Artist vendored, submoduled, copied, or externally found?
2. Is Skia selected through Artist, Elements, or CMake options?
3. Does Elements know about Artist backends directly?
4. Where should `artist_2026_dev` be selected?
5. Where should the Cairo backend be enabled?
6. Are backend choices compile-time, link-time, or runtime?
7. Are there platform-specific build paths?

Record exact files and line numbers that define Artist inclusion and backend
selection before recommending the minimal change.

## Deliverable

Update:

```text
ai/architecture.md
ai/build-and-test.md
ai/handoff.md
```

Suggested `ai/architecture.md` section:

```md
# Artist Dependency and Backend Selection

## Current Artist dependency location

...

## Current backend selection mechanism

...

## Skia backend path

...

## Cairo backend path

...

## Proposed minimal change

...
```

---

# Stage 3: Update Artist Dependency to `artist_2026_dev`

## Goal

Point Elements at the latest Artist rendering library from branch `artist_2026_dev`.

## Tasks

Depending on how Artist is included:

### If Artist is a submodule

```sh
cd path/to/artist
git fetch
git checkout artist_2026_dev
cd -
git status
```

Then commit the submodule pointer update.

### If Artist is vendored/copied

Update the vendored Artist source carefully from `artist_2026_dev`.

Record exactly what was replaced.

### If Artist is fetched by CMake

Update the CMake branch/tag/reference to `artist_2026_dev`.

### If Artist is externally found

Document the required checkout/install path and version.

## Rules

- Do not modify Elements rendering code yet.
- Do not create an adapter.
- Do not change public Elements APIs.
- Do not enable Cairo yet unless the build naturally requires selecting a backend.
- First try to build the existing Skia path with the updated Artist library.
- Do not run `git pull` unless explicitly instructed.

## Deliverable

A small dependency/build configuration diff plus updates to:

```text
ai/build-and-test.md
ai/handoff.md
```

---

# Stage 4: Verify Existing Skia Backend with Artist 2026

## Goal

Confirm that Elements builds against `artist_2026_dev` using the existing Skia backend path.

This validates the key assumption that the Artist API remained compatible.
Cairo work must not begin until the existing Skia backend builds against
`artist_2026_dev`, or until any Skia build failure is documented as a blocker.

## Tasks

1. Configure/build with the existing Skia backend.
2. Record compile errors, if any.
3. If compile errors appear, classify them:
   - dependency/include issue
   - namespace/name movement
   - CMake/link issue
   - actual Artist API incompatibility
   - backend selection issue
4. Fix only the minimum necessary issue.
5. Run the smallest demo/example.

## If API incompatibilities are found

Create:

```text
ai/api-incompatibilities.md
```

Use this format:

```md
# API Incompatibilities Found

| File | Symbol/API | Error | Likely cause | Minimal fix |
|---|---|---|---|---|
| ... | ... | ... | ... | ... |
```

## Deliverable

Update:

```text
ai/build-and-test.md
ai/handoff.md
```

## Rule

Only introduce compatibility code if actual incompatibilities are found.

---

# Stage 5: Enable the Cairo Backend

## Goal

Enable the new Cairo backend from Artist 2026.

Before making build changes, document backend dependencies:

```md
## Backend Dependencies

| Backend | macOS | Linux | Windows | Notes |
|---|---|---|---|---|
| Skia | ... | ... | ... | ... |
| Cairo | ... | ... | ... | ... |
```

## Tasks

1. Find Artist’s Cairo backend option or build target.
2. Add the minimal Elements/CMake option needed to select Cairo.
3. Keep Skia backend available if practical.
4. Build with Cairo enabled.
5. Resolve only actual Cairo-specific build/link issues.
6. Run the smallest demo/example.

## Possible CMake Options

Use the actual project style. Possible examples:

```cmake
ELEMENTS_ARTIST_BACKEND=skia
ELEMENTS_ARTIST_BACKEND=cairo
```

or:

```cmake
ELEMENTS_USE_ARTIST_CAIRO=ON
```

or reuse existing Artist backend options if they already exist.

## Deliverable

A small build/backend-selection diff plus:

```text
ai/cairo-backend-notes.md
ai/build-and-test.md
ai/handoff.md
```

Suggested `ai/cairo-backend-notes.md`:

```md
# Cairo Backend Notes

## How to enable

...

## Build command

...

## Dependencies

...

## Current status

...

## Known issues

...
```

---

# Stage 6: Smoke Test Rendering

## Goal

Verify that the Cairo backend renders enough of Elements to continue.

## Test Order

1. Blank window or minimal app
2. Clear/background color
3. Basic rectangle
4. Basic text label
5. Button
6. Slider or knob
7. Small existing demo
8. Larger demo/plugin UI if available

## What to Check

- Window opens
- Background clears
- Basic shapes render
- Text appears
- UI scale is reasonable
- Mouse interaction still works
- No obvious clipping errors
- No obvious transform errors
- No crashes on resize/repaint

## Deliverable

Update:

```text
ai/build-and-test.md
ai/cairo-backend-notes.md
ai/handoff.md
```

If widget differences appear, create:

```text
ai/widget-parity.md
```

But only if needed.

---

# Stage 7: Fix Cairo-Specific Rendering Issues

## Goal

Fix real rendering differences discovered during smoke tests.

## Important Rule

Do not preemptively rewrite rendering code.

Only fix issues that appear during build or demo testing.

## Possible Issue Categories

- Cairo surface creation
- Pixel format mismatch
- Alpha premultiplication
- Coordinate transform differences
- Clip behavior differences
- Stroke alignment differences
- Font discovery
- Text metrics differences
- Image loading
- HiDPI scaling
- Resize/repaint invalidation

## Fix Strategy

For each issue:

1. Reproduce with the smallest demo.
2. Identify whether the issue is in:
   - Elements usage
   - Artist backend selection
   - Artist Cairo backend
   - platform/window glue
   - dependency setup
3. Make the smallest fix.
4. Record the fix.
5. Stop after one clean diff.

If the issue is clearly in Artist rather than Elements, document it in the
Elements handoff and pause before editing the Artist repository unless
explicitly instructed.

## Deliverable

Update:

```text
ai/cairo-backend-notes.md
ai/build-and-test.md
ai/handoff.md
```

---

# Stage 8: Text Verification

## Goal

Verify that existing Elements text rendering works with Artist 2026 and Cairo.

This is verification first, not redesign.

## Tasks

1. Run demos with labels/buttons/text.
2. Check:
   - text appears
   - font size is correct
   - baseline is reasonable
   - text metrics do not break layout
   - buttons and labels align correctly
3. If text breaks, classify the issue:
   - font discovery
   - Cairo text backend
   - HarfBuzz/Freetype dependency
   - metrics mismatch
   - missing glyph/fallback
   - platform font path

## Deliverable

Only create this file if needed:

```text
ai/text-migration.md
```

Better default:

```text
ai/cairo-backend-notes.md
```

with a text section:

```md
# Text Status

...
```

## Rule

Do not redesign text layout unless existing text rendering fails.

---

# Stage 9: Image and Resource Verification

## Goal

Verify images and resource lifetime with Artist 2026 and Cairo.

## Tasks

1. Run demos that use images.
2. Check:
   - images load
   - images draw at expected size
   - scaling works
   - transparency works
   - no lifetime crashes
   - no missing resource path issues

## Deliverable

Update:

```text
ai/cairo-backend-notes.md
ai/build-and-test.md
ai/handoff.md
```

---

# Stage 10: HiDPI and Platform Verification

## Goal

Verify platform behavior with Cairo backend.

## Tasks

Check:

1. HiDPI/Retina scale
2. Resize behavior
3. Repaint/invalidation
4. Mouse coordinates
5. Offscreen surfaces, if used
6. Plugin-host surfaces, if applicable
7. Platform-specific dependencies

## Deliverable

Update:

```text
ai/cairo-backend-notes.md
ai/build-and-test.md
ai/handoff.md
```

---

# Stage 11: Cleanup and Documentation

## Goal

Clean up only after Skia and Cairo backend builds are understood.

## Tasks

1. Document how to build with Skia.
2. Document how to build with Cairo.
3. Document required dependencies.
4. Document known issues.
5. Remove temporary debug code.
6. Keep old backend available unless explicitly decided otherwise.

## Deliverable

Update:

```text
README.md
ai/build-and-test.md
ai/cairo-backend-notes.md
ai/decisions.md
ai/handoff.md
```

Do not remove old Skia backend unless explicitly requested.

---

# Suggested Agent Work Packets

## AR1.1: Create Branch and Initial Notes

```text
We are working on Cycfi Elements GUI library.

Baseline branch:
skia_2024

Working branch to create:
artist_2026

Task:
Create artist_2026 from skia_2024 and add the initial migration notes.

Instructions:
1. Check out skia_2024.
2. Do not run git pull unless explicitly instructed.
3. Create a new branch named artist_2026.
4. Confirm the current branch is artist_2026 and record the commit.
5. Create or update:
   - ai/current-task.md
   - ai/build-and-test.md
   - ai/handoff.md
6. Record that no build has been run yet.

Rules:
- Do not change production code.
- Do not update Artist yet.
- Do not remove Skia.
- Do not build yet.
- Stop after documentation changes only.
```

---

## AR1.2: Baseline Build

```text
Task:
Run the baseline build on artist_2026 before updating Artist.

Instructions:
1. Confirm the current branch is artist_2026.
2. Identify the smallest available demo/example target and record it.
3. Build the project using the current documented process.
4. Run the smallest available demo/example if practical.
5. Update:
   - ai/build-and-test.md
   - ai/handoff.md

Rules:
- Do not change production code.
- Do not update Artist yet.
- Do not fix unrelated baseline issues unless they block further progress.
- Stop after documentation changes only.
```

---

## AR2: Locate Artist Dependency and Backend Selection

```text
Task:
Locate how Elements currently includes Artist and selects the rendering backend.

Important clarification:
Do not assume an API migration is needed. Artist 2026 is expected to be API-compatible with the Elements skia_2024 code. The main addition is the Cairo backend.

Instructions:
1. Confirm the current branch is artist_2026.
2. Inspect CMake and dependency files.
3. Locate the Artist dependency.
4. Locate backend selection for Skia.
5. Find where Cairo backend should be enabled.
6. Record exact files and line numbers for the dependency/backend wiring.
7. Create or update:
   - ai/architecture.md
   - ai/build-and-test.md
   - ai/handoff.md

Rules:
- Do not modify production code.
- Do not create a compatibility layer.
- Stop after documentation changes only.
```

---

## AR3: Update Artist Dependency

```text
Task:
Update the Artist dependency to use branch artist_2026_dev.

Instructions:
1. Confirm the current branch is artist_2026.
2. Update the Artist dependency according to how it is included:
   - submodule pointer,
   - vendored source,
   - CMake FetchContent branch,
   - or documented external checkout.
3. Do not change Elements rendering code.
4. Build using the existing Skia backend first.
5. Record any build errors.
6. Update:
   - ai/build-and-test.md
   - ai/handoff.md
7. If actual API incompatibilities are found, create:
   - ai/api-incompatibilities.md

Rules:
- Do not enable Cairo yet unless required by the build.
- Do not begin Cairo work until the Skia backend builds against artist_2026_dev, or the Skia failure is documented as a blocker.
- Do not create a compatibility shim unless real compiler errors prove it is necessary.
- Stop after one clean diff.
```

---

## AR4: Enable Cairo Backend

```text
Task:
Enable the Artist 2026 Cairo backend in Elements.

Instructions:
1. Confirm the current branch is artist_2026.
2. Keep the existing Skia backend available if practical.
3. Find the minimal CMake/build change needed to select Cairo.
4. Build with Cairo enabled.
5. Run the smallest demo/example.
6. Create or update:
   - ai/cairo-backend-notes.md
   - ai/build-and-test.md
   - ai/handoff.md

Rules:
- Do not rewrite rendering abstractions.
- Do not change public Elements APIs.
- Fix only actual Cairo-specific build/link issues.
- Stop after one clean compiling diff.
```

---

## AR5: Cairo Smoke Test

```text
Task:
Smoke test Elements rendering using the Artist 2026 Cairo backend.

Instructions:
1. Confirm the current branch is artist_2026.
2. Run the smallest available Elements demo.
3. Then test at least one demo with:
   - basic shapes,
   - text,
   - a button or simple control,
   - resize/repaint behavior.
4. Record observed rendering differences.
5. Update:
   - ai/cairo-backend-notes.md
   - ai/build-and-test.md
   - ai/handoff.md
6. Create ai/widget-parity.md only if real widget differences appear.

Rules:
- Do not redesign text.
- Do not rewrite rendering code.
- Fix nothing unless the failure is tiny and obvious.
- Stop after documentation changes unless a minimal fix is required.
```

---

## AR6: Fix First Cairo Rendering Issue

```text
Task:
Fix the first confirmed Cairo-specific rendering issue.

Instructions:
1. Confirm the current branch is artist_2026.
2. Pick one issue from ai/cairo-backend-notes.md.
3. Reproduce it with the smallest demo.
4. Make the smallest targeted fix.
5. Rebuild and rerun the same demo.
6. Update:
   - ai/cairo-backend-notes.md
   - ai/build-and-test.md
   - ai/handoff.md

Rules:
- Fix only one issue.
- Do not bundle unrelated cleanup.
- Do not create a compatibility shim unless this specific issue requires it.
- Stop after one clean diff.
```

---

# First Prompt to Give Codex or Claude Code

```text
We are working on Cycfi Elements GUI library.

Baseline branch:
skia_2024

Working branch:
artist_2026

Before making changes:
1. Check out skia_2024.
2. Do not run git pull unless explicitly instructed.
3. Create a new branch named artist_2026 from skia_2024.
4. Confirm that the current branch is artist_2026.
5. Do all work on artist_2026.

Important clarification:
The current skia_2024 Elements code already uses an older Cycfi Artist-derived rendering library. The latest Artist branch, artist_2026_dev, is expected to remain API-compatible with this code. The main addition is the new Cairo backend.

Objective:
Update Elements to use Artist artist_2026_dev, verify the existing Skia backend still builds, then enable and test the new Cairo backend.

Your first task:
1. Create the artist_2026 branch from skia_2024.
2. Create or update:
   - ai/current-task.md
   - ai/build-and-test.md
   - ai/handoff.md
3. Record that no build has been run yet.

Rules:
- Do not rewrite rendering abstractions.
- Do not create a compatibility shim.
- Do not change public Elements APIs.
- Do not remove Skia.
- Do not update Artist yet.
- Do not build yet.
- Treat API incompatibilities as unexpected and record them only if they appear.
- Stop after documentation changes only.
```

---

# Main Risks

## High Risk

- Build integration
- Cairo dependency setup
- Platform surface setup
- Font discovery under Cairo
- HiDPI surface scaling
- Pixel format and alpha handling

## Medium Risk

- Text metrics differences
- Image transparency/scaling
- Clip behavior
- Stroke alignment
- Resize/repaint behavior

## Lower Risk

- Artist API compatibility
- Basic shape drawing
- Existing Skia backend path

---

# Summary

This update should be much simpler than a rendering migration.

The working assumption is that the Elements `skia_2024` code should already be compatible with Artist `artist_2026_dev`.

The work is mainly:

1. Create `artist_2026` from `skia_2024`.
2. Add initial `ai/` notes; do not build in the first packet.
3. Run and document the unchanged baseline build.
4. Locate and document Artist dependency/backend selection.
5. Update the Artist dependency.
6. Verify the existing Skia path still builds.
7. Enable the new Cairo backend.
8. Fix only real backend-specific issues that appear.
9. Document how to build and test both paths.

Do not introduce a compatibility layer unless the compiler or runtime proves it is necessary.
