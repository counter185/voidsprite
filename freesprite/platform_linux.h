#pragma once

#include <fstream>

#include <pwd.h>
#include <spawn.h>
#include <unistd.h>
#include <sys/utsname.h>

#include "EventCallbackListener.h"
#include "Notification.h"
#include "portable-file-dialogs/portable-file-dialogs.h"

#include "platform_universal.h"

// I gave up on trying to make this work
// first sdl2 was too old
// then sdl2_image was too old
// then g++ was one version too old to support #include <format>
// then the png.h include threw 500 errors

// Good fucking luck i can't be bothered with this shit

// update: WE ARE SO BACK

u32 platformSupportedFeatures() {
    return 
        VSP_FEATURE_CLIPBOARD 
        | VSP_FEATURE_UNRESTRICTED_FILE_SYSTEM;
}

extern char **environ;

std::string linux_getCPUName() {
    std::ifstream cpuInfoFile("/proc/cpuinfo");
    if (cpuInfoFile.is_open()) {
        std::string line;
        while (std::getline(cpuInfoFile, line)) {
            if (line.find("model name") != std::string::npos) {
                std::string cpuName = line.substr(line.find(":") + 1);
                cpuInfoFile.close();
                return cpuName;
            }
        }
    }
    else {
        return "(failed to read /proc/cpuinfo)\n";
    }
    
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
void platformPostInit() {}

void platformDeinit() {}

//todo
bool platformAssocFileTypes(std::vector<std::string> extensions, std::vector<std::string> additionalArgs) { return false; }

void platformTrySaveImageFile(EventCallbackListener *caller) {}
void platformTryLoadImageFile(EventCallbackListener *caller) {}



void platformTrySaveOtherFile(
    EventCallbackListener *caller,
    std::vector<std::pair<std::string, std::string>> filetypes,
    std::string windowTitle, int evt_id) {

    if (!g_config.useSystemFileDialog) {
        universal_platformTrySaveOtherFile(caller, filetypes, windowTitle, evt_id);
        return;
    }

    std::vector<std::string> fileTypeStrings;
    for (auto &p : filetypes) {
        fileTypeStrings.push_back(p.second);
        fileTypeStrings.push_back("*" + p.first);
    }
    auto result = pfd::save_file("voidsprite: " + windowTitle,
                                 pfd::path::home(), fileTypeStrings, pfd::opt::none);
    std::string filename = result.result();
    if (filename.length() > 0) {
        // uh oh we need to manually find the filter index
        int index = findIndexByExtension(filetypes, filename);
        if (index != -1) {
            caller->eventFileSaved(evt_id, filename, index);
        } else {
            g_addNotification(ErrorNotification(
                "Linux error", "Please add the extension to the file name"));
        }
    }
}

void platformTryLoadOtherFile(
    EventCallbackListener *listener,
    std::vector<std::pair<std::string, std::string>> filetypes,
    std::string windowTitle, int evt_id) {

    if (!g_config.useSystemFileDialog) {
        universal_platformTryLoadOtherFile(listener, filetypes, windowTitle, evt_id);
        return;
    }

    std::vector<std::string> fileTypeStrings;
    for (auto &p : filetypes) {
        fileTypeStrings.push_back(p.second);
        fileTypeStrings.push_back("*" + p.first);
    }
    auto result =
        pfd::open_file("voidsprite: " + windowTitle, pfd::path::home(),
                       fileTypeStrings, pfd::opt::multiselect);
    std::vector<std::string> filenames = result.result();
    if (filenames.size() > 0) {
        // uh oh we need to manually find the filter index again
        int index = findIndexByExtension(filetypes, filenames[0]);
        if (index != -1) {
            listener->eventFileOpen(evt_id, filenames[0], index);
        } else {
            g_addNotification(ErrorNotification(
                "Linux error", "File type could not be found"));
        }
    }
}

void platformOpenFileLocation(PlatformNativePathString path) {
    pid_t pid;
    char *cpath = (char *)path.c_str();
    char *argv[] = {(char *)"xdg-open", cpath, NULL};
    int status;
    if ((status = posix_spawn(&pid, "/usr/bin/xdg-open", NULL, NULL, argv,
                              environ)) != 0) {
        g_addNotification(
            ErrorNotification("Cannot open path", strerror(status)));
    }
}

PlatformNativePathString platformEnsureDirAndGetConfigFilePath() {
    char *path = getenv("XDG_CONFIG_HOME");
    if (path != NULL) {
        std::string sp = std::string(path);
        if (!sp.ends_with("/"))
            sp.push_back('/');
        sp.append("voidsprite/");
        return sp;
    }
    path = getenv("HOME");
    if (path != NULL) {
        std::string sp = std::string(path);
        if (!sp.ends_with("/"))
            sp.push_back('/');
        sp.append(".config/voidsprite/");
        return sp;
    }

    int buf_max = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (buf_max == -1) {
        g_addNotification(ErrorNotification("Linux error", strerror(errno)));
        return "";
    }
    int uid = getuid();
    char *buf = new char[buf_max];

    passwd passwd_;
    passwd *passwdp = &passwd_;

    int res = getpwuid_r(uid, passwdp, buf, buf_max, &passwdp);
    if (res != 0) {
        g_addNotification(ErrorNotification("Linux error", strerror(errno)));
        delete[] buf;
        return "";
    }

    std::string cpppath = std::string(passwd_.pw_dir);
    if (!cpppath.ends_with("/"))
        cpppath.push_back('/');
    cpppath.append(".config/voidsprite/");
    delete[] buf;
    return cpppath;
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
    std::string ret = "";

    struct utsname buffer;
    char* p;
    long ver[16];
    int i = 0;

    errno = 0;
    if (uname(&buffer) != 0) {
        ret += "(Error running uname)\n";
    }
    else {
        ret += std::format("OS: {} {} {}\n", buffer.sysname, buffer.version, buffer.release);
    }

    auto distroInfo = parseINI("/etc/os-release");
    std::string distroInfoString =
        distroInfo.contains("PRETTY_NAME") ? distroInfo["PRETTY_NAME"]
        : distroInfo.contains("NAME") ? std::format("{} {}", distroInfo["NAME"], distroInfo["VERSION"])
        : "";
    if (distroInfoString != "") {
        ret += std::format("os-release info: {}\n", distroInfoString);
    }

    ret += std::format("CPU: {}\n", linux_getCPUName());
    ret += std::format("System memory: {} MiB\n", SDL_GetSystemRAM());

    //todo: get gpu, maybe the hardware model if possible, distro info

    return ret;
}

std::vector<RootDirInfo> platformListRootDirectories() {
    std::vector<RootDirInfo> ret;
    
    char *homeDir = getenv("HOME");
    if (homeDir != NULL) {
        std::string homeDirStr = homeDir;
        ret.push_back({"User home", homeDirStr});
    }

    ret.push_back({"Root", "/"});
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
	return universal_fetchTextFile(url);
}