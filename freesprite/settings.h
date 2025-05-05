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
    bool brushColorPreview = false;
    std::string preferredRenderer =
#if _WIN32
        "direct3d"
#else
        ""
#endif
        ;
    int autosaveInterval = 20;
    bool rowColIndexesStartAt1 = false;
    std::string language = "en-us";
    bool vfxEnabled = true;
    bool overrideCursor = true;
    std::string customVisualConfigPath = "";
    bool useSystemFileDialog = 
#if __ANDROID__
        false
#else
        true
#endif   
        ;

    std::vector<std::string> lastOpenFiles;

    //this is used only during initial load. not updated at runtime.
    std::map<std::string, SDL_Keycode> keybinds;
};

inline bool g_configWasLoaded = false;
inline GlobalConfig g_config;
inline std::vector<std::string> g_availableRenderersNow;

bool g_saveConfig();
void g_loadConfig();

void g_tryPushLastFilePath(std::string a);