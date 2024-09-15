#pragma once

#include "EventCallbackListener.h"
#include "Notification.h"
#include "portable-file-dialogs/portable-file-dialogs.h"

//I gave up on trying to make this work
//first sdl2 was too old
//then sdl2_image was too old
//then g++ was one version too old to support #include <format>
//then the png.h include threw 500 errors


//Good fucking luck i can't be bothered with this shit

//update: WE ARE SO BACK

void platformPreInit() {}
void platformInit() {}
void platformPostInit() {}

void platformTrySaveImageFile(EventCallbackListener* caller) {}
void platformTryLoadImageFile(EventCallbackListener* caller) {}

int findIndexByExtension(std::vector<std::pair<std::string, std::string>>& filetypes, std::string filename){
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    for (int x = 0; x < filetypes.size(); x++) {
        auto& p = filetypes[x];
        if (filename.size() > p.first.size()) {
            if (filename.substr(filename.size()-p.first.size()) == p.first) {
                return x+1;
            }
        }
    }
    return -1;
}

void platformTrySaveOtherFile(EventCallbackListener* caller, std::vector<std::pair<std::string,std::string>> filetypes, std::string windowTitle, int evt_id) 
{
    std::vector<std::string> fileTypeStrings;
    for (auto& p : filetypes) {
        fileTypeStrings.push_back(p.second);
        fileTypeStrings.push_back("*" + p.first);
    }
    auto result = pfd::save_file("voidsprite: " + windowTitle, pfd::path::home(), fileTypeStrings, true);
    std::string filename = result.result();
    if (filename.length() > 0) {
        //uh oh we need to manually find the filter index
        int index = findIndexByExtension(filetypes, filename);
        if (index != -1) {
            caller->eventFileSaved(evt_id, filename, index);
        }
        else {
            g_addNotification(ErrorNotification("Linux error", "Please add the extension to the file name"));
        }
    }
}

void platformTryLoadOtherFile(EventCallbackListener* listener, std::vector<std::pair<std::string, std::string>> filetypes, std::string windowTitle, int evt_id) 
{
    std::vector<std::string> fileTypeStrings;
    for (auto& p : filetypes) {
        fileTypeStrings.push_back(p.second);
        fileTypeStrings.push_back("*" + p.first);
    }
    auto result = pfd::open_file("voidsprite: " + windowTitle, pfd::path::home(), fileTypeStrings, true);
    std::vector<std::string> filenames = result.result();
    if (filenames.size() > 0) {
        //uh oh we need to manually find the filter index again
        int index = findIndexByExtension(filetypes, filenames[0]);
        if (index != -1) {
            listener->eventFileOpen(evt_id, filenames[0], index);
        }
        else {
            g_addNotification(ErrorNotification("Linux error", "File type could not be found"));
        }
    }
}

void platformOpenFileLocation(PlatformNativePathString path) {}

Layer* platformGetImageFromClipboard() { return NULL; }

FILE* platformOpenFile(PlatformNativePathString path, PlatformNativePathString mode) {
    FILE* ret = fopen(path.c_str(), mode.c_str());
    return ret;
}
