#pragma once

#include <sys/stat.h>

#include "EventCallbackListener.h"
#include "Notification.h"

#include "platform_universal.h"

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

u32 platformSupportedFeatures() {
    return
        VSP_FEATURE_UNRESTRICTED_FILE_SYSTEM
        | VSP_FEATURE_WEB_FETCH
        | VSP_FEATURE_MULTIWINDOW;
}

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
    std::string terminalCode = "printf \"" + frmt(saveFileAppleScript, fileTypesString, windowTitle) + "\" | osascript";

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
        loginfo(output);
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
            catch (std::exception&) {
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
    std::string terminalCode = "printf \"" + frmt(loadFileAppleScript, windowTitle, fileTypesString) + "\" | osascript";

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
        loginfo(output);
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
    return universal_platformPushLayerToClipboard(l);
}

Layer *platformGetImageFromClipboard() { return universal_platformGetLayerFromClipboard(); }

FILE *platformOpenFile(PlatformNativePathString path,
                       PlatformNativePathString mode) {
    FILE *ret = fopen(path.c_str(), mode.c_str());
    return ret;
}

std::string platformGetSystemInfo() {
    std::string ret = "";
    ret += frmt("macOS version {}\n", universal_runCommandAndGetOutput("sw_vers -productVersion"));
    ret += frmt("{}\n", universal_runCommandAndGetOutput("sysctl -n kern.version"));
    ret += frmt("System architecture: {}\n", universal_runCommandAndGetOutput("uname -m"));
    ret += frmt("CPU: {}\n", universal_runCommandAndGetOutput("sysctl -n machdep.cpu.brand_string"));
    ret += frmt("GPU: {}\n", universal_runCommandAndGetOutput("system_profiler SPDisplaysDataType | grep 'Chipset Model'"));
    ret += frmt("System memory: {} MiB\n", SDL_GetSystemRAM());
    return ret;
}

std::vector<RootDirInfo> platformListRootDirectories() {
    std::vector<RootDirInfo> ret;
    
    char *homeDir = getenv("HOME");
    if (homeDir != NULL) {
        //it's mac so home should always be there
        std::string homeDirStr = homeDir;
        ret.push_back({"User home", homeDirStr});
    }

    std::string volumes = "/Volumes";
    for (const auto& entry : std::filesystem::directory_iterator(volumes)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            ret.push_back({name, entry.path().string()});
        }
    }

    ret.push_back({"Root", "/"});
    return ret;
}

bool platformHasFileAccessPermissions() {
    return true;
}
void platformRequestFileAccessPermissions() {}

void platformOpenWebpageURL(std::string url) {
    //let's assume this works
	if (!stringStartsWithIgnoreCase(url, "http")) {
		url = "http://" + url;
	}
	std::string command = "open \"" + url + "\"";
	system(command.c_str());
}

std::string platformFetchTextFile(std::string url) {
	return universal_fetchTextFile(url);
}

std::vector<u8> platformFetchBinFile(std::string url) {
    return universal_fetchBinFile(url);
}

void platformPrintDocument(Layer* editor) {

}

std::vector<NetworkAdapterInfo> platformGetNetworkAdapters() {
    return universal_platformGetNetworkAdapters();
}

std::vector<PlatformNativePathString> platformGetSystemFontPaths() {
    std::vector<PlatformNativePathString> ret = {
        "/Library/Fonts"
    };
	std::string homeDir;
	char* homeDirCStr = getenv("HOME");
    if (homeDirCStr != NULL) {
        homeDir = homeDirCStr;
        ret.push_back(homeDir + "/Library/Fonts");
	}

    return ret;
}