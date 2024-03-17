#pragma once

#include <dwmapi.h>
#include <SDL_syswm.h>
#include <d3d9.h>

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
        HWND WINhWnd = wmInfo.info.win.window;
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