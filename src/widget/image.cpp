/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/widget/image.hpp>
#include <photon/theme.hpp>
#include <photon/support.hpp>
#include <photon/app.hpp>
#include <photon/window.hpp>
#include <nanovg.h>

namespace photon
{
   image::image(char const* filename)
    : _filename(filename)
    , _handle(0)
    , _canvas(0)
   {}

   image::~image()
   {
      if (_canvas && _handle)
         nvgDeleteImage(_canvas, _handle);
   }

   point image::size(context const& ctx) const
   {
      if (!_handle)
      {
         _canvas = ctx.canvas().context();
         _handle = nvgCreateImage(_canvas, _filename, 0);
      }

      int w, h;
      nvgImageSize(ctx.canvas().context(), _handle, &w, &h);
      return { double(w), double(h) };
   }

   void image::draw(context const& ctx)
   {
      if (!_handle)
      {
         _canvas = ctx.canvas().context();
         _handle = nvgCreateImage(_canvas, _filename, 0);
      }

      float x = ctx.bounds.left;
      float y = ctx.bounds.top;
      float w = ctx.bounds.width();
      float h = ctx.bounds.height();

      nvgSave(_canvas);
      NVGpaint image = nvgImagePattern(_canvas, x, y, w, h, 0.0, _handle, 1.0);
      nvgBeginPath(_canvas);
      nvgRect(_canvas, x, y, w, h);
      nvgFillPaint(_canvas, image);
      nvgFill(_canvas);
      nvgRestore(_canvas);
   }
}
