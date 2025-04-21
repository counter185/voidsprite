#pragma once

#include <jni.h>
#include "EventCallbackListener.h"
#include "Notification.h"

#include "platform_universal.h"



void platformPreInit() {
    std::filesystem::create_directory(platformEnsureDirAndGetConfigFilePath());
    std::filesystem::create_directory(platformEnsureDirAndGetConfigFilePath() + "/patterns");
    std::filesystem::create_directory(platformEnsureDirAndGetConfigFilePath() + "/templates");
    std::filesystem::create_directory(platformEnsureDirAndGetConfigFilePath() + "/9segmentpatterns");
    std::filesystem::create_directory(platformEnsureDirAndGetConfigFilePath() + "/palettes");
    std::filesystem::create_directory(platformEnsureDirAndGetConfigFilePath() + "/autosaves");
    std::filesystem::create_directory(platformEnsureDirAndGetConfigFilePath() + "/visualconfigs");
}
void platformInit() {}
void platformPostInit() {}

std::string appdataPath = "";

extern "C"
JNIEXPORT void JNICALL
Java_pl_cntrpl_voidsprite_VSPActivity_passAppdataPathString(JNIEnv *env, jclass clazz,
        jstring appdata_path) {
    const char *path = env->GetStringUTFChars(appdata_path, 0);
    appdataPath = std::string(path);
}

//todo
bool platformAssocFileTypes(std::vector<std::string> extensions, std::vector<std::string> additionalArgs) { return false; }

void platformTrySaveImageFile(EventCallbackListener *caller) {}
void platformTryLoadImageFile(EventCallbackListener *caller) {}

int findIndexByExtension(
        std::vector<std::pair<std::string, std::string>> &filetypes,
        std::string filename) {
    std::transform(filename.begin(), filename.end(), filename.begin(),
                   ::tolower);
    for (int x = 0; x < filetypes.size(); x++) {
        auto &p = filetypes[x];
        if (filename.size() > p.first.size()) {
            if (filename.substr(filename.size() - p.first.size()) == p.first) {
                return x + 1;
            }
        }
    }
    return -1;
}

void platformTrySaveOtherFile(
        EventCallbackListener *caller,
        std::vector<std::pair<std::string, std::string>> filetypes,
        std::string windowTitle, int evt_id) {
}

void platformTryLoadOtherFile(
        EventCallbackListener *listener,
        std::vector<std::pair<std::string, std::string>> filetypes,
        std::string windowTitle, int evt_id) {
}

void platformOpenFileLocation(PlatformNativePathString path) {
}

PlatformNativePathString platformEnsureDirAndGetConfigFilePath() {
    return appdataPath;
}

bool platformCopyFile(PlatformNativePathString from, PlatformNativePathString to) {
    try {
        std::filesystem::copy(from, to);
        return true;
    }
    catch (std::exception&) {
        return false;
    }
}

std::vector<PlatformNativePathString> platformListFilesInDir(PlatformNativePathString path, std::string filterExtension) {
    std::vector<PlatformNativePathString> ret;
    for (const auto& file : std::filesystem::directory_iterator(path)) {
        if (filterExtension == "" || stringEndsWithIgnoreCase(file.path(), convertStringOnWin32(filterExtension))) {
            ret.push_back(file.path());
        }
    }
    return ret;
}

bool platformPutImageInClipboard(Layer* l) {
    return universal_platformPushLayerToClipboard(l);
}

Layer *platformGetImageFromClipboard() { return universal_platformGetLayerFromClipboard(); }

FILE *platformOpenFile(PlatformNativePathString path,
                       PlatformNativePathString mode) {
    FILE *ret = fopen(path.c_str(), mode.c_str());
    return ret;
}

std::string platformGetSystemInfo() {
    std::string ret = "Android";
    return ret;
}