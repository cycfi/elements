/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include "drag_and_drop.hpp"
#include "utils.hpp"

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
STDMETHODIMP DropTarget::DragEnter(IDataObject* data_obj, DWORD key_state, POINTL pt, DWORD* effect)
{
   makeDropInfo(data_obj);
   if (_drop_valid)
   {
      _drop_info.where = get_point(pt);
      _vptr->track_drop(_drop_info, cursor_tracking::entering);
      *effect &= DROPEFFECT_COPY;
   }
   return S_OK;
}

STDMETHODIMP DropTarget::DragOver(DWORD key_state, POINTL pt, DWORD* effect)
{
   if (_drop_valid)
   {
      _drop_info.where = get_point(pt);
      _vptr->track_drop(_drop_info, cursor_tracking::hovering);
      *effect &= DROPEFFECT_COPY;
   }
   return S_OK;
}

STDMETHODIMP DropTarget::DragLeave()
{
   if (_drop_valid)
      _vptr->track_drop(_drop_info, cursor_tracking::leaving);
   return S_OK;
}

STDMETHODIMP DropTarget::Drop(IDataObject* data_obj, DWORD key_state, POINTL pt, DWORD* effect)
{
   if (_drop_valid)
   {
      _drop_info.where = get_point(pt);
      _vptr->drop(_drop_info);
   }
   return S_OK;
}

namespace cycfi::elements
{
   // UTF8 conversion utils defined in base_view.cpp

   // Convert a wide Unicode string to an UTF8 string
   std::string utf8_encode(std::wstring const& wstr);

   // Convert an UTF8 string to a wide Unicode String
   std::wstring utf8_decode(std::string const& str);
}

void DropTarget::makeDropInfo(IDataObject* data_obj)
{
   using cycfi::elements::utf8_encode;

   // Retrieve file paths from IDataObject
   FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
   STGMEDIUM stg_medium;

   _drop_valid = false;
   if (SUCCEEDED(data_obj->GetData(&fmt, &stg_medium)))
   {
      HDROP drop = static_cast<HDROP>(GlobalLock(stg_medium.hGlobal));
      if (UINT num_files = DragQueryFile(drop, 0xFFFFFFFF, nullptr, 0))
      {
         std::string paths;
         for (UINT i = 0; i < num_files; ++i)
         {
            WCHAR path[MAX_PATH];
            if (UINT length = DragQueryFileW(drop, i, path, MAX_PATH))
            {
               if (i != 0)
                  paths += "\n";
               std::wstring wpath(&path[0], length);
               paths += std::string("file://") + utf8_encode(wpath);
               _drop_valid = true;
            }
         }

         if (_drop_valid)
            _drop_info.data["text/uri-list"] = paths;
      }

      GlobalUnlock(stg_medium.hGlobal);
      ReleaseStgMedium(&stg_medium);
   }
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

