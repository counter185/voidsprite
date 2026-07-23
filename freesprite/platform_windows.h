#pragma once

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "iphlpapi.lib")

//disable for reactos/winxp
#ifndef VSP_WIN32_DWMAPI
    #define VSP_WIN32_DWMAPI 1
#endif

#if VSP_WIN32_DWMAPI
    #include <dwmapi.h>
#endif

#define popen _popen
#define pclose _pclose

#include <SDL3/SDL_properties.h>

#include <d3d9.h>
#include <windows.h>
#include <VersionHelpers.h>
#include <tlhelp32.h>
#include <commdlg.h>
#include <winhttp.h>
#include <iphlpapi.h>

#include "platform_universal.h"
#include "maineditor.h"
#include "io/io_png.h"
#include "Notification.h"
#include "multiwindow.h"
#include "main.h"

u32 platformSupportedFeatures() {
    return VSP_FEATURE_ALL & ~VSP_FEATURE_OS_SHARE;
    //windows is the primary platform and should support everything
    //okay maybe not sharing
}

wchar_t fileNameBuffer[MAX_PATH] = { 0 };
int lastFilterIndex = 1;
std::vector<PlatformNativePathString> tempFilesToDeleteOnDeinit;
std::jthread* ipcThread = NULL;

int windows_numProcessesRunning(std::wstring name) {
    int ret = 0;
    HANDLE hProcessSnap;
    PROCESSENTRY32W pe32;
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return false;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32FirstW(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);     // clean the snapshot object
        return 0;
    }
    do {
        if (name == pe32.szExeFile) {
            ret++;
        }
    } while (Process32NextW(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);
    return ret;
}

bool windows_isProcessRunning(std::wstring name) {
    return windows_numProcessesRunning(name) > 0;
}

std::string windows_readStringFromRegistry(HKEY rootKey, std::wstring path, std::wstring keyname) {
    HKEY hKey;
    std::string ret = "";
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t stringData[1024];
        DWORD size = sizeof(stringData);
        if (RegQueryValueExW(hKey, keyname.c_str(), NULL, NULL, (LPBYTE)stringData, &size) == ERROR_SUCCESS) {
            ret = convertStringToUTF8OnWin32(stringData);
        }
        RegCloseKey(hKey);
    }
    else {
        ret = "";
    }
    return ret;
}

std::string windows_getActiveGPUName() {
    IDirect3D9* d3dobject = Direct3DCreate9(D3D_SDK_VERSION);
    D3DADAPTER_IDENTIFIER9 ident;
    d3dobject->GetAdapterIdentifier(0, 0, &ident);
    std::string ret = ident.Description;
    d3dobject->Release();
    return ret;
}

HWND windows_getWindowHWND(VSPWindow* w) {
    if (w != NULL) {
        return (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(w->wd), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    }
    return NULL;
}

LRESULT (CALLBACK *originalWndProc)(HWND, UINT, WPARAM, LPARAM) = NULL;

void platformPreInit() {
    auto thisPID = GetCurrentProcessId();
    std::wstring thisExeName = convertStringOnWin32(fileNameFromPath(g_programExePath));
}
void platformInit() {
    platformRegisterURI("voidsprite", {});
}
void platformPostInit() {
    for (auto& [id,wd] : g_windows) {
        SDL_ShowWindow(wd->wd);
    }
#if _M_ARM64
    g_addNotification(Notification("arm64 Build", "Experimental build. Things may not work.", 5000, NULL, COLOR_INFO));
#endif
}

void platformWindowCreated(VSPWindow* wd) {
    static bool firstWindow = true;
    if (firstWindow) {
        firstWindow = false;
#if VSP_WIN32_DWMAPI
        //run this only on the first window
        BOOL USE_DARK_MODE = true;
        bool SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(
            windows_getWindowHWND(wd), 20,
            &USE_DARK_MODE, sizeof(USE_DARK_MODE)));
        //not needed on win11 but this makes win10 repaint the whole window
        //SDL_HideWindow(wd->wd);
        //SDL_ShowWindow(wd->wd);
#endif
    }

    //SetWindowLongPtrW(hwnd, -4, (LONG_PTR)windows_WndProc);
}
void platformWindowDestroyed(VSPWindow*) {}

void platformDeinit() {
    for (auto& deleteTemp : tempFilesToDeleteOnDeinit) {
        try {
            std::filesystem::remove(deleteTemp);
        }
        catch (std::exception& e) {
            logerr(frmt("error deleting temp file\n {}", e.what()));
        }
    }
    if (ipcThread != NULL) {
        TerminateThread(ipcThread->native_handle(), 0);
    }
}

bool platformRegisterURI(std::string uriProtocol, std::vector<std::string> additionalArgs) {
    //add the program into hkey_classes_root
    WCHAR path[MAX_PATH];
    if (GetModuleFileNameW(NULL, path, MAX_PATH) > 0) {
        HKEY classesRootKey;
        if (RegOpenKeyW(HKEY_CURRENT_USER, L"SOFTWARE\\Classes", &classesRootKey) != ERROR_SUCCESS) {
            logerr("failed to open hkey_classes_root");
            return false;
        }
        std::wstring pathWstr = path;
        for (auto& arg : additionalArgs) {
            pathWstr += L" " + convertStringOnWin32(arg);
        }
        pathWstr += L" \"%1\"";

        std::wstring prot = convertStringOnWin32(uriProtocol);

        HKEY voidspriteRootKey;
        if (RegCreateKeyExW(classesRootKey, prot.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &voidspriteRootKey, NULL) == ERROR_SUCCESS) {
            HKEY hKey;
            RegSetValueExW(voidspriteRootKey, L"URL Protocol", 0, REG_SZ, (const BYTE*)L"", sizeof(L""));
            if (RegCreateKeyExW(voidspriteRootKey, L"DefaultIcon", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
                std::wstring assocIconPath = convertStringOnWin32(pathInProgramDirectory("assets\\icon_fileassoc.ico"));

                RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)assocIconPath.c_str(), (assocIconPath.size() + 1) * sizeof(wchar_t));
                RegCloseKey(hKey);
            }
            if (RegCreateKeyExW(voidspriteRootKey, L"shell\\open\\command", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
                RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)pathWstr.c_str(), (pathWstr.size() + 1) * sizeof(wchar_t));
                RegCloseKey(hKey);
            }
            if (RegCreateKeyExW(voidspriteRootKey, L"shell\\open", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
                RegSetValueExW(hKey, L"FriendlyAppName", 0, REG_SZ, (const BYTE*)L"voidsprite", sizeof(L"voidsprite"));
                RegCloseKey(hKey);
            }
            RegCloseKey(voidspriteRootKey);
        }
        else {
            logerr("failed to create key in hkey_classes_root");
            RegCloseKey(classesRootKey);
            return false;
        }
        RegCloseKey(classesRootKey);

        return true;
    }
    return false;
}

bool platformAssocFileTypes(std::vector<std::string> extensions, std::vector<std::string> additionalArgs) {

    //add the program into hkey_classes_root

    platformRegisterURI("voidsprite-file", additionalArgs);

    WCHAR path[MAX_PATH];
    if (GetModuleFileNameW(NULL, path, MAX_PATH) > 0) {
        HKEY classesRootKey;
        if (RegOpenKeyW(HKEY_CURRENT_USER, L"SOFTWARE\\Classes", &classesRootKey) != ERROR_SUCCESS) {
            logerr("failed to open hkey_classes_root");
            return false;
        }

        //add all of the filetypes to hkey_classes_root too
        for (auto& ext : extensions) {
            HKEY hKey;
            std::wstring extw = utf8StringToWstring(ext);
            if (RegCreateKeyExW(classesRootKey, extw.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
                RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)L"voidsprite-file", sizeof(L"voidsprite-file"));
                RegCloseKey(hKey);
            }
            else {
                logerr(frmt("failed to create {} key in hkey_classes_root", ext.c_str()));
                //RegCloseKey(classesRootKey);
                //return false;
            }
        }
        RegCloseKey(classesRootKey);

        return true;
    }
    return false;
}
std::string platformGetFileAssocForExtension(std::string extension) { 
    HKEY classKey;
    std::wstring pathW = convertStringOnWin32(frmt("SOFTWARE\\Classes\\{}", extension));
    std::wstring pathW2 = convertStringOnWin32(frmt("SOFTWARE\\Classes\\{}\\OpenWithProgids", extension));
    if (RegOpenKeyExW(HKEY_CURRENT_USER, pathW.c_str(), 0, KEY_QUERY_VALUE, &classKey) == ERROR_SUCCESS) {
        DoOnReturn aa([&](){ RegCloseKey(classKey); });
        wchar_t className[256];
        DWORD classNameSize = sizeof(className);
        memset(className, 0, sizeof(className));
        if (RegGetValueW(classKey, NULL, NULL, RRF_RT_REG_SZ, NULL, (LPBYTE)className, &classNameSize) == ERROR_SUCCESS) {
            std::wstring classNameWstr(className);
            return convertStringToUTF8OnWin32(classNameWstr);
        }
        else {
            HKEY openWithProgidsKey;
            if (RegOpenKeyExW(HKEY_CURRENT_USER, pathW2.c_str(), 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &openWithProgidsKey) == ERROR_SUCCESS) {
                DoOnReturn ab([&](){ RegCloseKey(openWithProgidsKey); });
                DWORD nSubKeys = 0;
                if (RegQueryInfoKeyW(openWithProgidsKey, NULL, NULL, NULL, NULL, NULL, NULL, &nSubKeys, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                    if (nSubKeys > 0) {
                        return "~";
                    }
                }

            }
        }
    }
    return "";
}

void platformTrySaveImageFile(EventCallbackListener* listener) {}

void platformTryLoadImageFile(EventCallbackListener* listener) {}

//pairs in format {extension, name}
void platformTrySaveOtherFile(EventCallbackListener* listener, std::vector<std::pair<std::string,std::string>> filetypes, std::string windowTitle, int evt_id) {
    if (!g_config.useSystemFileDialog) {
        universal_platformTrySaveOtherFile(listener, filetypes, windowTitle, evt_id);
        return;
    }

    OPENFILENAMEW ofna;
    ZeroMemory(&ofna, sizeof(ofna));
    ofna.lStructSize = sizeof(ofna);
    ofna.hwndOwner = windows_getWindowHWND(g_currentWindow);
    std::wstring filterString = L"";
    for (auto& ft : filetypes) {
        filterString += utf8StringToWstring(ft.second) + L" ("+ utf8StringToWstring(ft.first) +L")";
        filterString.push_back('\0');
        filterString += L"*" + utf8StringToWstring(ft.first);
        filterString.push_back('\0');
    }
    filterString.push_back('\0');
    ofna.lpstrFilter = filterString.c_str();
    ofna.lpstrCustomFilter = NULL;
    ofna.nFilterIndex = lastFilterIndex;
    ofna.lpstrFile = fileNameBuffer;
    ofna.nMaxFile = MAX_PATH;
    ofna.lpstrFileTitle = NULL;
    ofna.lpstrInitialDir = NULL;
    ofna.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

    std::wstring windowTitleW = L"voidsprite: " + utf8StringToWstring(windowTitle);
    ofna.lpstrTitle = windowTitleW.c_str();

    std::wstring extensionW = utf8StringToWstring(filetypes[0].first);
    std::wstring extensionWtr = extensionW.substr(1);
    ofna.lpstrDefExt = extensionWtr.c_str();

    if (GetSaveFileNameW(&ofna)) {
        lastFilterIndex = ofna.nFilterIndex;
        std::wstring fileName = fileNameBuffer;
        std::wstring fileNameLower = fileName;
        std::transform(fileNameLower.begin(), fileNameLower.end(), fileNameLower.begin(), ::tolower);

        extensionW = utf8StringToWstring(filetypes[ofna.nFilterIndex-1].first);
        if (fileNameLower.size() < extensionW.size() || fileNameLower.substr(fileNameLower.size() - extensionW.size()) != extensionW) {
            fileName += extensionW;
        }
        listener->eventFileSaved(evt_id, fileName, ofna.nFilterIndex);
    }
    else {
        u32 winError = GetLastError();
        u32 commDlgError = CommDlgExtendedError();
        logerr(frmt("windows error opening dialog: {:08X} {:08X}", winError, commDlgError));
        // "unspecified error"
        if (winError == E_FAIL || commDlgError == 0xFFFF) {
            universal_platformTrySaveOtherFile(listener, filetypes, windowTitle, evt_id);
        }
    }
}

void platformTryLoadOtherFile(EventCallbackListener* listener, std::vector<std::pair<std::string, std::string>> filetypes, std::string windowTitle, int evt_id) {
    if (!g_config.useSystemFileDialog) {
        universal_platformTryLoadOtherFile(listener, filetypes, windowTitle, evt_id);
        return;
    }

    OPENFILENAMEW ofna;
    ZeroMemory(&ofna, sizeof(ofna));
    ofna.lStructSize = sizeof(ofna);
    ofna.hwndOwner = windows_getWindowHWND(g_currentWindow);
    std::wstring filterString = L"";
    for (auto& ft : filetypes) {
        if (!ft.first.empty()) {
            filterString += utf8StringToWstring(frmt("{} ({})", ft.second, ft.first));
        }
        else {
            filterString += utf8StringToWstring(ft.second);
        }
        filterString.push_back('\0');
        filterString += L"*" + utf8StringToWstring(ft.first);
        filterString.push_back('\0');
    }
    filterString.push_back('\0');
    ofna.lpstrFilter = filterString.c_str();
    ofna.lpstrCustomFilter = NULL;
    ofna.nFilterIndex = lastFilterIndex;
    ofna.lpstrFile = fileNameBuffer;
    ofna.nMaxFile = MAX_PATH;
    ofna.lpstrFileTitle = NULL;
    ofna.lpstrInitialDir = NULL;
    ofna.Flags = OFN_EXPLORER;

    std::wstring windowTitleW = L"voidsprite: " + utf8StringToWstring(windowTitle);
    ofna.lpstrTitle = windowTitleW.c_str();

    std::wstring extensionWtr;
    if (!filetypes[0].first.empty()) {
        std::wstring extensionW = utf8StringToWstring(filetypes[0].first);
        extensionWtr = extensionW.substr(1);
        ofna.lpstrDefExt = extensionWtr.c_str();
    }

    if (GetOpenFileNameW(&ofna)) {
        std::wstring fileName = fileNameBuffer;
        listener->eventFileOpen(evt_id, fileName, ofna.nFilterIndex);
    }
    else {
        u32 winError = GetLastError();
        u32 commDlgError = CommDlgExtendedError();
        logerr(frmt("windows error opening dialog: {:08X} {:08X}", winError, commDlgError));
        // "unspecified error"
        if (winError == E_FAIL || commDlgError == 0xFFFF) {
            universal_platformTryLoadOtherFile(listener, filetypes, windowTitle, evt_id);
        }
    }
}

bool platformCreateDirectory(PlatformNativePathString path) {
    std::filesystem::create_directory(path);
    return true;
}
bool platformRenameFile(PlatformNativePathString path, PlatformNativePathString newPath) {
    std::filesystem::rename(path, newPath);
    return true;
}

bool platformCopyFile(PlatformNativePathString from, PlatformNativePathString to) {
    return CopyFileW(from.c_str(), to.c_str(), FALSE);
}

void platformOpenFileLocation(PlatformNativePathString path) {
    std::wstring command = L"explorer /select,\"";
    command += path;
    command += L"\"";
    _wsystem(command.c_str());
}

FILE* platformOpenFile(PlatformNativePathString path, PlatformNativePathString mode) {
    FILE* ret;
    errno_t err = _wfopen_s(&ret, path.c_str(), mode.c_str());
    if (err != 0) {
        logprintf("Error opening file: %i\n", err);
    }
    return ret;
}

PlatformNativePathString platformEnsureDirAndGetConfigFilePath() {
    wchar_t fileNameBuffer[MAX_PATH+1];
    memset(fileNameBuffer, 0, (MAX_PATH+1) * sizeof(wchar_t));
    GetEnvironmentVariableW(L"APPDATA", fileNameBuffer, MAX_PATH);

    std::wstring appdataDir = fileNameBuffer;
    appdataDir += L"\\voidsprite\\";
    CreateDirectoryW(appdataDir.c_str(), NULL);

    std::wstring subDir = appdataDir + L"patterns\\";
    CreateDirectoryW(subDir.c_str(), NULL);

    subDir = appdataDir + L"templates\\";
    CreateDirectoryW(subDir.c_str(), NULL);

    subDir = appdataDir + L"9segmentpatterns\\";
    CreateDirectoryW(subDir.c_str(), NULL);

    subDir = appdataDir + L"palettes\\";
    CreateDirectoryW(subDir.c_str(), NULL);

    subDir = appdataDir + L"autosaves\\";
    CreateDirectoryW(subDir.c_str(), NULL);

    subDir = appdataDir + L"visualconfigs\\";
    CreateDirectoryW(subDir.c_str(), NULL);
    
    subDir = appdataDir + L"plugins\\";
    CreateDirectoryW(subDir.c_str(), NULL);

    return appdataDir;
}

std::vector<PlatformNativePathString> platformListFilesInDir(PlatformNativePathString path, std::string filterExtension) {

    std::vector<PlatformNativePathString> ret;
    for (const auto& file : std::filesystem::directory_iterator(path)) {
        if (filterExtension == "" || stringEndsWithIgnoreCase(file.path(), convertStringOnWin32(filterExtension))) {
            ret.push_back(file.path());
        }
    }
    return ret;
    /*std::vector<PlatformNativePathString> ret;
    WIN32_FIND_DATAW findData;
    HANDLE hFind = filterExtension == "" ? FindFirstFileW((path + L"\\*").c_str(), &findData) : FindFirstFileW((path + L"\\*" + convertStringOnWin32(filterExtension)).c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                continue;
            }
            ret.push_back(path + findData.cFileName);
        } while (FindNextFileW(hFind, &findData));
        FindClose(hFind);
    }
    return ret;*/
}

bool platformPutImageInClipboard(Layer* l) {
    //return universal_platformPushLayerToClipboard(l);

    if (OpenClipboard(windows_getWindowHWND(g_currentWindow))) {
        EmptyClipboard();

        int writtenFormats = 0;

        std::vector<u8> dibv5data = writeDIBv5ToMem(l);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, dibv5data.size());
        if (!hMem) {
            CloseClipboard();
            return false;
        }
        memcpy(GlobalLock(hMem), dibv5data.data(), dibv5data.size());
        GlobalUnlock(hMem);
        if (!SetClipboardData(CF_DIBV5, hMem)) {
            GlobalFree(hMem);
        }
        else {
            writtenFormats++;
        }

        UINT pngFormat = RegisterClipboardFormatW(L"PNG");
        std::vector<u8> pngData = writePNGToMem(l);
        uint64_t fileLength = pngData.size();

        HGLOBAL pngHMem = GlobalAlloc(GMEM_MOVEABLE, fileLength);
        if (!pngHMem) {
            CloseClipboard();
            return false;
        }
        memcpy(GlobalLock(pngHMem), pngData.data(), fileLength);
        GlobalUnlock(pngHMem);
        if (!SetClipboardData(pngFormat, pngHMem)) {
            GlobalFree(pngHMem);
        }
        else {
            writtenFormats++;
        }

        CloseClipboard();
        return writtenFormats > 0;
    }
    else {
        return false;
    }
    
}

Layer* platformGetImageFromClipboard() {
    HWND currentHwnd = windows_getWindowHWND(g_currentWindow);
    bool res;
    HANDLE dataHandle;
    res = OpenClipboard(currentHwnd);
    UINT fileNameFormat = RegisterClipboardFormatW(L"FileNameW");
    dataHandle = GetClipboardData(fileNameFormat);
    Layer* foundImage = NULL;
    if (dataHandle != NULL) {
        void* pData = GlobalLock(dataHandle);
        if (pData) {
            SIZE_T size = GlobalSize(dataHandle);
            std::wstring filePath;
            filePath.resize(size);
            memcpy(&filePath[0], pData, size);
            GlobalUnlock(dataHandle);
            if (std::filesystem::exists(filePath)) {
                foundImage = loadAnyIntoFlat(convertStringToUTF8OnWin32(filePath));
            }
        }
    }
    else {
        loginfo("No file path data in clipboard, moving on to reading the whole image...");
    }
    if (foundImage != NULL) {
        return foundImage;
    }

    CloseClipboard();

    Layer* foundPNG = NULL;
    for (std::wstring pngFormatName : {L"PNG", L"image/png"}) { //browsers use PNG, krita saves to image/png
        res = OpenClipboard(currentHwnd);
        UINT pngFormat = RegisterClipboardFormatW(pngFormatName.c_str());
        dataHandle = GetClipboardData(pngFormat);
        if (dataHandle != NULL) {
            void* pData = GlobalLock(dataHandle);
            if (pData) {
                SIZE_T size = GlobalSize(dataHandle);
                u8* mem = (u8*)tracked_malloc(size);
                memcpy(mem, pData, size);
                GlobalUnlock(dataHandle);
                foundPNG = readPNGFromMem(mem, size);
                tracked_free(mem);
                CloseClipboard();
                break;
            }
        }
        CloseClipboard();
    }

    if (foundPNG != NULL) {
        return foundPNG;
    }
    else {
        loginfo("No PNG data in clipboard");
    }

    //dibv5
    res = OpenClipboard(currentHwnd);
    Layer* foundDIBV5 = NULL;
    dataHandle = GetClipboardData(CF_DIBV5);
    if (dataHandle != NULL) {
        void* pData = GlobalLock(dataHandle);
        if (pData) {
            SIZE_T size = GlobalSize(dataHandle);
            u8* mem = (u8*)tracked_malloc(size);
            memcpy(mem, pData, size);
            GlobalUnlock(dataHandle);
            foundDIBV5 = readDIBv5FromMem(mem, size);
            tracked_free(mem);
        }
    }
    CloseClipboard();

    //if there's no PNG in the clipboard, read as a bitmap
    res = OpenClipboard(currentHwnd);
    dataHandle = GetClipboardData(CF_BITMAP);
    if (dataHandle == NULL) {
        CloseClipboard();
        return NULL;
    }
    HBITMAP bmp = (HBITMAP)dataHandle;
    BITMAP bitmap;
    GetObjectW(bmp, sizeof(BITMAP), &bitmap);
    HDC hdc = GetDC(currentHwnd);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, bmp);
    Layer* layer = new Layer(bitmap.bmWidth, bitmap.bmHeight);
    for (int y = 0; y < bitmap.bmHeight; y++) {
        for (int x = 0; x < bitmap.bmWidth; x++) {
            COLORREF color = GetPixel(memDC, x, y);
            layer->setPixel({ x, y }, sdlcolorToUint32(SDL_Color{ GetRValue(color), GetGValue(color), GetBValue(color), 255 }));
        }
    }
    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
    ReleaseDC(currentHwnd, hdc);
    CloseClipboard();
    return layer;
}

std::string platformGetSystemInfo() {
    std::string ret;
    ret += frmt("OS: {} {} {}\n", 
        windows_readStringFromRegistry(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName"), 
        windows_readStringFromRegistry(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"DisplayVersion"), 
        windows_readStringFromRegistry(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"BuildLab"));
    if (!windows_isProcessRunning(L"csrss.exe")) {
        if (std::filesystem::exists("Z:\\bin\\sh")) {
            ret += "  (Running on a compatibility layer)\n";
        }
        else {
            ret += "  (Running on a compatibility layer?)\n";
        }
    }

    SYSTEM_INFO sysinfo;
    SYSTEM_INFO sysinfoNative;
    GetSystemInfo(&sysinfo);
    GetNativeSystemInfo(&sysinfoNative);
    std::string cpuArch = 
        sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "x64"
        : sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM ? "ARM"
        : sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64 ? "ARM64"
        : sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL ? "x86"
        : sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ? "Intel Itanium"   //lmao
        : "???";
    std::string cpuArchNative = 
        sysinfoNative.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "x64"
        : sysinfoNative.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM ? "ARM"
        : sysinfoNative.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64 ? "ARM64"
        : sysinfoNative.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL ? "x86"
        : sysinfoNative.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ? "Intel Itanium"
        : "???";

    std::wstring pcName;
    u32 pcNameSize = 256;
    pcName.resize(pcNameSize);
    if (GetComputerNameW(pcName.data(), (LPDWORD)&pcNameSize)) {
        pcName.resize(pcNameSize);
        ret += "Network name: " + convertStringToUTF8OnWin32(pcName) + "\n";
    }

    ret += frmt("System: {} {}\n",
        windows_readStringFromRegistry(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"SystemManufacturer"),
        windows_readStringFromRegistry(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"SystemProductName"));
    ret += "CPU: " + windows_readStringFromRegistry(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"ProcessorNameString") + "\n";
    ret += frmt("CPU arch: {} {}\n", cpuArch, cpuArch != cpuArchNative ? frmt("  (real: {})", cpuArchNative) : "");
    ret += "GPU: " + windows_getActiveGPUName() + "\n";

    ret += frmt("System memory: {} MiB\n", SDL_GetSystemRAM());

    return ret;
}

std::vector<RootDirInfo> platformListRootDirectories() {
    std::vector<RootDirInfo> ret;

    //get userprofile dir
    wchar_t userProfilePath[MAX_PATH + 1];
    memset(userProfilePath, 0, (MAX_PATH + 1) * sizeof(wchar_t));
    GetEnvironmentVariableW(L"USERPROFILE", userProfilePath, MAX_PATH);
    std::wstring userProfilePathW = userProfilePath;

    for (auto& [subdir, name] : std::vector<std::pair<std::wstring, std::string>>{ 
        {L"", "User home"},
        {L"Documents", "Documents"},
        {L"Desktop", "Desktop"},
        {L"Downloads", "Downloads"},
        {L"Pictures", "Pictures"},
        {L"Videos", "Videos"},
        {L"Music", "Music"},
    }) {
        std::wstring fullSubdir = appendPath(userProfilePathW, subdir);
        if (std::filesystem::exists(fullSubdir)) {
            ret.push_back({name, fullSubdir });
        }
    }

    u32 logicalDrives = GetLogicalDrives();
    u32 maskNow = 1;
    for (char drive = 'A'; drive <= 'Z'; drive++) {
        if (logicalDrives & maskNow) {
            std::wstring drivePath = frmt(L"{}:\\", drive);
            UINT driveType = GetDriveTypeW(drivePath.c_str());
            if (driveType != DRIVE_NO_ROOT_DIR) {
                std::string name = 
                    driveType == DRIVE_RAMDISK ? frmt("RAM Disk {}:\\", drive)
                    : driveType == DRIVE_CDROM ? frmt("CD-ROM {}:\\", drive)
                    : driveType == DRIVE_REMOVABLE ? frmt("Removable Drive {}:\\", drive)
                    : frmt("Drive {}:\\", drive);
                ret.push_back({name, drivePath});
            }
        }
        maskNow <<= 1;
    }

    return ret;
}

bool platformHasFileAccessPermissions() {
    return true;
}
void platformRequestFileAccessPermissions() {}

void platformOpenWebpageURL(std::string url) {
    if (!stringStartsWithIgnoreCase(url, "http")) {
        url = "http://" + url;
    }
    ShellExecuteW(NULL, L"open", convertStringOnWin32(url).c_str(), NULL, NULL, SW_SHOWNORMAL);
}

std::string platformFetchTextFile(std::string url) {
    std::wstring wurl = convertStringOnWin32(url);

    URL_COMPONENTS urlComp{};
    wchar_t host[256], path[1024];
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.lpszHostName = host;
    urlComp.dwHostNameLength = _countof(host);
    urlComp.lpszUrlPath = path;
    urlComp.dwUrlPathLength = _countof(path);

    if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &urlComp)) {
        throw std::runtime_error("Invalid URL");
    }

    HINTERNET hSession = WinHttpOpen(L"voidsprite/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) throw std::runtime_error("WinHttpOpen failed");
    DoOnReturn a1([&]() {
        WinHttpCloseHandle(hSession);
    });

    HINTERNET hConnect = WinHttpConnect(hSession, urlComp.lpszHostName,
        urlComp.nPort, 0);
    if (!hConnect) {
        throw std::runtime_error("WinHttpConnect failed");
    }
    DoOnReturn a2([&]() {
        WinHttpCloseHandle(hConnect);
    });

    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
        L"GET",
        urlComp.lpszUrlPath,
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        (urlComp.nScheme == INTERNET_SCHEME_HTTPS)
        ? WINHTTP_FLAG_SECURE
        : 0);
    if (!hRequest) {
        throw std::runtime_error("WinHttpOpenRequest failed");
    }
    DoOnReturn a3([&]() {
        WinHttpCloseHandle(hRequest);
    });

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        throw std::runtime_error("WinHttpSendRequest failed");
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        throw std::runtime_error("WinHttpReceiveResponse failed");
    }

    DWORD statusCode = 0, size = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        NULL, &statusCode, &size, NULL);
    if (statusCode != 200) {
        throw std::out_of_range("HTTP status code: " + std::to_string(statusCode));
    }

    std::stringstream responseStream;
    DWORD bytesAvailable = 0;
    do {
        WinHttpQueryDataAvailable(hRequest, &bytesAvailable);
        if (bytesAvailable == 0) break;

        std::string buffer(bytesAvailable, '\0');
        DWORD bytesRead = 0;
        WinHttpReadData(hRequest, &buffer[0], bytesAvailable, &bytesRead);
        responseStream.write(buffer.data(), bytesRead);
    } while (bytesAvailable > 0);

    return responseStream.str();
}

std::vector<u8> platformFetchBinFile(std::string url) {
    std::wstring wurl = convertStringOnWin32(url);

    URL_COMPONENTS urlComp{};
    wchar_t host[256], path[1024];
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.lpszHostName = host;
    urlComp.dwHostNameLength = _countof(host);
    urlComp.lpszUrlPath = path;
    urlComp.dwUrlPathLength = _countof(path);

    if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &urlComp)) {
        throw std::runtime_error("Invalid URL");
    }

    HINTERNET hSession = WinHttpOpen(L"voidsprite/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) throw std::runtime_error("WinHttpOpen failed");
    DoOnReturn a1([&]() {
        WinHttpCloseHandle(hSession);
        });

    HINTERNET hConnect = WinHttpConnect(hSession, urlComp.lpszHostName,
        urlComp.nPort, 0);
    if (!hConnect) {
        throw std::runtime_error("WinHttpConnect failed");
    }
    DoOnReturn a2([&]() {
        WinHttpCloseHandle(hConnect);
        });

    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
        L"GET",
        urlComp.lpszUrlPath,
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        (urlComp.nScheme == INTERNET_SCHEME_HTTPS)
        ? WINHTTP_FLAG_SECURE
        : 0);
    if (!hRequest) {
        throw std::runtime_error("WinHttpOpenRequest failed");
    }
    DoOnReturn a3([&]() {
        WinHttpCloseHandle(hRequest);
        });

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        throw std::runtime_error("WinHttpSendRequest failed");
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        throw std::runtime_error("WinHttpReceiveResponse failed");
    }

    DWORD statusCode = 0, size = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        NULL, &statusCode, &size, NULL);
    if (statusCode != 200) {
        throw std::out_of_range("HTTP status code: " + std::to_string(statusCode));
    }

    std::vector<u8> responseData;
    u64 bytesWritten = 0;
    DWORD bytesAvailable = 0;
    do {
        WinHttpQueryDataAvailable(hRequest, &bytesAvailable);
        if (bytesAvailable == 0) break;

        responseData.resize(bytesWritten + bytesAvailable);
        DWORD bytesRead = 0;
        WinHttpReadData(hRequest, &responseData[bytesWritten], bytesAvailable, &bytesRead);
        bytesWritten += bytesRead;
    } while (bytesAvailable > 0);

    return responseData;
}

void platformPrintDocument(Layer* layer) {
    PlatformNativePathString tempPath;
    tempPath.resize(MAX_PATH);
    int s = GetTempPathW(tempPath.size(), tempPath.data());
    tempPath.resize(s);

    PlatformNativePathString tempFile;
    tempFile.resize(MAX_PATH);
    UINT tempFileLen = GetTempFileNameW(tempPath.c_str(), L"vsp", 0, tempFile.data());
    tempFile.resize(lstrlenW(tempFile.data()));

    if (tempFileLen != 0) {
        tempFile += L".png";
        if (writePNG(tempFile, layer)) {
            tempFilesToDeleteOnDeinit.push_back(tempFile);
            ShellExecuteW(windows_getWindowHWND(g_currentWindow), L"print", tempFile.c_str(), L"", L"", FALSE);
            //todo: delete these temp files
            loginfo(frmt("Sent print command for: {}", convertStringToUTF8OnWin32(tempFile)));
        }
        else {
            logerr("writepng failed [print]");
        }
    }
    else {
        logerr("failed to get temp file name");
    }
}

void platformShareImage(Layer* layer) {}

std::vector<NetworkAdapterInfo> platformGetNetworkAdapters() {
    std::vector<NetworkAdapterInfo> ret = {};

    IP_ADAPTER_INFO inf;
    unsigned long infSize = sizeof(inf);

    PIP_ADAPTER_INFO infActual;

    if (GetAdaptersInfo(&inf, &infSize) == ERROR_BUFFER_OVERFLOW) {
        infActual = (PIP_ADAPTER_INFO)tracked_malloc(infSize);
    }
    else {
        infActual = (PIP_ADAPTER_INFO)tracked_malloc(sizeof(inf));
    }
    DoOnReturn f([infActual]() {tracked_free(infActual); });

    if (GetAdaptersInfo(infActual, &infSize) == ERROR_SUCCESS) {
        PIP_ADAPTER_INFO next = infActual;
        while (next) {
            NetworkAdapterInfo newInf{};
            u32 ipv4 = parseIpAddress(next->IpAddressList.IpAddress.String);
            if (ipv4 != 0) {
                /*loginfo(frmt(
                    "Name: {} /\n     {}\n  IP: {}\n  mask: {}\n",
                    next->AdapterName,
                    next->Description,
                    next->IpAddressList.IpAddress.String,
                    next->IpAddressList.IpMask.String
                ));*/
                newInf.name = next->Description;
                newInf.thisMachineAddress = next->IpAddressList.IpAddress.String;

                u32 mask = parseIpAddress(next->IpAddressList.IpMask.String);
                newInf.broadcastAddress = ipToString((ipv4 & mask) | ~mask);
                ret.push_back(newInf);

                //loginfo(frmt("Network adapter:\n  Name: {}\n  IP: {}\n  Broadcast address: {}\n", 
                    //newInf.name, newInf.thisMachineAddress, newInf.broadcastAddress));
            }
            
            next = next->Next;
        }
    }

    return ret;
}

std::vector<PlatformNativePathString> platformGetSystemFontPaths() {
    return {
        L"C:\\Windows\\Fonts"
    };
}

const std::string ipcPipePath = "\\\\.\\pipe\\voidspriteIPC";
void windows_ipcThread() {
    while (true) {
        HANDLE pipe = CreateNamedPipeA(
            ipcPipePath.c_str(),
            PIPE_ACCESS_INBOUND,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,
            1024,
            1024,
            0,
            NULL
        );

        if (pipe == INVALID_HANDLE_VALUE) {
            logerr("failed to open IPC pipe");
            return;
        }

        //absolute win32
        if (ConnectNamedPipe(pipe, NULL) || (GetLastError() == ERROR_PIPE_CONNECTED)) {
            char buffer[1024];
            DWORD bytesRead;

            if (ReadFile(pipe, buffer, 1024, &bytesRead, NULL)) {
                std::string ipcCommand;
                ipcCommand.resize(bytesRead);
                memcpy(ipcCommand.data(), buffer, bytesRead);

                main_handleIPCMessage(ipcCommand);
            }
        }

        DisconnectNamedPipe(pipe);
        CloseHandle(pipe);
    }
}
bool platformSetupIPC() {
    HANDLE vspMutex = CreateMutexA(NULL, TRUE, "voidsprite_SingleInstance");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return false;
    }
    else {
        ipcThread = new std::jthread(windows_ipcThread);
        return true;
    }
}

bool platformSendIPCToMainInstance(std::string s) {
    HANDLE pipe = CreateFileA(
        ipcPipePath.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD written;
    WriteFile(pipe, s.c_str(), s.size(), &written, NULL);

    CloseHandle(pipe);
    return true;
}