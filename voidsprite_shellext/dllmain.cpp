#include "win32_boilerplate.h"
#include "voidsn_read.h"

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment( linker, "/export:DllGetClassObject" )
#pragma comment( linker, "/export:DllCanUnloadNow" )

#ifndef EXPORT
    #if defined(_MSC_VER)
        #define EXPORT __declspec(dllexport)
    #elif defined(__GNUC__)
        #define EXPORT __attribute__((visibility("default")))
    #endif
#endif

#define STDAPI_EXPORT EXTERN_C EXPORT HRESULT STDAPICALLTYPE

struct Rect {
    int x, y, w, h;
};

Rect fitInside(Rect outer, Rect inner)
{
    if (outer.w == 0 || outer.h == 0 || inner.w == 0 || inner.h == 0) {
        return Rect{ 0,0,0,0 };
    }
    double aspectOuter = outer.w / (double)outer.h;
    double aspectInner = inner.w / (double)inner.h;
    Rect ret = { 0,0,0,0 };
    if (aspectOuter > aspectInner) {
        //letterbox sides
        ret.h = outer.h;
        ret.w = (int)(outer.h * aspectInner);
        ret.x = outer.x + (outer.w - ret.w) / 2;
        ret.y = outer.y;
    }
    else {
        //letterbox top/bottom
        ret.w = outer.w;
        ret.h = (int)(outer.w / aspectInner);
        ret.x = outer.x;
        ret.y = outer.y + (outer.h - ret.h) / 2;
    }
    return ret;
}

static uint32_t packRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

HRESULT FillThumbnailFromStream(IStream* pStream, UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha)
{

    Layer* v = readVOIDSN(pStream);
    if (v != NULL) {

        Rect outer = { 0,0,(int)cx,(int)cx };
        Rect inner = fitInside(outer, Rect{ 0,0,v->w,v->h });

        Layer* vv = v->copyCurrentVariantScaled({ inner.w, inner.h });
        delete v;

        if (vv != NULL) {
            HDC dc = GetDC(NULL);
            BITMAPINFO bm = { sizeof(BITMAPINFOHEADER),
                inner.w,
                -inner.h, 1, 32,
                BI_RGB, inner.w * inner.h * 4, 0, 0, 0, 0 };
            void* ppvbits;
            HBITMAP h = CreateDIBSection(dc, &bm, DIB_RGB_COLORS, &ppvbits, NULL, 0);
            uint32_t* px32 = (uint32_t*)ppvbits;
            for (int x = 0; x < inner.w; x++) {
                for (int y = 0; y < inner.h; y++) {
                    px32[x + y * inner.w] = vv->getPixelAt({ x,y });//packRGBA(0, 0, 255, x % 256);
                }
            }
            delete vv;

            ReleaseDC(NULL, dc);

            *phbmp = h;
            *pdwAlpha = WTSAT_ARGB;
            return S_OK;
        }
    }
    return E_INVALIDARG;
}

//half the win32 boilerplate is copied from places
//because i would never figure this out on my own

//dll exports
HINSTANCE g_hInstance = nullptr;

EXPORT BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD ul_reason_for_call, LPVOID)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        g_hInstance = hInstance;
        DisableThreadLibraryCalls(hInstance);
    }
    return TRUE;
}

STDAPI DllCanUnloadNow()
{
    if (g_serverLocks == 0)
        return S_OK;
    else
        return S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = nullptr;

    if (!IsEqualCLSID(rclsid, CLSID_voidsnThumbnailProvider))
        return CLASS_E_CLASSNOTAVAILABLE;

    ClassFactory* pFactory = new (std::nothrow) ClassFactory();
    if (!pFactory) return E_OUTOFMEMORY;

    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();
    return hr;
}

static std::wstring GuidToString(REFGUID guid)
{
    LPOLESTR oleStr = nullptr;
    HRESULT hr = StringFromCLSID(guid, &oleStr);
    if (oleStr != nullptr) {
        std::wstring ret = oleStr;
        CoTaskMemFree(oleStr);
        return ret;
    }
    return L"";
}

static HRESULT SetRegKeyAndValue(HKEY hRoot, std::wstring pszSubKey, std::wstring pszValue)
{
    HKEY hKey = nullptr;
    LONG lRes = RegCreateKeyExW(hRoot, pszSubKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
    if (lRes != ERROR_SUCCESS) return HRESULT_FROM_WIN32(lRes);
    if (!pszValue.empty())
    {
        lRes = RegSetValueExW(hKey, nullptr, 0, REG_SZ, (const BYTE*)pszValue.data(), (DWORD)((pszValue.size() + 1) * sizeof(WCHAR)));
    }
    RegCloseKey(hKey);
    return HRESULT_FROM_WIN32(lRes);
}

static HRESULT DeleteRegTree(HKEY hRoot, std::wstring pszSubKey)
{
    LONG lRes = SHDeleteKeyW(hRoot, pszSubKey.c_str());
    if (lRes == ERROR_SUCCESS || lRes == ERROR_FILE_NOT_FOUND)
        return S_OK;
    return HRESULT_FROM_WIN32(lRes);
}

static std::vector<std::wstring> voidsnExtensions = {
    L".voidsn",
    L".voidsnv1",
    L".voidsnv2",
    L".voidsnv3",
    L".voidsnv4",
    L".voidsnv5",
    L".voidsnv6",
    L".voidsnv7",
};

STDAPI_EXPORT DllRegisterServer(void)
{
    HRESULT hr = S_OK;
    WCHAR szModulePath[MAX_PATH];
    if (!GetModuleFileNameW(g_hInstance, szModulePath, ARRAYSIZE(szModulePath))) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    //register the thumbnail provider in HKCR\CLSID\{CLSID}\InprocServer32
    std::wstring pszCLSID = GuidToString(CLSID_voidsnThumbnailProvider);

    std::wstring keyPath = L"CLSID\\" + pszCLSID;

    //add InprocServer32
    std::wstring inprocKey = keyPath + L"\\InprocServer32";
    hr = SetRegKeyAndValue(HKEY_CLASSES_ROOT, inprocKey, szModulePath);
    if (FAILED(hr)) { return hr; }

    //set ThreadingModel = Apartment for some reason (krita does this so i'll do it too)
    HKEY hInproc = nullptr;
    if (RegOpenKeyExW(HKEY_CLASSES_ROOT, inprocKey.c_str(), 0, KEY_WRITE, &hInproc) == ERROR_SUCCESS)
    {
        RegSetValueExW(hInproc, L"ThreadingModel", 0, REG_SZ, (const BYTE*)L"Apartment", (DWORD)((wcslen(L"Apartment") + 1) * sizeof(WCHAR)));
        RegCloseKey(hInproc);
    }

    //set this for all extensions:
    //HKCR\{extension}\ShellEx\{e357fccd-a995-4576-b01f-234630154e96} = {CLSID}
    std::wstring pszShellExtGUID = GuidToString(SID_ShellThumbnailHandler);

    for (auto& ext : voidsnExtensions) {
        std::wstring extKey = ext + L"\\shellex\\" + pszShellExtGUID;
        hr = SetRegKeyAndValue(HKEY_CLASSES_ROOT, extKey, pszCLSID);
        if (FAILED(hr)) return hr;
    }

    return S_OK;
}

STDAPI_EXPORT DllUnregisterServer(void)
{
    HRESULT hr = S_OK;

    //unregister all the extensions
    for (auto& ext : voidsnExtensions) {
        std::wstring pszShellExtGUID = GuidToString(SID_ShellThumbnailHandler);
        std::wstring extKeyPath = ext + L"\\shellex\\" + pszShellExtGUID;
        DeleteRegTree(HKEY_CLASSES_ROOT, extKeyPath);
    }

    //remove CLSID key
    std::wstring pszCLSID = GuidToString(CLSID_voidsnThumbnailProvider);
    std::wstring clsidKey = L"CLSID\\" + pszCLSID;
    DeleteRegTree(HKEY_CLASSES_ROOT, clsidKey);

    return S_OK;
}

