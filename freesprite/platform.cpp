#include "globals.h"
#include "platform.h"
#include "EventCallbackListener.h"
#include "FileIO.h"

#if VSP_PLATFORM == VSP_PLATFORM_WIN32
    #include "platform_windows.h"
#elif VSP_PLATFORM == VSP_PLATFORM_MAC
    #include "platform_macos.h"
#elif VSP_PLATFORM == VSP_PLATFORM_VITA
    #include "platform_vita.h"
#elif VSP_PLATFORM == VSP_PLATFORM_ANDROID
    #include "platform_android.h"
#elif VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
    #include "platform_emscripten.h"
#else
    #include "platform_linux.h"
#endif

std::ofstream platformOpenOFStream(PlatformNativePathString path) {
#if VSP_PLATFORM == VSP_PLATFORM_WIN32 && !defined(_MSC_VER)
    //it's really weird
    std::ofstream ret = std::ofstream(std::filesystem::path(path));
#else
    std::ofstream ret = std::ofstream(path);
#endif
    return ret;
}
std::ifstream platformOpenIFStream(PlatformNativePathString path, int mode) {
#if VSP_PLATFORM == VSP_PLATFORM_WIN32 && !defined(_MSC_VER)
    std::ifstream ret = std::ifstream(std::filesystem::path(path), (std::ios_base::openmode)mode);
#else
    std::ifstream ret = std::ifstream(path, (std::ios_base::openmode)mode);
#endif
    return ret;
}
