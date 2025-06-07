#include "pch.h"
#include "voidsprite_sdk.h"

voidspriteSDK* vsp;

// Sample file importing function
VSPLayer* importBIN(char* path)
{
    printf("Importing file: %s\n", path);
    FILE* f = vsp->util_fopenUTF8(path, "rb");
    if (f != NULL) {
        // Create a new layer
        VSPLayer* layer = vsp->layerAllocNew(VSP_LAYER_RGBA, 256, 256);

        if (layer != NULL) {
            // For direct pixel access, we can get the raw pixel data pointer
            uint32_t* pixels = vsp->layerGetRawPixelData(layer);
            memset(pixels, 0, 256 * 256 * 4); // Clear the layer to #0000000000 (transparent black)

            // Let's read the raw bytes from the file directly into the layer
            for (int y = 0; y < 256; y++) {
                for (int x = 0; x < 256; x++) {
                    uint8_t colors[3];
                    fread(colors, 3, 1, f); // Read R, G and B

                    uint32_t color = (0xFF << 24) | (colors[0] << 16) | (colors[1] << 8) | colors[2]; // Pack the color into 0xAARRGGBB format
                    vsp->layerSetPixel(layer, x, y, color); // Set the pixel in the layer
                }
            }
        }

        // Close the file
        fclose(f);
        return layer;
    }
    return NULL;
}

// Sample image filter
void invertFilter(VSPLayer* layer, VSPFilter* filter) {
    // Get the parameters we have added
    bool preserveAlpha = vsp->filterGetBoolValue(filter, "preserve alpha");

    // Do any operations on the layer here
    VSPLayerInfo* info = vsp->layerGetInfo(layer);
    for (int y = 0; y < info->height; y++) {
        for (int x = 0; x < info->width; x++) {
            uint32_t pixel = vsp->layerGetPixel(layer, x, y);
            uint8_t alpha = (pixel >> 24) & 0xFF;
            uint32_t invertedPixel = 0xFFFFFFFF - pixel; // Invert the color
            if (preserveAlpha) {
                invertedPixel = (alpha << 24) | (invertedPixel & 0xFFFFFF); // Preserve the alpha channel
            }
            vsp->layerSetPixel(layer, x, y, invertedPixel);
        }
    }
    free(info);
}

void pluginInit(voidspriteSDK* sdk)
{
    vsp = sdk;

    printf("Hello from pluginInit\n");

    // Register a sample file importer.
    vsp->registerLayerImporter("BIN file test", ".bin", VSP_LAYER_RGBA, NULL, importBIN, NULL);

    // Register a sample filter.
    VSPFilter* f = vsp->registerFilter("Invert colors", invertFilter);
    // Add parameters to our filter
    // **If a filter has no parameters, it will execute instantly after choosing it.
    vsp->filterNewBoolParameter(f, "preserve alpha", true);
}

const char* getPluginName()
{
    return "voidsprite sample plugin";
}

const char* getPluginVersion()
{
    return "v1.0";
}

const char* getPluginDescription()
{
    return "cool plugin that does things";
}

const char* getPluginAuthors()
{
    return "me";
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

