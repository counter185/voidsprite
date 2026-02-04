#pragma once

class GlobalConfig {
public:
    bool openSavedPath = ONPLATFORM(VSP_PLATFORM_ANDROID, false, true);
    int animatedBackground = 1; //0:off, 1:sharp, 2:smooth, 3:sharp(static), 4:smooth(static)
    int maxUndoHistory = ONPLATFORM(VSP_PLATFORM_EMSCRIPTEN, 15, 20);
    bool scrollWithTouchpad = false;
    bool isolateRectOnLockTile = false;
    bool fillToolTileBound = true;
    bool vsync = true;
    bool saveLoadFlatImageExtData = true;
    bool useDiscordRPC = false;
    bool brushColorPreview = false;
    std::string preferredRenderer = ONPLATFORM(VSP_PLATFORM_WIN32, "direct3d", "");
    int autosaveInterval = 20;
    bool rowColIndexesStartAt1 = false;
    std::string language = "en-us";
    bool vfxEnabled = true;
    bool overrideCursor = true;
    std::string customVisualConfigPath = "";
    bool useSystemFileDialog = ONPLATFORM(VSP_PLATFORM_ANDROID, false, true);
    bool showPenPressure = true;
    bool showFPS = false;
    bool checkUpdates = true;
    int powerSaverLevel = ONPLATFORM(VSP_PLATFORM_ANDROID, 2,
                          ONPLATFORM(VSP_PLATFORM_EMSCRIPTEN, 1, 3));
                                // 0: none
                                // 1: delay 45 on unfocus
                                // 2: delay 500 on unfocus
                                // 3: auto      (0 if no battery, 1 if battery, 2 if battery <15%)
    bool singleInstance = true;
    int canvasZoomSensitivity = 1;//1 to 50
    bool compactEditor = false;
    bool autoViewportScale = ONPLATFORM(VSP_PLATFORM_ANDROID, true, false);
    bool longErrorNotifs = false;
    bool hueWheelInsteadOfSlider = false;
    u32 backtraceColor = 0xFF0000FF;
    u32 fwdtraceColor = 0xFF00FF00;
    bool smoothFonts = true;
    bool acrylicPanels = false;

    std::vector<std::string> lastOpenFiles;

    //this is only during initial load. not updated at runtime.
    std::vector<std::string> keybinds;
};

struct DebugSettings {
    bool debugShowScrollPanelBounds = false;
    bool debugColorSliderGradients = false;
    bool debugTestLocalization = false;
    bool debugShowTilesRPG2K = false;
    bool debugColorWheel = false;
    bool debugBlurBehind = false;
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