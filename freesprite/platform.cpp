#include "globals.h"
#include "platform.h"
#include "EventCallbackListener.h"
#include "FileIO.h"

#if _WIN32
	#include "platform_windows.h"
#elif __APPLE__
	#include "platform_macos.h"
#else
	#include "platform_linux.h"
#endif