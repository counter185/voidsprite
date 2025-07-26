#include "io_base.h"
#include "io_voidsprite.h"

#include "../EditorLayerPicker.h"

#include <zlib.h>


void voidsnWriteU32(FILE* f, u32 num)
{
    u32 nvalBuffer = num;
    fwrite(&nvalBuffer, 4, 1, f);
}

void voidsnWriteU64(FILE* f, u64 num)
{
    u64 nvalBuffer = num;
    fwrite(&nvalBuffer, 8, 1, f);
}

void voidsnWriteString(FILE* f, std::string str)
{
    u32 nvalBuffer = str.size();
    voidsnWriteU32(f, nvalBuffer);
    fwrite(str.c_str(), nvalBuffer, 1, f);
}

u32 voidsnReadU32(FILE* f)
{
    u32 nvalBuffer;
    fread(&nvalBuffer, 4, 1, f);
    return nvalBuffer;
}

u64 voidsnReadU64(FILE* f)
{
    u64 nvalBuffer;
    fread(&nvalBuffer, 8, 1, f);
    return nvalBuffer;
}

std::string voidsnReadString(FILE* f)
{
    u32 nvalBuffer = voidsnReadU32(f);
    std::string ret;
    ret.resize(nvalBuffer);
    fread(ret.data(), nvalBuffer, 1, f);
    return ret;
}

bool writeVOIDSNv1(PlatformNativePathString path, XY projDimensions, std::vector<Layer*> data)
{
    if (data[0]->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Palettized image export not implemented"));
        return false;
    }

    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        uint8_t voidsnVersion = 0x01;
        fwrite(&voidsnVersion, 1, 1, outfile);
        uint32_t nvalBuffer;

        nvalBuffer = projDimensions.x;
        fwrite(&nvalBuffer, 4, 1, outfile);
        nvalBuffer = projDimensions.y;
        fwrite(&nvalBuffer, 4, 1, outfile);

        nvalBuffer = data.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : data) {
            if (lr->w * lr->h != projDimensions.x * projDimensions.y) {
                logprintf("[VOIDSNv1] INVALID LAYER DIMENSIONS (THIS IS BAD)");
            }
            fwrite(lr->pixels32(), lr->w * lr->h, 4, outfile);
        }

        fclose(outfile);
        return true;
    }
    return false;
}

bool writeVOIDSNv2(PlatformNativePathString path, MainEditor* editor)
{
    if (editor->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Palettized image export not implemented"));
        return false;
    }

    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        uint8_t voidsnVersion = 0x02;
        fwrite(&voidsnVersion, 1, 1, outfile);
        uint32_t nvalBuffer;

        nvalBuffer = editor->canvas.dimensions.x;
        fwrite(&nvalBuffer, 4, 1, outfile);
        nvalBuffer = editor->canvas.dimensions.y;
        fwrite(&nvalBuffer, 4, 1, outfile);

        fwrite(&editor->tileDimensions.x, 4, 1, outfile);
        fwrite(&editor->tileDimensions.y, 4, 1, outfile);

        nvalBuffer = editor->layers.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : editor->layers) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logprintf("[VOIDSNv2] INVALID LAYER DIMENSIONS (THIS IS BAD)");
            }
            voidsnWriteString(outfile, lr->name);

            fwrite(lr->pixels32(), lr->w * lr->h, 4, outfile);
        }

        fclose(outfile);
        return true;
    }
    return false;
}

bool writeVOIDSNv3(PlatformNativePathString path, MainEditor* editor)
{
    if (editor->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Palettized image export not implemented"));
        return false;
    }

    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        uint8_t voidsnVersion = 0x03;
        fwrite(&voidsnVersion, 1, 1, outfile);
        uint32_t nvalBuffer;

        nvalBuffer = editor->canvas.dimensions.x;
        fwrite(&nvalBuffer, 4, 1, outfile);
        nvalBuffer = editor->canvas.dimensions.y;
        fwrite(&nvalBuffer, 4, 1, outfile);

        //fwrite(&editor->tileDimensions.x, 4, 1, outfile);
        //fwrite(&editor->tileDimensions.y, 4, 1, outfile);

        std::string commentsData = editor->makeCommentDataString();

        std::string layerVisibilityData = "";
        for (Layer*& lr : editor->layers) {
            layerVisibilityData += lr->hidden ? '0' : '1';
        }

        std::string layerOpacityData = "";
        for (Layer*& lr : editor->layers) {
            layerOpacityData += std::to_string(lr->layerAlpha) + ';';
        }

        fwrite("/VOIDSN.META/", 1, 13, outfile);
        std::map<std::string, std::string> extData = {
            {"tile.dim.x", std::to_string(editor->tileDimensions.x)},
            {"tile.dim.y", std::to_string(editor->tileDimensions.y)},
            {"sym.enabled", std::format("{}{}", (editor->symmetryEnabled[0] ? '1' : '0'), (editor->symmetryEnabled[1] ? '1' : '0'))},
            {"sym.x", std::to_string(editor->symmetryPositions.x)},
            {"sym.y", std::to_string(editor->symmetryPositions.y)},
            {"comments", commentsData},
            {"layer.selected", std::to_string(editor->selLayer)},
            {"layer.visibility", layerVisibilityData},
            {"layer.opacity", layerOpacityData},
            {"editor.altbg", editor->usingAltBG() ? "1" : "0"}
        };

        nvalBuffer = extData.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (auto& [key, value] : extData) {
            voidsnWriteString(outfile, key);
            voidsnWriteString(outfile, value);
        }

        nvalBuffer = editor->layers.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : editor->layers) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logprintf("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
            }
            voidsnWriteString(outfile, lr->name);

            fwrite(lr->colorKeySet ? "\1" : "\0", 1, 1, outfile);
            fwrite(&lr->colorKey, 4, 1, outfile);

            fwrite(lr->pixels32(), lr->w * lr->h, 4, outfile);
        }

        fclose(outfile);
        return true;
    }
    return false;
}

bool writeVOIDSNv4(PlatformNativePathString path, MainEditor* editor)
{
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        uint8_t voidsnVersion = 0x04;
        fwrite(&voidsnVersion, 1, 1, outfile);
        uint32_t nvalBuffer;

        nvalBuffer = editor->canvas.dimensions.x;
        fwrite(&nvalBuffer, 4, 1, outfile);
        nvalBuffer = editor->canvas.dimensions.y;
        fwrite(&nvalBuffer, 4, 1, outfile);

        //fwrite(&editor->tileDimensions.x, 4, 1, outfile);
        //fwrite(&editor->tileDimensions.y, 4, 1, outfile);

        std::string commentsData = editor->makeCommentDataString();

        std::string layerVisibilityData = "";
        for (Layer*& lr : editor->layers) {
            layerVisibilityData += lr->hidden ? '0' : '1';
        }

        fwrite("/VOIDSN.META/", 1, 13, outfile);
        std::map<std::string, std::string> extData = {
            {"tile.dim.x", std::to_string(editor->tileDimensions.x)},
            {"tile.dim.y", std::to_string(editor->tileDimensions.y)},
            {"sym.enabled", std::format("{}{}", (editor->symmetryEnabled[0] ? '1' : '0'), (editor->symmetryEnabled[1] ? '1' : '0'))},
            {"sym.x", std::to_string(editor->symmetryPositions.x)},
            {"sym.y", std::to_string(editor->symmetryPositions.y)},
            {"comments", commentsData},
            {"layer.selected", std::to_string(editor->selLayer)},
            {"layer.visibility", layerVisibilityData},
            {"palette.enabled", editor->isPalettized ? "1" : "0"},
            {"editor.altbg", editor->usingAltBG() ? "1" : "0"}
        };

        if (editor->isPalettized) {
            MainEditorPalettized* upcastEditor = ((MainEditorPalettized*)editor);
            std::string paletteData = "";
            paletteData += std::format("{};", upcastEditor->palette.size());
            for (uint32_t& c : upcastEditor->palette) {
                paletteData += std::format("{:08X};", c);
            }
            extData["palette.colors"] = paletteData;

            extData["palette.index"] = std::to_string(upcastEditor->pickedPaletteIndex);
        }
        else {
            std::string layerOpacityData = "";
            for (Layer*& lr : editor->layers) {
                layerOpacityData += std::to_string(lr->layerAlpha) + ';';
            }
            extData["layer.opacity"] = layerOpacityData;
            extData["activecolor"] = std::format("{:06X}", editor->pickedColor);
        }

        nvalBuffer = extData.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (auto& [key, value] : extData) {
            voidsnWriteString(outfile, key);
            voidsnWriteString(outfile, value);
        }

        nvalBuffer = editor->layers.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : editor->layers) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logprintf("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
            }
            voidsnWriteString(outfile, lr->name);

            fwrite(lr->colorKeySet ? "\1" : "\0", 1, 1, outfile);
            fwrite(&lr->colorKey, 4, 1, outfile);

            fwrite(lr->pixels32(), lr->w * lr->h, 4, outfile);
        }

        fclose(outfile);
        return true;
    }
    return false;
}

bool writeVOIDSNv5(PlatformNativePathString path, MainEditor* editor)
{
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        uint8_t voidsnVersion = 0x05;
        fwrite(&voidsnVersion, 1, 1, outfile);

        voidsnWriteU32(outfile, editor->canvas.dimensions.x);
        voidsnWriteU32(outfile, editor->canvas.dimensions.y);

        std::string commentsData = editor->makeCommentDataString();

        std::string guidelinesData;
        guidelinesData += std::format("{};", editor->guidelines.size());
        for (Guideline& g : editor->guidelines) {
            guidelinesData += std::format("{}-{};", g.vertical ? "v" : "h", g.position);
        }

        std::string layerVisibilityData = "";
        for (Layer*& lr : editor->layers) {
            layerVisibilityData += lr->hidden ? '0' : '1';
        }

        fwrite("/VOIDSN.META/", 1, 13, outfile);
        std::map<std::string, std::string> extData = {
            {"tile.dim.x", std::to_string(editor->tileDimensions.x)},
            {"tile.dim.y", std::to_string(editor->tileDimensions.y)},
            {"tile.dim.padrx", std::to_string(editor->tileGridPaddingBottomRight.x)},
            {"tile.dim.padby", std::to_string(editor->tileGridPaddingBottomRight.y)},
            {"sym.enabled", std::format("{}{}", (editor->symmetryEnabled[0] ? '1' : '0'), (editor->symmetryEnabled[1] ? '1' : '0'))},
            {"sym.x", std::to_string(editor->symmetryPositions.x)},
            {"sym.y", std::to_string(editor->symmetryPositions.y)},
            {"comments", commentsData},
            {"layer.selected", std::to_string(editor->selLayer)},
            {"layer.visibility", layerVisibilityData},
            {"palette.enabled", editor->isPalettized ? "1" : "0"},
            {"guidelines", guidelinesData},
            {"edit.time", std::to_string(editor->editTime)},
            {"editor.altbg", editor->usingAltBG() ? "1" : "0"}
        };

        if (editor->isPalettized) {
            MainEditorPalettized* upcastEditor = ((MainEditorPalettized*)editor);
            std::string paletteData = "";
            paletteData += std::format("{};", upcastEditor->palette.size());
            for (uint32_t& c : upcastEditor->palette) {
                paletteData += std::format("{:08X};", c);
            }
            extData["palette.colors"] = paletteData;

            extData["palette.index"] = std::to_string(upcastEditor->pickedPaletteIndex);
        }
        else {
            std::string layerOpacityData = "";
            for (Layer*& lr : editor->layers) {
                layerOpacityData += std::to_string(lr->layerAlpha) + ';';
            }
            extData["layer.opacity"] = layerOpacityData;
            extData["activecolor"] = std::format("{:06X}", editor->pickedColor);
        }

        voidsnWriteU32(outfile, extData.size());

        for (auto& [key, value] : extData) {
            voidsnWriteString(outfile, key);
            voidsnWriteString(outfile, value);
        }

        voidsnWriteU32(outfile, editor->layers.size());

        for (Layer*& lr : editor->layers) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logprintf("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
            }
            voidsnWriteString(outfile, lr->name);

            fwrite(lr->colorKeySet ? "\1" : "\0", 1, 1, outfile);
            fwrite(&lr->colorKey, 4, 1, outfile);

            uint64_t maxCompressedDataSize = compressBound(lr->w * lr->h * 4);
            uint64_t compressedDataSize = maxCompressedDataSize;
            uint8_t* compressedData = new uint8_t[maxCompressedDataSize];
            int res = compress(compressedData, (uLongf*)&compressedDataSize, lr->pixels8(), lr->w * lr->h * 4);

            fwrite(&compressedDataSize, 8, 1, outfile);
            fwrite(compressedData, compressedDataSize, 1, outfile);
            delete[] compressedData;
        }

        fclose(outfile);
        return true;
    }
    return false;
}

bool writeVOIDSNv6(PlatformNativePathString path, MainEditor* editor)
{
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        uint8_t voidsnVersion = 0x06;
        fwrite(&voidsnVersion, 1, 1, outfile);
        fwrite("voidsprite", sizeof("voidsprite"), 1, outfile);

        voidsnWriteU32(outfile, editor->canvas.dimensions.x);
        voidsnWriteU32(outfile, editor->canvas.dimensions.y);

        std::string commentsData = editor->makeCommentDataString();

        std::string guidelinesData;
        guidelinesData += std::format("{};", editor->guidelines.size());
        for (Guideline& g : editor->guidelines) {
            guidelinesData += std::format("{}-{};", g.vertical ? "v" : "h", g.position);
        }

        std::string layerVisibilityData = "";
        for (Layer*& lr : editor->layers) {
            layerVisibilityData += lr->hidden ? '0' : '1';
        }

        fwrite("/VOIDSN.META/", 1, 13, outfile);
        std::map<std::string, std::string> extData = {
            {"tile.dim.x", std::to_string(editor->tileDimensions.x)},
            {"tile.dim.y", std::to_string(editor->tileDimensions.y)},
            {"tile.dim.padrx", std::to_string(editor->tileGridPaddingBottomRight.x)},
            {"tile.dim.padby", std::to_string(editor->tileGridPaddingBottomRight.y)},
            {"sym.enabled", std::format("{}{}", (editor->symmetryEnabled[0] ? '1' : '0'), (editor->symmetryEnabled[1] ? '1' : '0'))},
            {"sym.x", std::to_string(editor->symmetryPositions.x)},
            {"sym.y", std::to_string(editor->symmetryPositions.y)},
            {"comments", commentsData},
            {"layer.selected", std::to_string(editor->selLayer)},
            {"layer.visibility", layerVisibilityData},
            {"palette.enabled", editor->isPalettized ? "1" : "0"},
            {"guidelines", guidelinesData},
            {"edit.time", std::to_string(editor->editTime)},
            {"editor.altbg", editor->usingAltBG() ? "1" : "0"}
        };

        if (editor->isPalettized) {
            MainEditorPalettized* upcastEditor = ((MainEditorPalettized*)editor);
            std::string paletteData = "";
            paletteData += std::format("{};", upcastEditor->palette.size());
            for (uint32_t& c : upcastEditor->palette) {
                paletteData += std::format("{:08X};", c);
            }
            extData["palette.colors"] = paletteData;

            extData["palette.index"] = std::to_string(upcastEditor->pickedPaletteIndex);
        }
        else {
            std::string layerOpacityData = "";
            for (Layer*& lr : editor->layers) {
                layerOpacityData += std::to_string(lr->layerAlpha) + ';';
            }
            extData["layer.opacity"] = layerOpacityData;
            extData["activecolor"] = std::format("{:06X}", editor->pickedColor);
        }

        voidsnWriteU32(outfile, extData.size());

        for (auto& [key, value] : extData) {
            voidsnWriteString(outfile, key);
            voidsnWriteString(outfile, value);
        }

        voidsnWriteU32(outfile, editor->layers.size());

        struct VOIDSNLayerData {
            std::string dataName;
            u64 dataSize;
            void* dataPtr;
            bool hintDelete = false;
        };

        for (Layer*& lr : editor->layers) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logprintf("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
            }
            std::vector<VOIDSNLayerData> layerData = {
                {"name", (u64)lr->name.size(), lr->name.data()},
                {"colorKeySet", 1, (void*)(lr->colorKeySet ? "\1" : "\0")},
                {"colorKey", 4, &lr->colorKey},
                {"currentVariant", 4, &lr->currentLayerVariant},
            };
            for (LayerVariant& lv : lr->layerData) {
                u64 maxCompressedDataSize = compressBound(lr->w * lr->h * 4);
                u64 compressedDataSize = maxCompressedDataSize;
                u8* compressedData = (u8*)tracked_malloc(maxCompressedDataSize, "Temp.memory");
                int res = compress(compressedData, (uLongf*)&compressedDataSize, lv.pixelData, lr->w * lr->h * 4);

                layerData.push_back({ "variant." + lv.name, compressedDataSize, compressedData, true });
            }

            voidsnWriteU32(outfile, layerData.size());
            for (VOIDSNLayerData& ld : layerData) {
                voidsnWriteString(outfile, ld.dataName);
                voidsnWriteU64(outfile, ld.dataSize);
                fwrite(ld.dataPtr, ld.dataSize, 1, outfile);

                if (ld.hintDelete) {
                    tracked_free(ld.dataPtr);
                }
            }
        }

        fclose(outfile);
        return true;
    }
    return false;
}

MainEditor* readVOIDSN(PlatformNativePathString path)
{
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        uint8_t voidsnversion;
        fread(&voidsnversion, 1, 1, infile);
        switch (voidsnversion) {
        case 1:
        {
            XY dimensions;
            fread(&dimensions.x, 4, 1, infile);
            fread(&dimensions.y, 4, 1, infile);
            std::vector<Layer*> layers;
            int nlayers;
            fread(&nlayers, 4, 1, infile);
            for (int x = 0; x < nlayers; x++) {
                Layer* newLayer = new Layer(dimensions.x, dimensions.y);
                fread(newLayer->pixels32(), newLayer->w * newLayer->h, 4, infile);
                layers.push_back(newLayer);
            }
            MainEditor* ret = new MainEditor(layers);
            fclose(infile);
            return ret;
        }
        break;
        case 2:
        {
            XY dimensions;
            fread(&dimensions.x, 4, 1, infile);
            fread(&dimensions.y, 4, 1, infile);

            XY tiledimensions;
            fread(&tiledimensions.x, 4, 1, infile);
            fread(&tiledimensions.y, 4, 1, infile);

            std::vector<Layer*> layers;
            int nlayers;
            fread(&nlayers, 4, 1, infile);
            for (int x = 0; x < nlayers; x++) {
                int nameLen;
                fread(&nameLen, 4, 1, infile);
                char* name = (char*)tracked_malloc(nameLen + 1);
                memset(name, 0, nameLen + 1);
                fread(name, nameLen, 1, infile);

                Layer* newLayer = new Layer(dimensions.x, dimensions.y);
                newLayer->name = std::string(name);
                tracked_free(name);
                fread(newLayer->pixels32(), newLayer->w * newLayer->h, 4, infile);
                layers.push_back(newLayer);
            }
            MainEditor* ret = new MainEditor(layers);
            ret->tileDimensions = tiledimensions;
            fclose(infile);
            return ret;
        }
        break;
        case 3:
        case 4:
        case 5:
        case 6:
        {
            if (voidsnversion >= 6) {
                char voidspriteHeader[11];
                fread(voidspriteHeader, 11, 1, infile);
                // this should equal "voidsprite"
                if (memcmp(voidspriteHeader, "voidsprite", 11) != 0) {
                    logerr("INVALID VOIDSN HEADER\n");
                    fclose(infile);
                    return NULL;
                }
            }

            XY dimensions;
            dimensions.x = voidsnReadU32(infile);
            dimensions.y = voidsnReadU32(infile);

            char metaHeader[13];
            fread(metaHeader, 13, 1, infile);
            // this should equal /VOIDSN.META/
            if (memcmp(metaHeader, "/VOIDSN.META/", 13) != 0) {
                logprintf("INVALID META HEADER\n");
            }
            int nExtData = voidsnReadU32(infile);
            std::map<std::string, std::string> extData;
            for (int x = 0; x < nExtData; x++) {
                std::string key = voidsnReadString(infile);
                std::string val = voidsnReadString(infile);
                extData[key] = val;
            }

            bool isPalettized = extData.contains("palette.enabled") && extData.contains("palette.colors") && std::stoi(extData["palette.enabled"]) == 1;

            std::vector<uint32_t> palette;
            if (isPalettized) {
                std::string paletteString = extData["palette.colors"];
                int nextSC = paletteString.find_first_of(';');
                int paletteColors = std::stoi(paletteString.substr(0, nextSC));
                paletteString = paletteString.substr(nextSC + 1);
                for (int x = 0; x < paletteColors; x++) {
                    nextSC = paletteString.find_first_of(';');
                    palette.push_back(std::stoul(paletteString.substr(0, nextSC), NULL, 16));
                    paletteString = paletteString.substr(nextSC + 1);
                }
            }

            int nlayers = voidsnReadU32(infile);

            MainEditor* ret;
            std::vector<Layer*> layers;
            for (int x = 0; x < nlayers; x++) {

                if (voidsnversion >= 6) {
                    std::vector<LayerVariant> layerData;
                    Layer* newLayer = isPalettized ? new LayerPalettized(dimensions.x, dimensions.y, layerData)
                        : new Layer(dimensions.x, dimensions.y, layerData);

                    u32 numLayerData = voidsnReadU32(infile);
                    for (u32 i = 0; i < numLayerData; i++) {
                        std::string dataName = voidsnReadString(infile);
                        u64 dataSize = voidsnReadU64(infile);
                        u8* data = (u8*)tracked_malloc(dataSize, "Temp.memory");
                        fread(data, dataSize, 1, infile);

                        if (dataName == "name") {
                            newLayer->name.resize(dataSize);
                            memcpy(newLayer->name.data(), data, dataSize);
                        }
                        else if (dataName == "colorKey") {
                            newLayer->colorKey = *(u32*)data;
                        }
                        else if (dataName == "colorKeySet") {
                            newLayer->colorKeySet = data[0] == '\1';
                        }
                        else if (dataName == "currentVariant") {
                            newLayer->currentLayerVariant = *(u32*)data;
                        }
                        else if (stringStartsWithIgnoreCase(dataName, "variant.")) {
                            std::string variantName = dataName.substr(8);
                            LayerVariant newVariant;
                            newVariant.name = variantName;
							newVariant.pixelData = (u8*)tracked_malloc(dimensions.x * dimensions.y * 4, "Layers");
							u64 decompressedDataSize = dimensions.x * dimensions.y * 4;
							uncompress(newVariant.pixelData, (uLongf*)&decompressedDataSize, data, dataSize);

                            layerData.push_back(newVariant);
                        }
                        tracked_free(data);
                    }

                    if (layerData.size() > 0) {
                        newLayer->layerData = layerData;
                        layers.push_back(newLayer);
                    }
                    else {
                        logerr(std::format("No variants in layer: {}", newLayer->name));
                        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), std::format("Error reading layer {}", newLayer->name)));
                    }
                }
                else {
                    std::string layerName = voidsnReadString(infile);
                    Layer* newLayer = isPalettized ? LayerPalettized::tryAllocIndexedLayer(dimensions.x, dimensions.y)
                        : Layer::tryAllocLayer(dimensions.x, dimensions.y);
                    if (newLayer != NULL) {
                        newLayer->name = layerName;

                        char colorKeySet;
                        fread(&colorKeySet, 1, 1, infile);
                        newLayer->colorKeySet = colorKeySet == '\1';
                        newLayer->colorKey = voidsnReadU32(infile);

                        //voidsn version 5+ uses zlib compression
                        if (voidsnversion < 5) {
                            fread(newLayer->pixels32(), newLayer->w * newLayer->h, 4, infile);
                        }
                        else {

                            uint64_t compressedLength = voidsnReadU64(infile);
                            uint8_t* compressedData = new uint8_t[compressedLength];
                            fread(compressedData, compressedLength, 1, infile);
                            uint64_t dstLength = newLayer->w * newLayer->h * 4;
                            uncompress(newLayer->pixels8(), (uLongf*)&dstLength, compressedData, compressedLength);
                            delete[] compressedData;
                        }

                        if (isPalettized) {
                            ((LayerPalettized*)newLayer)->palette = palette;
                        }
                        layers.push_back(newLayer);
                    }
                    else {
                        logerr(std::format("Failed to allocate layer: {}", layerName));
                        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.mallocfail")));
                    }
                }
            }
            if (isPalettized) {
                std::vector<LayerPalettized*> indexedLayers;
                //cast all layers to LayerPalettized*
                std::transform(layers.begin(), layers.end(), std::back_inserter(indexedLayers), [](Layer* l) {
                    return (LayerPalettized*)l;
                });
                ret = new MainEditorPalettized(indexedLayers);
            }
            else {
                ret = new MainEditor(layers);
            }

            if (extData.contains("tile.dim.x")) { ret->tileDimensions.x = std::stoi(extData["tile.dim.x"]); }
            if (extData.contains("tile.dim.y")) { ret->tileDimensions.y = std::stoi(extData["tile.dim.y"]); }
            if (extData.contains("tile.dim.padrx")) { ret->tileGridPaddingBottomRight.x = std::stoi(extData["tile.dim.padrx"]); }
            if (extData.contains("tile.dim.padby")) { ret->tileGridPaddingBottomRight.y = std::stoi(extData["tile.dim.padby"]); }
            if (extData.contains("sym.x")) { ret->symmetryPositions.x = std::stoi(extData["sym.x"]); }
            if (extData.contains("sym.y")) { ret->symmetryPositions.y = std::stoi(extData["sym.y"]); }
            if (extData.contains("layer.selected")) { ret->selLayer = std::stoi(extData["layer.selected"]); }
            if (extData.contains("edit.time")) { ret->editTime = std::stoull(extData["edit.time"]); }
            if (extData.contains("editor.altbg")) { ret->setAltBG(extData["editor.altbg"] == "1"); }
            if (extData.contains("sym.enabled")) {
                ret->symmetryEnabled[0] = extData["sym.enabled"][0] == '1';
                ret->symmetryEnabled[1] = extData["sym.enabled"][1] == '1';
            }
            if (extData.contains("layer.visibility")) {
                std::string layerVisibilityData = extData["layer.visibility"];
                for (int x = 0; x < nlayers && x < layerVisibilityData.size(); x++) {
                    ret->layers[x]->hidden = layerVisibilityData[x] == '0';
                }
                ret->layerPicker->updateLayers();
            }
            if (extData.contains("comments")) {
                std::string commentsData = extData["comments"];
                ret->comments = ret->parseCommentDataString(commentsData);
            }
            if (extData.contains("guidelines")) {
                std::string guidelinesData = extData["guidelines"];
                try {
                    int nextSC = guidelinesData.find_first_of(';');
                    int guidelinesCount = std::stoi(guidelinesData.substr(0, nextSC));
                    guidelinesData = guidelinesData.substr(nextSC + 1);
                    for (int x = 0; x < guidelinesCount; x++) {
                        Guideline newGD;
                        nextSC = guidelinesData.find_first_of(';');
                        std::string gdString = guidelinesData.substr(0, nextSC);
                        newGD.vertical = guidelinesData.substr(0, guidelinesData.find_first_of('-')) == "v";
                        newGD.position = std::stoi(guidelinesData.substr(guidelinesData.find_first_of('-') + 1));
                        ret->guidelines.push_back(newGD);
                        guidelinesData = guidelinesData.substr(nextSC + 1);
                    }
                }
                catch (std::exception&) {
                }
            }
            if (!ret->isPalettized && extData.contains("layer.opacity")) {
                std::string layerOpacityData = extData["layer.opacity"];
                for (int x = 0; x < nlayers; x++) {
                    int nextSC = layerOpacityData.find_first_of(';');
                    ret->layers[x]->layerAlpha = (uint8_t)std::stoi(layerOpacityData.substr(0, nextSC));
                    ret->layers[x]->lastConfirmedlayerAlpha = ret->layers[x]->layerAlpha;
                    layerOpacityData = layerOpacityData.substr(nextSC + 1);
                }
                ret->layerPicker->updateLayers();
            }
            if (!ret->isPalettized && extData.contains("activecolor")) {
                uint32_t c = std::stoul(extData["activecolor"], NULL, 16);
                ret->setActiveColor(c);
            }
            if (ret->isPalettized && extData.contains("palette.index")) {
                ((MainEditorPalettized*)ret)->pickedPaletteIndex = std::stoi(extData["palette.index"]);
            }
            fclose(infile);
            return ret;
        }
        break;
        default:
            logprintf("VOIDSN FILE v%i NOT SUPPORTED\n", voidsnversion);
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), std::format("VOIDSN file v{} not supported", voidsnversion)));
            fclose(infile);
            return NULL;
        }

        fclose(infile);
    }
    return NULL;
}

std::pair<bool, std::vector<uint32_t>> readPltVOIDPLT(PlatformNativePathString name)
{
    FILE* f = platformOpenFile(name, PlatformFileModeRB);
    if (f != NULL) {
        char header[7];
        fread(header, 7, 1, f);
        if (memcmp(header, "VOIDPLT", 7) == 0) {
            uint8_t fileversion;
            fread(&fileversion, 1, 1, f);
            if (fileversion == 1) {
                std::vector<uint32_t> newPalette;
                uint32_t count;
                fread(&count, 1, 4, f);
                for (int x = 0; x < count; x++) {
                    uint32_t col;
                    fread(&col, 1, 4, f);
                    newPalette.push_back(col);
                }
                fclose(f);
                return { true, newPalette };
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Unsupported VOIDPLT file version"));
            }
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid palette file"));
        }
        fclose(f);
    }
    return { false, {} };
}

bool writePltVOIDPLT(PlatformNativePathString path, std::vector<u32> palette)
{
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        fwrite("VOIDPLT", 1, 7, outfile);
        //file version 1
        fwrite("\x1", 1, 1, outfile);

        voidsnWriteU32(outfile, palette.size());
        for (u32& c : palette) {
            voidsnWriteU32(outfile, c);
        }
        fclose(outfile);
        return true;
    }
	return false;
}
