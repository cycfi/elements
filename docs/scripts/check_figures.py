#!/usr/bin/env python3
"""
Check the hand-drawn SVG figures for the things that make them hard to read.

The figures are placed at a fixed width on the page, so a font size in the
file renders at `size * PAGE_WIDTH / canvas width`. Anything below
MIN_RENDERED is too small to read. The rest of the checks are for arrows and
labels: an arrowhead needs a stem to point along, it has to point the way
its line runs, and a label should not straddle a box it does not belong to.

Usage: python3 check_figures.py [figure.svg ...]
"""

import glob
import html
import math
import os
import re
import sys

PAGE_WIDTH = 750.0      # what the pages place a figure at
MIN_RENDERED = 12.0     # the smallest readable size, on the page
MIN_STEM = 15.0         # visible line before an arrowhead, in user units
MIN_PAD = 10.0          # clear space each side of a label, in page pixels
CAP_CENTRE = 0.35       # a glyph's cap height centres at this fraction below
                        # its baseline, so a label in a circle sits at
                        # cy + CAP_CENTRE * size
HEAD = 9.0              # how far an arrowhead reaches back from its tip

# Rough advance widths, as a fraction of the font size, for the sans stack
# the figures use. Wide enough to catch a label that will not fit.
NARROW = set('iljtfIr .,:;!|\'')
WIDE = set('mwMW@')


try:
    from PIL import ImageFont
    _FONT = ImageFont.truetype('/System/Library/Fonts/Helvetica.ttc', 100)
except Exception:                                   # pragma: no cover
    _FONT = None


def text_width(s, size):
    # The real advance widths when the font is at hand, else an estimate.
    if _FONT:
        return _FONT.getlength(s) * size / 100
    w = 0.0
    for c in s:
        w += 0.30 if c in NARROW else (0.90 if c in WIDE else 0.55)
    return w * size


def parse(path):
    s = open(path, errors='ignore').read()
    vb = re.search(r'viewBox="[\d.\- ]*?([\d.]+) ([\d.]+)"', s)
    canvas = float(vb.group(1)) if vb else None
    height = float(vb.group(2)) if vb else None
    root = re.search(r'<svg[^>]*?font-size="([\d.]+)"', s)
    default = float(root.group(1)) if root else 16.0

    rects = [tuple(float(v) for v in m) for m in re.findall(
        r'<rect x="([\d.]+)" y="([\d.]+)" width="([\d.]+)" height="([\d.]+)"', s)]
    circles = [tuple(float(v) for v in m) for m in re.findall(
        r'<circle cx="([\d.]+)" cy="([\d.]+)" r="([\d.]+)"', s)]
    lines = [tuple(float(v) for v in m) for m in re.findall(
        r'<line x1="([\d.]+)" y1="([\d.]+)" x2="([\d.]+)" y2="([\d.]+)"', s)]

    # Connectors drawn as paths count too: what matters is where the run
    # starts and where it ends.
    # Every straight run of a path counts; a curve only moves the pen.
    for d in re.findall(r'<path[^>]*\sd="([^"]+)"', s):
        here, runs = None, []
        for cmd, args in re.findall(r'([MLQHVZmlqhvz])([^MLQHVZmlqhvz]*)', d):
            nums = [float(v) for v in
                    re.findall(r'-?(?:\d+\.?\d*|\.\d+)', args)]
            if cmd == 'M' and len(nums) >= 2:
                here = (nums[0], nums[1])
            elif cmd == 'L' and here:
                for x, y in zip(nums[0::2], nums[1::2]):
                    runs.append((here[0], here[1], x, y))
                    here = (x, y)
            elif cmd == 'H' and here:
                runs.append((here[0], here[1], nums[-1], here[1]))
                here = (nums[-1], here[1])
            elif cmd == 'V' and here:
                runs.append((here[0], here[1], here[0], nums[-1]))
                here = (here[0], nums[-1])
            elif cmd == 'Q' and len(nums) >= 4:
                here = (nums[-2], nums[-1])
        lines.extend(runs)
    heads = []
    for m in re.finditer(
            r'<polygon points="([\d.]+),([\d.]+) ([\d.]+),([\d.]+) ([\d.]+),([\d.]+)"', s):
        v = [float(x) for x in m.groups()]
        heads.append(((v[0], v[1]), ((v[2] + v[4]) / 2, (v[3] + v[5]) / 2)))

    texts = []
    for m in re.finditer(r'<text x="([\d.]+)" y="([\d.]+)"([^>]*)>([^<]*)</text>', s):
        x, y, attrs, body = float(m.group(1)), float(m.group(2)), m.group(3), m.group(4)
        size = float(re.search(r'font-size="([\d.]+)"', attrs).group(1)) \
            if 'font-size=' in attrs else default
        anchor = 'middle' if 'middle' in attrs else ('end' if '"end"' in attrs else 'start')
        body = html.unescape(body)      # one glyph, not seven characters
        w = text_width(body, size)
        x0 = x - w / 2 if anchor == 'middle' else (x - w if anchor == 'end' else x)
        texts.append((x0, y - size * 0.8, x0 + w, y + size * 0.25, body, size))
    return canvas, rects, lines, heads, texts, height, circles


def check(path):
    raw = open(path, errors='ignore').read()
    if 'matplotlib' in raw:
        return []              # a generated plot, drawn to its own rules

    canvas, rects, lines, heads, texts, height, circles = parse(path)
    out = []
    if not canvas:
        return ['no viewBox, cannot judge the rendered size']
    scale = PAGE_WIDTH / canvas

    # Anything past the viewBox is cut off when the figure is placed. A
    # drawing that positions with transforms has no absolute coordinates
    # here, so it is left alone.
    if 'transform=' in open(path, errors='ignore').read():
        rects_in_bounds = []
    else:
        rects_in_bounds = rects
    for rx, ry, rw, rh in rects_in_bounds:
        if rx + rw > canvas + 0.5 or (height and ry + rh > height + 0.5):
            out.append('a box runs past the canvas at (%g,%g)' % (rx + rw, ry + rh))
    if rects_in_bounds or not rects:
        for tx0, ty0, tx1, ty1, body, size in texts:
            if tx1 > canvas + 0.5 or (height and ty1 > height + 0.5):
                out.append('label "%s" runs past the canvas' % body[:30])

    small = {}
    for _, _, _, _, body, size in texts:
        if size * scale < MIN_RENDERED - 0.05:
            small.setdefault(size, 0)
            small[size] += 1
    for size in sorted(small):
        out.append('%d label(s) at %gpx render at %.1fpx, under %.0f'
                   % (small[size], size, size * scale, MIN_RENDERED))

    # An arrowhead stops at the edge of the block it points to. A container,
    # a box that holds other boxes, is not one of those: arrows live inside
    # it.
    blocks = []
    for rx, ry, rw, rh in rects:
        holds = any(ox >= rx and oy >= ry and ox + ow <= rx + rw
                    and oy + oh <= ry + rh and (ox, oy, ow, oh) != (rx, ry, rw, rh)
                    for ox, oy, ow, oh in rects)
        if not holds:
            blocks.append((rx, ry, rw, rh))

    for tip, base in heads:
        for rx, ry, rw, rh in blocks:
            if rx + 1 < tip[0] < rx + rw - 1 and ry + 1 < tip[1] < ry + rh - 1:
                out.append('arrowhead at (%g,%g) lands inside a box' % tip)
                break

    for tip, base in heads:
        best = None
        for x1, y1, x2, y2 in lines:
            for near, far in (((x2, y2), (x1, y1)), ((x1, y1), (x2, y2))):
                if math.hypot(near[0] - tip[0], near[1] - tip[1]) < 2:
                    ln = math.hypot(x2 - x1, y2 - y1)
                    ok = math.hypot(far[0] - base[0], far[1] - base[1]) < \
                        math.hypot(far[0] - tip[0], far[1] - tip[1])
                    if best is None or ln > best[0]:
                        best = (ln, ok)
        if best is None:
            out.append('arrowhead at (%g,%g) has no line' % tip)
        else:
            ln, ok = best
            if not ok:
                out.append('arrowhead at (%g,%g) points backwards' % tip)
            if ln - HEAD < MIN_STEM:
                out.append('arrowhead at (%g,%g): stem only %.0f units'
                           % (tip[0], tip[1], ln - HEAD))

    # A glyph in a circle sits on the baseline that centres it.
    for cx, cy, r in circles:
        for tx0, ty0, tx1, ty1, body, size in texts:
            mid_x = (tx0 + tx1) / 2
            base = ty1 - size * 0.25      # back to the baseline
            if abs(mid_x - cx) > r or abs(base - cy) > r + size:
                continue
            want = cy + CAP_CENTRE * size
            if abs(base - want) > 1.5:
                out.append('"%s" in the circle at (%g,%g) sits at %g, not %g'
                           % (body[:12], cx, cy, base, round(want, 1)))
            break

    # A connector keeps out of the blocks it does not touch.
    for x1, y1, x2, y2 in lines:
        for rx, ry, rw, rh in blocks:
            if x1 == x2:                       # vertical
                inside = rx + 1 < x1 < rx + rw - 1 and \
                    min(y1, y2) < ry + rh - 1 and max(y1, y2) > ry + 1
            elif y1 == y2:                     # horizontal
                inside = ry + 1 < y1 < ry + rh - 1 and \
                    min(x1, x2) < rx + rw - 1 and max(x1, x2) > rx + 1
            else:
                inside = False
            if inside:
                out.append('a connector runs through the box at (%g,%g)'
                           % (rx, ry))
                break

    for tx0, ty0, tx1, ty1, body, _ in texts:
        for rx, ry, rw, rh in rects:
            rx1, ry1 = rx + rw, ry + rh
            inside = tx0 >= rx and tx1 <= rx1 and ty0 >= ry and ty1 <= ry1
            over = not (tx1 <= rx or tx0 >= rx1 or ty1 <= ry or ty0 >= ry1)
            if over and not inside:
                out.append('label "%s" straddles a box edge' % body[:36])
                break
            # A label inside a box keeps its distance from the sides.
            mid_x, mid_y = (tx0 + tx1) / 2, (ty0 + ty1) / 2
            if rx <= mid_x <= rx1 and ry <= mid_y <= ry1 and inside:
                pad = min(tx0 - rx, rx1 - tx1)
                if pad * scale < MIN_PAD:
                    out.append('label "%s" has only %.0fpx of padding'
                               % (body[:30], pad * scale))
                break
    return out


def main():
    files = sys.argv[1:] or sorted(glob.glob(
        os.path.join(os.path.dirname(__file__),
                     '../modules/ROOT/images/*.svg')))
    bad = 0
    for f in files:
        problems = check(f)
        if problems:
            bad += 1
            print('%s' % os.path.basename(f))
            for p in dict.fromkeys(problems):
                print('   ' + p)
    print('\n%d of %d figures have something to fix' % (bad, len(files)))


if __name__ == '__main__':
    main()
