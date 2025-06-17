/*
FOR WINDOWS BUILDS ONLY
    Under public domain, copied from here:
https://github.com/inventordave2/IFileDialog-gcc-Fix/blob/main/ifiledialog.h
*/
#ifndef _IFileDialog_Fix_ID
#define _IFileDialog_Fix_ID
#include <shobjidl.h>
#include <Windows.h>

// 'shobjidl.h' line: 25174
typedef enum FDAP {
    FDAP_BOTTOM = 0,
    FDAP_TOP = 1
} FDAP;

// 'shobjidl.h' line: 25405
typedef enum FILEOPENDIALOGOPTIONS {
  FOS_OVERWRITEPROMPT,
  FOS_STRICTFILETYPES,
  FOS_NOCHANGEDIR,
  FOS_PICKFOLDERS,
  FOS_FORCEFILESYSTEM,
  FOS_ALLNONSTORAGEITEMS,
  FOS_NOVALIDATE,
  FOS_ALLOWMULTISELECT,
  FOS_PATHMUSTEXIST,
  FOS_FILEMUSTEXIST,
  FOS_CREATEPROMPT,
  FOS_SHAREAWARE,
  FOS_NOREADONLYRETURN,
  FOS_NOTESTFILECREATE,
  FOS_HIDEMRUPLACES,
  FOS_HIDEPINNEDPLACES,
  FOS_NODEREFERENCELINKS,
  FOS_OKBUTTONNEEDSINTERACTION,
  FOS_DONTADDTORECENT,
  FOS_FORCESHOWHIDDEN,
  FOS_DEFAULTNOMINIMODE,
  FOS_FORCEPREVIEWPANEON,
  FOS_SUPPORTSTREAMABLEITEMS
  
} FILEOPENDIALOGOPTIONS;

// 'shobjidl.h' line: 25432
DEFINE_GUID(IID_IFileDialog, 0x42f85136, 0xdb7e, 0x439c, 0x85,0xf1, 0xe4,0x07,0x5d,0x13,0x5f,0xc8);
MIDL_INTERFACE("42f85136-db7e-439c-85f1-e4075d135fc8")
IFileDialog : public IModalWindow	{
	
    virtual HRESULT STDMETHODCALLTYPE SetFileTypes(
        UINT cFileTypes,
        const COMDLG_FILTERSPEC *rgFilterSpec) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFileTypeIndex(
        UINT iFileType) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetFileTypeIndex(
        UINT *piFileType) = 0;

    virtual HRESULT STDMETHODCALLTYPE Advise(
        IFileDialogEvents *pfde,
        DWORD *pdwCookie) = 0;

    virtual HRESULT STDMETHODCALLTYPE Unadvise(
        DWORD dwCookie) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetOptions(
        FILEOPENDIALOGOPTIONS fos) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetOptions(
        FILEOPENDIALOGOPTIONS *pfos) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDefaultFolder(
        IShellItem *psi) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFolder(
        IShellItem *psi) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetFolder(
        IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCurrentSelection(
        IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFileName(
        LPCWSTR pszName) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetFileName(
        LPWSTR *pszName) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetTitle(
        LPCWSTR pszTitle) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetOkButtonLabel(
        LPCWSTR pszText) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFileNameLabel(
        LPCWSTR pszLabel) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetResult(
        IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE AddPlace(
        IShellItem *psi,
        FDAP fdap) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDefaultExtension(
        LPCWSTR pszDefaultExtension) = 0;

    virtual HRESULT STDMETHODCALLTYPE Close(
        HRESULT hr) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetClientGuid(
        REFGUID guid) = 0;

    virtual HRESULT STDMETHODCALLTYPE ClearClientData(
        ) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFilter(
        IShellItemFilter *pFilter) = 0;

};
__CRT_UUID_DECL(IFileDialog, 0x42f85136, 0xdb7e, 0x439c, 0x85,0xf1, 0xe4,0x07,0x5d,0x13,0x5f,0xc8)

// 'shobjidl.h' line: 26326
DEFINE_GUID(IID_IFileOpenDialog, 0xd57c7288, 0xd4ad, 0x4768, 0xbe,0x02, 0x9d,0x96,0x95,0x32,0xd9,0x60);
MIDL_INTERFACE("d57c7288-d4ad-4768-be02-9d969532d960")
IFileOpenDialog : public IFileDialog	{
	
    virtual HRESULT STDMETHODCALLTYPE GetResults(
        IShellItemArray **ppenum) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetSelectedItems(
        IShellItemArray **ppsai) = 0;

};
__CRT_UUID_DECL(IFileOpenDialog, 0xd57c7288, 0xd4ad, 0x4768, 0xbe,0x02, 0x9d,0x96,0x95,0x32,0xd9,0x60)

// 'shobjidl.h' line: 25953
DEFINE_GUID(IID_IFileSaveDialog, 0x84bccd23, 0x5fde, 0x4cdb, 0xae,0xa4, 0xaf,0x64,0xb8,0x3d,0x78,0xab);
MIDL_INTERFACE("84bccd23-5fde-4cdb-aea4-af64b83d78ab")
IFileSaveDialog : public IFileDialog	{
	
    virtual HRESULT STDMETHODCALLTYPE SetSaveAsItem(
        IShellItem *psi) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetProperties(
        IPropertyStore *pStore) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetCollectedProperties(
        IPropertyDescriptionList *pList,
        WINBOOL fAppendDefault) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetProperties(
        IPropertyStore **ppStore) = 0;

    virtual HRESULT STDMETHODCALLTYPE ApplyProperties(
        IShellItem *psi,
        IPropertyStore *pStore,
        HWND hwnd,
        IFileOperationProgressSink *pSink) = 0;

};
__CRT_UUID_DECL(IFileSaveDialog, 0x84bccd23, 0x5fde, 0x4cdb, 0xae,0xa4, 0xaf,0x64,0xb8,0x3d,0x78,0xab)

#endif
