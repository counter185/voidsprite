#pragma once

#include <cstdint>


#define VSP_LAYER_RGBA 1
#define VSP_LAYER_INDEXED 2

#ifndef VSPLayer
#define VSPLayer void
#endif

#ifndef VSPFileExporter
#define VSPFileExporter void
#endif

#pragma pack(push, 1)
struct voidspriteSDK {
	void (*registerLayerImporter)(
		const char* name, 
		const char* extension, 
		VSPFileExporter* matchingExporter, 
		VSPLayer* (*importFunction)(char* path), 
		bool (*canImportFunction)(char* path));

	VSPFileExporter* (*registerLayerExporter)(const char* name, 
		const char* extension, 
		bool (*exportFunction)(VSPLayer* layer, char* path), 
		bool (*canExportFunction)(VSPLayer* layer));

	VSPLayer* (*layerAllocNew)(int type, int width, int height) = 0;
	void (*layerFree)(VSPLayer* layer) = 0;
	bool (*layerIsIndexed)(VSPLayer*) = 0;
	void (*layerSetPixel)(VSPLayer* layer, int x, int y, uint32_t color) = 0;
	uint32_t(*layerGetPixel)(VSPLayer* layer, int x, int y) = 0;
	uint32_t* (*layerGetRawPixelData)(VSPLayer* layer) = 0;
};
#pragma pack(pop)