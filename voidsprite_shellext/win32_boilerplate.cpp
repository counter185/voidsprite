#include "win32_boilerplate.h"

IFACEMETHODIMP VSPThumbnailProvider::QueryInterface(REFIID riid, void** ppv)
{
    if (ppv == NULL) {
        return E_POINTER;
    }
    *ppv = nullptr;

    if (IsEqualIID(riid, IID_IUnknown))
        *ppv = static_cast<IInitializeWithStream*>(this);
    else if (IsEqualIID(riid, IID_IInitializeWithStream))
        *ppv = static_cast<IInitializeWithStream*>(this);
    else if (IsEqualIID(riid, IID_IThumbnailProvider))
        *ppv = static_cast<IThumbnailProvider*>(this);
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

IFACEMETHODIMP VSPThumbnailProvider::Initialize(IStream* pStream, DWORD grfMode)
{
    if (!pStream) return E_INVALIDARG;

    if (m_pStream)
        m_pStream->Release();

    m_pStream = pStream;
    m_pStream->AddRef();

    LARGE_INTEGER zero = {};
    ULARGE_INTEGER newPos = {};
    HRESULT hr = pStream->Seek(zero, STREAM_SEEK_SET, &newPos);
    (void)hr;
    return S_OK;
}

IFACEMETHODIMP VSPThumbnailProvider::GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha)
{
    if (!phbmp || !pdwAlpha) return E_INVALIDARG;
    *phbmp = nullptr;
    *pdwAlpha = WTSAT_UNKNOWN;

    if (!m_pStream) return E_FAIL;

    LARGE_INTEGER zero = {};
    ULARGE_INTEGER newPos = {};
    HRESULT hrSeek = m_pStream->Seek(zero, STREAM_SEEK_SET, &newPos);
    if (FAILED(hrSeek)) return hrSeek;

    return FillThumbnailFromStream(m_pStream, cx, phbmp, pdwAlpha);
}

IFACEMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
        *ppv = static_cast<IClassFactory*>(this);
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

// IClassFactory
IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv)
{
    if (pUnkOuter) return CLASS_E_NOAGGREGATION;
    if (!ppv) return E_POINTER;
    *ppv = nullptr;

    VSPThumbnailProvider* p = new (std::nothrow) VSPThumbnailProvider();
    if (!p) return E_OUTOFMEMORY;

    HRESULT hr = p->QueryInterface(riid, ppv);
    p->Release();
    return hr;
}
