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
    bool showPenPressure = true;
    bool showFPS = false;
    bool checkUpdates = true;
    int powerSaverLevel = 3;    // 0: none
                                // 1: delay 45 on unfocus
                                // 2: delay 500 on unfocus
                                // 3: auto      (0 if no battery, 1 if battery, 2 if battery <15%)
    bool singleInstance = true;

    std::vector<std::string> lastOpenFiles;

    //this is only during initial load. not updated at runtime.
    std::vector<std::string> keybinds;
};

struct DebugSettings {
    bool debugShowScrollPanelBounds = false;
    bool debugColorSliderGradients = false;
    bool debugTestLocalization = false;
    bool debugShowTilesRPG2K = false;
};

inline bool g_configWasLoaded = false;
inline GlobalConfig g_config;
inline std::vector<std::string> g_availableRenderersNow;

inline DebugSettings g_debugConfig{};


bool g_saveConfig();
void g_loadConfig();

void g_tryPushLastFilePath(std::string a);
void g_removeFromLastOpenFiles(std::string a);
void g_clearLastOpenFiles();