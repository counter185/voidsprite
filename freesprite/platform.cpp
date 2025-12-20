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