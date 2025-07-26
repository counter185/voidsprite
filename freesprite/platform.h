#pragma once

#if _WIN32
#define _WIDEPATHS 1
#define PlatformNativePathString std::wstring
#define PlatformFileModeRB L"rb"
#define PlatformFileModeWB L"wb"
#else
#define _WIDEPATHS 0
#define PlatformNativePathString std::string
#define PlatformFileModeRB "rb"
#define PlatformFileModeWB "wb"
#endif


#define VSP_FEATURE_FILE_ASSOC               0b0000001
#define VSP_FEATURE_CLIPBOARD                0b0000010
#define VSP_FEATURE_WEB_FETCH                0b0000100
#define VSP_FEATURE_DISCORD_RPC              0b0001000
#define VSP_FEATURE_UNRESTRICTED_FILE_SYSTEM 0b0010000
#define VSP_FEATURE_MULTIWINDOW              0b0100000
#define VSP_FEATURE_INSTANCE_IPC		     0b1000000
#define VSP_FEATURE_ALL                      ~0
u32 platformSupportedFeatures();
inline bool platformSupportsFeature(u32 feature) {
    return (platformSupportedFeatures() & feature) == feature;
}

void platformPreInit();
void platformInit();
void platformPostInit();

void platformDeinit();

struct RootDirInfo {
    std::string friendlyName;
    PlatformNativePathString path;
};

std::string platformGetSystemInfo();

bool platformAssocFileTypes(std::vector<std::string> extensions, std::vector<std::string> additionalArgs);
bool platformRegisterURI(std::string uriProtocol, std::vector<std::string> additionalArgs);
std::string platformGetFileAssocForExtension(std::string extension);

void platformTrySaveImageFile(EventCallbackListener* caller);
void platformTryLoadImageFile(EventCallbackListener* caller);
void platformTrySaveOtherFile(EventCallbackListener* caller, std::vector<std::pair<std::string,std::string>> filetypes, std::string windowTitle, int evt_id);
void platformTryLoadOtherFile(EventCallbackListener* listener, std::vector<std::pair<std::string, std::string>> filetypes, std::string windowTitle, int evt_id);
void platformOpenFileLocation(PlatformNativePathString path);
bool platformCopyFile(PlatformNativePathString from, PlatformNativePathString to);

void platformOpenWebpageURL(std::string url);
std::string platformFetchTextFile(std::string url);

bool platformHasFileAccessPermissions();
void platformRequestFileAccessPermissions();
PlatformNativePathString platformEnsureDirAndGetConfigFilePath();
std::vector<PlatformNativePathString> platformListFilesInDir(PlatformNativePathString path, std::string filterExtension = "");
std::vector<RootDirInfo> platformListRootDirectories();

bool platformPutImageInClipboard(Layer* l);
Layer* platformGetImageFromClipboard();

FILE* platformOpenFile(PlatformNativePathString path, PlatformNativePathString mode);
