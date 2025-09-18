#pragma once

#include <stdint.h>
#include <stdio.h>


#define VSP_LAYER_RGBA		0b01
#define VSP_LAYER_INDEXED	0b10

#ifndef VSPLayer
struct VSPLayer { char _placeholder; };
#endif
#ifndef VSPFilter
struct VSPFilter { char _placeholder; };
#endif
#ifndef VSPFileExporter
struct VSPFileExporter { char _placeholder; };
#endif
#ifndef VSPEditorContext
struct VSPEditorContext { char _placeholder; };
#endif
#ifndef VSPBrush
struct VSPBrush { char _placeholder; };
#endif

#pragma pack(push, 1)
struct VSPLayerInfo {
    int32_t type;
    int32_t width;
    int32_t height;
};

struct voidspriteSDK {
    /// <summary>
    /// Equivalent to fopen, but takes a UTF-8 encoded path.
    /// </summary>
    FILE* (*util_fopenUTF8)(char* path_utf8, const char* mode) = 0;

    VSPFilter* (*registerFilter)(
        const char* name,
        void (*filterFunction)(VSPLayer* layer, VSPFilter* filter)) = 0;

    /// <summary>
    /// Registers a new file type importer for single-layer filetypes.
    /// layerTypes is a bitmask of layer types that this importer can output (e.g. VSP_LAYER_RGBA | VSP_LAYER_INDEXED).
    /// If matchingExporter is NULL, the importer will not be associated with an exporter.
    /// importFunction is the function that will be called to import the file and must return either NULL or a valid layer.
    /// canImportFunction is a function that checks if this importer should be used for a given file (like checking the magic number, etc.) and may be NULL.
    /// Paths will be UTF-8 encoded.
    /// </summary>
    void (*registerLayerImporter)(
        const char* name,
        const char* extension,
        int layerTypes,
        VSPFileExporter* matchingExporter,
        VSPLayer* (*importFunction)(char* path),
        bool (*canImportFunction)(char* path)) = 0;

    /// <summary>
    /// Registers a new file type exporter for single-layer filetypes.
    /// layerTypes is a bitmask of layer types that this exporter can export (e.g. VSP_LAYER_RGBA | VSP_LAYER_INDEXED).
    /// exportFunction is the function that will be called to export the layer to a file and should return true on success and false on failure.
    /// canExportFunction is a function that checks if this exporter can export the given layer and may be NULL.
    /// Paths will be UTF-8 encoded.
    /// </summary>
    VSPFileExporter* (*registerLayerExporter)(
        const char* name,
        const char* extension,
        int layerTypes,
        bool (*exportFunction)(VSPLayer* layer, char* path),
        bool (*canExportFunction)(VSPLayer* layer)) = 0;

    /// <summary>
    /// Allocates a new layer of the specified type and dimensions. 
    /// If this function fails (e.g. not enough memory), NULL is returned.
    /// </summary>
    VSPLayer* (*layerAllocNew)(int type, int width, int height) = 0;
    /// <summary>
    /// Deallocates a layer and frees its memory.
    /// No operation if the layer is NULL.
    /// </summary>
    void (*layerFree)(VSPLayer* layer) = 0;
    /// <summary>
    /// Gets information about a layer. Free the returned VSPLayerInfo pointer after use.
    /// If the layer is NULL, NULL is returned.
    /// </summary>
    VSPLayerInfo* (*layerGetInfo)(VSPLayer*) = 0;
    /// <summary>
    /// Sets a specific pixel in a layer. 
    /// For an RGBA layer, pixels are in 0xAARRGGBB format.
    /// For an indexed layer, the color is an index in the palette, or -1 for transparent.
    /// No operation if the layer is NULL or the position is out of bounds.
    /// </summary>
    void (*layerSetPixel)(VSPLayer* layer, int x, int y, uint32_t color) = 0;
    /// <summary>
    /// Gets a specific pixel from the layer.
    /// For an RGBA layer, the pixel will be in 0xAARRGGBB format.
    /// For an indexed layer, the pixel will be an index in the palette, or -1 for transparent.
    /// If the layer is NULL or the position is out of bounds, 0 will be returned.
    /// </summary>
    uint32_t (*layerGetPixel)(VSPLayer* layer, int x, int y) = 0;
    /// <summary>
    /// Returns a pointer to the layer's raw pixel data for faster read/write access.
    /// The size of the data will be width * height * 4 bytes for both RGBA and indexed layers.
    /// Do not free this pointer or access outside the bounds of this array.
    /// If the layer is NULL, NULL is returned.
    /// </summary>
    uint32_t* (*layerGetRawPixelData)(VSPLayer* layer) = 0;

    void (*filterNewBoolParameter)(VSPFilter* filter, const char* name, bool defaultValue) = 0;
    void (*filterNewIntParameter)(VSPFilter* filter, const char* name, int minValue, int maxValue, int defaultValue) = 0;
    void (*filterNewDoubleParameter)(VSPFilter* filter, const char* name, double minValue, double maxValue, double defaultValue) = 0;
    void (*filterNewDoubleRangeParameter)(VSPFilter* filter, const char* name, double minValue, double maxValue, double defaultValueLow, double defaultValueHigh, uint32_t color) = 0;

    double (*filterGetDoubleValue)(VSPFilter* filter, const char* name) = 0;
    int (*filterGetIntValue)(VSPFilter* filter, const char* name) = 0;
    double (*filterGetRangeValue1)(VSPFilter* filter, const char* name) = 0;
    double (*filterGetRangeValue2)(VSPFilter* filter, const char* name) = 0;
    bool (*filterGetBoolValue)(VSPFilter* filter, const char* name) = 0;

    void (*util_free)(void*) = 0;

    /// <summary>
    /// Gets the current active color.
    /// For RGB sessions, this will be in 0xAARRGGBB format.
    /// For indexed sessions, this will be an index in the palette.
    /// If editor is NULL, 0 is returned.
    /// </summary>
    uint32_t(*editorGetActiveColor)(VSPEditorContext* editor) = 0;
    /// <summary>
    /// Gets the number of layers in the current session.
    /// </summary>
    int (*editorGetNumLayers)(VSPEditorContext* editor) = 0;
    VSPLayer* (*editorGetLayer)(VSPEditorContext* editor, int index) = 0;
    VSPLayer* (*editorGetActiveLayer)(VSPEditorContext* editor) = 0;

    VSPBrush* (*registerBrush)(
        const char* name,
        const char* tooltip,
        bool doublePosPrecision,
        void (*clickAt)(VSPBrush*, VSPEditorContext* editor, int x, int y),
        void (*dragAt)(VSPBrush*, VSPEditorContext* editor, int xFrom, int yFrom, int xTo, int yTo),
        void (*releaseAt)(VSPBrush*, VSPEditorContext* editor, int x, int y)
        ) = 0;

    void (*editorSetPixel)(VSPEditorContext* editor, int x, int y, uint32_t color) = 0;
};
#pragma pack(pop)