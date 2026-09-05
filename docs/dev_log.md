# Elements development log

Newest first. Each entry records what landed on a branch, with the detail that
would otherwise have lived in commit bodies (commit messages are kept to a
single subject line). Hashes are the post-rewrite short hashes on the branch
named in the heading.

## 2026-06-09 to 06-14 -- master hotfixes

**TL;DR:** Three master-only CI/build fixes on top of the 2025-09 line.

- `b52d542a` ci: fix Windows MSVC build -- use vswhere to locate Visual Studio
  instead of a hardcoded path; widen .gitignore to cover `build-*/`,
  `vcpkg_installed/`, `lib/artist/`, `ai/`.
- `c7f37447` ci: skip build for docs-only changes.
- `8a4d0913` ci: remove the Coverity scan workflow (#448) -- the scheduled scan
  had been failing and is not worth maintaining.

## 2026-06-06 -- artist submodule bumps: text-editor perf and backend-free headers

**TL;DR:** Track artist's text-engine perf work and its backend-free header
refactor through `lib/artist`.

- `de77f06e` bump lib/artist to the text-editor perf work (#370): skip full reflow
  when width is unchanged (per-keystroke editing O(1) in document size instead of
  O(paragraphs)); viewport-cull draw to visible paragraphs (drawing O(visible));
  `para_at_y` via `std::upper_bound`. Large-document editing/scrolling is now
  responsive.
- `6b6226ce` bump to artist `b8aff39` (draw member-template; backend-free engine
  header), then `85cf0f5a` bump to `408a358` (the final clean `draw(canvas&)` and
  backend-free-includable canvas/image/path headers, superseding the
  member-template step).

## 2026-06-05 -- text editor on the rope engine, artist fixes, logging/tracing

**TL;DR:** The text widgets are re-backed onto artist's multi-paragraph
`text_layout` engine so a keystroke reshapes only the touched paragraph, a
runtime-tunable logging facility plus an element-traversal tracer are added, and
several artist text fixes and Windows build warnings are picked up.

- `1f7135d0` Text editor on the rope-backed multi-paragraph engine (#370, squash
  of feature/text-editor-engine): re-back
  static_text_box/basic_text_box/basic_input_box onto `artist::text_layout` with
  the public API unchanged. insert/replace/erase route to the engine's incremental
  methods; `get_text()`/`value()` return a view into a lazily-materialized cache (a
  copy, not a reshape); undo still snapshots the u32string. Bumps lib/artist to the
  engine (artist develop `ef70b80`: rope + paragraph_index + text_layout; former
  text_layout renamed text_run). Also carries the macOS Skia host live-resize gap
  fix (disable implicit CALayer animation on the Metal sublayer).
- `14a3d2b6` logging and element-traversal tracing:
  - Logging (Part 1, always compiled, runtime-gated via `CYCFI_LOG=cat=level`):
    quill v9.0.3 (no external deps), bounded/drop async queue so it never blocks
    the UI/audio thread. Categories app/window/view/render/input/resource/layout/
    error/trace, each its own level. `base_view` exposes cross-platform on_open/
    on_size_change/on_scale_change/on_close that every host (macOS/Windows/GTK)
    calls and `view` overrides to log, so host code carries no LOG. `error_handler`
    is a global singleton with hookable non-null handlers (default LOG_ERROR) and
    an error_id enum (no_gpu_device, graphics_context_failed, render_surface_failed,
    font_load_failed, image_load_failed), wired at the render-context/font/image
    sites and working mid-construction and in plugins. Configurable log location
    via `log_init(app_name)` with per-OS dirs and a `CYCFI_LOG_DIR` override.
  - Tracing (Part 2, always compiled, runtime-gated `CYCFI_LOG=trace=debug`, off by
    default): `trace_scope` reads the context tree and emits JSON-lines
    (enter/exit/leaf) to `<app>-trace.jsonl` with call args, return values, child
    index, and the actual leaf control that handled the gesture; decorator proxies
    are skipped. `tools/trace_graph.py` reconstructs the per-gesture call tree as a
    YAML report or a Mermaid flowchart. Also adds `class_name()` overrides across
    element classes (trace labels and general debugging).
- `862362c4` bump artist (`4552432`): text no longer disappears when a string ends
  in a ligature; no .notdef box for a trailing newline after a ligature; a trailing
  hard break opens an empty line so the caret advances on a single Return
  (Skia/Quartz, matching Cairo) (#384).
- `b3a70585` bump artist: fix CJK characters dropped on line wrap (characters were
  lost at every soft wrap; Skia read past the glyph array on the force-break path).
  Verified on the text_edit example across Skia/Cairo/Quartz2D (#430).
- `82a69cf8` fix Windows build warnings (#439): drop a self-assignment in
  busy_bar_base (-Wsequence-point); parenthesize win32 scroll-direction comparisons
  and default-init `view_info::_scroll_dir` (-Wparentheses,
  -Wmissing-field-initializers); make win32 DropTarget's destructor virtual
  (-Wdelete-non-virtual-dtor).

## 2026-06-04 -- text selection and GTK partial-refresh fixes

**TL;DR:** Double-click word selection uses artist's UAX-29 word boundaries, and a
GTK partial-refresh offset bug that left edges unpainted is fixed.

- `0e373b2e` text_box: select whole words on double-click and fix the next-word
  caret. Artist `word_break` now exposes UAX-29 boundaries (a break after a word's
  last character); consume them so the first word on a line selects fully and
  Ctrl+Right lands after the word, not on its last character. Bumps the artist
  submodule for the word_break change and the earlier GTK+Cairo device_to_user fix.
- `da614b5f` gtk host: fix partial refresh invalidating the wrong region.
  `base_view::refresh(rect)` added the widget allocation offset to the
  `queue_draw_area` coordinates, but the drawing area is windowless so the
  coordinates are already widget-local; the extra offset shifted every partial
  invalidation by the decoration margin, leaving the left/top edge (e.g. a
  first-character selection highlight) unpainted until a full expose. Use
  widget-local coordinates, matching master.

## 2026-06-02 -- 2026 integration landing (Artist 2026, Skia/Cairo, Linux/Windows, CI)

**TL;DR:** The 2024 Elements line is modernized to C++20 and integrated with the
upgraded Artist 2026 backend (Skia m148 + Cairo + Quartz2D) across macOS, Linux,
and Windows, with the R2-based Skia binary distribution and a backend x platform
CI matrix; master fixes are folded in and the lib/artist submodule is pointed at
the artist develop trunk.

- `f515a9ea` Elements 2024: C++20 concepts, API modernization, and documentation
  pass. `27fdd91b` sprite button & dial styler rework (SpriteSubject concepts).
  `5b377881` fixes: vmax_size (#424) and non-resizable window (#428).
- `6586954e` back-merge from master (2025-09). `cb0034bb` cherry-pick master fixes
  (#444 selection keys, #446 Asio) and cache Cairo Windows vcpkg deps.
- `3625921a` integrate the Artist 2026 Cairo backend into Elements. `59a45730`
  Skia 2026 sync, Linux/Windows host ports, backend x platform CI matrix.
- `a5677fa4` Skia binary distribution: R2 vcpkg cache + prebuilt bundles +
  compat gate + CMake presets (mirrors the artist scheme).
- `a23edb3b` point the lib/artist submodule at artist's canonical develop trunk
  (was on a now-retired work-branch commit; tracks `9219d1f`).
- `5c069ed1` README: refresh the 2026 news for the unified develop trunk (unified
  Skia + Quartz2D + Cairo on the upgraded Artist backend; default backends and the
  automatic prebuilt-Skia fetch).
