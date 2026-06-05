#!/usr/bin/env python3
"""
Reconstruct element-traversal reports from the Elements trace log.

The `trace` log category (enable with CYCFI_LOG=trace=debug) writes one JSON
record per line to <log-dir>/<app>-trace.jsonl. Each input gesture (a click or
drag) is a group of records sharing an "ev" id; enter/exit records nest to form
the path the event took down the element tree, and a terminal "leaf" record
names the control that handled it.

This tool pairs those records back into a call tree and prints, per gesture, a
structured report showing each call's arguments and return value.

Usage:
    python3 tools/trace_graph.py [FILE] [--mermaid]

    FILE      trace .jsonl (default: ~/Library/Logs/elements/elements-trace.jsonl)
    --mermaid emit a Mermaid flowchart per gesture instead of the report
"""
import argparse
import json
import os
import sys
from collections import OrderedDict


def load_gestures(path):
    gestures = OrderedDict()  # ev -> records in seq order
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                rec = json.loads(line)
            except json.JSONDecodeError:
                continue
            gestures.setdefault(rec["ev"], []).append(rec)
    for ev in gestures:
        gestures[ev].sort(key=lambda r: r["seq"])
    return gestures


def gesture_method(records):
    for r in records:
        if "method" in r:
            return r["method"]
    return "?"


def build_tree(records):
    """Pair enter/exit (and leaf) records into a nested call tree."""
    roots = []
    stack = []
    for r in records:
        phase = r["phase"]
        if phase in ("enter", "leaf"):
            node = {
                "class": r["class"],
                "method": r["method"],
                "bounds": r.get("bounds"),
                "args": r.get("args"),
                "ret": r.get("ret"),          # leaf carries it inline
                "routed_to": None,
                "leaf": phase == "leaf",
                "children": [],
            }
            (stack[-1]["children"] if stack else roots).append(node)
            if phase == "enter":
                stack.append(node)
        elif phase == "exit" and stack:
            node = stack.pop()
            if "ret" in r:
                node["ret"] = r["ret"]
            if "index" in r:
                node["routed_to"] = r["index"]
    return roots


def fmt(v):
    if isinstance(v, bool):
        return "true" if v else "false"
    return str(v)


def emit_node(node, findent, my_index, parent_args, dash):
    lead = (" " * (findent - 2) + "- ") if dash else (" " * findent)
    print(f"{lead}call: {node['class']}.{node['method']}")
    fp = " " * findent
    if node["leaf"]:
        print(f"{fp}leaf: true")
    if my_index is not None:
        print(f"{fp}index: {my_index}")
    if node["bounds"]:
        print(f"{fp}bounds: {node['bounds']}")
    # Only emit an argument that differs from the parent's: invariant args appear
    # once at the root; a rare changed one (e.g. down on release) shows where it
    # changes.
    changed = {k: v for k, v in (node["args"] or {}).items()
               if parent_args is None or parent_args.get(k) != v}
    if changed:
        print(f"{fp}args:")
        for k, v in changed.items():
            print(f"{fp}  {k}: {fmt(v)}")
    if node["ret"] is not None:
        print(f"{fp}return: {fmt(node['ret'])}")
    if node["children"]:
        print(f"{fp}children:")
        for c in node["children"]:
            emit_node(c, findent + 4, node["routed_to"], node["args"], True)


def print_report(ev, records):
    print("---")
    print(f"gesture: {ev}")
    print(f"method: {gesture_method(records)}")
    for root in build_tree(records):
        emit_node(root, 0, None, None, False)


def print_mermaid(ev, records):
    print(f"%% gesture {ev} ({gesture_method(records)})")
    print("flowchart TD")

    def nid(ptr):
        return "n" + ptr.replace("0x", "")

    handled = {}
    for r in records:
        if r["phase"] in ("exit", "leaf") and "ret" in r:
            handled[r["node"]] = r["ret"]

    seen = set()
    for r in records:
        if r["phase"] not in ("enter", "leaf"):
            continue
        n = nid(r["node"])
        if n not in seen:
            seen.add(n)
            label = r["class"] + (" ✓" if handled.get(r["node"]) else "")
            shape = (f'(["{label}"])' if r["phase"] == "leaf" else f'["{label}"]')
            print(f"    {n}{shape}")

    stack = []
    for r in records:
        phase = r["phase"]
        if phase == "exit":
            if stack:
                stack.pop()
            continue
        if stack:
            print(f"    {nid(stack[-1])} --> {nid(r['node'])}")
        if phase == "enter":
            stack.append(r["node"])
    print()


def main():
    default = os.path.expanduser("~/Library/Logs/elements/elements-trace.jsonl")
    ap = argparse.ArgumentParser(description="Reconstruct element-traversal reports.")
    ap.add_argument("file", nargs="?", default=default)
    ap.add_argument("--mermaid", action="store_true", help="emit Mermaid flowcharts")
    args = ap.parse_args()

    if not os.path.exists(args.file):
        sys.exit(f"trace file not found: {args.file}")

    gestures = load_gestures(args.file)
    if not gestures:
        sys.exit("no gestures in trace file")

    for ev, records in gestures.items():
        (print_mermaid if args.mermaid else print_report)(ev, records)


if __name__ == "__main__":
    main()
