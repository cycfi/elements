/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#pragma once

#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>

class DropTarget : public IDropTarget
{
public:

   static IDropTarget *CreateInstance(HWND hwnd);

private:

   DropTarget(HWND hwnd);
   ~DropTarget();

   // IUnknown methods
   STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj);
   STDMETHOD_(ULONG, AddRef)();
   STDMETHOD_(ULONG, Release)();

   // IDropTarget methods
   STDMETHOD(DragEnter)(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
   STDMETHOD(DragOver)(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
   STDMETHOD(DragLeave)();
   STDMETHOD(Drop)(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

   ULONG _ref_count;
   HWND _hwnd;
};

inline IDropTarget* DropTarget::CreateInstance(HWND hwnd)
{
   DropTarget *pDropTarget = new DropTarget(hwnd);
   if (pDropTarget)
   {
      pDropTarget->AddRef();
      return pDropTarget;
   }
   return NULL;
}

inline DropTarget::DropTarget(HWND hwnd)
 : _ref_count(1)
 , _hwnd(hwnd)
{}

inline DropTarget::~DropTarget()
{}

// IUnknown methods
inline STDMETHODIMP DropTarget::QueryInterface(REFIID riid, void** ppvObj)
{
   if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropTarget))
   {
      *ppvObj = static_cast<IDropTarget*>(this);
      AddRef();
      return S_OK;
   }

   *ppvObj = nullptr;
   return E_NOINTERFACE;
}

inline STDMETHODIMP_(ULONG) DropTarget::AddRef()
{
   return InterlockedIncrement(&_ref_count);
}

inline STDMETHODIMP_(ULONG) DropTarget::Release()
{
   ULONG refCount = InterlockedDecrement(&_ref_count);
   if (refCount == 0)
      delete this;
   return refCount;
}

// IDropTarget methods
inline STDMETHODIMP DropTarget::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
   // Handle drag enter, provide feedback
   // (e.g., change the cursor)
   SetCursor(LoadCursor(nullptr, IDC_HAND));

   *pdwEffect &= DROPEFFECT_COPY;  // Allow only copy operation

   return S_OK;
}

inline STDMETHODIMP DropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
   // Handle drag over, provide feedback
   // (e.g., change the cursor)

   *pdwEffect &= DROPEFFECT_COPY;  // Allow only copy operation

   return S_OK;
}

inline STDMETHODIMP DropTarget::DragLeave()
{
   // Handle drag leave, reset feedback
   // (e.g., revert the cursor to default)
   SetCursor(LoadCursor(nullptr, IDC_ARROW));

   return S_OK;
}

inline STDMETHODIMP DropTarget::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
   // Handle drop, process the data

   // Example: Retrieve file paths from IDataObject
   FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
   STGMEDIUM stgMedium;

   if (SUCCEEDED(pDataObj->GetData(&fmt, &stgMedium)))
   {
      HDROP hDrop = static_cast<HDROP>(GlobalLock(stgMedium.hGlobal));
      UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);

      for (UINT i = 0; i < fileCount; ++i)
      {
         TCHAR filePath[MAX_PATH];
         DragQueryFile(hDrop, i, filePath, MAX_PATH);

         // Process the dropped file (filePath)
         // Example: Display a message box with the file path
//         MessageBoxW(_hwnd, filePath, L"Dropped File", MB_OK | MB_ICONINFORMATION);
      }

      GlobalUnlock(stgMedium.hGlobal);
      ReleaseStgMedium(&stgMedium);
   }

   // Reset feedback
   SetCursor(LoadCursor(nullptr, IDC_ARROW));

   return S_OK;
}
