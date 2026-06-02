# Architecture Notes

## AR2: Artist dependency and backend selection

Date: 2026-05-31

## Current Artist dependency location

Artist is a Git submodule:

| File | Lines | Evidence |
|---|---:|---|
| `.gitmodules` | 1-3 | `lib/artist` submodule points to `https://github.com/cycfi/artist.git` |

Current submodule state:

```text
e573d53c67bc8b3bbe1468a7c6ff888b3d592767 lib/artist (almost-alpha-185-ge573d53)
```

Inside the submodule, the current branch is:

```text
artist_2026_dev
```

## Current backend selection mechanism

Backend selection is compile-time.

Elements sets Artist backend cache options before adding the Artist submodule:

| File | Lines | Behavior |
|---|---:|---|
| `lib/CMakeLists.txt` | 14-15 | disables Artist examples/tests |
| `lib/CMakeLists.txt` | 17-21 | maps `ELEMENTS_QUARTZ_2D` to `ARTIST_QUARTZ_2D` and `ELEMENTS_SKIA` to `ARTIST_SKIA` |
| `lib/CMakeLists.txt` | 23 | `add_subdirectory(artist)` |

The vendored Artist branch then defines backend defaults:

| File | Lines | Behavior |
|---|---:|---|
| `lib/artist/CMakeLists.txt` | 46-51 | macOS defaults to Quartz2D; non-Apple defaults to Skia |
| `lib/artist/CMakeLists.txt` | 53-55 | `ARTIST_SKIA` disables `ARTIST_QUARTZ_2D` |
| `lib/artist/lib/CMakeLists.txt` | 230-238 | Quartz2D implementation sources on macOS |
| `lib/artist/lib/CMakeLists.txt` | 240-269 | Skia implementation sources |
| `lib/artist/lib/CMakeLists.txt` | 338-352 | publishes `ARTIST_SKIA` or `ARTIST_QUARTZ_2D` compile definitions |

## Host UI selection

Host platform selection is separate from Artist backend selection:

| File | Lines | Behavior |
|---|---:|---|
| `CMakeLists.txt` | 22 | declares `ELEMENTS_HOST_UI_LIBRARY` cache string |
| `cmake/ElementsConfigCommon.cmake` | 47-53 | defaults to `win32`, `gtk`, or `cocoa` by platform |
| `lib/CMakeLists.txt` | 294-316 | publishes `ELEMENTS_HOST_UI_LIBRARY_*` compile definitions |

## Current Skia backend path

Skia is selected by setting `ELEMENTS_SKIA`, which causes `lib/CMakeLists.txt`
to set `ARTIST_SKIA` before `add_subdirectory(artist)`.

Current host code has explicit Skia paths:

| File | Lines | Notes |
|---|---:|---|
| `lib/host/macos/base_view.mm` | 16-28 | includes Skia headers when `ARTIST_SKIA`; errors on unknown backend |
| `lib/host/macos/base_view.mm` | 193-210 | stores Skia window context only for `ARTIST_SKIA` |
| `lib/host/macos/base_view.mm` | 244-255 | creates GL/raster Skia context |
| `lib/host/macos/base_view.mm` | 387-417 | draws via Skia surface |
| `lib/host/gtk3/base_view.cpp` | 19-25, 95-99, 174-239 | GTK host is currently Skia/GL-oriented |
| `lib/host/windows/base_view.cpp` | 51-57, 123-140, 148-190, 586-590 | Windows host is currently Skia/GL-oriented |

## Current Quartz2D backend path

On macOS, if neither `ELEMENTS_SKIA` nor `ELEMENTS_QUARTZ_2D` is set, the
current Artist submodule defaults to Quartz2D.

The baseline AR1.2 configure selected:

```text
Building Artist lib for MacOS with Quartz2D.
```

macOS host code supports Quartz2D directly:

| File | Lines | Notes |
|---|---:|---|
| `lib/host/macos/base_view.mm` | 25-28 | accepts `ARTIST_QUARTZ_2D`, errors on unknown backend |
| `lib/host/macos/base_view.mm` | 372-379 | wraps `NSGraphicsContext.currentContext.CGContext` as Artist canvas |

## Cairo backend path

The current Elements branch now has:

1. `ELEMENTS_CAIRO` top-level CMake option.
2. `ELEMENTS_CAIRO` -> `ARTIST_CAIRO` mapping in `lib/CMakeLists.txt`.
3. A fail-fast configure guard when Cairo files are missing from vendored
   Artist.

The vendored Artist checkout is now on `artist_2026_dev`, where Cairo backend
build options and sources are present.

Current macOS status:

1. Configure with `-DELEMENTS_CAIRO=ON` succeeds.
2. `EmptyStarter` builds and launches with Cairo.
3. `lib/host/macos/base_view.mm` now includes an `ARTIST_CAIRO` draw path.

## Porting references

Use both sources when porting additional host paths and behavior:

1. Artist host examples under `lib/artist/examples/host/*` (especially
   `macos/cairo_app.mm` as the canonical Cairo host rendering pattern).
2. Elements `master` branch legacy Cairo-only backend implementation as a
   behavioral reference for host integration details.

## Proposed minimal change

AR3 and AR4 baseline goals are complete:

1. Artist submodule updated to `artist_2026_dev`.
2. Existing baseline backend path still builds.
3. Cairo selector enabled and first macOS host blocker fixed.

Next focus is AR5 smoke testing breadth (text + controls) and then platform
host parity (GTK/Windows) using the reference sources above.
