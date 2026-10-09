#!/usr/bin/env python3
"""
Draw the documentation's block diagrams from a description.

A figure says what the blocks are and what connects to what; this works out
the sizes, the places and the routes, and writes SVG that keeps the rules
the docs' block diagrams follow (see the Block diagrams section of the KB's
documentation conventions):

    one font size, names only      a label is measured and the block is
                                   sized around it, never the other way
    a clear gutter                 PAD units each side of every label
    the page's own width           1 unit is 1 px at the width pages use
    arrows that read               a stem before every head, the head on
                                   the block's edge, and no line crossing a
                                   block it does not touch

Figures are written as small scripts against this (see
gen_block_figures.py) and checked with check_figures.py.
"""

import os
from html import escape

try:
    from PIL import ImageFont
except ImportError:                                 # pragma: no cover
    ImageFont = None

WIDTH = 750.0           # what the pages place a figure at
FONT = 14.0             # the one size
PAD = 14.0              # clear space each side of a label
GAP = 38.0              # between blocks, which leaves room for a stem
HEAD = 9.0              # how far an arrowhead reaches back
ROW = 44.0              # a block's height
LINE = 17.0             # between the lines of a two-line label

# The roles a block can have, as colours.
STYLES = {
    'component': ('#e8f2fd', '#1565c0'),     # the library's own
    'yours':     ('#ffffff', '#43a047'),     # what the example writes
    'input':     ('#fdeaf2', '#d81b60'),     # what arrives from outside
    'plain':     ('#ffffff', '#5d5d5d'),     # anything else, the console
    'member':    ('#ffffff', '#1565c0'),     # one of many, inside a container
    'accent':    ('#fff3e0', '#ef6c00'),     # feedback, taps
    'container': ('#f7fbff', '#1565c0'),
}

LINES = {
    'signal':  '#1565c0',
    'control': '#43a047',
    'midi':    '#d81b60',
    'dry':     '#5d5d5d',
    'plain':   '#5d5d5d',
    'accent':  '#ef6c00',
}

FONT_FAMILY = (
    "-apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, Helvetica, "
    "Arial, sans-serif, \"Apple Color Emoji\", \"Segoe UI Emoji\", "
    "\"Segoe UI Symbol\""
)

_FONTS = ['/System/Library/Fonts/Helvetica.ttc',
          '/System/Library/Fonts/HelveticaNeue.ttc']


def text_width(s, size=FONT):
    """The widest the label gets in the fonts the pages actually use."""
    if ImageFont:
        widest = 0.0
        for path in _FONTS:
            if not os.path.exists(path):
                continue
            try:
                widest = max(widest, ImageFont.truetype(path, int(size))
                             .getlength(s))
            except Exception:
                pass
        if widest:
            return widest
    return sum(0.32 if c in 'iljtfI .,:;!|\'' else
               (0.95 if c in 'mwMW@' else 0.58) for c in s) * size


BEND = 8.0              # the radius a connector turns a corner with


def rounded(pts):
    """An orthogonal route as path data, each corner turned on a curve."""
    def at(p):
        return '%g %g' % (round(p[0], 2), round(p[1], 2))
    d = 'M ' + at(pts[0])
    for a, p, b in zip(pts, pts[1:], pts[2:]):
        la = ((p[0] - a[0]) ** 2 + (p[1] - a[1]) ** 2) ** 0.5
        lb = ((b[0] - p[0]) ** 2 + (b[1] - p[1]) ** 2) ** 0.5
        if not la or not lb:
            continue
        r = min(BEND, la / 2, lb / 2)
        enter = (p[0] - r * (p[0] - a[0]) / la, p[1] - r * (p[1] - a[1]) / la)
        leave = (p[0] + r * (b[0] - p[0]) / lb, p[1] + r * (b[1] - p[1]) / lb)
        d += ' L %s Q %s %s' % (at(enter), at(p), at(leave))
    return d + ' L ' + at(pts[-1])


class Block:
    def __init__(self, label, kind='component', width=None, height=None,
                 shape='box', dashed=False):
        self.label = label
        self.kind = kind
        self.shape = shape                      # box, pill or circle
        self.dashed = dashed
        lines = label.split('\n') if label else ['']
        self.h = height or ROW + (len(lines) - 1) * LINE
        self.w = width or (max(text_width(t) for t in lines) + 2 * PAD)
        if shape == 'circle':
            self.w = self.h = 32.0
        self.x = self.y = 0.0

    # the sides, once it has been placed
    @property
    def left(self):    return self.x
    @property
    def right(self):   return self.x + self.w
    @property
    def top(self):     return self.y
    @property
    def bottom(self):  return self.y + self.h
    @property
    def cx(self):      return self.x + self.w / 2
    @property
    def cy(self):      return self.y + self.h / 2

    def move(self, dx, dy):
        self.x += dx
        self.y += dy

    def svg(self):
        fill, stroke = STYLES[self.kind]
        out = []
        if self.shape == 'circle':
            r = self.w / 2
            out.append('  <circle cx="%g" cy="%g" r="%g" fill="%s" '
                       'stroke="%s" stroke-width="1.5"/>'
                       % (self.cx, self.cy, r, fill, stroke))
        else:
            rx = self.h / 2 - 1 if self.shape == 'pill' else 8
            dash = ' stroke-dasharray="5,3"' if self.dashed else ''
            out.append('  <rect x="%g" y="%g" width="%g" height="%g" rx="%g" '
                       'fill="%s" stroke="%s" stroke-width="1.5"%s/>'
                       % (self.x, self.y, self.w, self.h, rx, fill, stroke,
                          dash))
        if self.shape == 'circle' and self.label in ('×', '+'):
            # drawn as strokes, so the sign sits dead centre in any font
            k = 6.0 if self.label == '×' else 7.0
            if self.label == '×':
                segs = [(-k, -k, k, k), (k, -k, -k, k)]
            else:
                segs = [(-k, 0, k, 0), (0, -k, 0, k)]
            for x1, y1, x2, y2 in segs:
                out.append('  <line x1="%g" y1="%g" x2="%g" y2="%g" '
                           'stroke="#1a1a1a" stroke-width="1.6"/>'
                           % (self.cx + x1, self.cy + y1,
                              self.cx + x2, self.cy + y2))
        elif self.label:
            lines = self.label.split('\n')
            y = self.cy + 0.35 * FONT - (len(lines) - 1) * LINE / 2
            for i, t in enumerate(lines):
                out.append('  <text x="%g" y="%g" text-anchor="middle" '
                           'fill="#1a1a1a">%s</text>'
                           % (self.cx, round(y + i * LINE, 1), escape(t)))
        return out


class Container(Block):
    """A block that holds others, drawn behind them with its name on top."""

    def __init__(self, label, children, pad=20, title_gap=30,
                 kind='container', dashed=False):
        super().__init__(label, kind=kind, width=1, height=1, dashed=dashed)
        self.children = children
        self.pad = pad
        self.title_gap = title_gap if label else 0

    def move(self, dx, dy):
        for c in self.children:
            c.move(dx, dy)
        self.fit()

    def fit(self):
        xs = [c.left for c in self.children] + [c.right for c in self.children]
        ys = [c.top for c in self.children] + [c.bottom for c in self.children]
        self.x = min(xs) - self.pad
        self.y = min(ys) - self.pad - self.title_gap
        self.w = max(xs) - min(xs) + 2 * self.pad
        self.h = max(ys) - min(ys) + 2 * self.pad + self.title_gap

    def svg(self):
        fill, stroke = STYLES[self.kind]
        dash = ' stroke-dasharray="7,5"' if self.dashed else ''
        out = ['  <rect x="%g" y="%g" width="%g" height="%g" rx="10" '
               'fill="%s" stroke="%s" stroke-width="1.5"%s/>'
               % (self.x, self.y, self.w, self.h, fill, stroke, dash)]
        if self.label:
            out.append('  <text x="%g" y="%g" text-anchor="middle" '
                       'fill="#1a1a1a">%s</text>'
                       % (self.cx, self.y + 30, escape(self.label)))
        return out


class Figure:
    def __init__(self, width=WIDTH, margin=14.0):
        self.width = width
        self.margin = margin
        self.blocks = []            # drawn in order
        self.containers = []
        self.edges = []             # (points, colour, dashed)
        self.labels = []            # loose text, for endpoint names
        self.dots = []

    # ---------------------------------------------------------------- layout
    def row(self, blocks, y=None, gap=GAP, x=None, centre=True, cy=None):
        """Place blocks left to right on one row, and return them.

        The row sits at the top y, or is centred on cy. A container in the
        row takes what it holds along with it."""
        total = sum(b.w for b in blocks) + gap * (len(blocks) - 1)
        x = x if x is not None else (
            (self.width - total) / 2 if centre else self.margin)
        for b in blocks:
            top = y if cy is None else cy - b.h / 2
            if isinstance(b, Container):
                b.move(x - b.x, top - b.y)
            else:
                b.x, b.y = x, top
                if b not in self.blocks:
                    self.blocks.append(b)
            x += b.w + gap
        return blocks

    def stack(self, label, members, width=None, kind='component',
              height=36, gap=8, member='member'):
        """A container holding a column of alike members, such as the
        voices in an array, with or without a title. Place it with row()."""
        width = width or max(text_width(t) for t in [label or ''] + members
                             ) + 2 * PAD
        pad = 14
        for i, name in enumerate(members):
            self.place(Block(name, member, width=width, height=height),
                       0, 30 + pad + i * (height + gap))
        return self.container(label, self.blocks[-len(members):], pad=pad,
                              kind=kind)

    def place(self, block, x, y):
        block.x, block.y = x, y
        self.blocks.append(block)
        return block

    def container(self, label, children, pad=20, title_gap=30,
                  kind='container', dashed=False):
        c = Container(label, children, pad, title_gap, kind, dashed)
        c.fit()
        self.containers.append(c)
        return c

    def label(self, text, x, y, anchor='start', colour='#1a1a1a'):
        self.labels.append((text, x, y, anchor, colour))

    def dot(self, x, y, kind='signal'):
        """Where a line branches."""
        self.dots.append((x, y, LINES[kind]))

    # the ends of the figure: what comes in and what goes out, as text
    def source(self, text, b, kind='signal', side='left', length=34,
               dashed=False, at=None):
        if side == 'left':
            y = b.cy if at is None else at
            self.path([(b.left - length, y), (b.left, y)], kind, dashed)
            self.label(text, b.left - length - 6, y + 0.35 * FONT, 'end')
        elif side == 'top':
            x = b.cx if at is None else at
            self.path([(x, b.top - length), (x, b.top)], kind, dashed)
            self.label(text, x, b.top - length - 8, 'middle')
        else:                                           # from below
            x = b.cx if at is None else at
            self.path([(x, b.bottom + length), (x, b.bottom)], kind, dashed)
            self.label(text, x, b.bottom + length + 18, 'middle')

    def sink(self, b, text, kind='signal', length=34, at=None):
        y = b.cy if at is None else at
        self.path([(b.right, y), (b.right + length, y)], kind)
        self.label(text, b.right + length + 6, y + 0.35 * FONT)

    def tap(self, x, y, top, text, kind='signal'):
        """An output taken upwards from a line, named at the top."""
        self.path([(x, y), (x, top)], kind)
        self.label(text, x, top - 8, 'middle')

    # ----------------------------------------------------------------- edges
    def arrow(self, a, b, kind='signal', dashed=False, lane=None,
              enter=None, leave=None):
        """Connect a to b, choosing a route that keeps clear of the blocks."""
        colour = LINES[kind]
        if b.left >= a.right - 1:                      # b is to the right
            if abs(a.cy - b.cy) < 1:
                pts = [(a.right, a.cy), (b.left, b.cy)]
            else:
                mid = max(a.right + 14, min((a.right + b.left) / 2,
                                            b.left - 32))
                pts = [(a.right, a.cy), (mid, a.cy), (mid, b.cy),
                       (b.left, b.cy)]
        elif b.top >= a.bottom - 1:                    # b is below
            x = enter if enter is not None else leave
            if x is None and a.left < b.right and b.left < a.right:
                x = max(a.left, b.left) + (min(a.right, b.right)
                                           - max(a.left, b.left)) / 2
            if x is not None and a.left <= x <= a.right and b.left <= x <= b.right:
                pts = [(x, a.bottom), (x, b.top)]
            else:
                ly = lane if lane is not None else max(
                    a.bottom + 14, min((a.bottom + b.top) / 2, b.top - 32))
                lx = leave if leave is not None else a.cx
                ex = enter if enter is not None else b.cx
                pts = [(lx, a.bottom), (lx, ly), (ex, ly), (ex, b.top)]
        elif b.bottom <= a.top + 1:                    # b is above
            x = enter if enter is not None else leave
            if x is None and a.left < b.right and b.left < a.right:
                x = max(a.left, b.left) + (min(a.right, b.right)
                                           - max(a.left, b.left)) / 2
            if x is not None and a.left <= x <= a.right and b.left <= x <= b.right:
                pts = [(x, a.top), (x, b.bottom)]
            else:
                ly = lane if lane is not None else min(
                    a.top - 14, max((a.top + b.bottom) / 2, b.bottom + 32))
                lx = leave if leave is not None else a.cx
                ex = enter if enter is not None else b.cx
                pts = [(lx, a.top), (lx, ly), (ex, ly), (ex, b.bottom)]
        else:                                          # b is to the left
            pts = [(a.left, a.cy), (b.right, b.cy)]
        self.edges.append((pts, colour, dashed))
        return pts

    def path(self, points, kind='signal', dashed=False, head=True):
        """A route given outright, for the few that need saying."""
        self.edges.append((list(points), LINES[kind], dashed, head))

    # ---------------------------------------------------------------- output
    # ------------------------------------------------------------- ports
    def _sides(self):
        """Every side a connector can land on: (block, side, axis, lo, hi)."""
        for b in self.blocks + self.containers:
            if b.shape == 'circle':
                continue                        # a circle takes its centre
            yield b, 'left', b.left, b.top, b.bottom
            yield b, 'right', b.right, b.top, b.bottom
            yield b, 'top', b.top, b.left, b.right
            yield b, 'bottom', b.bottom, b.left, b.right

    def _spread(self):
        """Space the connectors on each side of a block evenly: with N of
        them on a side of length L, at L*k/(N+1), k = 1..N."""
        edges = [list(e) for e in self.edges]
        for e in edges:
            e[0] = [list(p) for p in e[0]]
        moves = {}                  # (edge, end) -> new coordinate
        for b, side, at, lo, hi in self._sides():
            vertical = side in ('left', 'right')   # the side runs up and down
            ends = []
            for i, e in enumerate(edges):
                pts = e[0]
                for end, near in ((0, 1), (-1, -2)):
                    p, q = pts[end], pts[near]
                    if vertical:
                        on = abs(p[0] - at) < 0.6 and lo - 0.6 <= p[1] <= hi + 0.6
                        on = on and abs(q[1] - p[1]) < 0.6   # arrives across
                        key = pts[-1 - end][1] if end == 0 else pts[0][1]
                    else:
                        on = abs(p[1] - at) < 0.6 and lo - 0.6 <= p[0] <= hi + 0.6
                        on = on and abs(q[0] - p[0]) < 0.6
                        key = pts[-1 - end][0] if end == 0 else pts[0][0]
                    if on:
                        ends.append((key, p[1] if vertical else p[0], i, end))
            ends.sort()
            for k, (_, _, i, end) in enumerate(ends):
                moves[(i, end)] = lo + (hi - lo) * (k + 1) / (len(ends) + 1)

        for i, e in enumerate(edges):
            pts = e[0]
            s, t = moves.get((i, 0)), moves.get((i, -1))
            if s is None and t is None:
                continue
            horizontal = abs(pts[0][1] - pts[1][1]) < 0.6
            c = 1 if horizontal else 0          # the coordinate a move sets
            if len(pts) == 2:
                a, z = pts
                s = a[c] if s is None else s
                t = z[c] if t is None else t
                a[c], z[c] = s, t
                if abs(s - t) > 0.6:            # the ends disagree: jog
                    o = 1 - c
                    mid = z[o] - (32 if z[o] > a[o] else -32)
                    if abs(mid - a[o]) < 14:
                        mid = (a[o] + z[o]) / 2
                    j1, j2 = [0, 0], [0, 0]
                    j1[o], j1[c] = mid, s
                    j2[o], j2[c] = mid, t
                    e[0] = [a, j1, j2, z]
                continue
            if s is not None:
                pts[0][c] = pts[1][c] = s
            if t is not None:
                c2 = 1 if abs(pts[-1][1] - pts[-2][1]) < 0.6 else 0
                pts[-1][c2] = pts[-2][c2] = t
        self.edges = [tuple([[tuple(p) for p in e[0]]] + e[1:]) for e in edges]

    def height(self):
        """Just tall enough for everything placed, plus the margin."""
        return max([b.bottom for b in self.blocks + self.containers]
                   + [y for e in self.edges for _, y in e[0]]
                   + [l[2] + 4 for l in self.labels]) + self.margin

    def svg(self, height=None):
        self._spread()
        height = height or self.height()
        out = ['<svg xmlns="http://www.w3.org/2000/svg" width="%g" '
               'height="%g"' % (self.width, height),
               '     viewBox="0 0 %g %g"' % (self.width, height),
               "     font-family='%s' font-size=\"%g\">" % (FONT_FAMILY, FONT),
               '']
        # outermost first, so a container never hides one it holds
        for c in sorted(self.containers, key=lambda c: -c.w * c.h):
            out += c.svg()
            out.append('')
        for b in self.blocks:
            out += b.svg()
            out.append('')
        heads = []
        for edge in self.edges:
            pts, colour, dashed = edge[0], edge[1], edge[2]
            head = edge[3] if len(edge) > 3 else True
            dash = ' stroke-dasharray="5,3"' if dashed else ''
            if len(pts) == 2:
                (x1, y1), (x2, y2) = pts
                out.append('  <line x1="%g" y1="%g" x2="%g" y2="%g" '
                           'stroke="%s" stroke-width="1.4"%s/>'
                           % (x1, y1, x2, y2, colour, dash))
            else:
                out.append('  <path d="%s" fill="none" stroke="%s" '
                           'stroke-width="1.4"%s/>'
                           % (rounded(pts), colour, dash))
            if head:
                (px, py), (tx, ty) = pts[-2], pts[-1]
                dx, dy = tx - px, ty - py
                n = (dx * dx + dy * dy) ** 0.5 or 1
                dx, dy = dx / n, dy / n
                bx, by = tx - HEAD * dx, ty - HEAD * dy
                heads.append('  <polygon points="%g,%g %g,%g %g,%g" '
                             'fill="%s"/>'
                             % (tx, ty, bx - 4 * dy, by + 4 * dx,
                                bx + 4 * dy, by - 4 * dx, colour))
        for x, y, colour in self.dots:
            out.append('  <circle cx="%g" cy="%g" r="3.5" fill="%s"/>'
                       % (x, y, colour))
        out.append('')
        for text, x, y, anchor, colour in self.labels:
            a = ' text-anchor="%s"' % anchor if anchor != 'start' else ''
            out.append('  <text x="%g" y="%g"%s fill="%s">%s</text>'
                       % (x, round(y, 1), a, colour, escape(text)))
        if heads:
            out.append('')
            out += heads
        out.append('</svg>')
        return '\n'.join(out) + '\n'

    def write(self, path, height=None):
        open(path, 'w').write(self.svg(height))
        return path
