#!/usr/bin/env python3
"""
Generate the documentation's block diagrams with blockdiag.py.

Produces, in docs/modules/ROOT/images/ (or the directory given):

   support/context_chain.svg   -- the contexts a draw passes down a view
   support/receiver_gui.svg    -- a drag: the value and the gesture
   support/receiver_button.svg -- a click: the value on release
   support/receiver_code.svg   -- from code: value, edit and the idle end
   support/receiver_button_code.svg -- from code: a button's edit
   elements/proxy_draw.svg     -- a proxy forwarding draw to its subject
   elements/proxy_classes.svg  -- the proxy classes, in UML
   elements/indirect_classes.svg -- the indirect classes, in UML
   elements/traversal_down.svg -- find_subject, down through subjects
   elements/traversal_up.svg   -- find_parent, up through parent contexts
   elements/composite_classes.svg -- the composite classes, in UML
   elements/composite_click.svg -- a composite dispatching a click
   elements/selection.svg      -- a selection list over a composite of items
   elements/tracker_classes.svg -- tracker and the controls built on it, in UML
   elements/tracker_drag.svg   -- a drag through a tracker
   styles/styler_classes.svg   -- a control and its styler, in UML
   styles/styler_pull.svg      -- a styler reading its control's state
   styles/styler_place.svg     -- a control placing its stylers
   styles/styler_push.svg      -- a control passing its value to its styler

Usage: python3 docs/scripts/gen_block_figures.py [out_dir]
"""

import os
import sys

from blockdiag import Figure, Block, GAP
from seqdiag import Sequence
from umldiag import ClassDiagram, Class, HEAD as HEAD_UML

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = (sys.argv[1] if len(sys.argv) > 1 else
       os.path.join(HERE, '..', 'modules', 'ROOT', 'images'))


def context_chain():
    # The view and the canvas feed the top context from either side; each
    # container makes its child's context from its own, one level at a time,
    # down to the slider.
    f = Figure()
    ctxs = [Block("vtile's context"), Block("margin's context"),
            Block("slider's context")]
    w = max(b.w for b in ctxs)
    for b in ctxs:
        b.w = w
    view, canvas = Block('view', 'plain'), Block('canvas', 'plain')
    f.row([view, ctxs[0], canvas], y=f.margin)
    y = ctxs[0].bottom + GAP
    for b in ctxs[1:]:
        f.place(b, ctxs[0].x, y)
        y = b.bottom + GAP

    f.arrow(view, ctxs[0])
    f.arrow(canvas, ctxs[0])
    for a, b in zip(ctxs, ctxs[1:]):
        f.arrow(a, b)

    os.makedirs(os.path.join(OUT, 'support'), exist_ok=True)
    return f.write(os.path.join(OUT, 'support', 'context_chain.svg'))


def receiver_gui():
    # A drag on a slider: the value goes to the model through on_change, and
    # the gesture goes to on_tracking, bracketed by the press and release.
    seq = Sequence([('user', 'input'), ('slider', 'component'),
                    ('model', 'component'), ('view', 'component'),
                    ('on_tracking', 'yours')])
    seq.call(0, 1, 'mouse down', 'midi')
    seq.call(1, 3, 'begin_tracking')
    seq.call(3, 4, 'begin_tracking', 'control')
    seq.call(0, 1, 'drag', 'midi')
    seq.call(1, 1, 'value(0.6)')
    seq.call(1, 2, 'on_change(0.6)')
    seq.call(2, 1, 'value(0.6)')
    seq.call(1, 3, 'while_tracking')
    seq.call(3, 4, 'while_tracking', 'control')
    seq.call(0, 1, 'mouse up', 'midi')
    seq.call(1, 3, 'end_tracking')
    seq.call(3, 4, 'end_tracking', 'control')
    os.makedirs(os.path.join(OUT, 'support'), exist_ok=True)
    return seq.write(os.path.join(OUT, 'support', 'receiver_gui.svg'))


def receiver_button():
    # A click on a toggle: the gesture is the press and release; the press
    # flips the value, and it goes out once, on release. No while_tracking.
    seq = Sequence([('user', 'input'), ('toggle', 'component'),
                    ('model', 'component'), ('view', 'component'),
                    ('on_tracking', 'yours')])
    seq.call(0, 1, 'mouse down', 'midi')
    seq.call(1, 3, 'begin_tracking')
    seq.call(3, 4, 'begin_tracking', 'control')
    seq.call(1, 1, 'value(true)')
    seq.call(0, 1, 'mouse up', 'midi')
    seq.call(1, 2, 'on_click(true)')
    seq.call(2, 1, 'value(true)')
    seq.call(1, 3, 'end_tracking')
    seq.call(3, 4, 'end_tracking', 'control')
    os.makedirs(os.path.join(OUT, 'support'), exist_ok=True)
    return seq.write(os.path.join(OUT, 'support', 'receiver_button.svg'))


def receiver_code():
    # From code: a model update reaches the control quietly through value;
    # edit counts as the user's, so it feeds the model and is reported, and
    # the view ends the gesture after a second with no change.
    seq = Sequence([('your code', 'yours'), ('model', 'component'),
                    ('slider', 'component'), ('view', 'component'),
                    ('on_tracking', 'yours')])
    seq.call(0, 1, 'model = 0.3')
    seq.call(1, 2, 'value(0.3)')
    seq.call(0, 2, 'edit(view, 0.75)')
    seq.call(2, 2, 'value(0.75)')
    seq.call(2, 1, 'on_change(0.75)')
    seq.call(1, 2, 'value(0.75)')
    seq.call(2, 3, 'manage_on_tracking')
    seq.call(3, 4, 'begin_tracking', 'control')
    seq.call(3, 4, 'while_tracking', 'control')
    seq.call(3, 3, 'poll(), 1 s idle')
    seq.call(3, 4, 'end_tracking', 'control')
    os.makedirs(os.path.join(OUT, 'support'), exist_ok=True)
    return seq.write(os.path.join(OUT, 'support', 'receiver_code.svg'))


def receiver_button_code():
    # From code: a toggle's edit sets its value and calls on_click, which a
    # binding turns into a model update; then the gesture.
    seq = Sequence([('your code', 'yours'), ('model', 'component'),
                    ('toggle', 'component'), ('view', 'component'),
                    ('on_tracking', 'yours')])
    seq.call(0, 1, 'model = true')
    seq.call(1, 2, 'value(true)')
    seq.call(0, 2, 'edit(view, true)')
    seq.call(2, 2, 'value(true)')
    seq.call(2, 1, 'on_click(true)')
    seq.call(1, 2, 'value(true)')
    seq.call(2, 3, 'manage_on_tracking')
    seq.call(3, 4, 'begin_tracking', 'control')
    seq.call(3, 4, 'while_tracking', 'control')
    seq.call(3, 3, 'poll(), 1 s idle')
    seq.call(3, 4, 'end_tracking', 'control')
    os.makedirs(os.path.join(OUT, 'support'), exist_ok=True)
    return seq.write(os.path.join(OUT, 'support', 'receiver_button_code.svg'))


def proxy_draw():
    # A proxy forwarding draw: it makes the subject's context from its own,
    # adjusts it in prepare_subject, hands it down, and restores it after.
    seq = Sequence([('view', 'component'), ('margin', 'component'),
                    ('slider', 'component')])
    seq.call(0, 1, 'draw(ctx)')
    seq.call(1, 1, 'sctx = {ctx, &subject, ctx.bounds}')
    seq.call(1, 1, 'prepare_subject(sctx)')
    seq.call(1, 2, 'draw(sctx)')
    seq.call(1, 1, 'restore_subject(sctx)')
    os.makedirs(os.path.join(OUT, 'elements'), exist_ok=True)
    return seq.write(os.path.join(OUT, 'elements', 'proxy_draw.svg'))


def proxy_classes():
    # proxy_base is an element that forwards to an element, its subject;
    # proxy<Subject, Base> holds the subject by value; the library's
    # proxies derive from proxy.
    f = ClassDiagram()
    element = Class('element', abstract=True)
    base = Class('proxy_base',
                 ['subject() : element&', 'prepare_subject(ctx)',
                  'restore_subject(ctx)'])
    subject = Class('Subject : element', kind='yours')
    prox = Class('proxy<Subject, Base>', ['subject() : Subject&'])
    margin = Class('margin_element<Subject>')
    align = Class('align_element<Subject>')

    f.place(element, 0, f.margin)
    element.x = f.width / 2 - 140 - element.w / 2
    y = element.bottom + 44
    f.place(base, element.cx - base.w / 2, y)
    f.place(subject, element.cx + 190, y)
    y = base.bottom + 44
    f.place(prox, base.cx - prox.w / 2, y)
    y = prox.bottom + 44
    f.row([margin, align], y=y, gap=30, x=prox.cx - (margin.w + align.w + 30) / 2 + 10,
          centre=False)

    f.centre()
    f.inherits(base, element)
    f.inherits(subject, element, side='right')
    f.inherits(prox, base)
    f.inherits(margin, prox)
    f.inherits(align, prox)
    f.holds(prox, subject, 'subject()')
    os.makedirs(os.path.join(OUT, 'elements'), exist_ok=True)
    return f.write(os.path.join(OUT, 'elements', 'proxy_classes.svg'))


def indirect_classes():
    # indirect_base names the element delegated to; reference and
    # shared_element are the two ways of reaching it, either side of the
    # element itself; indirect<Base> adds the forwarding of the whole
    # protocol on top of either.
    f = ClassDiagram()
    element = Class('element', abstract=True)
    base = Class('indirect_base', ['get() : element&'], abstract=True)
    ref = Class('reference<Element>', ['get() : Element&'])
    shared = Class('shared_element<Element>', ['get() : Element&'])
    target = Class('Element : element', kind='yours')
    ind = Class('indirect<Base>')

    f.place(element, 0, f.margin)
    y = element.bottom + 44
    f.place(base, 0, y)
    y = base.bottom + 44
    f.row([ref, target, shared], y=y, gap=100, x=0, centre=False)
    target.y = ref.cy - target.h / 2
    mid = (ref.left + shared.right) / 2
    element.x = mid - element.w / 2
    base.x = mid - base.w / 2
    f.place(ind, mid - ind.w / 2, ref.bottom + 56)
    f.centre()

    f.inherits(base, element)
    f.inherits(ref, base)
    f.inherits(shared, base)
    f.refers(ref, target, 'Element&')
    f.refers(shared, target, 'shared_ptr', side='left')
    # indirect<Base> derives from Base, one of the two
    lane = ref.bottom + 26
    f.path([(ind.cx, ind.top), (ind.cx, lane), (ref.cx, lane),
            (ref.cx, ref.bottom + HEAD_UML)], 'plain', head=False)
    f.path([(ind.cx, lane), (shared.cx, lane),
            (shared.cx, shared.bottom + HEAD_UML)], 'plain', head=False)
    f.markers.append(('triangle', (ref.cx, ref.bottom), (0, -1)))
    f.markers.append(('triangle', (shared.cx, shared.bottom), (0, -1)))
    f.label('Base', ind.cx + 8, lane + 16)
    os.makedirs(os.path.join(OUT, 'elements'), exist_ok=True)
    return f.write(os.path.join(OUT, 'elements', 'indirect_classes.svg'))


def _traversal_chain(f):
    # One branch of a tree, top to bottom, with a sibling beside it.
    chain = [Block('htile'), Block('margin'), Block('align_center'),
             Block('hold'), Block('slider', 'yours')]
    w = max(b.w for b in chain)
    for b in chain:
        b.w = w
    x = f.width / 2 - w / 2 + 30
    y = f.margin
    for b in chain:
        f.place(b, x, y)
        y = b.bottom + 34
    sibling = Block('label', width=w)
    f.place(sibling, x + w + 60, chain[1].y)
    return chain, sibling


def traversal_down():
    # find_subject walks down through subjects, and stops at a composite's
    # children: from the margin it reaches the slider; from the htile it
    # reaches nothing.
    f = Figure()
    chain, sibling = _traversal_chain(f)
    top, bottom = chain[0], chain[-1]
    ly = (top.bottom + sibling.top) / 2
    f.path([(top.cx, top.bottom), (top.cx, ly), (sibling.cx, ly),
            (sibling.cx, sibling.top)], 'plain')
    f.path([(top.cx, ly), (chain[1].cx, ly), (chain[1].cx, chain[1].top)],
           'plain', head=True)
    f.label('children', sibling.cx + 8, ly - 4)
    for a, b in zip(chain[1:], chain[2:]):
        f.path([(a.cx, a.bottom), (b.cx, b.top)], 'signal')
    f.label('subject', chain[1].cx + 8, chain[2].top - 12)
    lx = chain[1].left - 50
    f.path([(lx, chain[1].cy), (lx, bottom.cy)], 'signal')
    f.label('find_subject<slider*>(&margin)', lx - 8, chain[1].cy + 5,
            anchor='end')
    os.makedirs(os.path.join(OUT, 'elements'), exist_ok=True)
    return f.write(os.path.join(OUT, 'elements', 'traversal_down.svg'))


def traversal_up():
    # find_parent walks up the parent contexts from the slider's own,
    # the path the current call came down, to the htile.
    f = Figure()
    chain, sibling = _traversal_chain(f)
    top, bottom = chain[0], chain[-1]
    ly = (top.bottom + sibling.top) / 2
    f.path([(sibling.cx, sibling.top), (sibling.cx, ly), (top.cx, ly),
            (top.cx, top.bottom)], 'plain', head=False)
    for a, b in zip(chain, chain[1:]):
        f.path([(b.cx, b.top), (a.cx, a.bottom)], 'control')
    f.label('parent', chain[1].cx + 8, chain[2].top - 12)
    rx = sibling.right + 50
    f.path([(rx, bottom.cy), (rx, top.cy)], 'control')
    f.label('find_parent<htile*>(ctx)', rx - 8, bottom.cy - 4, anchor='end')
    f.label('find_composite(ctx)', rx - 8, bottom.cy + 14, anchor='end')
    f.label('ctx', bottom.right + 8, bottom.cy + 5)
    os.makedirs(os.path.join(OUT, 'elements'), exist_ok=True)
    return f.write(os.path.join(OUT, 'elements', 'traversal_up.svg'))


def composite_classes():
    # composite_base is an element and a storage; composite<Container,
    # Base> supplies the storage from a std container; a layout class such
    # as vtile_element sits between, with limits, layout and bounds_of.
    f = ClassDiagram()
    element = Class('element', abstract=True)
    storage = Class('storage', ['size() : size_t', 'at(ix) : element&'],
                    abstract=True)
    base = Class('composite_base',
                 ['bounds_of(ctx, ix) : rect', 'hit_element(ctx, p, control)',
                  'for_each_visible(ctx, f)', 'focus_index() : int'],
                 abstract=True)
    vtile = Class('vtile_element',
                  ['limits(ctx)', 'layout(ctx)', 'bounds_of(ctx, ix)'])
    comp = Class('composite<Container, Base>',
                 ['size() : size_t', 'at(ix) : element&'])
    arr = Class('array_composite<N, Base>')
    vec = Class('vector_composite<Base>')

    f.row([element, storage], y=f.margin, gap=60, x=0, centre=False)
    y = max(element.bottom, storage.bottom) + 44
    f.place(base, 0, y)
    base.x = (element.cx + storage.cx) / 2 - base.w / 2
    y = base.bottom + 44
    f.place(vtile, base.cx - vtile.w / 2, y)
    y = vtile.bottom + 44
    f.place(comp, base.cx - comp.w / 2, y)
    y = comp.bottom + 44
    f.row([arr, vec], y=y, gap=30, x=0, centre=False)
    mid = base.cx
    arr.x = mid - (arr.w + 30 + vec.w) / 2
    vec.x = arr.right + 30
    f.centre()

    f.inherits(base, element)
    f.inherits(base, storage)
    f.inherits(vtile, base)
    f.inherits(comp, vtile)
    f.label('Base', comp.cx + 8, vtile.bottom + 24)
    f.inherits(arr, comp)
    f.inherits(vec, comp)
    f.label('std::array<element_ptr, N>', arr.cx, arr.bottom + 20,
            anchor='middle', colour='#5d5d5d')
    f.label('std::vector<element_ptr>', vec.cx, vec.bottom + 20,
            anchor='middle', colour='#5d5d5d')
    os.makedirs(os.path.join(OUT, 'elements'), exist_ok=True)
    return f.write(os.path.join(OUT, 'elements', 'composite_classes.svg'))


def composite_click():
    # A press on a composite: it finds the child under the point, gives it
    # the focus if it wants it, and hands it the click in a context of its
    # own; the drags and the release follow to the same child.
    seq = Sequence([('view', 'component'), ('vtile', 'component'),
                    ('child', 'yours')])
    seq.call(0, 1, 'click(ctx, btn)  btn.down')
    seq.call(1, 1, 'hit_element(ctx, btn.pos)')
    seq.call(1, 1, 'new_focus(ix)')
    seq.call(1, 2, 'click(cctx, btn)')
    seq.call(0, 1, 'drag(ctx, btn)')
    seq.call(1, 2, 'drag(cctx, btn)')
    seq.call(0, 1, 'click(ctx, btn)  !btn.down')
    seq.call(1, 2, 'click(cctx, btn)')
    os.makedirs(os.path.join(OUT, 'elements'), exist_ok=True)
    return seq.write(os.path.join(OUT, 'elements', 'composite_click.svg'))


def selection():
    # The proxy owns the policy; the items only record their state; the
    # composite between is found, not held, so a scroller may sit in the
    # way. Three items are drawn selected.
    f = Figure()
    user = Block('user', 'input')
    proxy = Block('selection_list', 'component')
    scroller = Block('vscroller', 'plain', dashed=True)
    tile = Block('vtile', 'component')
    code = Block('your code', 'yours')
    f.row([user, proxy, scroller, tile], y=f.margin + 30, gap=46)
    items = []
    y = tile.bottom + 40 + 30 + 14
    for i in range(5):
        kind = 'accent' if i in (1, 2, 3) else 'member'
        items.append(f.place(Block('item %d' % i, kind, width=90, height=30),
                             tile.cx - 45, y))
        y += 38
    stack = f.container('selectable', items, pad=14)
    f.place(code, user.x, items[0].cy - code.h / 2)

    # centre the whole drawing
    dx = (f.width - (stack.right - user.left)) / 2 - user.left
    for b in f.blocks:
        b.move(dx, 0)
    stack.move(dx, 0)

    f.arrow(user, proxy, 'midi')
    f.label('click, keys', (user.right + proxy.left) / 2, user.top - 8,
            anchor='middle')
    f.arrow(proxy, scroller, 'signal')
    f.arrow(scroller, tile, 'signal')
    lane = proxy.top - 22
    f.path([(proxy.cx, proxy.top), (proxy.cx, lane), (tile.cx, lane),
            (tile.cx, tile.top)], 'control')
    f.label('find_subject<composite_base*>', (proxy.cx + tile.cx) / 2,
            lane - 6, anchor='middle')
    f.arrow(tile, stack, 'signal')
    f.path([(proxy.cx + 30, proxy.bottom), (proxy.cx + 30, items[2].cy),
            (stack.left, items[2].cy)], 'control')
    f.label('select(true)', proxy.cx + 36, items[2].cy - 6)
    f.path([(proxy.cx - 30, proxy.bottom), (proxy.cx - 30, code.cy),
            (code.right, code.cy)], 'control')
    f.label('on_select(1, 3)', proxy.cx - 36, (proxy.bottom + code.cy) / 2 + 4,
            anchor='end')
    os.makedirs(os.path.join(OUT, 'elements'), exist_ok=True)
    return f.write(os.path.join(OUT, 'elements', 'selection.svg'))


def tracker_classes():
    # tracker<Base, TrackerInfo> derives from any element Base and owns the
    # gesture's state while it lasts; the library's draggable controls
    # derive from it.
    f = ClassDiagram()
    base = Class('Base : element', kind='yours')
    info = Class('tracker_info',
                 ['start, current, previous', 'offset, modifiers',
                  'distance(), movement()'])
    trk = Class('tracker<Base, TrackerInfo>',
                ['click(ctx, btn)', 'drag(ctx, btn)',
                 'begin_tracking(ctx, info)', 'keep_tracking(ctx, info)',
                 'end_tracking(ctx, info)'])
    slider = Class('slider_base')
    dial = Class("basic_dial")
    movable = Class('movable_base')

    f.place(base, 0, f.margin)
    y = base.bottom + 30
    f.place(trk, base.cx - trk.w / 2, y)
    f.place(info, 0, 0)
    y = trk.bottom + 40
    f.row([slider, dial, movable], y=y, gap=30, x=0, centre=False)
    mid = trk.cx
    dial.x = mid - dial.w / 2
    slider.x = dial.left - 30 - slider.w
    movable.x = dial.right + 30
    info.y = trk.cy - info.h / 2
    info.x = trk.right + 110
    base.x = trk.cx - base.w / 2
    f.centre()

    f.inherits(trk, base)
    f.inherits(slider, trk, lane=True)
    f.inherits(dial, trk, lane=True)
    f.inherits(movable, trk, lane=True)
    f.holds(trk, info, 'state')
    f.label('TrackerInfo', info.cx, info.top - 8, anchor='middle',
            colour='#5d5d5d')
    os.makedirs(os.path.join(OUT, 'elements'), exist_ok=True)
    return f.write(os.path.join(OUT, 'elements', 'tracker_classes.svg'))


def tracker_drag():
    # A drag: tracker's click and drag make the state, call the three
    # hooks, and report the gesture to the view's on_tracking.
    seq = Sequence([('view', 'component'), ('tracker', 'component'),
                    ('your element', 'yours'), ('on_tracking', 'yours')])
    seq.call(0, 1, 'click(ctx, btn)  down')
    seq.call(1, 1, 'state = new_state(...)')
    seq.call(1, 3, 'begin_tracking', 'control')
    seq.call(1, 2, 'begin_tracking(ctx, info)')
    seq.call(0, 1, 'drag(ctx, btn)')
    seq.call(1, 1, 'info.previous, current')
    seq.call(1, 2, 'keep_tracking(ctx, info)')
    seq.call(1, 3, 'while_tracking', 'control')
    seq.call(0, 1, 'click(ctx, btn)  up')
    seq.call(1, 3, 'end_tracking', 'control')
    seq.call(1, 2, 'end_tracking(ctx, info)')
    seq.call(1, 1, 'state.reset()')
    os.makedirs(os.path.join(OUT, 'elements'), exist_ok=True)
    return seq.write(os.path.join(OUT, 'elements', 'tracker_drag.svg'))


def styler_classes():
    # A control is a proxy: it holds the state and handles the input; its
    # subject, the styler, is any element and draws it.
    f = ClassDiagram()
    element = Class('element', abstract=True)
    base = Class('proxy_base', ['subject() : element&'])
    button = Class('basic_button',
                   ['value(), hilite(), tracking()', 'click(ctx, btn)'])
    styler = Class('Styler : element', ['draw(ctx)'], kind='yours')
    prox = Class('proxy<Styler, basic_button>', ['subject() : Styler&'])

    f.place(element, 0, f.margin)
    y = element.bottom + 44
    f.place(base, 0, y)
    y = base.bottom + 44
    f.place(button, 0, y)
    y = button.bottom + 44
    f.place(prox, 0, y)
    element.x = 0
    base.x = element.cx - base.w / 2
    button.x = element.cx - button.w / 2
    prox.x = element.cx - prox.w / 2
    f.place(styler, prox.right + 110, prox.cy - 0)
    styler.y = prox.cy - styler.h / 2
    f.centre()

    f.inherits(base, element)
    f.inherits(button, base)
    f.inherits(prox, button)
    f.inherits(styler, element, side='right')
    f.holds(prox, styler, 'subject()')
    os.makedirs(os.path.join(OUT, 'styles'), exist_ok=True)
    return f.write(os.path.join(OUT, 'styles', 'styler_classes.svg'))


def styler_pull():
    # Pull: the control forwards draw; the styler finds the control up the
    # parent contexts and reads its state.
    seq = Sequence([('view', 'component'), ('button', 'component'),
                    ('styler', 'yours')])
    seq.call(0, 1, 'draw(ctx)')
    seq.call(1, 2, 'draw(sctx)')
    seq.call(2, 2, 'btn = find_parent<basic_button*>(sctx)')
    seq.call(2, 1, 'btn->value(), btn->hilite()')
    seq.call(2, 2, 'draw from the state and sctx.enabled')
    os.makedirs(os.path.join(OUT, 'styles'), exist_ok=True)
    return seq.write(os.path.join(OUT, 'styles', 'styler_pull.svg'))


def styler_place():
    # Placement: the slider computes each styler's bounds from its value;
    # the stylers draw within the bounds they are given.
    seq = Sequence([('view', 'component'), ('slider', 'component'),
                    ('track', 'yours'), ('thumb', 'yours')])
    seq.call(0, 1, 'draw(ctx)')
    seq.call(1, 1, 'tctx.bounds = track_bounds(tctx)')
    seq.call(1, 2, 'draw(tctx)')
    seq.call(1, 1, 'mctx.bounds = thumb_bounds(mctx)  from value()')
    seq.call(1, 3, 'draw(mctx)')
    os.makedirs(os.path.join(OUT, 'styles'), exist_ok=True)
    return seq.write(os.path.join(OUT, 'styles', 'styler_place.svg'))


def styler_push():
    # Push: the thumbwheel passes its value to a styler that is a receiver,
    # when the value is set and before each call it forwards.
    seq = Sequence([('code', 'yours'), ('view', 'component'),
                    ('thumbwheel', 'component'), ('styler', 'yours')])
    seq.call(0, 2, 'value(p)')
    seq.call(2, 2, 'r = find_subject<receiver<double>*>(this)')
    seq.call(2, 3, 'r->value(p.y)')
    seq.call(1, 2, 'draw(ctx)')
    seq.call(2, 3, 'r->value(p.y)  in prepare_subject')
    seq.call(2, 3, 'draw(sctx)')
    os.makedirs(os.path.join(OUT, 'styles'), exist_ok=True)
    return seq.write(os.path.join(OUT, 'styles', 'styler_push.svg'))


FIGURES = [context_chain, receiver_gui, receiver_button, receiver_code,
           receiver_button_code, proxy_draw, proxy_classes, indirect_classes,
           traversal_down, traversal_up, composite_classes, composite_click,
           selection, tracker_classes, tracker_drag, styler_classes,
           styler_pull, styler_place, styler_push]

if __name__ == '__main__':
    for make in FIGURES:
        print(make())
