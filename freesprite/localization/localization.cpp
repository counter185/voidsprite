#include "../globals.h"

std::map<std::string, LocalizationData> g_localizations = {
#include "localization_english.txt"
#include "localization_romanian.txt"
#include "localization_japanese.txt"
};

#ifndef ENGLISH_LOC_INCLUDED
#error "English lang must be included"
#endif

std::string g_getLocString(std::string key) {

    std::string lang = g_config.language;
    if (lang == "keys") {
        return UTF8_EMPTY_DIAMOND + key;
    }

    //safe to assume that en-us localization will always be available
    if (g_localizations.contains(lang) && g_localizations[lang].kvs.contains(key)) {
#if _DEBUG
        if (g_debugConfig.debugTestLocalization) {
            return UTF8_DIAMOND + g_localizations[lang].kvs[key];
        }
#endif
        return g_localizations[lang].kvs[key];
    }
    else if (g_localizations["en-us"].kvs.contains(key)) {
#if _DEBUG
        if (g_debugConfig.debugTestLocalization) {
            return UTF8_EMPTY_DIAMOND + g_localizations["en-us"].kvs[key];
        }
#endif
        return g_localizations["en-us"].kvs[key];
    }
    else {
        logerr(std::format("Translation key not found in language {}:\n {}", lang, key));
        return std::string("--NO KEY: ") + key;
    }
}

double g_getLocCompletionPercentage(std::string locale)
{
    if (g_localizations.contains(locale)) {
        return g_localizations[locale].kvs.size() / (double)g_localizations["en-us"].kvs.size();
    }
    return 0.0;
}

//compiler bug workaround
std::map<std::string, LocalizationData>& getLocalizations() {
    return g_localizations;
}