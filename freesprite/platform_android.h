#pragma once

#include <jni.h>
#include "EventCallbackListener.h"
#include "Notification.h"

#include "platform_universal.h"
#include "PopupFilePicker.h"

JNIEnv* lastJNI = NULL;

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
    if (!SDL_IsDeXMode() && !SDL_IsTablet()) {
        SDL_SetWindowFullscreen(g_wd, true);
    }
}

std::string appdataPath = "";
std::string systemInformation = "Android";

//jni calls
extern "C" {

JNIEXPORT void JNICALL
Java_pl_cntrpl_voidsprite_VSPActivity_passAppdataPathString(JNIEnv *env, jclass clazz,
                                                            jstring appdata_path) {
    lastJNI = env;
    const char *path = env->GetStringUTFChars(appdata_path, 0);
    appdataPath = std::string(path);
    env->ReleaseStringUTFChars(appdata_path, path);
}

JNIEXPORT void JNICALL
Java_pl_cntrpl_voidsprite_VSPActivity_passSystemInformationString(JNIEnv *env, jclass clazz,
                                                                  jstring system_info) {
    lastJNI = env;
    const char* inf = env->GetStringUTFChars(system_info, 0);
    systemInformation = std::string(inf);
    env->ReleaseStringUTFChars(system_info, inf);

}

}

std::vector<RootDirInfo> android_getStoragePathsFromProcMounts() {
    std::vector<RootDirInfo> ret;

    std::ifstream mounts("/proc/mounts");
    if (mounts.is_open()) {
        std::string line;
        while (std::getline(mounts, line)) {
            auto mps = splitString(line, ' ');
            if (mps.size() > 1) {
                if (mps[0] == "/dev/fuse") {
                    std::string mountPoint = mps[1];
                    if (mountPoint.starts_with("/storage/") &&
                        !mountPoint.starts_with("/storage/emulated")) {
                        ret.push_back(
                                {std::format("SD: {}", fileNameFromPath(mountPoint)), mountPoint});
                    } else if (mountPoint.starts_with("/mnt/media_rw")) {
                        ret.push_back(
                                {std::format("USB: {}", fileNameFromPath(mountPoint)), mountPoint});
                    }
                }
            }
        }
        mounts.close();
    } else {
        logerr("Failed to open /proc/mounts");
    }
    return ret;
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
    fp->setCallbackListener(evt_id, caller);
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
    ret.push_back({"Internal storage", "/sdcard"});
    ret.push_back({"Application data", platformEnsureDirAndGetConfigFilePath()});

    for (auto& rtDir: android_getStoragePathsFromProcMounts()) {
        ret.push_back(rtDir);
    }
    return ret;
}

bool platformHasFileAccessPermissions() {
    jclass vspactivity = lastJNI->FindClass("pl/cntrpl/voidsprite/VSPActivity");
    if (vspactivity != nullptr) {
        jmethodID checkMethod = lastJNI->GetStaticMethodID(vspactivity, "hasFileAccessPermission", "()Z");
        if (checkMethod != nullptr) {
            return lastJNI->CallStaticBooleanMethod(vspactivity, checkMethod);
        }
    }
    return false;
}

void platformRequestFileAccessPermissions() {
    jclass vspactivity = lastJNI->FindClass("pl/cntrpl/voidsprite/VSPActivity");
    if (vspactivity != nullptr) {
        jmethodID checkMethod = lastJNI->GetStaticMethodID(vspactivity, "requestAllFilesPermission", "()V");
        if (checkMethod != nullptr) {
            lastJNI->CallStaticVoidMethod(vspactivity, checkMethod);
        }
    }
}

void platformOpenWebpageURL(std::string url) {
    jclass vspactivity = lastJNI->FindClass("pl/cntrpl/voidsprite/VSPActivity");
    if (vspactivity != nullptr) {
        jmethodID checkMethod = lastJNI->GetStaticMethodID(vspactivity, "openUrl", "(Ljava/lang/String;)V");
        if (checkMethod != nullptr) {
            lastJNI->CallStaticVoidMethod(vspactivity, checkMethod, lastJNI->NewStringUTF(url.c_str()));
        }
    }
}