# Elements: Host-Layer Cairo and Font Integration

*Source: direct inspection of `/Users/joel/dev/cycfi/elements`. Artist submodule
pinned to `artist_2026_dev` branch (`2cf9be5` at time of writing).*

---

## macOS — `lib/host/macos/base_view.mm`

### Drawing

`drawRect:` has three `#if` branches. The `ARTIST_CAIRO` path:

```objc
auto context_ref = NSGraphicsContext.currentContext.CGContext;
cairo_surface_t* surface =
   cairo_quartz_surface_create_for_cg_context(context_ref, w, h);
cairo_t* context = cairo_create(surface);
auto cnv = canvas{context};
_view->draw(cnv);
cairo_surface_destroy(surface);
cairo_destroy(context);
```

- Surface and context are **created and destroyed every frame**. No caching.
- `bounds.size` is in points. The CGContext from AppKit already encodes the
  Retina device scale; no `cairo_surface_set_device_scale` is needed.
- `isFlipped` returns `YES`, giving a top-down CGContext. The Quartz Cairo backend
  wraps it without an additional flip, so Cairo's y-axis is top-down throughout.

The other two branches for reference:
- `ARTIST_QUARTZ_2D`: raw `CGContextRef` cast directly to `canvas_impl*`.
- `ARTIST_SKIA`: `sk_app::WindowContext` → `getBackbufferSurface()` →
  `SkCanvas*`, scaled by `_scale` (Retina factor).

### Font activation — `resource_setter`

Initialised once (via `static resource_setter init_resources` in `elements_init:`):

```cpp
cycfi::artist::add_search_path(resource_path);   // Artist file search
for (auto& p : fs::directory_iterator{resource_path})
   if (p.path().extension() == ".ttf")
      activate_font(p.path());                    // CTFontManagerRegisterFontsForURL
```

`activate_font` registers each bundled `.ttf` with Core Text at process scope.
This is critical for plugins: the host process is not an app bundle, so bundled
fonts are invisible to Core Text unless explicitly registered. Without it,
`CGFontCreateWithFontName` silently fails and the CG-backed Cairo font face
(required for text rendering on Quartz surfaces) is never created.

`init_paths()` and `get_user_fonts_directory()` (also in `base_view.mm`) return
the same resource path to Artist's Fontconfig layer so `fc_match()` can resolve
bundled fonts as well.

---

## Linux — `lib/host/gtk3/base_view.cpp`

### Drawing

The Linux host uses a `GtkGLArea` widget and renders entirely via Skia over
OpenGL (`GrDirectContext` + `SkSurface::MakeFromBackendRenderTarget`). **There is
no Cairo drawing path.** The `cairo_t* _cr` field in `host_view` captures the
GTK `draw` signal's context only to trigger a `gtk_widget_draw()` call that
forces the first Skia frame — it is never used for rendering.

To add `ARTIST_CAIRO` support, the GTK `draw` signal handler should wrap the
provided `cairo_t*` in a `canvas` and call `view.draw(cnv)` directly. The
`on_draw` callback (currently just saves `_cr`) is the right place.
`examples/host/linux/cairo_app.cpp` in Artist shows the full pattern.

### Font / resource path

`init_paths()` calls `add_search_path(find_resources())` where `find_resources()`
resolves to `<exe-dir>/resources` or the installed
`<prefix>/share/<appname>/resources`. No explicit font activation is needed:
Fontconfig discovers bundled fonts via `FcConfigAppFontAddDir`, which Artist's
`fc_state` initialiser calls automatically.

---

## Windows — `lib/host/windows/base_view.cpp`

### Drawing

The Windows host also renders via Skia over OpenGL
(`sk_app::window_context_factory::MakeGLForWin`, raster fallback). **There is no
Cairo drawing path.** To add `ARTIST_CAIRO` support, `on_paint()` needs a branch
that creates a `cairo_win32_surface_create` from the HDC and calls
`view->draw(cnv)`. `examples/host/windows/cairo_app.cpp` in Artist shows the
pattern.

### Font / resource path

Same structure as Linux: `init_paths()` → `add_search_path(find_resources())`.
`find_resources()` looks for `<exe-dir>\resources`. Fontconfig handles font
resolution; no activation step is required.

---

## `view.cpp` — Offscreen Scratch Contexts

`lib/src/view.cpp` never touches the live window surface directly. All non-paint
operations — layout (`set_limits`), click, drag, cursor, scroll, key, text — go
through a helper:

```cpp
image img{1, 1};
offscreen_image offscr{img};
canvas cnv{offscr.context()};
context ctx{*this, cnv, &_main_element, _current_bounds};
f(ctx, _main_element);
```

The 1×1 size is nominal. Only font metrics, element limits, and hit-testing
are performed; no pixels are read back.

### Backend implementations

**Cairo** (`lib/artist/lib/impl/cairo/image.cpp`):
`image{1,1}` creates a 1×1 `CAIRO_FORMAT_ARGB32` image surface.
`offscreen_image` wraps it in a `cairo_t*` via `cairo_create`. This is a plain
image surface, not a recording surface.

**Skia** (`lib/artist/lib/impl/skia/image.cpp`):
`offscreen_image` uses `SkPictureRecorder::beginRecording()`, returning a
recording `SkCanvas*`. On destruction the picture is finalised into the `image`.
The Cairo equivalent would be `cairo_recording_surface_create`, but for the
current use (metrics and hit testing only) the image surface is sufficient and
correct.

The `image` / `offscreen_image` abstraction is backend-polymorphic — each backend
supplies its own `image.cpp` — so `view.cpp` itself requires no changes for Cairo
integration.

---

## Font Architecture

### Font creation pipeline

Non-macOS (`lib/artist/lib/impl/cairo/font.cpp`):

```
font_descr
  → fc_match()     Fontconfig: family/size/weight/slant/stretch → FcPattern*
  → cairo_ft_font_face_create_for_pattern()   → cairo_font_face_t* (FT-backed)
  → cairo_scaled_font_create()   HINT_METRICS_OFF + HINT_STYLE_NONE
  → cairo_ft_scaled_font_lock_face() → FT_Face
  → make_hb_font()   hb_ot_font_set_funcs, scale = (upem, upem)
  → font_impl{ _face=nullptr, _scaled_font, _size, _hb_font }
```

On macOS the same pipeline runs, and additionally a CG-backed face is created:

```
CGFontCreateWithFontName(FC_FULLNAME)
  → cairo_quartz_font_face_create_for_cgfont()
  → font_impl{ _face=cg_face, _scaled_font=ft_sf, _size, _hb_font }
```

`_face` is only used on `CAIRO_SURFACE_TYPE_QUARTZ`. The FT scaled font handles
standalone metrics (`font::metrics()`, `font::measure_text()`) and all
non-Quartz rendering (tests, offscreen). `HINT_METRICS_OFF` is essential: it
keeps fractional metrics so that `floor(ascent)` comparisons in tests match
expected values on all platforms.

### Canvas font dispatch (`lib/artist/lib/impl/cairo/canvas.cpp`)

```cpp
#ifdef __APPLE__
if (fi->_face &&
    cairo_surface_get_type(cairo_get_target(ctx)) == CAIRO_SURFACE_TYPE_QUARTZ)
{
   cairo_set_font_face(ctx, fi->_face);   // CG face → Core Graphics rasterisation
   cairo_set_font_size(ctx, fi->_size);
}
else
   cairo_set_scaled_font(ctx, fi->_scaled_font);
#else
   cairo_set_scaled_font(ctx, fi->_scaled_font);
#endif
```

### Text drawing

`canvas::fill_text()` uses HarfBuzz when `fi->_hb_font` is set:

```
shape_text(hb_font, size, utf8)  →  shaped_run  (glyph IDs, advances, offsets)
make_cairo_glyphs(run, x, y)     →  cairo_glyph_t[]
cairo_move_to(ctx, first.x, first.y)   ← required on Quartz CG backend
cairo_show_glyphs(ctx, glyphs)
```

`cairo_move_to` before `cairo_show_glyphs` is mandatory on the Quartz backend:
without it, `cairo_show_glyphs` silently produces no output.

---

## Integration Gaps

| Platform | Cairo draw path | Status |
|----------|----------------|--------|
| macOS    | `cairo_quartz_surface_create_for_cg_context` | Complete |
| Linux    | Not implemented | GTK `draw` signal can provide `cairo_t*` directly |
| Windows  | Not implemented | Needs `cairo_win32_surface_create` in `on_paint()` |

**Artist submodule pin.** The submodule is at `2cf9be5`, one commit before the
Linux font-metrics fix (`ce4f9a0`, which restores `HINT_METRICS_OFF` in the
non-macOS `make_font_impl`). Linux Cairo builds in Elements will show off-by-1
font metric values and a typography golden regression until the pin is advanced.

**`offscreen_image` vs. recording surface.** The Cairo backend uses a plain
image surface where Skia uses `SkPictureRecorder`. This asymmetry is harmless for
the current use (metrics and hit testing), but would need revisiting if deferred
or vector-semantic offscreen rendering were required.
