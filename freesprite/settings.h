#pragma once

struct GlobalConfig {
    bool openSavedPath = true;
    int animatedBackground = 1; //0:off, 1:sharp, 2:smooth, 3:sharp(static), 4:smooth(static)
    int maxUndoHistory = 20;
    bool scrollWithTouchpad = false;
    bool isolateRectOnLockTile = false;
    bool fillToolTileBound = true;
    bool vsync = true;

    //this is used only during initial load. not updated at runtime.
    std::map<std::string, SDL_Keycode> keybinds;
};

inline GlobalConfig g_config;

bool g_saveConfig();
void g_loadConfig();