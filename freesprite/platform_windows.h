#pragma once

#include <dwmapi.h>
#include <SDL_syswm.h>
#include <d3d9.h>
#include <windows.h>
#include <commdlg.h>

HWND WINhWnd = NULL;
wchar_t fileNameBuffer[MAX_PATH] = { 0 };

void platformPreInit() {

}
void platformInit() {}
void platformPostInit() {
    static bool d = false;
    if (!d) {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(g_wd, &wmInfo);
        BOOL USE_DARK_MODE = true;
        WINhWnd = wmInfo.info.win.window;
        bool SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(
            WINhWnd, 20,
            &USE_DARK_MODE, sizeof(USE_DARK_MODE)));
        SDL_HideWindow(g_wd);
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
    ofna.lpstrFilter = (LPWSTR)L"PNG Files (.png)\0*.png\0voidsprite Session Files v1 (.voidsn)\0*.voidsn\0All files\0*.*\0\0";
    ofna.lpstrCustomFilter = NULL;
    ofna.nFilterIndex = 1;
    ofna.lpstrFile = fileNameBuffer;
    ofna.nMaxFile = MAX_PATH;
    ofna.lpstrFileTitle = NULL;
    ofna.lpstrInitialDir = NULL;
    ofna.Flags = OFN_EXPLORER;
    ofna.lpstrTitle = L"voidsprite: Save Image";
    ofna.lpstrDefExt = L"png";
    if (GetSaveFileNameW(&ofna)) {
        listener->eventFileSavedW(EVENT_MAINEDITOR_SAVEFILE, std::wstring(fileNameBuffer));
    }
    else {
        printf("windows error: %i\n", GetLastError());
    }
}