#include <fstream>

#include "globals.h"
#include "mathops.h"
#include "brush/BaseBrush.h"
#include "keybinds.h"

bool g_saveConfig() {
    PlatformNativePathString path = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("config.txt");
    std::ofstream file(path);
    if (file.is_open()) {
        file << "openSavedPath=" << (g_config.openSavedPath ? "1" : "0") << std::endl;
        file << "animatedBackground=" << std::to_string(g_config.animatedBackground) << std::endl;
        file << "maxUndoHistory=" << std::to_string(g_config.maxUndoHistory) << std::endl;
        file << "scrollWithTouchpad=" << (g_config.scrollWithTouchpad ? "1" : "0") << std::endl;
        file << "isolateRectOnLockTile=" << (g_config.isolateRectOnLockTile ? "1" : "0") << std::endl;
        file << "fillToolTileBound=" << (g_config.fillToolTileBound ? "1" : "0") << std::endl;
        file << "vsync=" << (g_config.vsync ? "1" : "0") << std::endl;
        file << "saveLoadFlatImageExtData=" << (g_config.saveLoadFlatImageExtData ? "1" : "0") << std::endl;
        file << "discordRPC=" << (g_config.useDiscordRPC ? "1" : "0") << std::endl;
        file << "renderer=" << g_config.preferredRenderer << std::endl;
        file << "autosaveInterval=" << g_config.autosaveInterval << std::endl;
        file << "rowColIndexesStartAt1=" << g_config.rowColIndexesStartAt1 << std::endl;
        file << "language=" << g_config.language << std::endl;
        file << "vfxEnabled=" << (g_config.vfxEnabled ? "1" : "0") << std::endl;
        file << "overrideCursor=" << (g_config.overrideCursor ? "1" : "0") << std::endl;
        file << "visualConfig=" << g_config.customVisualConfigPath << std::endl;
        file << "useSystemFileDialog=" << (g_config.useSystemFileDialog ? "1" : "0") << std::endl;
        file << "brushColorPreview=" << (g_config.brushColorPreview ? "1" : "0") << std::endl;
        file << "showPenPressure=" << (g_config.showPenPressure ? "1" : "0") << std::endl;
        file << "showFPS=" << (g_config.showFPS ? "1" : "0") << std::endl;
        file << "checkUpdates=" << (g_config.checkUpdates ? "1" : "0") << std::endl;
        file << "powerSaverLevel=" << g_config.powerSaverLevel << std::endl;
        file << "singleInstance=" << (g_config.singleInstance ? "1" : "0") << std::endl;
        file << "canvasZoomSensitivity=" << g_config.canvasZoomSensitivity << std::endl;
        file << "compactEditor=" << (g_config.compactEditor ? "1" : "0") << std::endl;
        file << "debug.testLocalization=" << (g_debugConfig.debugTestLocalization ? "1" : "0") << std::endl;
        
        auto keybinds = g_keybindManager.serializeKeybinds();
        for (const std::string& keybind : keybinds) {
            file << "keybind@=" << keybind << std::endl;
        }

        for (std::string& p : g_config.lastOpenFiles) {
            file << "lastfile=" << p << std::endl;
        }

        file.close();
        return true;
    }
    else {
        return false;
    }
}

void g_loadConfig() {
    PlatformNativePathString path = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("config.txt");
    std::ifstream file(path);
    if (file.is_open()) {
        std::map<std::string, std::string> config;
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("=") != std::string::npos) {
                std::string key = line.substr(0, line.find("="));
                std::string value = line.substr(line.find("=") + 1);
                config[key] = value;

                if (key == "keybind@") {
                    g_config.keybinds.push_back(value);
                }
                else if (key == "lastfile") {
                    g_config.lastOpenFiles.push_back(value);
                }
            }
        }

        if (config.contains("openSavedPath")) { g_config.openSavedPath = config["openSavedPath"] == "1"; }
        if (config.contains("animatedBackground")) { try { g_config.animatedBackground = std::stoi(config["animatedBackground"]); } catch (std::exception&) {} }
        if (config.contains("maxUndoHistory")) { try { g_config.maxUndoHistory = std::stoi(config["maxUndoHistory"]); } catch (std::exception&) {} }
        if (config.contains("scrollWithTouchpad")) { g_config.scrollWithTouchpad = config["scrollWithTouchpad"] == "1"; }
        if (config.contains("isolateRectOnLockTile")) { g_config.isolateRectOnLockTile = config["isolateRectOnLockTile"] == "1"; }
        if (config.contains("fillToolTileBound")) { g_config.fillToolTileBound = config["fillToolTileBound"] == "1"; }
        if (config.contains("vsync")) { g_config.vsync = config["vsync"] == "1"; }
        if (config.contains("saveLoadFlatImageExtData")) { g_config.saveLoadFlatImageExtData = config["saveLoadFlatImageExtData"] == "1"; }
        if (config.contains("discordRPC")) { g_config.useDiscordRPC = config["discordRPC"] == "1"; }
        if (config.contains("renderer")) { g_config.preferredRenderer = config["renderer"]; }
        if (config.contains("autosaveInterval")) { try { g_config.autosaveInterval = std::stoi(config["autosaveInterval"]); } catch (std::exception&) {} }
        if (config.contains("rowColIndexesStartAt1")) { g_config.rowColIndexesStartAt1 = config["rowColIndexesStartAt1"] == "1"; }
        if (config.contains("language")) { g_config.language = config["language"]; }
        if (config.contains("vfxEnabled")) { g_config.vfxEnabled = config["vfxEnabled"] == "1"; }
        if (config.contains("overrideCursor")) { g_config.overrideCursor = config["overrideCursor"] == "1"; }
        if (config.contains("visualConfig")) { g_config.customVisualConfigPath = config["visualConfig"]; }
        if (config.contains("useSystemFileDialog")) { g_config.useSystemFileDialog = config["useSystemFileDialog"] == "1"; }
        if (config.contains("brushColorPreview")) { g_config.brushColorPreview = config["brushColorPreview"] == "1"; }
        if (config.contains("showPenPressure")) { g_config.showPenPressure = config["showPenPressure"] == "1"; }
        if (config.contains("showFPS")) { g_config.showFPS = config["showFPS"] == "1"; }
        if (config.contains("checkUpdates")) { g_config.showFPS = config["checkUpdates"] == "1"; }
        if (config.contains("powerSaverLevel")) { try { g_config.powerSaverLevel = std::stoi(config["powerSaverLevel"]); } catch (std::exception&) {} }
        if (config.contains("singleInstance")) { g_config.singleInstance = config["singleInstance"] == "1"; }
        if (config.contains("canvasZoomSensitivity")) { try { g_config.canvasZoomSensitivity = std::stoi(config["canvasZoomSensitivity"]); } catch (std::exception&) {} }
        if (config.contains("compactEditor")) { g_config.compactEditor = config["compactEditor"] == "1"; }
        if (config.contains("debug.testLocalization")) { g_debugConfig.debugTestLocalization = config["debug.testLocalization"] == "1"; }

        g_configWasLoaded = true;
        file.close();
    }
}

void g_tryPushLastFilePath(std::string a) {
    auto pos = std::find(g_config.lastOpenFiles.begin(),g_config.lastOpenFiles.end(), a);
    if ((pos != g_config.lastOpenFiles.begin() && g_config.lastOpenFiles.size() > 0) || (g_config.lastOpenFiles.size() == 0)) {
        if (pos != g_config.lastOpenFiles.end()) {
            g_config.lastOpenFiles.erase(pos);
        }
        g_config.lastOpenFiles.insert(g_config.lastOpenFiles.begin(), a);
        while (g_config.lastOpenFiles.size() > 7) {
            g_config.lastOpenFiles.pop_back();
        }
        g_saveConfig();
    }
}

void g_removeFromLastOpenFiles(std::string a) {
    auto pos = std::find(g_config.lastOpenFiles.begin(), g_config.lastOpenFiles.end(), a);
    if (pos != g_config.lastOpenFiles.end()) {
        g_config.lastOpenFiles.erase(pos);
        g_saveConfig();
    }
}

void g_clearLastOpenFiles() {
    g_config.lastOpenFiles.clear();
    g_saveConfig();
}