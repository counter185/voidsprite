#pragma once

#include <stdint.h>

#ifndef EXPORT
	#if defined(_MSC_VER)
		#define EXPORT __declspec(dllexport)
	#elif defined(__GNUC__)
		#define EXPORT __attribute__((visibility("default")))
	#endif
#endif


#define VS_SDK_VERSION 1

#include "sdk_structs.h"

extern "C" {
	EXPORT int voidspriteSDKVersion() { return VS_SDK_VERSION; }
	EXPORT void pluginInit(voidspriteSDK*);
	EXPORT const char* getPluginName();
	EXPORT const char* getPluginVersion();
	EXPORT const char* getPluginDescription();
	EXPORT const char* getPluginAuthors();
}