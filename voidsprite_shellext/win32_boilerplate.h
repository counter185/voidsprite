#pragma once

#include <windows.h>
#include <shlobj.h>
#include <thumbcache.h>
#include <strsafe.h>
#include <shlwapi.h>
#include <assert.h>
#include <new>

static const CLSID CLSID_voidsnThumbnailProvider =
{ 0x06062000, 0x1f7d, 0x4a8d, { 0xa8, 0xb8, 0x2b, 0x7b, 0x73, 0x90, 0x12, 0x34 } };

//shell thumbnail handler id
//{e357fccd-a995-4576-b01f-234630154e96}
//must stay like this
static const GUID SID_ShellThumbnailHandler =
{ 0xe357fccd, 0xa995, 0x4576, { 0xb0, 0x1f, 0x23, 0x46, 0x30, 0x15, 0x4e, 0x96 } };

static LONG g_serverLocks = 0;

HRESULT FillThumbnailFromStream(IStream* pStream, UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha);

//gonna be real half this code is copied and changed
//i barely have an idea what's going on here

class VSPThumbnailProvider : public IInitializeWithStream, public IThumbnailProvider
{
public:
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        ULONG c = InterlockedDecrement(&m_cRef);
        if (c == 0)
            delete this;
        return c;
    }

    IFACEMETHODIMP Initialize(IStream* pStream, DWORD grfMode);

    IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha);

    VSPThumbnailProvider() : m_cRef(1), m_pStream(nullptr) {}
    ~VSPThumbnailProvider()
    {
        if (m_pStream) m_pStream->Release();
    }

private:
    LONG     m_cRef;
    IStream* m_pStream;
};

class ClassFactory : public IClassFactory
{
public:
    ClassFactory() : m_ref(1) {}
    ~ClassFactory() {}

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);
    IFACEMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_ref); }
    IFACEMETHODIMP_(ULONG) Release()
    {
        ULONG c = InterlockedDecrement(&m_ref);
        if (0 == c) delete this;
        return c;
    }

    IFACEMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv);

    IFACEMETHODIMP LockServer(BOOL fLock)
    {
        if (fLock) {
            InterlockedIncrement(&g_serverLocks);
        }
        else {
            InterlockedDecrement(&g_serverLocks);
        }
        return S_OK;
    }

private:
    LONG m_ref;
};