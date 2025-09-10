#include "globals.h"
#include "platform.h"
#include "EventCallbackListener.h"
#include "FileIO.h"

#if _WIN32
	#include "platform_windows.h"
#elif __APPLE__
	#include "platform_macos.h"
#elif VITASDK
	#include "platform_vita.h"
#elif __ANDROID__
    #include "platform_android.h"
#else
	#include "platform_linux.h"
#endif