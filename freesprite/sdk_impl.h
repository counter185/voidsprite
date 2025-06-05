#pragma once

#include "globals.h"
#include "Layer.h"
#include "FileIO.h"

#define VSPLayer Layer
#define VSPFileExporter FileExporter

#include "sdk_structs.h"

inline std::map<int, voidspriteSDK*> g_vspsdks;

void impl_registerLayerImporter(
    const char* name,
    const char* extension,
    int layerTypes,
    VSPFileExporter* matchingExporter,
    VSPLayer* (*importFunction)(char* path),
    bool (*canImportFunction)(char* path));

VSPFileExporter* impl_registerLayerExporter(
    const char* name,
    const char* extension,
    int layerTypes,
    bool (*exportFunction)(VSPLayer* layer, char* path),
    bool (*canExportFunction)(VSPLayer* layer));

VSPLayer*	impl_layerAllocNew(int type, int width, int height);
void		impl_layerFree(VSPLayer* layer);
VSPLayerInfo*	impl_layerGetInfo(VSPLayer*);
void		impl_layerSetPixel(VSPLayer* layer, int x, int y, uint32_t color);
uint32_t	impl_layerGetPixel(VSPLayer* layer, int x, int y);
uint32_t*	impl_layerGetRawPixelData(VSPLayer* layer);

inline void g_createVSPSDK() {
    voidspriteSDK* v1SDK = new voidspriteSDK();
	v1SDK->util_fopenUTF8 = [](char* path_utf8, const char* mode) { return platformOpenFile(convertStringOnWin32(path_utf8), convertStringOnWin32(mode)); };
    v1SDK->registerLayerImporter = impl_registerLayerImporter;
    v1SDK->registerLayerExporter = impl_registerLayerExporter;
    v1SDK->layerAllocNew = impl_layerAllocNew;
    v1SDK->layerFree = impl_layerFree;
    v1SDK->layerGetInfo = impl_layerGetInfo;
    v1SDK->layerSetPixel = impl_layerSetPixel;
    v1SDK->layerGetPixel = impl_layerGetPixel;
    v1SDK->layerGetRawPixelData = impl_layerGetRawPixelData;
    g_vspsdks[1] = v1SDK;
}