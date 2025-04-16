#pragma once

#include <sys/stat.h>

#include "EventCallbackListener.h"
#include "Notification.h"

//i hope i never have to use this language again
constexpr const char* loadFileAppleScript =
"set outfile to POSIX path of (choose file with prompt \\\"voidsprite: {}\\\" of type {})\n"
"copy \\\"OK:\\\" & (outfile as string) to stdout\n";

constexpr const char* saveFileAppleScript = 
"set ftypes to {}\n"
"set fnames to {{}}\n"
"set fexts to {{}}\n"
"repeat with x from 1 to length of ftypes\n"
"copy item 1 of item x of ftypes to end of fexts\n"
"copy item 2 of item x of ftypes to end of fnames\n"
"end repeat\n"
"set extch to choose from list fnames with prompt \\\"voidsprite: pick extension\\\"\n"
"if extch is not false then\n"
"set fext_index to - 1\n"
"set fext to \\\"\\\"\n"
"repeat with x from 1 to length of fexts\n"
"if (item x of fnames) starts with extch then\n"
"set fext to item x of fexts\n"
"set fext_index to x\n"
"exit repeat\n"
"end if\n"
"end repeat\n"
"set fp to POSIX path of(choose file name with prompt \\\"voidsprite: {}\\\")\n"
"if not fp ends with fext then\n"
"set fp to fp& fext\n"
"end if\n"
"copy(fext_index as string) & \\\";\\\" & (fp as string) to stdout\n"
"else\n"
"copy \\\"err\\\" to stdout\n"
"end if\n";


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
    g_addNotification(Notification("macOS Build", "Experimental build. Things may not work.", 5000, NULL, COLOR_INFO));
}

//todo
bool platformAssocFileTypes(std::vector<std::string> extensions) { return false; }

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

    std::string fileTypesString = "";
    fileTypesString += '{';
    for (int fileIdx = 0; fileIdx < filetypes.size(); fileIdx++) {
        auto& p = filetypes[fileIdx];
        fileTypesString += "{\\\"";
        fileTypesString += p.first;
        fileTypesString += "\\\",\\\"";
        fileTypesString += p.second;
        fileTypesString += "\\\"}";

        if (fileIdx != filetypes.size() - 1) {
            fileTypesString += ',';
        }
    }
    fileTypesString += '}';
    std::string terminalCode = "printf \"" + std::format(saveFileAppleScript, fileTypesString, windowTitle) + "\" | osascript";

    FILE* pipe = popen(terminalCode.c_str(), "r");
    if (!pipe) {
        g_addNotification(ErrorNotification("macOS error", "Failed to open file dialog"));
        return;
    }
    else {
        std::string output;
        char b;
        while (!feof(pipe)) {
            if (fread(&b, 1, 1, pipe) > 0) {
                if (b != '\n' && b != '\r') {
                    output += b;
                }
            }
        }
        pclose(pipe);
        std::cout << output;
        if (output.find(';') != std::string::npos) {
            std::string indexStr = output.substr(0, output.find(';'));
            std::string filename = output.substr(output.find(';') + 1);
            
            try {
                int index = std::stoi(indexStr);
                if (index != -1) {
                    caller->eventFileSaved(evt_id, filename, index);
                }
                else {
                    g_addNotification(ErrorNotification("macOS error", "Invalid file type"));
                }
            }
            catch (std::exception) {
                g_addNotification(ErrorNotification("macOS error", "Invalid file type"));
            }
        }
        else {
            g_addNotification(ErrorNotification("macOS error", "Operation cancelled"));
        }
    }
}

void platformTryLoadOtherFile(
    EventCallbackListener *listener,
    std::vector<std::pair<std::string, std::string>> filetypes,
    std::string windowTitle, int evt_id) {
    
    std::string fileTypesString = "";
    fileTypesString += '{';
    for (int fileIdx = 0; fileIdx < filetypes.size(); fileIdx++) {
        auto& p = filetypes[fileIdx];
        fileTypesString += "\\\"";
        fileTypesString += p.first.empty() ? "*" : p.first.substr(1);
        fileTypesString += "\\\"";

        if (fileIdx != filetypes.size() - 1) {
            fileTypesString += ',';
        }
    }
    fileTypesString += '}';
    std::string terminalCode = "printf \"" + std::format(loadFileAppleScript, windowTitle, fileTypesString) + "\" | osascript";

    FILE* pipe = popen(terminalCode.c_str(), "r");
    if (!pipe) {
        g_addNotification(ErrorNotification("macOS error", "Failed to open file dialog"));
        return;
    }
    else {
        std::string output;
        char b;
        while (!feof(pipe)) {
            if (fread(&b, 1, 1, pipe) > 0) {
                if (b != '\n' && b != '\r') {
                    output += b;
                }
            }
        }
        pclose(pipe);
        std::cout << output;
        if (output.find("OK:") != std::string::npos) {
            std::string filename = output.substr(3);
            int index = findIndexByExtension(filetypes, filename);
            if (index != -1) {
                listener->eventFileOpen(evt_id, filename, index);
            }
            else {
                g_addNotification(ErrorNotification("macOS error", "Invalid file type"));
            }
        }
        else {
            g_addNotification(ErrorNotification("macOS error", "Operation cancelled"));
        }
    }
}

void platformOpenFileLocation(PlatformNativePathString path) {
    std::string fullOpenCommand = "open ";
    auto pos = path.find_last_of('/');
    if (pos != std::string::npos){
        fullOpenCommand += '\"' + path.substr(0, pos) + '\"';
        system(fullOpenCommand.c_str());
    }
}

PlatformNativePathString platformEnsureDirAndGetConfigFilePath() {
    const char* homeDir = getenv("HOME");
    if (homeDir != NULL) {
		std::string configDir = std::string(homeDir) + "/Library/Application Support/voidsprite/";
	    mkdir(configDir.c_str(), 0777);

        std::string subDir = configDir + "patterns/";
        mkdir(subDir.c_str(), 0777);

        return configDir;
	}
    g_addNotification(ErrorNotification("macOS error", "Failed to create config directory"));
    return "";
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

bool platformCopyFile(PlatformNativePathString from, PlatformNativePathString to) {
    try {
        std::filesystem::copy(from, to);
        return true;
    }
    catch (std::exception&) {
        return false;
    }
}

bool platformPutImageInClipboard(Layer* l) {
    return false;
}

Layer *platformGetImageFromClipboard() { return NULL; }

FILE *platformOpenFile(PlatformNativePathString path,
                       PlatformNativePathString mode) {
    FILE *ret = fopen(path.c_str(), mode.c_str());
    return ret;
}

std::string platformGetSystemInfo() {
    //todo
    std::string ret = "";
    ret += "macOS\n";
    return ret;
}