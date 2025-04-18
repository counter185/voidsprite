#include "../globals.h"

std::map<std::string, LocalizationData> g_localizations = {
#include "localization_english.txt"
#include "localization_romanian.txt"
#include "localization_japanese.txt"
};

#ifndef ENGLISH_LOC_INCLUDED
#error "English lang must be included"
#endif

//set this to 1 to see what strings are localizable and which ones aren't
//localized strings will have a diamond in front of them
//strings that are localized in english but not in the current active language will have an empty diamond instead
#define TEST_LOC_STRINGS 0

std::string g_getLocString(std::string key) {
	//safe to assume that en-us localization will always be available
    if (g_localizations.contains(g_config.language) && g_localizations[g_config.language].kvs.contains(key)) {
#if TEST_LOC_STRINGS
        return UTF8_DIAMOND + g_localizations[g_config.language].kvs[key];
#else
        return g_localizations[g_config.language].kvs[key];
#endif
    }
    else if (g_localizations["en-us"].kvs.contains(key)) {
#if TEST_LOC_STRINGS
        return UTF8_EMPTY_DIAMOND + g_localizations["en-us"].kvs[key];
#else
        return g_localizations["en-us"].kvs[key];
#endif
    }
    else {
        logerr(std::format("Translation key not found in language {}:\n {}", g_config.language, key));
        return std::string("--NO KEY: ") + key;
    }
}

//compiler bug workaround
std::map<std::string, LocalizationData>& getLocalizations() {
    return g_localizations;
}