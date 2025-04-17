#include "globals.h"
#include "visual_config.h"
#include "mathops.h"
#include "json/json.hpp"

std::unordered_map<std::string, std::string> defaultVisualConfig = {
    {"meta/ver", "1"},
    {"meta/name", "void" UTF8_DIAMOND "sprite default"},
    {"meta/description", UTF8_EMPTY_DIAMOND "voidsprite's default visual configuration"},
    {"meta/author", "your name here"},
    {"general/font", ""},
    {"launchpad/bg", Fill::Gradient(0xFF000000,0xFF000000,0xFF000000,0xFF202020).serialize()},
};

std::map<std::string, SDL_Texture*> visualConfigTextureCache = {};

std::unordered_map<std::string, std::string> customVisualConfig = {};
PlatformNativePathString customVisualConfigRoot;
bool customVisualConfigLoaded = false;

std::string visualConfigValue(std::string key) {
    if (customVisualConfig.contains(key)) {
        return customVisualConfig[key];
    }
    else if (defaultVisualConfig.contains(key)) {
        return defaultVisualConfig[key];
    } 
    else {
        return "";
    }
}
Fill visualConfigFill(std::string key) {
    return Fill::deserialize(visualConfigValue(key));
}

void evalVisualConfigJsonTree(nlohmann::json& object, std::unordered_map<std::string, std::string>& target, std::string jsonPathNow = "") {
    for (auto& [key, value] : object.items()) {
        if (value.is_string()) {
            target[jsonPathNow + key] = value.get<std::string>();
        }
        else if (value.is_number_integer()) {
            target[jsonPathNow + key] = std::to_string(value.get<int>());
        }
        else if (value.is_boolean()) {
            target[jsonPathNow + key] = value.get<bool>() ? "true" : "false";
        }
        else if (value.is_object()) {
            evalVisualConfigJsonTree(value, target, jsonPathNow + key + "/");
        }
        else {
            logprintf("Unknown type in visual config JSON: %s\n", value.dump().c_str());
        }
    }
}

std::unordered_map<std::string, std::string> loadVisualConfig(PlatformNativePathString path) {
    std::ifstream infile(path);
    if (infile.is_open()) {
        nlohmann::json j;
        infile >> j;
        std::unordered_map<std::string, std::string> conf;
        evalVisualConfigJsonTree(j, conf);
        infile.close();
        return conf;
    }
    else {
        logprintf("Error opening file for reading: %s\n", path.c_str());
        return {};
    }
}

bool g_usingDefaultVisualConfig()
{
    return !customVisualConfigLoaded;
}
PlatformNativePathString g_getCustomVisualConfigRoot() {
    return customVisualConfigRoot;
}

std::vector<VisualConfigMeta> g_getAvailableVisualConfigs()
{
    PlatformNativePathString vcpath = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/visualconfigs");
    std::vector<VisualConfigMeta> ret;
    for (auto& vc : platformListFilesInDir(vcpath)) {
        if (std::filesystem::exists(vc + convertStringOnWin32("/config.json"))) {
            try {
                auto newvc = loadVisualConfig(vc + convertStringOnWin32("/config.json"));
                VisualConfigMeta meta{};
                meta.path = vc;
                meta.name = newvc["meta/name"];
                meta.description = newvc["meta/description"];
                ret.push_back(meta);
            }
            catch (std::exception& e) {
                logprintf("Error loading visual config %s: %s\n", convertStringToUTF8OnWin32(vc).c_str(), e.what());
                continue;
            }
        }
    }
    return ret;
}
bool g_loadVisualConfig(PlatformNativePathString path) {
    if (path.empty()) {
        clearVisualConfigTextureCache();
        customVisualConfig = {};
        customVisualConfigLoaded = false;
        customVisualConfigRoot = convertStringOnWin32("");
        return true;
    }
    else if (std::filesystem::exists(path) && std::filesystem::exists(path + convertStringOnWin32("/config.json"))) {
        clearVisualConfigTextureCache();
        customVisualConfig = loadVisualConfig(path + convertStringOnWin32("/config.json"));
        customVisualConfigLoaded = customVisualConfig.size() > 0;
        customVisualConfigRoot = path;
        return customVisualConfigLoaded;
    }
    return false;
}

std::unordered_map<std::string, std::string>& getDefaultVisualConf()
{
    return defaultVisualConfig;
}

void serializeVisualConfig(std::unordered_map<std::string, std::string>& conf, std::string path) {
    nlohmann::json j;
    for (auto& [key, value] : conf) {
        std::vector<std::string> path = splitString(key, '/');
        nlohmann::json* current = &j;
        for (const auto& part : path) {
            if (current->find(part) == current->end()) {
                (*current)[part] = nlohmann::json::object();
            }
            current = &(*current)[part];
        }
        (*current) = value;
    }
    std::ofstream fileout(path);
    if (fileout.is_open()) {
        fileout << j.dump(4);
    }
    else {
        logprintf("Error opening file for writing: %s\n", path.c_str());
    }
    fileout.close();
}

SDL_Texture* getVisualConfigTexture(std::string key) {
	if (visualConfigTextureCache.contains(key)) {
		return visualConfigTextureCache[key];
	}
	else {
		SDL_Texture* tex = IMGLoadAssetToTexture(key);
		visualConfigTextureCache[key] = tex;
		return tex;
	}
}

void clearVisualConfigTextureCache() {
	for (auto& [key, tex] : visualConfigTextureCache) {
		tracked_destroyTexture(tex);
	}
	visualConfigTextureCache.clear();
}