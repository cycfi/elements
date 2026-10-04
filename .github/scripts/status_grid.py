#!/usr/bin/env python3
###############################################################################
#  Copyright (c) 2016-2026 Joel de Guzman
#
#  Distributed under the MIT License (https://opensource.org/licenses/MIT)
###############################################################################
# Draws a workflow run's jobs as a status grid, an SVG: a title bar, then a
# cell per job, its name beside a green "pass" or a red "fail". The jobs come
# as JSON lines of {"name": ..., "conclusion": ...}, as `gh api` lists them:
#
#   gh api repos/$REPO/actions/runs/$RUN/jobs --paginate \
#      --jq '.jobs[] | {name, conclusion}' > jobs.jsonl
#   status_grid.py --title "Elements build" --exclude Status \
#      --out build.svg jobs.jsonl
#
# A job that succeeded passes. One skipped, cancelled or still running has no
# result and is left out; any other conclusion fails.

import argparse
import html
import json
import re

LABEL_BG = '#555'
PASS_BG = '#4c1'
FAIL_BG = '#e05d44'
TITLE_BG = '#444'
TEXT = '#fff'

FONT = 'Verdana,DejaVu Sans,Geneva,sans-serif'
CHAR_W = 7.2          # average glyph width at 12 px Verdana
CELL_H = 26
TITLE_H = 36
PAD = 10
GAP = 1

NO_RESULT = {None, 'skipped', 'cancelled', 'neutral'}


def text_width(s):
    return int(len(s) * CHAR_W) + 2 * PAD


def load(path, exclude):
    entries = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            job = json.loads(line)
            name, conclusion = job['name'], job.get('conclusion')
            if exclude and re.search(exclude, name):
                continue
            if conclusion in NO_RESULT:
                continue
            entries.append((name, conclusion == 'success'))
    return sorted(entries)


def render(title, entries, columns):
    columns = max(1, min(columns, len(entries)))
    rows = (len(entries) + columns - 1) // columns
    label_w = max([text_width(n) for n, _ in entries] + [60])
    value_w = text_width('fail')
    col_w = label_w + value_w
    width = max(columns * col_w + (columns - 1) * GAP, text_width(title))
    height = TITLE_H + rows * (CELL_H + GAP)

    out = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" '
        f'height="{height}" role="img" aria-label="{html.escape(title)}">',
        f'<title>{html.escape(title)}</title>',
        f'<rect width="{width}" height="{height}" rx="4" fill="{TITLE_BG}"/>',
        f'<g fill="{TEXT}" font-family="{FONT}" text-anchor="middle">',
        f'<text x="{width / 2}" y="{TITLE_H / 2 + 7}" font-size="18" '
        f'font-weight="bold">{html.escape(title)}</text>',
    ]

    for i, (name, ok) in enumerate(entries):
        x = (i // rows) * (col_w + GAP)
        y = TITLE_H + (i % rows) * (CELL_H + GAP)
        out += [
            f'<rect x="{x}" y="{y}" width="{label_w}" height="{CELL_H}" '
            f'fill="{LABEL_BG}"/>',
            f'<rect x="{x + label_w}" y="{y}" width="{value_w}" '
            f'height="{CELL_H}" fill="{PASS_BG if ok else FAIL_BG}"/>',
            f'<text x="{x + label_w - PAD}" y="{y + 17}" font-size="12" '
            f'text-anchor="end">{html.escape(name)}</text>',
            f'<text x="{x + label_w + value_w / 2}" y="{y + 17}" '
            f'font-size="12">{"pass" if ok else "fail"}</text>',
        ]

    out += ['</g>', '</svg>']
    return '\n'.join(out) + '\n'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--title', required=True)
    ap.add_argument('--columns', type=int, default=2)
    ap.add_argument('--exclude', help='leave out jobs whose name matches')
    ap.add_argument('--out', required=True)
    ap.add_argument('jobs', help='JSON lines of {name, conclusion}')
    args = ap.parse_args()

    entries = load(args.jobs, args.exclude)
    if not entries:
        raise SystemExit('status_grid: no job results')
    with open(args.out, 'w') as f:
        f.write(render(args.title, entries, args.columns))


if __name__ == '__main__':
    main()
