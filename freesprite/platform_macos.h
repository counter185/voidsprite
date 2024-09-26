#pragma once

#include "EventCallbackListener.h"
#include "Notification.h"
#include "portable-file-dialogs/portable-file-dialogs.h"

void platformPreInit() {}
void platformInit() {}
void platformPostInit() {
    g_addNotification(Notification("macOS Build", "Experimental build. Things may not work.", 5000, NULL, COLOR_INFO));
}

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
    std::vector<std::string> fileTypeStrings;
    for (auto &p : filetypes) {
        fileTypeStrings.push_back(p.second);
        fileTypeStrings.push_back("*" + p.first);
    }
    std::string windowTitle2 = "voidsprite: ";
    windowTitle2 += windowTitle;
    pfd::save_file result = pfd::save_file(windowTitle2,
                                 pfd::path::home(), fileTypeStrings, pfd::opt::none);
    std::string filename = result.result();
    if (filename.length() > 0) {
        // uh oh we need to manually find the filter index
        int index = findIndexByExtension(filetypes, filename);
        if (index != -1) {
            caller->eventFileSaved(evt_id, filename, index);
        } else {
            g_addNotification(ErrorNotification(
                "macOS error", "Please add the extension to the file name"));
        }
    }
}

void platformTryLoadOtherFile(
    EventCallbackListener *listener,
    std::vector<std::pair<std::string, std::string>> filetypes,
    std::string windowTitle, int evt_id) {
    std::vector<std::string> fileTypeStrings;
    for (auto &p : filetypes) {
        fileTypeStrings.push_back(p.second);
        fileTypeStrings.push_back("*" + p.first);
    }
    std::string windowTitle2 = "voidsprite: ";
    windowTitle2 += windowTitle;
    pfd::open_file result =
        pfd::open_file(windowTitle2, pfd::path::home(),
                       fileTypeStrings, pfd::opt::multiselect);
    std::vector<std::string> filenames = result.result();
    if (filenames.size() > 0) {
        // uh oh we need to manually find the filter index again
        int index = findIndexByExtension(filetypes, filenames[0]);
        if (index != -1) {
            listener->eventFileOpen(evt_id, filenames[0], index);
        } else {
            g_addNotification(ErrorNotification(
                "macOS error", "File type could not be found"));
        }
    }
}

void platformOpenFileLocation(PlatformNativePathString path) {
    std::string fullOpenCommand = "open ";
    fullOpenCommand += '\"' + path + '\"';
    system(fullOpenCommand.c_str());
}

PlatformNativePathString platformEnsureDirAndGetConfigFilePath() {
    return "";
}

Layer *platformGetImageFromClipboard() { return NULL; }

FILE *platformOpenFile(PlatformNativePathString path,
                       PlatformNativePathString mode) {
    FILE *ret = fopen(path.c_str(), mode.c_str());
    return ret;
}
