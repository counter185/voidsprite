#pragma once

//no dwmapi on xp
#if WINDOWS_XP == 0
#include <dwmapi.h>
#endif

#ifdef __GNUC__
#include <SDL2/SDL_syswm.h>
#else
#include <SDL_syswm.h>
#endif

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
            SDL_SysWMinfo wmInfo;
            SDL_VERSION(&wmInfo.version);
            SDL_GetWindowWMInfo(g_wd, &wmInfo);
            BOOL USE_DARK_MODE = true;
            WINhWnd = wmInfo.info.win.window;
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

void platformTrySaveImageFile(EventCallbackListener* listener) {
    OPENFILENAMEW ofna;
    ZeroMemory(&ofna, sizeof(ofna));
    ofna.lStructSize = sizeof(ofna);
    ofna.hwndOwner = WINhWnd;
    std::wstring filterString = L"";
    std::vector<std::wstring> filterStrings;
    for (FileExportMultiLayerNPath f : g_fileExportersMLNPaths) {
        filterString += utf8StringToWstring(f.name) + std::format(L"({})", utf8StringToWstring(f.extension));
        filterString.push_back('\0');
        filterString += L"*" + utf8StringToWstring(f.extension);
        filterString.push_back('\0');
        filterStrings.push_back(utf8StringToWstring(f.extension));
    }
    for (FileExportFlatNPath f : g_fileExportersFlatNPaths) {
        filterString += utf8StringToWstring(f.name) + std::format(L"({})", utf8StringToWstring(f.extension));
        filterString.push_back('\0');
        filterString += L"*" + utf8StringToWstring(f.extension);
        filterString.push_back('\0');
        filterStrings.push_back(utf8StringToWstring(f.extension));
    }
    filterString.push_back('\0');
    //ofna.lpstrFilter = (LPWSTR)L"PNG Files (.png)\0*.png\0OpenRaster Files (.ora)\0*.ora\0voidsprite Session Files v2 (.voidsn)\0*.voidsn\0BMP Files (.bmp)\0*.bmp\0CaveStory PBM Files (.pbm)\0*.pbm\0RPG2000/2003 XYZ Files (.xyz)\0*.xyz\0All files\0*.*\0\0";
    ofna.lpstrFilter = filterString.c_str();
    ofna.lpstrCustomFilter = NULL;
    ofna.nFilterIndex = lastFilterIndex;
    ofna.lpstrFile = fileNameBuffer;
    ofna.nMaxFile = MAX_PATH;
    ofna.lpstrFileTitle = NULL;
    ofna.lpstrInitialDir = NULL;
    ofna.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
    ofna.lpstrTitle = L"voidsprite: Save Image";
    ofna.lpstrDefExt = L"png";
    if (GetSaveFileNameW(&ofna)) {
        lastFilterIndex = ofna.nFilterIndex;
        std::wstring fileName = fileNameBuffer;
        std::wstring extension = filterStrings[ofna.nFilterIndex - 1];
        if (fileName.size() < extension.size() || fileName.substr(fileName.size() - extension.size()) != extension) {
            fileName += extension;
        }
        listener->eventFileSaved(EVENT_MAINEDITOR_SAVEFILE, fileName, ofna.nFilterIndex);
    }
    else {
        printf("windows error: %i\n", GetLastError());
    }
}

void platformTryLoadImageFile(EventCallbackListener* listener) {
    OPENFILENAMEW ofna;
    ZeroMemory(&ofna, sizeof(ofna));
    ofna.lStructSize = sizeof(ofna);
    ofna.hwndOwner = WINhWnd;
    std::wstring filterString = L"";
    std::vector<std::wstring> filterStrings;
    for (FileSessionImportNPath f : g_fileSessionImportersNPaths) {
        filterString += utf8StringToWstring(f.name) + std::format(L"({})", utf8StringToWstring(f.extension));
        filterString.push_back('\0');
        filterString += L"*" + utf8StringToWstring(f.extension);
        filterString.push_back('\0');
        filterStrings.push_back(utf8StringToWstring(f.extension));
    }
    for (FileImportNPath f : g_fileImportersNPaths) {
        filterString += utf8StringToWstring(f.name) + std::format(L"({})", utf8StringToWstring(f.extension));
        filterString.push_back('\0');
        filterString += L"*" + utf8StringToWstring(f.extension);
        filterString.push_back('\0');
        filterStrings.push_back(utf8StringToWstring(f.extension));
    }
    filterString.push_back('\0');
    //ofna.lpstrFilter = (LPWSTR)L"PNG Files (.png)\0*.png\0OpenRaster Files (.ora)\0*.ora\0voidsprite Session Files v2 (.voidsn)\0*.voidsn\0BMP Files (.bmp)\0*.bmp\0CaveStory PBM Files (.pbm)\0*.pbm\0RPG2000/2003 XYZ Files (.xyz)\0*.xyz\0All files\0*.*\0\0";
    ofna.lpstrFilter = filterString.c_str();
    ofna.lpstrCustomFilter = NULL;
    ofna.nFilterIndex = lastFilterIndex;
    ofna.lpstrFile = fileNameBuffer;
    ofna.nMaxFile = MAX_PATH;
    ofna.lpstrFileTitle = NULL;
    ofna.lpstrInitialDir = NULL;
    ofna.Flags = OFN_EXPLORER;
    ofna.lpstrTitle = L"voidsprite: Open Image";
    ofna.lpstrDefExt = L"png";
    if (GetOpenFileNameW(&ofna)) {
        lastFilterIndex = ofna.nFilterIndex;
        std::wstring fileName = fileNameBuffer;
        std::wstring extension = filterStrings[ofna.nFilterIndex - 1];
        if (fileName.size() < extension.size() || fileName.substr(fileName.size() - extension.size()) != extension) {
            fileName += extension;
        }
        listener->eventFileOpen(EVENT_MAINEDITOR_SAVEFILE, fileName, ofna.nFilterIndex);
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
    _wfopen_s(&ret, path.c_str(), mode.c_str());
    return ret;
}