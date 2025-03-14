#pragma once

//no dwmapi on xp
#if WINDOWS_XP == 0
#include <dwmapi.h>
#endif

#include <SDL3/SDL_properties.h>

#include <d3d9.h>
#include <windows.h>
#include <commdlg.h>

HWND WINhWnd = NULL;
wchar_t fileNameBuffer[MAX_PATH] = { 0 };
int lastFilterIndex = 1;

void platformPreInit() {

}
void platformInit() {}
void platformPostInit() {
    static bool d = false;
    if (!d) {
		#if WINDOWS_XP == 0
            BOOL USE_DARK_MODE = true;
            WINhWnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(g_wd), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
            bool SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(
                WINhWnd, 20,
                &USE_DARK_MODE, sizeof(USE_DARK_MODE)));
            SDL_HideWindow(g_wd);
		#endif
        SDL_ShowWindow(g_wd);
        //RedrawWindow(WINhWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        //UpdateWindow(WINhWnd);
        //RedrawWindow(WINhWnd, NULL, NULL, RDW_INVALIDATE);
        //SendMessageW(WINhWnd, WM_PAINT, NULL, NULL);
        d = true;
    }

    IDirect3D9* d3dobject = Direct3DCreate9(D3D_SDK_VERSION);
    D3DADAPTER_IDENTIFIER9 ident;
    d3dobject->GetAdapterIdentifier(0, 0, &ident);
    printf("GPU: %s\n", ident.Description);
    d3dobject->Release();
}

void platformTrySaveImageFile(EventCallbackListener* listener) {}

void platformTryLoadImageFile(EventCallbackListener* listener) {}


//pairs in format {extension, name}
void platformTrySaveOtherFile(EventCallbackListener* listener, std::vector<std::pair<std::string,std::string>> filetypes, std::string windowTitle, int evt_id) {
    OPENFILENAMEW ofna;
    ZeroMemory(&ofna, sizeof(ofna));
    ofna.lStructSize = sizeof(ofna);
    ofna.hwndOwner = WINhWnd;
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
        printf("windows error: %i\n", GetLastError());
    }
}

void platformTryLoadOtherFile(EventCallbackListener* listener, std::vector<std::pair<std::string, std::string>> filetypes, std::string windowTitle, int evt_id) {
    OPENFILENAMEW ofna;
    ZeroMemory(&ofna, sizeof(ofna));
    ofna.lStructSize = sizeof(ofna);
    ofna.hwndOwner = WINhWnd;
    std::wstring filterString = L"";
    for (auto& ft : filetypes) {
        filterString += utf8StringToWstring(ft.second) + L" (" + utf8StringToWstring(ft.first) + L")";
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

    std::wstring extensionW = utf8StringToWstring(filetypes[0].first);
    std::wstring extensionWtr = extensionW.substr(1);
    ofna.lpstrDefExt = extensionWtr.c_str();

    if (GetOpenFileNameW(&ofna)) {
        std::wstring fileName = fileNameBuffer;
        listener->eventFileOpen(evt_id, fileName, ofna.nFilterIndex);
    }
    else {
        printf("windows error: %i\n", GetLastError());
    }
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
        printf("Error opening file: %i\n", err);
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

Layer* platformGetImageFromClipboard() {
    bool res = OpenClipboard(WINhWnd);
    HANDLE dataHandle = GetClipboardData(CF_BITMAP);
    if (dataHandle == NULL) {
        CloseClipboard();
        return NULL;
    }
    HBITMAP bmp = (HBITMAP)dataHandle;
    BITMAP bitmap;
    GetObject(bmp, sizeof(BITMAP), &bitmap);
    HDC hdc = GetDC(WINhWnd);
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
	ReleaseDC(WINhWnd, hdc);
	CloseClipboard();
	return layer;
}