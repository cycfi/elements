/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include "drag_and_drop.hpp"
#include "utils.hpp"
#include "transfer.hpp"
#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#ifndef UNICODE
# define UNICODE
#endif

IDropTarget* DropTarget::CreateInstance(base_view* vptr, HWND hwnd)
{
   if (DropTarget* p = new DropTarget(vptr, hwnd))
   {
      p->AddRef();
      return p;
   }
   return nullptr;
}

DropTarget::DropTarget(base_view* vptr, HWND hwnd)
 : _ref_count(1)
 , _vptr(vptr)
 , _hwnd{hwnd}
{}

DropTarget::~DropTarget()
{}

// IUnknown methods
STDMETHODIMP DropTarget::QueryInterface(REFIID riid, void** pobj)
{
   if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropTarget))
   {
      *pobj = static_cast<IDropTarget*>(this);
      AddRef();
      return S_OK;
   }

   *pobj = nullptr;
   return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) DropTarget::AddRef()
{
   return InterlockedIncrement(&_ref_count);
}

STDMETHODIMP_(ULONG) DropTarget::Release()
{
   ULONG refCount = InterlockedDecrement(&_ref_count);
   if (refCount == 0)
      delete this;
   return refCount;
}

using cycfi::elements::cursor_tracking;

// IDropTarget methods
//
// While the drag hovers, the drop names the types on offer, with no data;
// the data is read on the drop.
STDMETHODIMP DropTarget::DragEnter(IDataObject* data_obj, DWORD /*key_state*/, POINTL pt, DWORD* effect)
{
   makeDropInfo(data_obj, false);
   if (_drop_valid)
   {
      _drop_info.where = get_point(pt);
      _vptr->track_drop(_drop_info, cursor_tracking::entering);
      *effect &= DROPEFFECT_COPY;
   }
   else
   {
      *effect = DROPEFFECT_NONE;
   }
   return S_OK;
}

STDMETHODIMP DropTarget::DragOver(DWORD /*key_state*/, POINTL pt, DWORD* effect)
{
   if (_drop_valid)
   {
      _drop_info.where = get_point(pt);
      _vptr->track_drop(_drop_info, cursor_tracking::hovering);
      *effect &= DROPEFFECT_COPY;
   }
   else
   {
      *effect = DROPEFFECT_NONE;
   }
   return S_OK;
}

STDMETHODIMP DropTarget::DragLeave()
{
   if (_drop_valid)
      _vptr->track_drop(_drop_info, cursor_tracking::leaving);
   _drop_valid = false;
   return S_OK;
}

STDMETHODIMP DropTarget::Drop(IDataObject* data_obj, DWORD /*key_state*/, POINTL pt, DWORD* effect)
{
   bool success = false;
   if (_drop_valid)
   {
      makeDropInfo(data_obj, true);
      if (_drop_valid)
      {
         _drop_info.where = get_point(pt);
         success = _vptr->drop(_drop_info);
      }
   }
   _drop_valid = false;
   *effect = success ? DROPEFFECT_COPY : DROPEFFECT_NONE;
   return S_OK;
}

namespace
{
   std::vector<UINT> offered_formats(IDataObject* data_obj)
   {
      std::vector<UINT> formats;
      IEnumFORMATETC* e = nullptr;
      if (SUCCEEDED(data_obj->EnumFormatEtc(DATADIR_GET, &e)) && e)
      {
         FORMATETC fmt;
         while (e->Next(1, &fmt, nullptr) == S_OK)
         {
            if (fmt.ptd)
               CoTaskMemFree(fmt.ptd);
            if (fmt.tymed & TYMED_HGLOBAL)
               formats.push_back(fmt.cfFormat);
         }
         e->Release();
      }
      return formats;
   }

   std::optional<std::string> format_bytes(IDataObject* data_obj, UINT format)
   {
      FORMATETC fmt = { CLIPFORMAT(format), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
      STGMEDIUM stg_medium;
      if (FAILED(data_obj->GetData(&fmt, &stg_medium)))
         return std::nullopt;
      std::optional<std::string> bytes;
      if (stg_medium.tymed == TYMED_HGLOBAL)
         bytes = cycfi::elements::transfer::global_bytes(stg_medium.hGlobal);
      ReleaseStgMedium(&stg_medium);
      return bytes;
   }
}

void DropTarget::makeDropInfo(IDataObject* data_obj, bool with_data)
{
   namespace transfer = cycfi::elements::transfer;

   auto formats = offered_formats(data_obj);
   auto types = transfer::mime_types_of(formats);
   if (auto const& accepted = _vptr->accepted_drop_types())
   {
      // Only the types the view takes drops of.
      types.erase(
         std::remove_if(types.begin(), types.end(),
            [&](auto const& type) { return !accepted->count(type); }),
         types.end());
   }
   _drop_info.data.clear();
   if (with_data)
   {
      _drop_info.data = transfer::read(formats,
         cycfi::elements::mime_types(types.begin(), types.end()),
         [&](UINT format) { return format_bytes(data_obj, format); });
   }
   else
   {
      for (auto const& type : types)
         _drop_info.data[type] = std::string{};
   }
   _drop_valid = !_drop_info.data.empty();
}

using cycfi::elements::point;
using cycfi::elements::get_scale_for_window;

point DropTarget::get_point(POINTL pt)
{
   POINT pos{pt.x, pt.y};
   ScreenToClient(_hwnd, &pos);
   auto scale = get_scale_for_window(_hwnd);
   return {float(pos.x)/scale, float(pos.y)/scale};
}

