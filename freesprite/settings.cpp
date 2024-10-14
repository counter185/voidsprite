#include "globals.h"
#include "mathops.h"
#include "BaseBrush.h"

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

        int x = 0;
        for (BaseBrush* brush : g_brushes) {
			file << "keybind:brush:" << x++ << "=" << std::to_string(brush->keybind) << std::endl;
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
            std::string key = line.substr(0, line.find("="));
            std::string value = line.substr(line.find("=") + 1);
            config[key] = value;

            if (stringStartsWithIgnoreCase(key, "keybind:")) {
                try {
                    int keybind = std::stoi(value);
                    g_config.keybinds[key.substr(key.find(':') + 1)] = keybind;
                } catch (std::exception){
                    printf("bad keybind for %s: %s\n", key.c_str(), value.c_str());
                }
            }
        }

        if (config.contains("openSavedPath")) { g_config.openSavedPath = config["openSavedPath"] == "1"; }
        if (config.contains("animatedBackground")) { try { g_config.animatedBackground = std::stoi(config["animatedBackground"]); } catch (std::exception) {} }
        if (config.contains("maxUndoHistory")) { try { g_config.maxUndoHistory = std::stoi(config["maxUndoHistory"]); } catch (std::exception) {} }
        if (config.contains("scrollWithTouchpad")) { g_config.scrollWithTouchpad = config["scrollWithTouchpad"] == "1"; }
        if (config.contains("isolateRectOnLockTile")) { g_config.isolateRectOnLockTile = config["isolateRectOnLockTile"] == "1"; }
        if (config.contains("fillToolTileBound")) { g_config.fillToolTileBound = config["fillToolTileBound"] == "1"; }
        if (config.contains("vsync")) { g_config.vsync = config["vsync"] == "1"; }

        g_configWasLoaded = true;
        file.close();
    }
}