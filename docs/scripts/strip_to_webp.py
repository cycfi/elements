#!/usr/bin/env python3
"""Turn a strip of frames, one under the other, into an animated WebP.

The page tests draw an animation as a strip: one image, its frames stacked
top to bottom. A browser will not paint a background image past 16384 px,
so a strip cannot be shown as one; an animated WebP has no such limit, and
plays in an image tag like a GIF.

   strip_to_webp.py strip.png frames out.webp [--ms 40] [--width W]
                    [--window TITLE]

`frames` is the number of frames in the strip; `--ms` the time each is
shown; `--width` scales the frames down to W pixels wide; `--window` puts
each frame, drawn at twice the pixels, in a macOS window titled TITLE,
with its shadow, as the screenshots on the pages have it.
"""

import argparse
from PIL import Image, ImageDraw, ImageFilter, ImageFont

# The window, as the 2x screenshots measure it.
MARGIN = (111, 75, 111, 147)     # left, top, right, bottom, around the window
TITLE_H = 44
RADIUS = 10
LIGHTS = [((252, 91, 87), 29), ((229, 191, 60), 69), ((87, 192, 56), 109)]
LIGHT_R = 12
FONTS = ['/System/Library/Fonts/HelveticaNeue.ttc',
         '/System/Library/Fonts/Helvetica.ttc']


def font(size):
   for f in FONTS:
      try:
         return ImageFont.truetype(f, size)
      except OSError:
         pass
   return ImageFont.load_default()


def window_chrome(content_size, title):
   """The window around content of content_size: its shadow, frame and
   title bar, and where the content goes."""
   cw, ch = content_size
   ww, wh = cw + 2, ch + TITLE_H + 2           # a 1 px border all round
   left, top, right, bottom = MARGIN
   size = (ww + left + right, wh + top + bottom)

   # The shadow: the window's shape, dark, lower, and blurred.
   shadow = Image.new('L', size, 0)
   ImageDraw.Draw(shadow).rounded_rectangle(
      (left, top + 22, left + ww, top + wh + 22), RADIUS, fill=150)
   shadow = shadow.filter(ImageFilter.GaussianBlur(34))
   img = Image.new('RGBA', size, (0, 0, 0, 0))
   img.putalpha(shadow)

   # The window: a dark border, then the title bar, a gradient.
   mask = Image.new('L', size, 0)
   ImageDraw.Draw(mask).rounded_rectangle(
      (left, top, left + ww - 1, top + wh - 1), RADIUS, fill=255)
   body = Image.new('RGBA', size, (0, 0, 0, 255))
   d = ImageDraw.Draw(body)
   for y in range(TITLE_H - 1):
      t = y / (TITLE_H - 2)
      c = tuple(int(a + (b - a) * t) for a, b in
                zip((126, 121, 118), (101, 97, 96)))
      d.line((left + 1, top + 1 + y, left + ww - 2, top + 1 + y), fill=c)
   d.line((left + 1, top + 1, left + ww - 2, top + 1), fill=(144, 140, 138))
   for colour, x in LIGHTS:
      cx, cy = left + x, top + TITLE_H // 2 + 1
      d.ellipse((cx - LIGHT_R, cy - LIGHT_R, cx + LIGHT_R, cy + LIGHT_R),
                fill=colour)
   f = font(26)
   tw = d.textlength(title, font=f)
   d.text((left + (ww - tw) / 2, top + TITLE_H // 2 + 1), title,
          font=f, fill=(235, 231, 229), anchor='lm')
   img.paste(body, (0, 0), mask)
   return img, (left + 1, top + TITLE_H + 1)


def main():
   ap = argparse.ArgumentParser()
   ap.add_argument('strip')
   ap.add_argument('frames', type=int)
   ap.add_argument('out')
   ap.add_argument('--ms', type=int, default=40)
   ap.add_argument('--width', type=int, default=0)
   ap.add_argument('--quality', type=int, default=85)
   ap.add_argument('--window', default=None)
   a = ap.parse_args()

   strip = Image.open(a.strip).convert('RGBA')
   w, h = strip.size
   fh = h // a.frames
   frames = [strip.crop((0, i * fh, w, (i + 1) * fh)) for i in range(a.frames)]

   if a.window is not None:
      chrome, at = window_chrome((w, fh), a.window)
      framed = []
      for f in frames:
         img = chrome.copy()
         img.paste(f, at)
         framed.append(img)
      frames = framed

   if a.width and a.width < frames[0].width:
      fw, fh2 = frames[0].size
      size = (a.width, round(fh2 * a.width / fw))
      frames = [f.resize(size, Image.LANCZOS) for f in frames]

   frames[0].save(a.out, save_all=True, append_images=frames[1:],
                  duration=a.ms, loop=0, quality=a.quality, method=6)


if __name__ == '__main__':
   main()
