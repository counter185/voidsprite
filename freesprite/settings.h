#pragma once
#include "mathops.h"

struct GlobalConfig {
    bool openSavedPath = true;
    int animatedBackground = 1; //0:off, 1:sharp, 2:smooth, 3:sharp(static), 4:smooth(static)
    int maxUndoHistory = 20;
    bool scrollWithTouchpad = false;
    bool isolateRectOnLockTile = false;
    bool fillToolTileBound = true;
};

inline GlobalConfig g_config;

inline bool g_saveConfig() {
    PlatformNativePathString path = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("config.txt");
    std::ofstream file(path);
    if (file.is_open()) {
        file << "openSavedPath=" << (g_config.openSavedPath ? "1" : "0") << std::endl;
        file << "animatedBackground=" << std::to_string(g_config.animatedBackground) << std::endl;
        file << "maxUndoHistory=" << std::to_string(g_config.maxUndoHistory) << std::endl;
        file << "scrollWithTouchpad=" << (g_config.scrollWithTouchpad ? "1" : "0") << std::endl;
        file << "isolateRectOnLockTile=" << (g_config.isolateRectOnLockTile ? "1" : "0") << std::endl;
        file << "fillToolTileBound=" << (g_config.fillToolTileBound ? "1" : "0") << std::endl;
        file.close();
        return true;
    }
    else {
        return false;
    }
}

inline void g_loadConfig() {
    PlatformNativePathString path = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("config.txt");
    std::ifstream file(path);
    if (file.is_open()) {
        std::map<std::string, std::string> config;
        std::string line;
        while (std::getline(file, line)) {
            std::string key = line.substr(0, line.find("="));
            std::string value = line.substr(line.find("=") + 1);
            config[key] = value;
        }

        if (config.contains("openSavedPath")) { g_config.openSavedPath = config["openSavedPath"] == "1"; }
        if (config.contains("animatedBackground")) { try { g_config.animatedBackground = std::stoi(config["animatedBackground"]); } catch (std::exception) {} }
        if (config.contains("maxUndoHistory")) { try { g_config.maxUndoHistory = std::stoi(config["maxUndoHistory"]); } catch (std::exception) {} }
        if (config.contains("scrollWithTouchpad")) { g_config.scrollWithTouchpad = config["scrollWithTouchpad"] == "1"; }
        if (config.contains("isolateRectOnLockTile")) { g_config.isolateRectOnLockTile = config["isolateRectOnLockTile"] == "1"; }
        if (config.contains("fillToolTileBound")) { g_config.fillToolTileBound = config["fillToolTileBound"] == "1"; }

        file.close();
    }
}
