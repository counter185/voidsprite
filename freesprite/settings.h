#pragma once

class GlobalConfig {
public:
    bool openSavedPath = true;
    int animatedBackground = 1; //0:off, 1:sharp, 2:smooth, 3:sharp(static), 4:smooth(static)
    int maxUndoHistory = 20;
    bool scrollWithTouchpad = false;
    bool isolateRectOnLockTile = false;
    bool fillToolTileBound = true;
    bool vsync = true;
    bool saveLoadFlatImageExtData = true;
    bool useDiscordRPC = false;

    std::vector<std::string> lastOpenFiles;

    //this is used only during initial load. not updated at runtime.
    std::map<std::string, SDL_Keycode> keybinds;
};

inline bool g_configWasLoaded = false;
inline GlobalConfig g_config;

bool g_saveConfig();
void g_loadConfig();

void g_tryPushLastFilePath(std::string a);