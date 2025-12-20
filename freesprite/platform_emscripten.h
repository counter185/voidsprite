#pragma once

#include <fstream>

#include "EventCallbackListener.h"
#include "Notification.h"
#include "PopupFilePicker.h"
#include "platform_universal.h"

u32 platformSupportedFeatures() {
    return VSP_FEATURE_CLIPBOARD
        | VSP_FEATURE_OS_PRINTER;
}

extern char **environ;


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

void platformDeinit() {}

//todo
bool platformAssocFileTypes(std::vector<std::string> extensions, std::vector<std::string> additionalArgs) { return false; }
bool platformRegisterURI(std::string uriProtocol, std::vector<std::string> additionalArgs) { return false; }
std::string platformGetFileAssocForExtension(std::string extension) { return "";}

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
    return "/vsp";
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
    return false;//universal_platformPushLayerToClipboard(l);
}

Layer *platformGetImageFromClipboard() { 
    return NULL;//universal_platformGetLayerFromClipboard(); 
}

FILE *platformOpenFile(PlatformNativePathString path,
                       PlatformNativePathString mode) {
    FILE *ret = fopen(path.c_str(), mode.c_str());
    return ret;
}

std::string platformGetSystemInfo() {
    std::string ret = "Emscripten";
    return ret;
}

std::vector<RootDirInfo> platformListRootDirectories() {
    std::vector<RootDirInfo> ret = {
        {"MEMFS Root", "/"}
    };
    return ret;
}

bool platformHasFileAccessPermissions() {
    return true;
}
void platformRequestFileAccessPermissions() {}

void platformOpenWebpageURL(std::string url) {
    //todo
}

std::string platformFetchTextFile(std::string url) {
	//return universal_fetchTextFile(url);
    throw std::runtime_error("Failed to fetch text file from URL: " + url);
}

std::vector<u8> platformFetchBinFile(std::string url) {
    //return universal_fetchBinFile(url);
    throw std::runtime_error("Failed to fetch binary file from URL: " + url);
}

void platformPrintDocument(Layer* editor) {

}

std::vector<NetworkAdapterInfo> platformGetNetworkAdapters() {
    return universal_platformGetNetworkAdapters();
}

std::vector<PlatformNativePathString> platformGetSystemFontPaths() {
    return {
    };
}