#pragma once

#include <jni.h>
#include "EventCallbackListener.h"
#include "Notification.h"

#include "platform_universal.h"
#include "PopupFilePicker.h"


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
void platformPostInit() {
    //sometimes it just doesn't do that
    SDL_MaximizeWindow(g_wd);
}

std::string appdataPath = "";
std::string systemInformation = "Android";

//jni calls
extern "C" {

JNIEXPORT void JNICALL
Java_pl_cntrpl_voidsprite_VSPActivity_passAppdataPathString(JNIEnv *env, jclass clazz,
                                                            jstring appdata_path) {
    const char *path = env->GetStringUTFChars(appdata_path, 0);
    appdataPath = std::string(path);
    env->ReleaseStringUTFChars(appdata_path, path);
}

JNIEXPORT void JNICALL
Java_pl_cntrpl_voidsprite_VSPActivity_passSystemInformationString(JNIEnv *env, jclass clazz,
                                                                  jstring system_info) {
    const char* inf = env->GetStringUTFChars(system_info, 0);
    systemInformation = std::string(inf);
    env->ReleaseStringUTFChars(system_info, inf);

}

}

//todo
bool platformAssocFileTypes(std::vector<std::string> extensions, std::vector<std::string> additionalArgs) { return false; }

void platformTrySaveImageFile(EventCallbackListener *caller) {}
void platformTryLoadImageFile(EventCallbackListener *caller) {}

void platformTrySaveOtherFile(
        EventCallbackListener *caller,
        std::vector<std::pair<std::string, std::string>> filetypes,
        std::string windowTitle, int evt_id) {
    PopupFilePicker* fp = PopupFilePicker::SaveFile(windowTitle, filetypes);
    fp->setCallbackListener(evt_id, listener);
    g_addPopup(fp);    
}

void platformTryLoadOtherFile(
        EventCallbackListener *listener,
        std::vector<std::pair<std::string, std::string>> filetypes,
        std::string windowTitle, int evt_id) {
    //universal_platformTryLoadOtherFile(listener, filetypes, windowTitle, evt_id);
    PopupFilePicker* fp = PopupFilePicker::OpenFile(windowTitle, filetypes);
    fp->setCallbackListener(evt_id, listener);
    g_addPopup(fp);
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
    return systemInformation;
}

std::vector<RootDirInfo> platformListRootDirectories() {
    std::vector<RootDirInfo> ret;
    RootDirInfo rootDir = {"Internal storage", "/sdcard"};
    ret.push_back(rootDir);
    return ret;
}