#pragma once

#include "globals.h"
#include "sdk_impl.h"

#if _WIN32
#include <windows.h>

#define ModuleHandle HMODULE
inline ModuleHandle platformLoadNativeModule(PlatformNativePathString path) { return LoadLibraryW(path.c_str()); }
inline void platformUnloadNativeModule(ModuleHandle module) { FreeLibrary(module); }
inline std::string moduleExtension = ".dll";
#else
#define ModuleHandle void*
inline ModuleHandle platformLoadNativeModule(PlatformNativePathString path) {return dlopen(path.c_str(), RTLD_LAZY); }
inline void platformUnloadNativeModule { dlclose(module); }
inline std::string moduleExtension = ".so";
#endif

class VSPPlugin {
public:
	int sdkVersion;
	std::string name, version, description, authors;
};

inline std::vector<VSPPlugin> g_loadedPlugins;

inline bool loadPluginObject(PlatformNativePathString path) {
	ModuleHandle module = platformLoadNativeModule(path);
	if (module == NULL) {
		logerr(std::format("Failed to load plugin: {}", convertStringToUTF8OnWin32(path)));
		return false;
	}
	VSPPlugin pluginInfo{};

	int (*sdkVersionFunc)() = (int(*)())GetProcAddress(module, "voidspriteSDKVersion");
	const char* (*getPluginNameFunc)() = (const char* (*)())GetProcAddress(module, "getPluginName");
	const char* (*getPluginVersionFunc)() = (const char* (*)())GetProcAddress(module, "getPluginVersion");
	const char* (*getPluginDescriptionFunc)() = (const char* (*)())GetProcAddress(module, "getPluginDescription");
	const char* (*getPluginAuthorsFunc)() = (const char* (*)())GetProcAddress(module, "getPluginAuthors");
	void* (*pluginInitFunc)(voidspriteSDK*) = (void* (*)(voidspriteSDK*))GetProcAddress(module, "pluginInit");

	if (sdkVersionFunc == NULL || pluginInitFunc == NULL) {
		logerr(std::format("Object at path {}\nis not a valid voidsprite plugin", convertStringToUTF8OnWin32(path)));
		FreeLibrary(module);
		return false;
	}
	else {
		pluginInfo.sdkVersion = sdkVersionFunc();
		if (getPluginNameFunc != NULL) { pluginInfo.name = getPluginNameFunc(); }
		if (getPluginVersionFunc != NULL) { pluginInfo.version = getPluginVersionFunc(); }
		if (getPluginDescriptionFunc != NULL) { pluginInfo.description = getPluginDescriptionFunc(); }
		if (getPluginAuthorsFunc != NULL) { pluginInfo.authors = getPluginAuthorsFunc(); }

		if (!g_vspsdks.contains(pluginInfo.sdkVersion)) {
			logerr(std::format("Plugin {} uses unsupported SDK version {}", pluginInfo.name, pluginInfo.sdkVersion));
			platformUnloadNativeModule(module);
			return false;
		}
		else {
			voidspriteSDK* sdk = g_vspsdks[pluginInfo.sdkVersion];
			pluginInitFunc(sdk);
			g_loadedPlugins.push_back(pluginInfo);
			loginfo(std::format("Loaded plugin: {} v{} by {}", pluginInfo.name, pluginInfo.version, pluginInfo.authors));
			return true;
		}
	}
	return false;
}

inline void g_loadPlugins() {
	PlatformNativePathString pluginsDir = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("plugins/");
	if (std::filesystem::exists(pluginsDir)) {
		for (const auto& entry : std::filesystem::directory_iterator(pluginsDir)) {
			if (entry.is_regular_file() && stringEndsWithIgnoreCase(entry.path().string(), moduleExtension)) {
				loadPluginObject(entry.path());
			}
		}
	}
	else {
		logwarn("Plugins directory does not exist, skipping plugin loading");
	}
}