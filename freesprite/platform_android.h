#pragma once

#include <fstream>
#include <sys/stat.h>
#include <filesystem>

#include <jni.h>
#include "EventCallbackListener.h"
#include "Notification.h"

#include "platform_universal.h"
#include "PopupFilePicker.h"
#include "FileIO.h"

JNIEnv* lastJNI = NULL;
JavaVM* lastJVM = NULL;
jclass vspActivityClass = NULL;

u32 platformSupportedFeatures() {
    return
        VSP_FEATURE_OS_SHARE
        | VSP_FEATURE_WEB_FETCH;
}

void platformPreInit() {
    platformCreateDirectory(platformEnsureDirAndGetConfigFilePath());
    platformCreateDirectory(platformEnsureDirAndGetConfigFilePath() + "/patterns");
    platformCreateDirectory(platformEnsureDirAndGetConfigFilePath() + "/templates");
    platformCreateDirectory(platformEnsureDirAndGetConfigFilePath() + "/9segmentpatterns");
    platformCreateDirectory(platformEnsureDirAndGetConfigFilePath() + "/palettes");
    platformCreateDirectory(platformEnsureDirAndGetConfigFilePath() + "/autosaves");
    platformCreateDirectory(platformEnsureDirAndGetConfigFilePath() + "/visualconfigs");

    //manually setting orientation not only doesn't maximize it but also adds garbage data on the sides
    /*if (!SDL_IsDeXMode() && !SDL_IsTablet()) {
        //only these on phones
        SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    }*/
}
void platformInit() {}
void platformPostInit() {
    if (!SDL_IsDeXMode() && !SDL_IsTablet()) {
        
        SDL_SetWindowFullscreen(g_wd, true);
    }
}

void platformDeinit() {
    if (vspActivityClass != NULL) {
        lastJNI->DeleteGlobalRef(vspActivityClass);
        vspActivityClass = NULL;
    }
}

void platformWindowCreated(VSPWindow* wd) {}
void platformWindowDestroyed(VSPWindow*) {}

std::string appdataPath = "";
std::string systemInformation = "Android";

//jni calls
extern "C" {

JNIEXPORT void JNICALL
Java_pl_cntrpl_voidsprite_VSPActivity_passAppdataPathString(JNIEnv *env, jclass clazz,
                                                            jstring appdata_path) {
    lastJNI = env;
    env->GetJavaVM(&lastJVM);
    vspActivityClass = (jclass)env->NewGlobalRef(clazz);
    const char *path = env->GetStringUTFChars(appdata_path, 0);
    appdataPath = std::string(path);
    env->ReleaseStringUTFChars(appdata_path, path);
}

JNIEXPORT void JNICALL
Java_pl_cntrpl_voidsprite_VSPActivity_passSystemInformationString(JNIEnv *env, jclass clazz,
                                                                  jstring system_info) {
    lastJNI = env;
    env->GetJavaVM(&lastJVM);
    vspActivityClass = (jclass)env->NewGlobalRef(clazz);
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
                                {frmt("SD: {}", fileNameFromPath(mountPoint)), mountPoint});
                    } else if (mountPoint.starts_with("/mnt/media_rw")) {
                        ret.push_back(
                                {frmt("USB: {}", fileNameFromPath(mountPoint)), mountPoint});
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
bool platformRegisterURI(std::string uriProtocol, std::vector<std::string> additionalArgs) { return false; }
std::string platformGetFileAssocForExtension(std::string extension) { return "";}

//we're doing this with jni because std::filesystem creates phantom folders on android when special characters are used
//that cannot be accessed from any other file manager
bool platformCreateDirectory(PlatformNativePathString path) {
    jmethodID checkMethod = lastJNI->GetStaticMethodID(vspActivityClass, "createDirectory", "(Ljava/lang/String;)Z");
    if (checkMethod != nullptr) {
        bool res = lastJNI->CallStaticBooleanMethod(vspActivityClass, checkMethod, lastJNI->NewStringUTF(path.c_str()));
        if (!res) {
            throw std::filesystem::filesystem_error("createDirectory failed");
        } else {
            return true;
        }
    }
    throw std::runtime_error("jni function not found");
}
bool platformRenameFile(PlatformNativePathString path, PlatformNativePathString newPath) {
    jmethodID checkMethod = lastJNI->GetStaticMethodID(vspActivityClass, "renameFile", "(Ljava/lang/String;Ljava/lang/String;)Z");
    if (checkMethod != nullptr) {
        bool res = lastJNI->CallStaticBooleanMethod(vspActivityClass, checkMethod, lastJNI->NewStringUTF(path.c_str()), lastJNI->NewStringUTF(newPath.c_str()));
        if (!res) {
            throw std::filesystem::filesystem_error("renameFile failed");
        } else {
            return true;
        }
    }
    throw std::runtime_error("jni function not found");
}

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
    jmethodID checkMethod = lastJNI->GetStaticMethodID(vspActivityClass, "hasFileAccessPermission", "()Z");
    if (checkMethod != nullptr) {
        return lastJNI->CallStaticBooleanMethod(vspActivityClass, checkMethod);
    }
    return false;
}

void platformRequestFileAccessPermissions() {
    jmethodID checkMethod = lastJNI->GetStaticMethodID(vspActivityClass, "requestAllFilesPermission", "()V");
    if (checkMethod != nullptr) {
        lastJNI->CallStaticVoidMethod(vspActivityClass, checkMethod);
    }
}

void platformOpenWebpageURL(std::string url) {
    jmethodID checkMethod = lastJNI->GetStaticMethodID(vspActivityClass, "openUrl", "(Ljava/lang/String;)V");
    if (checkMethod != nullptr) {
        lastJNI->CallStaticVoidMethod(vspActivityClass, checkMethod, lastJNI->NewStringUTF(url.c_str()));
    }
}

std::string platformFetchTextFile(std::string url) {
    JNIEnv* jni;
    lastJVM->AttachCurrentThread(&jni, NULL);
    auto ab = DoOnReturn([&]() {
        lastJVM->DetachCurrentThread();
    });

    jmethodID m = jni->GetStaticMethodID(vspActivityClass, "fetchStringHTTP", "(Ljava/lang/String;)Ljava/lang/String;");
    if (m != nullptr) {
        jstring jurl = jni->NewStringUTF(url.c_str());
        auto aa = DoOnReturn([&]() {
            jni->DeleteLocalRef(jurl);
        });
        jstring result = (jstring)jni->CallStaticObjectMethod(vspActivityClass, m, jurl);
        if (result != NULL) {
            const char *resultStr = jni->GetStringUTFChars(result, 0);
            std::string ret(resultStr);
            jni->ReleaseStringUTFChars(result, resultStr);
            return ret;
        }
    }
    throw std::runtime_error("Failed to fetch text file from URL: " + url);
}

std::vector<u8> platformFetchBinFile(std::string url) {
    JNIEnv* jni;
    lastJVM->AttachCurrentThread(&jni, NULL);
    auto ab = DoOnReturn([&]() {
        lastJVM->DetachCurrentThread();
    });

    jmethodID m = jni->GetStaticMethodID(vspActivityClass, "fetchDataHTTP", "(Ljava/lang/String;)[B");
    if (m != nullptr) {
        jstring jurl = jni->NewStringUTF(url.c_str());
        auto aa = DoOnReturn([&]() { jni->DeleteLocalRef(jurl); });
        jbyteArray result = (jbyteArray)jni->CallStaticObjectMethod(vspActivityClass, m, jurl);
        if (result != NULL) {
            jsize len = jni->GetArrayLength(result);
            std::vector<u8> ret(len);
            jni->GetByteArrayRegion(result, 0, len, (jbyte*)ret.data());
            return ret;
        }
    }
    throw std::runtime_error("Failed to fetch binary file from URL: " + url);
}

void platformPrintDocument(Layer* editor) {

}

void platformShareImage(Layer* layer) {
    auto path = newTempFile() + ".png";
    if (writePNG(path, layer)) {
        auto ppath = fileNameFromPath(path);

        jmethodID m = lastJNI->GetStaticMethodID(vspActivityClass, "shareImageFromTempPath", "(Ljava/lang/String;)V");
        if (m != nullptr) {
            jstring jpath = lastJNI->NewStringUTF(ppath.c_str());
            auto aa = DoOnReturn([&]() { lastJNI->DeleteLocalRef(jpath); });
            lastJNI->CallStaticVoidMethod(vspActivityClass, m, jpath);
        }
    }

}

std::vector<NetworkAdapterInfo> platformGetNetworkAdapters() {
    return universal_platformGetNetworkAdapters();
}

std::vector<PlatformNativePathString> platformGetSystemFontPaths() {
    return {
        "/system/fonts"
    };
}

void platformRunAutoUpdate() {
    jmethodID checkMethod = lastJNI->GetStaticMethodID(vspActivityClass, "runUpdaterActivity", "()V");
    if (checkMethod != nullptr) {
        lastJNI->CallStaticVoidMethod(vspActivityClass, checkMethod);
    }
}

bool platformSetupIPC() {
    return true;
}
bool platformSendIPCToMainInstance(std::string s) {
    return false;
}