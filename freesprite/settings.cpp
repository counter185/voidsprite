#include <fstream>

#include "globals.h"
#include "mathops.h"
#include "brush/BaseBrush.h"
#include "keybinds.h"

struct ConfigBoolOption {
    std::string configKey;
    bool* target;
};

std::vector<ConfigBoolOption> getBoolOptions() {
    return {
        {"openSavedPath", &g_config.openSavedPath },
        {"scrollWithTouchpad", &g_config.scrollWithTouchpad },
        {"isolateRectOnLockTile", &g_config.isolateRectOnLockTile },
        {"fillToolTileBound", &g_config.fillToolTileBound },
        {"vsync", &g_config.vsync },
        {"saveLoadFlatImageExtData", &g_config.saveLoadFlatImageExtData },
        {"discordRPC", &g_config.useDiscordRPC },
        {"rowColIndexesStartAt1", &g_config.rowColIndexesStartAt1 },
        {"vfxEnabled", &g_config.vfxEnabled },
        {"overrideCursor", &g_config.overrideCursor },
        {"useSystemFileDialog", &g_config.useSystemFileDialog },
        {"brushColorPreview", &g_config.brushColorPreview },
        {"showPenPressure", &g_config.showPenPressure },
        {"showFPS", &g_config.showFPS },
        {"checkUpdates", &g_config.checkUpdates },
        {"singleInstance", &g_config.singleInstance },
        {"compactEditor", &g_config.compactEditor },
        {"autoScreenScale", &g_config.autoViewportScale},
        {"longErrorNotifs", &g_config.longErrorNotifs},
        {"hueWheelInsteadOfSlider", &g_config.hueWheelInsteadOfSlider},
        {"debug.testLocalization", &g_debugConfig.debugTestLocalization }
    };
}

bool g_saveConfig() {

    std::vector<ConfigBoolOption> saveBoolOptions = getBoolOptions();

    PlatformNativePathString path = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("config.txt");
    std::ofstream file(path);
    if (file.is_open()) {
        for (auto& bopt : saveBoolOptions) {
            file << bopt.configKey << "=" << (*(bopt.target) ? "1" : "0") << std::endl;
        }

        file << "animatedBackground=" << std::to_string(g_config.animatedBackground) << std::endl;
        file << "maxUndoHistory=" << std::to_string(g_config.maxUndoHistory) << std::endl;
        file << "renderer=" << g_config.preferredRenderer << std::endl;
        file << "autosaveInterval=" << g_config.autosaveInterval << std::endl;
        file << "language=" << g_config.language << std::endl;
        file << "visualConfig=" << g_config.customVisualConfigPath << std::endl;
        file << "powerSaverLevel=" << g_config.powerSaverLevel << std::endl;
        file << "canvasZoomSensitivity=" << g_config.canvasZoomSensitivity << std::endl;
        file << "backtraceColor=" << frmt("{:06X}", g_config.backtraceColor) << std::endl;
        file << "fwdtraceColor=" << frmt("{:06X}", g_config.fwdtraceColor) << std::endl;
        
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

        std::vector<ConfigBoolOption> saveBoolOptions = getBoolOptions();
        for (auto& bopt : saveBoolOptions) {
            if (config.contains(bopt.configKey)) {
                *(bopt.target) = config[bopt.configKey] == "1";
            }
        }

        if (config.contains("animatedBackground")) { try { g_config.animatedBackground = std::stoi(config["animatedBackground"]); } catch (std::exception&) {} }
        if (config.contains("maxUndoHistory")) { try { g_config.maxUndoHistory = std::stoi(config["maxUndoHistory"]); } catch (std::exception&) {} }
        if (config.contains("renderer")) { g_config.preferredRenderer = config["renderer"]; }
        if (config.contains("autosaveInterval")) { try { g_config.autosaveInterval = std::stoi(config["autosaveInterval"]); } catch (std::exception&) {} }
        if (config.contains("language")) { g_config.language = config["language"]; }
        if (config.contains("visualConfig")) { g_config.customVisualConfigPath = config["visualConfig"]; }
        if (config.contains("powerSaverLevel")) { try { g_config.powerSaverLevel = std::stoi(config["powerSaverLevel"]); } catch (std::exception&) {} }
        if (config.contains("canvasZoomSensitivity")) { try { g_config.canvasZoomSensitivity = std::stoi(config["canvasZoomSensitivity"]); } catch (std::exception&) {} }
        if (config.contains("backtraceColor")) { try { g_config.backtraceColor = std::stoul(config["backtraceColor"], nullptr, 16); } catch (std::exception&) {} }
        if (config.contains("fwdtraceColor")) { try { g_config.fwdtraceColor = std::stoul(config["fwdtraceColor"], nullptr, 16); } catch (std::exception&) {} }

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
        while (g_config.lastOpenFiles.size() > 30) {
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