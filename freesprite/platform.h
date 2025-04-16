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

void platformPreInit();
void platformInit();
void platformPostInit();

std::string platformGetSystemInfo();

bool platformAssocFileTypes(std::vector<std::string> extensions);

void platformTrySaveImageFile(EventCallbackListener* caller);
void platformTryLoadImageFile(EventCallbackListener* caller);
void platformTrySaveOtherFile(EventCallbackListener* caller, std::vector<std::pair<std::string,std::string>> filetypes, std::string windowTitle, int evt_id);
void platformTryLoadOtherFile(EventCallbackListener* listener, std::vector<std::pair<std::string, std::string>> filetypes, std::string windowTitle, int evt_id);
void platformOpenFileLocation(PlatformNativePathString path);
bool platformCopyFile(PlatformNativePathString from, PlatformNativePathString to);

PlatformNativePathString platformEnsureDirAndGetConfigFilePath();
std::vector<PlatformNativePathString> platformListFilesInDir(PlatformNativePathString path, std::string filterExtension = "");

bool platformPutImageInClipboard(Layer* l);
Layer* platformGetImageFromClipboard();

FILE* platformOpenFile(PlatformNativePathString path, PlatformNativePathString mode);