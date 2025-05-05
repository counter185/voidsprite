#pragma once

struct VisualConfigMeta {
    PlatformNativePathString path;
    std::string name;
    std::string description;
};

bool g_usingDefaultVisualConfig();
std::vector<VisualConfigMeta> g_getAvailableVisualConfigs();
bool g_loadVisualConfig(PlatformNativePathString path);
PlatformNativePathString g_getCustomVisualConfigRoot();

std::unordered_map<std::string, std::string>& getDefaultVisualConf();
std::string visualConfigValue(std::string key);
Fill visualConfigFill(std::string key);
u32 visualConfigHexU32(std::string key);
SDL_Color visualConfigColor(std::string key);
void serializeVisualConfig(std::unordered_map<std::string, std::string>& conf, std::string path);

SDL_Texture* getVisualConfigTexture(std::string key);
void clearVisualConfigTextureCache();