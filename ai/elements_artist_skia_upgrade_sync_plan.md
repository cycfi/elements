# Cycfi Elements Artist `skia_upgrade` Sync Plan

## Objective

Advance the Elements `lib/artist` submodule from `artist_2026_dev` to the
latest `artist_2026_skia_upgrade`, then verify Elements still builds and runs
against it on macOS.

This is not a rendering API migration and not new backend work.

## Important clarification

`artist_2026_skia_upgrade` is `artist_2026_dev` **plus 15 commits, with no
divergence** — the merge-base equals the current `dev` HEAD, so the submodule
update is a clean fast-forward.

Artist's **public API is unchanged**. `git diff 5e54e7e..08a51ba -- include/`
(run in the Artist repo) is empty. The only header churn is inside the vendored
`lib/external/skia/` tree, where the hand-managed prebuilt Skia was removed in
favor of a vcpkg-provided Skia. No Elements source changes are expected.

The 15 new commits are: Skia m148 upgrade via vcpkg (macOS), Cairo Windows
portability, Skia Linux/Windows ports, CI restructuring, and build-guide docs.
None touch the canvas / font / path / image API Elements consumes.

All of `dev`'s Cairo work — font-map perf fix (`061d426`), HarfBuzz teardown
fix (`a8cd07b`), shadow-bitmap caching (`5e54e7e`) — is included in
`skia_upgrade`.

**All three backends must build: Quartz2D, Skia, and Cairo.** Quartz2D and
Cairo are source-compatible and already exercised on macOS. Skia is the one
that changed materially: `skia_upgrade` removed the hand-managed prebuilt Skia
in favor of a **vcpkg-provided Skia (m148)**, so the Elements Skia build now
requires the vcpkg toolchain wired through the Artist submodule. Building Skia
is therefore in scope for this sync and is the main integration risk.

## Repository Context

Working Elements branch (created for this sync, off `artist_2026`):

```text
artist_element_sync
```

Artist dependency:

```text
lib/artist  (git submodule, url git@github.com:cycfi/artist.git)
```

Current Artist branch / commit (`.gitmodules` `branch =`):

```text
artist_2026_dev  @ 5e54e7e
```

Target Artist branch / commit:

```text
artist_2026_skia_upgrade  @ 08a51ba
```

Project notes live in:

```text
ai/current-task.md
ai/build-and-test.md
ai/handoff.md
ai/elements_artist_skia_upgrade_sync_plan.md  (this file)
```

Operating rules for AI coding agents:

- Do all work on branch `artist_element_sync`.
- Work in stages. Make a point commit at each milestone (each stage).
- Always write a handoff entry in `ai/handoff.md` after each stage.
- Do not rewrite rendering abstractions.
- Do not change public Elements APIs.
- Do not create a compatibility shim unless real API incompatibilities appear.
- Do not run `git pull` on Elements unless explicitly instructed.
- Reuse existing ignored build directories where possible.
- Do not commit generated build files, logs, binaries, screenshots, or CMake
  cache files unless explicitly requested.
- Update `ai/handoff.md` after each useful step.
- Stop after one clean diff per logical change.

## High-level Strategy

```text
fetch skia_upgrade into the submodule
→ repoint .gitmodules branch to artist_2026_skia_upgrade
→ check out 08a51ba in the submodule
→ verify Cairo build still compiles and runs
→ verify Quartz2D build still compiles and runs
→ verify Skia build (now vcpkg m148) compiles and runs
→ commit submodule pointer + .gitmodules
→ document results
```

All three backends (Quartz2D, Skia, Cairo) must build before the sync is
considered complete.

Treat any actual API incompatibility as unexpected; record it before fixing.

---

# Stage 0: Update the Artist submodule pointer

## Goal

Move `lib/artist` to `artist_2026_skia_upgrade` (`08a51ba`) as a clean
fast-forward, and repoint `.gitmodules` at the new branch.

## Tasks

1. Fetch the branch into the submodule (the clone does not have it yet):

   ```sh
   git -C lib/artist fetch origin artist_2026_skia_upgrade
   ```

2. Update `.gitmodules`: `branch = artist_2026_dev` →
   `branch = artist_2026_skia_upgrade` for `lib/artist`.

3. Check out the new commit in the submodule:

   ```sh
   git -C lib/artist checkout artist_2026_skia_upgrade
   ```

4. Confirm the new submodule state:

   ```sh
   git submodule status lib/artist
   git -C lib/artist log --oneline -1
   ```

## Deliverable

Staged superproject changes:

```text
.gitmodules
lib/artist  (submodule pointer)
```

Update `ai/build-and-test.md` and `ai/handoff.md` with the before/after
commits.

## Rules

- Do not modify Elements rendering code.
- Do not build in this stage.

---

# Stage 1: Verify Cairo backend builds and runs

## Goal

Confirm Elements builds against `artist_2026_skia_upgrade` using the Cairo
backend and that a representative example launches.

## Tasks

1. Reuse the existing Cairo Release build directory:

   ```sh
   cmake --build build-ar5-cairo-release --target BasicSlidersAndKnobs -j6
   open build-ar5-cairo-release/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
   ```

2. Optionally cover text + controls:

   ```sh
   cmake --build build-ar5-cairo-release --target TextAndIcons -j6
   ```

3. Record compiler, build result, and any new warnings/errors beyond the known
   pre-existing enum-conversion warnings.

## Deliverable

Update:

```text
ai/build-and-test.md
ai/handoff.md
```

## Rules

- Fix only actual build/link breakage caused by the submodule move.
- If a break is clearly inside Artist, document it and pause before editing the
  Artist repo unless explicitly instructed.

---

# Stage 2: Verify Quartz2D backend builds and runs

## Goal

Confirm the Quartz2D path is unaffected by the submodule move.

## Tasks

1. Reuse the existing Quartz2D build directory:

   ```sh
   cmake --build build-ar5-quartz2d --target BasicSlidersAndKnobs -j6
   open build-ar5-quartz2d/examples/basic_sliders_and_knobs/BasicSlidersAndKnobs.app
   ```

2. Record build result and launch outcome.

## Deliverable

Update:

```text
ai/build-and-test.md
ai/handoff.md
```

## Rules

- Verification only; no rendering-code changes.

---

# Stage 3: Verify Skia backend builds and runs (vcpkg m148)

## Goal

Confirm Elements builds and runs against the new vcpkg-provided Skia (m148)
that `skia_upgrade` introduced. This is the highest-risk backend because the
Skia dependency mechanism changed.

## Host-integration findings (must address)

- Elements' Skia host (`lib/host/macos/base_view.mm`) is built on
  `sk_app::WindowContext` / `MakeGLForMac` (`tools/sk_app/*` + OpenGL). The m148
  upgrade **deleted `tools/sk_app`**, and vcpkg's Skia port does not install the
  `tools/` headers — so the Skia path no longer compiles.
- The new Artist reference (`examples/host/macos/skia_app.mm`) replaces it with
  **CAMetalLayer + `GrDirectContexts::MakeMetal`** and per-frame
  `nextDrawable` → `GrBackendRenderTargets::MakeMtl` →
  `SkSurfaces::WrapBackendRenderTarget`.
- Elements is **invalidation-driven** (`refresh()` → `setNeedsDisplay:` →
  `drawRect:`); the Artist example self-drives `render`. A CAMetalLayer-*hosting*
  view does not get `drawRect:`.

## Chosen approach: render-on-demand Metal layer

Keep Elements' invalidation model. Add the CAMetalLayer as a **sublayer** (the
view stays non-layer-hosting, so `drawRect:` still fires on `setNeedsDisplay:`),
size it to the view bounds, and run the Metal/Ganesh render inside the
`ARTIST_SKIA` branch of `drawRect:`. This is the smallest change to Elements'
event/redraw flow and reuses the existing `refresh()`/`displayIfNeeded` plumbing.

## Tasks

1. Locate how Elements selects the Skia backend (`ELEMENTS_SKIA`) and how the
   Artist submodule expects the vcpkg toolchain to be passed through
   (`CMAKE_TOOLCHAIN_FILE=lib/artist/lib/external/vcpkg/scripts/buildsystems/vcpkg.cmake`,
   plus `-DVCPKG_TARGET_TRIPLET=arm64-osx` on Apple Silicon).
2. Bootstrap vcpkg in the submodule if not already done:
   `lib/artist/lib/external/vcpkg/bootstrap-vcpkg.sh -disableMetrics`.
3. Configure a dedicated Skia build directory with `-DELEMENTS_SKIA=ON` and the
   vcpkg toolchain; the first configure builds Skia from source (slow).
4. Build `BasicSlidersAndKnobs`; launch it.
5. Resolve only actual Skia-specific build/link/host-glue issues. Record the
   exact CMake invocation that works.

## Deliverable

Update:

```text
ai/build-and-test.md
ai/handoff.md
```

Record the working Skia configure command. If the macOS Skia host glue needs
changes, document them as their own clean diff.

## Rules

- Do not change public Elements APIs.
- If a break is clearly inside Artist (not Elements glue), document it and pause
  before editing the Artist repo unless explicitly instructed.
- Stop after one clean compiling/running diff.

---

# Stage 4: Commit and document

## Goal

Land the submodule bump as one clean diff and record the sync.

## Tasks

1. Commit the submodule pointer + `.gitmodules` change on `artist_2026`:

   ```sh
   git add .gitmodules lib/artist ai/
   git commit
   ```

2. Add a handoff entry summarizing the before/after Artist commits, the verified
   backends, and any warnings.

## Deliverable

```text
ai/handoff.md
ai/build-and-test.md
```

## Rules

- Do not bundle unrelated cleanup into the sync commit.
- Do not force-push or rewrite history unless explicitly requested.

---

# Suggested Agent Work Packets

## AS1: Bump submodule to skia_upgrade

```text
Task:
Advance the Elements lib/artist submodule from artist_2026_dev to
artist_2026_skia_upgrade (08a51ba) as a clean fast-forward.

Instructions:
1. Confirm the current Elements branch is artist_2026.
2. git -C lib/artist fetch origin artist_2026_skia_upgrade
3. Edit .gitmodules: branch = artist_2026_skia_upgrade for lib/artist.
4. git -C lib/artist checkout artist_2026_skia_upgrade
5. Stage .gitmodules and the submodule pointer.
6. Update ai/build-and-test.md and ai/handoff.md.

Rules:
- Do not change Elements rendering code.
- Do not build in this packet.
- Stop after the staged submodule diff.
```

## AS2: Verify all backends against skia_upgrade

```text
Task:
Verify Elements builds and runs against artist_2026_skia_upgrade on macOS for
ALL THREE backends: Cairo, Quartz2D, and Skia (vcpkg m148).

Instructions:
1. Cairo: build BasicSlidersAndKnobs in build-ar5-cairo-release; launch it.
2. Quartz2D: build BasicSlidersAndKnobs in build-ar5-quartz2d; launch it.
3. Skia: configure a fresh build dir with -DELEMENTS_SKIA=ON and the Artist
   submodule vcpkg toolchain (+ -DVCPKG_TARGET_TRIPLET=arm64-osx on Apple
   Silicon); build and launch BasicSlidersAndKnobs.
4. Record results and any new warnings in ai/build-and-test.md.
5. Write a handoff entry after each backend is verified.

Rules:
- Fix only actual breakage from the submodule move / Skia mechanism change.
- If a break is inside Artist, document and pause.
- Commit per milestone; do a handoff after each stage.
```

## AS3: Commit the sync

```text
Task:
Commit the submodule bump + .gitmodules + ai notes on artist_2026.

Rules:
- One clean diff; no unrelated cleanup.
- No force-push or history rewrite unless requested.
```

---

# Risks

## Lower Risk

- Artist public API compatibility (verified: no include/ delta).
- Cairo and Quartz2D build paths (no source changes for those backends).

## Higher Risk

- **Skia backend via vcpkg.** `skia_upgrade` replaced prebuilt Skia with
  vcpkg-built Skia m148. The Elements Skia build must pass the vcpkg toolchain
  through to the Artist submodule, the first configure builds Skia from source,
  and the macOS Skia host glue may need alignment with Artist's updated
  Metal/`skia_app` host. This is the main work item of the sync.

## Out of scope

- Linux/Windows Elements Cairo host draw paths (still Skia-only; see
  `ai/elements_cairo_host_integration.md`).
- Any Elements public API or rendering-abstraction changes.

---

# Summary

This is a fast-forward submodule bump, not a migration. `skia_upgrade` is a
strict superset of `dev` with an unchanged public Artist API, so the work is:

1. Fetch `artist_2026_skia_upgrade` into `lib/artist` and repoint `.gitmodules`.
2. Check out `08a51ba` in the submodule.
3. Verify all three backends — Cairo, Quartz2D, and the new vcpkg Skia m148 —
   compile and launch.
4. Commit each stage as its own point commit; write a handoff entry after each.
5. Document the result in `ai/handoff.md` and `ai/build-and-test.md`.

The Cairo and Quartz2D bumps are trivial; the Skia (vcpkg m148) build is the
real integration work.

Do not introduce compatibility code unless the compiler or runtime proves it is
necessary.
