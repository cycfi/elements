#!/usr/bin/env python3
"""
Draw the documentation's sequence diagrams on blockdiag.py.

A sequence diagram shows calls in time order: the parties across the top,
a dashed lifeline down from each, and one arrow per call, top to bottom,
labelled with the call. It keeps blockdiag's rules (one font size, boxes
sized around their labels, baked arrowheads with a stem, the page's own
width) and adds what a sequence needs:

    labelled calls              a call names itself; the label sits above
                                its arrow, centred in the gap next to the
                                caller, so it crosses no lifeline
    columns that fit            lifelines are spaced so every label fits
                                between the two it joins
    calls to self               a short loop on the right of the lifeline,
                                labelled beside it

Figures are written as small scripts (see gen_block_figures.py) and checked
with check_figures.py.
"""

from blockdiag import Figure, Block, GAP, PAD, HEAD, FONT, text_width

STEP = 38.0             # between calls
LOOP = 36.0             # how far a call to self reaches right
LOOP_H = 20.0           # and down


class Sequence:
    def __init__(self, parties):
        """parties: (label, kind) pairs, left to right."""
        self.parties = [Block(label, kind) for label, kind in parties]
        self.calls = []

    def call(self, a, b, text, kind='signal'):
        """A call from party a to party b (indices); a == b calls itself."""
        self.calls.append((a, b, text, kind))

    def _spacing(self):
        p = self.parties
        gaps = [p[i].w / 2 + p[i + 1].w / 2 + GAP for i in range(len(p) - 1)]
        right = p[-1].w / 2          # room needed right of the last lifeline
        for a, b, text, _ in self.calls:
            if a == b:
                need = LOOP + 8 + text_width(text) + PAD
                if a == len(p) - 1:
                    right = max(right, need)
                else:
                    gaps[a] = max(gaps[a], need)
                continue
            # The label sits in the gap next to the caller, so it never
            # crosses a lifeline the call passes over.
            g = a if b > a else a - 1
            need = text_width(text) + 2 * PAD + HEAD
            gaps[g] = max(gaps[g], need)
        return gaps, right

    def write(self, path):
        f = Figure()
        p = self.parties
        gaps, right = self._spacing()
        total = p[0].w / 2 + sum(gaps) + right
        x = (f.width - total) / 2 + p[0].w / 2      # the first lifeline
        xs = [x]
        for g in gaps:
            x += g
            xs.append(x)
        for b, cx in zip(p, xs):
            f.place(b, cx - b.w / 2, f.margin)

        y = max(b.bottom for b in p) + STEP
        for a, b, text, kind in self.calls:
            if a == b:
                x0 = xs[a]
                f.path([(x0, y), (x0 + LOOP, y), (x0 + LOOP, y + LOOP_H),
                        (x0, y + LOOP_H)], kind)
                f.label(text, x0 + LOOP + 8, y + LOOP_H / 2 + 0.35 * FONT)
                y += LOOP_H + STEP
            else:
                f.path([(xs[a], y), (xs[b], y)], kind)
                n = a + 1 if b > a else a - 1       # the caller's neighbour
                f.label(text, (xs[a] + xs[n]) / 2, y - 7, anchor='middle')
                y += STEP

        bottom = y - STEP / 2
        for b, cx in zip(p, xs):
            f.path([(cx, b.bottom), (cx, bottom)], 'plain', dashed=True,
                   head=False)
        return f.write(path)
