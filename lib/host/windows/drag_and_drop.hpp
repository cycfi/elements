/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#if !defined(WIN32_DRAG_AND_DROP_NOVEMBER_13_2023)
#define WIN32_DRAG_AND_DROP_NOVEMBER_13_2023

#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <elements/base_view.hpp>

class DropTarget : public IDropTarget
{
public:

   using base_view = cycfi::elements::base_view;
   static IDropTarget* CreateInstance(base_view* vptr, HWND hwnd);

private:

   using drop_info = cycfi::elements::drop_info;
   using point = cycfi::elements::point;

   DropTarget(base_view* vptr, HWND hwnd);
   ~DropTarget();

   // IUnknown methods
   STDMETHOD(QueryInterface)(REFIID riid, void** pobj);
   STDMETHOD_(ULONG, AddRef)();
   STDMETHOD_(ULONG, Release)();

   // IDropTarget methods
   STDMETHOD(DragEnter)(IDataObject* data_obj, DWORD key_state, POINTL pt, DWORD* effect);
   STDMETHOD(DragOver)(DWORD key_state, POINTL pt, DWORD* effect);
   STDMETHOD(DragLeave)();
   STDMETHOD(Drop)(IDataObject* data_obj, DWORD key_state, POINTL pt, DWORD* effect);

   void  makeDropInfo(IDataObject* data_obj);
   point get_point(POINTL pt);

   ULONG       _ref_count;
   base_view*  _vptr = nullptr;
   drop_info   _drop_info;
   bool        _drop_valid;
   HWND        _hwnd;
};

#endif