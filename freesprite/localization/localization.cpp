#include "../globals.h"

std::map<std::string, LocalizationData> g_localizations = {
#include "localization_english.txt"
};

std::string g_getLocString(std::string key) {
	//safe to assume that en-us localization will always be available
    if (g_localizations.contains(g_config.language) && g_localizations[g_config.language].kvs.contains(key)) {
        return g_localizations[g_config.language].kvs[key];
    }
    else if (g_localizations["en-us"].kvs.contains(key)) {
        return g_localizations["en-us"].kvs[key];
    }
    else {
        return std::string("--NO KEY: ") + key;
    }
}