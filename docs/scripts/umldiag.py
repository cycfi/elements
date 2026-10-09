#!/usr/bin/env python3
"""
Draw the documentation's class diagrams on blockdiag.py, in UML.

A class is a box with its name on top and, below a rule, the members the
page talks about. Two relations:

    inherits                a line from the subclass up to the base, ending
                            in a hollow triangle at the base
    holds                   a line from the owner to what it holds by value,
                            starting with a filled diamond at the owner,
                            labelled with the member's name

Everything else (one font, the page's width, colours) is blockdiag's.
Figures are written as small scripts (see gen_block_figures.py) and
checked with check_figures.py.
"""

from blockdiag import Figure, Block, STYLES, LINES, FONT, LINE, PAD, escape

HEAD = 11.0             # the triangle's and the diamond's reach
RULE_GAP = 6.0          # above and below the rule between compartments


class Class(Block):
    def __init__(self, name, members=(), kind='component', abstract=False):
        self.name = name
        self.members = list(members)
        self.abstract = abstract
        lines = [name] + self.members
        label = '\n'.join(lines)
        super().__init__(label, kind=kind)
        if self.members:
            self.h += 2 * RULE_GAP
        self.w = max(self.w, 96)

    def svg(self):
        fill, stroke = STYLES[self.kind]
        out = ['  <rect x="%g" y="%g" width="%g" height="%g" rx="4" '
               'fill="%s" stroke="%s" stroke-width="1.5"/>'
               % (self.x, self.y, self.w, self.h, fill, stroke)]
        y = self.y + LINE + 0.35 * FONT + 4
        style = ' font-style="italic"' if self.abstract else ''
        out.append('  <text x="%g" y="%g" text-anchor="middle"%s '
                   'fill="#1a1a1a">%s</text>'
                   % (self.cx, round(y, 1), style, escape(self.name)))
        if self.members:
            ry = y + RULE_GAP + 4
            out.append('  <line x1="%g" y1="%g" x2="%g" y2="%g" '
                       'stroke="%s" stroke-width="1"/>'
                       % (self.x, ry, self.right, ry, stroke))
            y = ry + RULE_GAP + 0.35 * FONT + 4
            for m in self.members:
                out.append('  <text x="%g" y="%g" fill="#1a1a1a">%s</text>'
                           % (self.x + PAD / 2, round(y, 1), escape(m)))
                y += LINE
        return out


class ClassDiagram(Figure):
    def __init__(self, *args, **kw):
        super().__init__(*args, **kw)
        self.markers = []
        self._stems = []

    def centre(self):
        """Shift what is placed so far to the middle of the page. Call
        it before drawing the relations."""
        lo = min(b.left for b in self.blocks)
        hi = max(b.right for b in self.blocks)
        dx = (self.width - (hi - lo)) / 2 - lo
        for b in self.blocks:
            b.move(dx, 0)

    def inherits(self, sub, base, side='bottom', lane=False):
        """sub derives from base. The line enters the base from below
        (sub under it) or from the right (sub off to the right). With
        lane, siblings join a shared lane under the base and one
        triangle."""
        if side == 'right':
            y = base.cy
            pts = [(sub.cx, sub.top), (sub.cx, y), (base.right + HEAD, y)]
            self.path(pts, 'plain', head=False)
            self.markers.append(('triangle', (base.right, y), (-1, 0)))
            return
        lo, hi = max(sub.left, base.left), min(sub.right, base.right)
        if lane and abs(sub.cx - base.cx) < 1:  # the middle sibling
            pts = [(sub.cx, sub.top), (sub.cx, base.bottom + HEAD)]
            tip = (sub.cx, base.bottom)
        elif hi - lo > 20 and not lane:         # straight up
            x = lo + (hi - lo) / 2
            pts = [(x, sub.top), (x, base.bottom + HEAD)]
            tip = (x, base.bottom)
        else:                                   # up, across: siblings
            lane_y = base.bottom + 22           # share one stem, one triangle
            pts = [(sub.cx, sub.top), (sub.cx, lane_y), (base.cx, lane_y)]
            tip = (base.cx, base.bottom)
            stem = [(base.cx, lane_y), (base.cx, base.bottom + HEAD)]
            if stem not in self._stems:
                self._stems.append(stem)
                self.path(stem, 'plain', head=False)
        self.path(pts, 'plain', head=False)
        if ('triangle', tip, (0, -1)) not in self.markers:
            self.markers.append(('triangle', tip, (0, -1)))

    def holds(self, owner, held, text):
        """owner holds held by value: a diamond at the owner's right, a
        line across and, if held sits higher, up into its bottom."""
        y = owner.cy
        start = owner.right + HEAD + 3
        if held.bottom < y:
            pts = [(start, y), (held.cx, y), (held.cx, held.bottom)]
            self.label(text, (start + held.cx) / 2, y - 6, anchor='middle')
        else:
            pts = [(start, y), (held.left, y)]
            self.label(text, (start + held.left) / 2, y - 6, anchor='middle')
        self.path(pts, 'plain', head=False)
        self.markers.append(('diamond', (owner.right, y), (1, 0)))

    def refers(self, owner, target, text, side='right', dashed=False):
        """owner refers to target without owning it: a plain line on one
        row, from the owner to the target on its right (or left), an open
        arrowhead at the target, the member's name above."""
        y = owner.cy
        if side == 'right':
            pts = [(owner.right, y), (target.left, y)]
            tip, d = (target.left, y), (1, 0)
        else:
            pts = [(owner.left, y), (target.right, y)]
            tip, d = (target.right, y), (-1, 0)
        self.label(text, (pts[0][0] + pts[1][0]) / 2, y - 6, anchor='middle')
        self.path(pts, 'plain', dashed=dashed, head=False)
        self.markers.append(('open', tip, d))

    def svg(self, height=None):
        s = super().svg(height)
        colour = LINES['plain']
        out = []
        for shape, (x, y), (dx, dy) in self.markers:
            if shape == 'triangle':         # tip at (x, y), base HEAD away
                bx, by = x - HEAD * dx, y - HEAD * dy
                out.append('  <polygon points="%g,%g %g,%g %g,%g" '
                           'fill="#ffffff" stroke="%s" stroke-width="1.4"/>'
                           % (x, y, bx - 6 * dy, by + 6 * dx,
                              bx + 6 * dy, by - 6 * dx, colour))
            elif shape == 'open':           # two strokes, tip at (x, y)
                bx, by = x - HEAD * dx, y - HEAD * dy
                out.append('  <polyline points="%g,%g %g,%g %g,%g" '
                           'fill="none" stroke="%s" stroke-width="1.4"/>'
                           % (bx - 5 * dy, by + 5 * dx, x, y,
                              bx + 5 * dy, by - 5 * dx, colour))
            else:                           # diamond from (x, y) outward
                d = HEAD + 3
                mx, my = x + d / 2 * dx, y + d / 2 * dy
                ex, ey = x + d * dx, y + d * dy
                out.append('  <polygon points="%g,%g %g,%g %g,%g %g,%g" '
                           'fill="%s"/>'
                           % (x, y, mx - 5 * dy, my + 5 * dx, ex, ey,
                              mx + 5 * dy, my - 5 * dx, colour))
        return s.replace('</svg>', '\n'.join(out) + '\n</svg>')
