#include <regex>
#include "io_base.h"
#include "io_voidsprite.h"

#include "../EditorLayerPicker.h"
#include "../FileIO.h"

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

std::map<std::string, std::string> voidsnReadKeyVals(PlatformNativePathString path)
{
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        DoOnReturn closeInfile([infile]() {fclose(infile); });
        uint8_t voidsnversion;
        fread(&voidsnversion, 1, 1, infile);
        if (voidsnversion >= 3 && voidsnversion <= 7) {

            if (voidsnversion >= 6) {
                char voidspriteHeader[11];
                fread(voidspriteHeader, 11, 1, infile);
                // this should equal "voidsprite"
                if (memcmp(voidspriteHeader, "voidsprite", 11) != 0) {
                    logerr("INVALID VOIDSN HEADER\n");
                    return {};
                }
            }

            XY dimensions;
            dimensions.x = voidsnReadU32(infile);
            dimensions.y = voidsnReadU32(infile);

            char metaHeader[13];
            fread(metaHeader, 13, 1, infile);
            // this should equal /VOIDSN.META/
            if (memcmp(metaHeader, "/VOIDSN.META/", 13) != 0) {
                logerr("INVALID META HEADER\n");
                return {};
            }
            int nExtData = voidsnReadU32(infile);
            std::map<std::string, std::string> extData;
            for (int x = 0; x < nExtData; x++) {
                std::string key = voidsnReadString(infile);
                std::string val = voidsnReadString(infile);
                extData[key] = val;
            }

            return extData;
        }
    }
    return {};
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

        fwrite(&editor->ssne.tileDimensions.x, 4, 1, outfile);
        fwrite(&editor->ssne.tileDimensions.y, 4, 1, outfile);

        nvalBuffer = editor->getLayerStack().size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : editor->getLayerStack()) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logerr("[VOIDSNv2] INVALID LAYER DIMENSIONS (THIS IS BAD)");
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

        std::string commentsData = editor->makeCommentDataString(editor->getCurrentFrame());

        std::string layerVisibilityData = "";
        for (Layer*& lr : editor->getLayerStack()) {
            layerVisibilityData += lr->hidden ? '0' : '1';
        }

        std::string layerOpacityData = "";
        for (Layer*& lr : editor->getLayerStack()) {
            layerOpacityData += std::to_string(lr->layerAlpha) + ';';
        }

        fwrite("/VOIDSN.META/", 1, 13, outfile);
        std::map<std::string, std::string> extData = {
            {"comments", commentsData},
            {"layer.selected", std::to_string(editor->selLayer)},
            {"layer.visibility", layerVisibilityData},
            {"layer.opacity", layerOpacityData},
        };

        for (auto& [k,v] : editor->ssne.serializeToKeyVals()) {
            extData[k] = v;
        }

        nvalBuffer = extData.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (auto& [key, value] : extData) {
            voidsnWriteString(outfile, key);
            voidsnWriteString(outfile, value);
        }

        nvalBuffer = editor->getLayerStack().size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : editor->getLayerStack()) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logerr("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
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

        std::string commentsData = editor->makeCommentDataString(editor->getCurrentFrame());

        std::string layerVisibilityData = "";
        for (Layer*& lr : editor->getLayerStack()) {
            layerVisibilityData += lr->hidden ? '0' : '1';
        }

        fwrite("/VOIDSN.META/", 1, 13, outfile);
        std::map<std::string, std::string> extData = {
            {"comments", commentsData},
            {"layer.selected", std::to_string(editor->selLayer)},
            {"layer.visibility", layerVisibilityData},
            {"palette.enabled", editor->isPalettized ? "1" : "0"},
        };

        for (auto& [k,v] : editor->ssne.serializeToKeyVals()) {
            extData[k] = v;
        }

        if (editor->isPalettized) {
            MainEditorPalettized* upcastEditor = ((MainEditorPalettized*)editor);
            std::string paletteData = "";
            paletteData += frmt("{};", upcastEditor->palette.size());
            for (uint32_t& c : upcastEditor->palette) {
                paletteData += frmt("{:08X};", c);
            }
            extData["palette.colors"] = paletteData;

            extData["palette.index"] = std::to_string(upcastEditor->pickedPaletteIndex);
        }
        else {
            std::string layerOpacityData = "";
            for (Layer*& lr : editor->getLayerStack()) {
                layerOpacityData += std::to_string(lr->layerAlpha) + ';';
            }
            extData["layer.opacity"] = layerOpacityData;
            extData["activecolor"] = frmt("{:06X}", editor->pickedColor);
            extData["activecolor.alpha"] = frmt("{:02X}", editor->pickedAlpha);
        }

        nvalBuffer = extData.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (auto& [key, value] : extData) {
            voidsnWriteString(outfile, key);
            voidsnWriteString(outfile, value);
        }

        nvalBuffer = editor->getLayerStack().size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : editor->getLayerStack()) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logerr("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
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

        std::string commentsData = editor->makeCommentDataString(editor->getCurrentFrame());

        std::string layerVisibilityData = "";
        for (Layer*& lr : editor->getLayerStack()) {
            layerVisibilityData += lr->hidden ? '0' : '1';
        }

        fwrite("/VOIDSN.META/", 1, 13, outfile);
        std::map<std::string, std::string> extData = {
            {"comments", commentsData},
            {"layer.selected", std::to_string(editor->selLayer)},
            {"layer.visibility", layerVisibilityData},
            {"palette.enabled", editor->isPalettized ? "1" : "0"},
            {"edit.time", std::to_string(editor->editTime)}
        };

        for (auto& [k,v] : editor->ssne.serializeToKeyVals()) {
            extData[k] = v;
        }

        if (editor->isPalettized) {
            MainEditorPalettized* upcastEditor = ((MainEditorPalettized*)editor);
            std::string paletteData = "";
            paletteData += frmt("{};", upcastEditor->palette.size());
            for (uint32_t& c : upcastEditor->palette) {
                paletteData += frmt("{:08X};", c);
            }
            extData["palette.colors"] = paletteData;

            extData["palette.index"] = std::to_string(upcastEditor->pickedPaletteIndex);
        }
        else {
            std::string layerOpacityData = "";
            for (Layer*& lr : editor->getLayerStack()) {
                layerOpacityData += std::to_string(lr->layerAlpha) + ';';
            }
            extData["layer.opacity"] = layerOpacityData;
            extData["activecolor"] = frmt("{:06X}", editor->pickedColor);
            extData["activecolor.alpha"] = frmt("{:02X}", editor->pickedAlpha);
        }

        voidsnWriteU32(outfile, extData.size());

        for (auto& [key, value] : extData) {
            voidsnWriteString(outfile, key);
            voidsnWriteString(outfile, value);
        }

        voidsnWriteU32(outfile, editor->getLayerStack().size());

        for (Layer*& lr : editor->getLayerStack()) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logerr("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
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

        std::string commentsData = editor->makeCommentDataString(editor->getCurrentFrame());

        std::string layerVisibilityData = "";
        for (Layer*& lr : editor->getLayerStack()) {
            layerVisibilityData += lr->hidden ? '0' : '1';
        }

        fwrite("/VOIDSN.META/", 1, 13, outfile);
        std::map<std::string, std::string> extData = {
            {"comments", commentsData},
            {"layer.selected", std::to_string(editor->selLayer)},
            {"layer.visibility", layerVisibilityData},
            {"palette.enabled", editor->isPalettized ? "1" : "0"},
            {"edit.time", std::to_string(editor->editTime)}
        };

        for (auto& [k,v] : editor->ssne.serializeToKeyVals()) {
            extData[k] = v;
        }

        for (auto& [key, value] : editor->toolProperties) {
            extData[frmt("tool.property:{}", key)] = std::to_string(value);
        }

        if (editor->isPalettized) {
            MainEditorPalettized* upcastEditor = ((MainEditorPalettized*)editor);
            std::string paletteData = "";
            paletteData += frmt("{};", upcastEditor->palette.size());
            for (uint32_t& c : upcastEditor->palette) {
                paletteData += frmt("{:08X};", c);
            }
            extData["palette.colors"] = paletteData;

            extData["palette.index"] = std::to_string(upcastEditor->pickedPaletteIndex);
        }
        else {
            std::string layerOpacityData = "";
            for (Layer*& lr : editor->getLayerStack()) {
                layerOpacityData += std::to_string(lr->layerAlpha) + ';';
            }
            extData["layer.opacity"] = layerOpacityData;
            extData["activecolor"] = frmt("{:06X}", editor->pickedColor);
            extData["activecolor.alpha"] = frmt("{:02X}", editor->pickedAlpha);
        }

        voidsnWriteU32(outfile, extData.size());

        for (auto& [key, value] : extData) {
            voidsnWriteString(outfile, key);
            voidsnWriteString(outfile, value);
        }

        voidsnWriteU32(outfile, editor->getLayerStack().size());

        struct VOIDSNLayerData {
            std::string dataName;
            u64 dataSize;
            void* dataPtr;
            bool hintDelete = false;
        };

        for (Layer*& lr : editor->getLayerStack()) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logprintf("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
            }
            std::vector<VOIDSNLayerData> layerData = {
                {"name", (u64)lr->name.size(), lr->name.data()},
                {"colorKeySet", 1, (void*)(lr->colorKeySet ? "\1" : "\0")},
                {"colorKey", 4, &lr->colorKey},
                {"currentVariant", 4, &lr->currentLayerVariant},
                {"colorTag", 4, &lr->colorTag}
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

bool writeVOIDSNv7(PlatformNativePathString path, MainEditor* editor)
{
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        uint8_t voidsnVersion = 0x07;
        fwrite(&voidsnVersion, 1, 1, outfile);
        fwrite("voidsprite", sizeof("voidsprite"), 1, outfile);

        voidsnWriteU32(outfile, editor->canvas.dimensions.x);
        voidsnWriteU32(outfile, editor->canvas.dimensions.y);

        fwrite("/VOIDSN.META/", 1, 13, outfile);
        std::map<std::string, std::string> extData = {
            {"layer.selected", std::to_string(editor->selLayer)},
            {"palette.enabled", editor->isPalettized ? "1" : "0"},
            {"edit.time", std::to_string(editor->editTime)},
            {"frame.active", std::to_string(editor->activeFrame)},
            {"frame.ms", std::to_string(editor->frameAnimMSPerFrame)},
            {"frame.backtrace", std::to_string(editor->backtraceFrames)},
            {"frame.fwdtrace", std::to_string(editor->fwdtraceFrames)},
        };

        for (auto& [k,v] : editor->ssne.serializeToKeyVals()) {
            extData[k] = v;
        }

        for (auto& [key, value] : editor->toolProperties) {
            extData[frmt("tool.property:{}", key)] = std::to_string(value);
        }

        if (editor->isPalettized) {
            MainEditorPalettized* upcastEditor = ((MainEditorPalettized*)editor);
            std::string paletteData = "";
            paletteData += frmt("{};", upcastEditor->palette.size());
            for (uint32_t& c : upcastEditor->palette) {
                paletteData += frmt("{:08X};", c);
            }
            extData["palette.colors"] = paletteData;

            extData["palette.index"] = std::to_string(upcastEditor->pickedPaletteIndex);
        }
        else {
            extData["activecolor"] = frmt("{:06X}", editor->pickedColor);
            extData["activecolor.alpha"] = frmt("{:02X}", editor->pickedAlpha);
        }

        voidsnWriteU32(outfile, extData.size());

        for (auto& [key, value] : extData) {
            voidsnWriteString(outfile, key);
            voidsnWriteString(outfile, value);
        }

        voidsnWriteU32(outfile, editor->frames.size());

        for (Frame*& f : editor->frames) {
            std::string commentsData = editor->makeCommentDataString(f);
            std::map<std::string, std::string> frameExtData = {
                {"layer.selected", std::to_string(f->activeLayer)},
                {"comments", commentsData},
            };

            voidsnWriteU32(outfile, frameExtData.size());
            for (auto& [key, value] : frameExtData) {
                voidsnWriteString(outfile, key);
                voidsnWriteString(outfile, value);
            }

            voidsnWriteU32(outfile, f->layers.size());

            struct VOIDSNLayerData {
                std::string dataName;
                u64 dataSize;
                void* dataPtr;
                bool hintDelete = false;
            };

            for (Layer*& lr : f->layers) {
                if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                    logerr("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
                }
                std::vector<VOIDSNLayerData> layerData = {
                    {"name", (u64)lr->name.size(), lr->name.data()},
                    {"colorKeySet", 1, (void*)(lr->colorKeySet ? "\1" : "\0")},
                    {"colorKey", 4, &lr->colorKey},
                    {"currentVariant", 4, &lr->currentLayerVariant},
                    {"opacity", 1, &lr->layerAlpha},
                    {"hidden", 1, (void*)(lr->hidden ? "\1" : "\0")},
                    {"colorTag", 4, &lr->colorTag}
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
        }

        fclose(outfile);
        return true;
    }
    return false;
}

MainEditor* readVOIDSN(PlatformNativePathString path, OperationProgressReport* progress)
{
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        uint8_t voidsnversion;
        fread(&voidsnversion, 1, 1, infile);
        progress->enterSection(frmt("voidsprite session v{}", voidsnversion));
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
            ret->ssne.tileDimensions = tiledimensions;
            fclose(infile);
            return ret;
        }
        break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
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
                auto colors = splitString(paletteString, ';');
                for (int i = 1; i < colors.size() - 1; i++) {
                    palette.push_back(std::stoul(colors[i], NULL, 16));
                }
            }

            MainEditor* ret;
            std::vector<Frame*> frames;
            int nframes = voidsnversion >= 7 ? voidsnReadU32(infile) : 1;

            for (int fidx = 0; fidx < nframes; fidx++) {
                progress->enterSection(frmt("Reading frame {}/{}", fidx + 1, nframes));
                Frame* frame = new Frame();
                frames.push_back(frame);

                if (voidsnversion >= 7) {
                    int nFrameExtData = voidsnReadU32(infile);
                    std::map<std::string, std::string> frameExtData;
                    for (int x = 0; x < nFrameExtData; x++) {
                        std::string key = voidsnReadString(infile);
                        std::string val = voidsnReadString(infile);
                        frameExtData[key] = val;
                    }

                    if (frameExtData.contains("layer.selected")) { frame->activeLayer = std::stoi(frameExtData["layer.selected"]); }
                    if (frameExtData.contains("comments")) {
                        frame->comments = MainEditor::parseCommentDataString(frameExtData["comments"]);
                    }
                }

                int nlayers = voidsnReadU32(infile);

                for (int x = 0; x < nlayers; x++) {
                    progress->enterSection(frmt("Decompressing layer {}/{}", x + 1, nlayers));

                    Layer* newLayer = NULL;
                    if (voidsnversion >= 6) {
                        std::vector<LayerVariant> layerData;
                        newLayer = isPalettized ? new LayerPalettized(dimensions.x, dimensions.y, layerData)
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
                            else if (dataName == "opacity") {
                                newLayer->layerAlpha = data[0];
                            }
                            else if (dataName == "hidden") {
                                newLayer->hidden = data[0] == '\1';
                            }
                            else if (dataName == "colorTag") {
                                newLayer->colorTag = *(u32*)data;
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
                            frame->layers.push_back(newLayer);
                        }
                        else {
                            logerr(frmt("No variants in layer: {}", newLayer->name));
                            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), frmt("Error reading layer {}", newLayer->name)));
                        }
                    }
                    else {
                        std::string layerName = voidsnReadString(infile);
                        newLayer = isPalettized ? LayerPalettized::tryAllocIndexedLayer(dimensions.x, dimensions.y)
                            : Layer::tryAllocLayer(dimensions.x, dimensions.y);
                        if (newLayer != NULL) {
                            newLayer->name = layerName;

                            if (isPalettized) {
                                ((LayerPalettized*)newLayer)->palette = palette;
                            }

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
                            frame->layers.push_back(newLayer);
                        }
                        else {
                            logerr(frmt("Failed to allocate layer: {}", layerName));
                            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.mallocfail")));
                        }
                    }
                    
                    if (newLayer != NULL && isPalettized) {
                        ((LayerPalettized*)newLayer)->palette = palette;
                    }
                    progress->exitSection();
                }
                progress->exitSection();
            }

            ret = isPalettized ? new MainEditorPalettized(frames) : new MainEditor(frames);

            ret->ssne = SessionEditorPrefs::deserializeFromKeyVals(extData);
            //ensure in-session palette gets loaded
            ret->colorPicker->reloadColorLists();

            if (extData.contains("layer.selected")) { ret->selLayer = frames[0]->activeLayer = std::stoi(extData["layer.selected"]); }
            if (extData.contains("edit.time")) { ret->editTime = std::stoull(extData["edit.time"]); }
            if (extData.contains("frame.active")) { ret->activeFrame = -1; ret->switchFrame(std::stoi(extData["frame.active"])); }
            if (extData.contains("frame.ms")) { ret->setMSPerFrame(std::stoi(extData["frame.ms"])); }
            if (extData.contains("frame.backtrace")) { ret->backtraceFrames = std::stoi(extData["frame.backtrace"]); }
            if (extData.contains("frame.fwdtrace")) { ret->fwdtraceFrames = std::stoi(extData["frame.fwdtrace"]); }
            if (extData.contains("layer.visibility")) {
                std::string layerVisibilityData = extData["layer.visibility"];
                for (int x = 0; x < frames.front()->layers.size() && x < layerVisibilityData.size(); x++) {
                    frames.front()->layers[x]->hidden = layerVisibilityData[x] == '0';
                }
                ret->layerPicker->updateLayers();
            }
            if (extData.contains("comments")) {
                std::string commentsData = extData["comments"];
                frames.front()->comments = MainEditor::parseCommentDataString(commentsData);
            }
            for (auto& [key, value] : extData) {
                if (stringStartsWithIgnoreCase(key, "tool.property:")) {
                    std::string propName = key.substr(14);
                    try {
                        ret->toolProperties[propName] = std::stod(value);
                    }
                    catch (std::exception& e) {
                        logerr(frmt("Failed to parse tool property {}:\n {}", propName, e.what()));
                    }
                }
            }
            ret->initToolParameters();
            if (!ret->isPalettized && extData.contains("layer.opacity")) {
                std::string layerOpacityData = extData["layer.opacity"];
                auto& layers = frames.front()->layers;
                for (int x = 0; x < layers.size(); x++) {
                    int nextSC = layerOpacityData.find_first_of(';');
                    layers[x]->layerAlpha = (uint8_t)std::stoi(layerOpacityData.substr(0, nextSC));
                    layers[x]->lastConfirmedlayerAlpha = ret->getLayerStack()[x]->layerAlpha;
                    layerOpacityData = layerOpacityData.substr(nextSC + 1);
                }
                ret->layerPicker->updateLayers();
            }
            if (!ret->isPalettized && extData.contains("activecolor")) {
                uint32_t c = std::stoul(extData["activecolor"], NULL, 16);
                ret->setActiveColor(c);
            }
            if (!ret->isPalettized && extData.contains("activecolor.alpha")) {
                uint8_t a = (uint8_t)std::stoul(extData["activecolor.alpha"], NULL, 16);
                ret->setActiveAlpha(a);
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
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), frmt("VOIDSN file v{} not supported", voidsnversion)));
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

MainEditor* loadSplitSession(PlatformNativePathString path)
{
    std::string utf8path = convertStringToUTF8OnWin32(path);
    std::string fullDirectory = (utf8path.find('/') != std::string::npos || utf8path.find('\\') != std::string::npos) ? utf8path.substr(0, utf8path.find_last_of("\\/")) : "";
    if (fullDirectory.length() > 0) {
        fullDirectory += "/";
    }

    std::ifstream f(path);
    if (f.good() && f.is_open()) {
        SplitSessionData ssn;
        ssn.set = true;

        std::vector<Layer*> layers;
        std::vector<CommentData> comments;
        std::map<std::string, std::string> ssneKeyvals;
        XY minImageDimensions = { 0,0 };

        std::string line;
        std::getline(f, line);
        if (stringStartsWithIgnoreCase(line, "voidsprite split session file v")) {
            int version;
            try {
                version = std::stoi(line.substr(31));
            }
            catch (std::exception&) {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid split session file"));
                f.close();   //not needed apparently
                return NULL;
            }

            switch (version) {
            case 0:
            {
                while (!f.eof()) {
                    std::getline(f, line);
                    if (stringStartsWithIgnoreCase(line, "#:")) {
                        //image properties in format: #:filename|x|y
                        try {
                            std::string originalFileName = line.substr(2, line.find('|') - 2);
                            std::string filename = fullDirectory + originalFileName;
                            int x = std::stoi(line.substr(line.find('|') + 1, line.find('|', line.find('|') + 1) - line.find('|') - 1));
                            int y = std::stoi(line.substr(line.find('|', line.find('|') + 1) + 1));
                            SplitSessionImage ssi;
                            ssi.fileName = filename;
                            ssi.originalFileName = originalFileName;
                            ssi.positionInOverallImage = XY{ x,y };
                            ssi.exporter = NULL;    //todo

                            FileImporter* foundImporter = NULL;
                            MainEditor* subsn = loadAnyIntoSession(filename, &foundImporter);
                            if (subsn != NULL) {
                                ssi.exporter = foundImporter->getCorrespondingExporter();
                                Layer* nlayer = subsn->flattenImage();
                                ssi.dimensions = { nlayer->w, nlayer->h };
                                layers.push_back(nlayer);
                                ssn.images.push_back(ssi);
                                if (x + nlayer->w > minImageDimensions.x) {
                                    minImageDimensions.x = x + nlayer->w;
                                }
                                if (y + nlayer->h > minImageDimensions.y) {
                                    minImageDimensions.y = y + nlayer->h;
                                }
                                delete subsn;
                            }
                            else {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to load split session fragment"));
                            }

                        }
                        catch (std::exception&) {
                            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to load split session fragment"));
                        }
                    }
                    else if (stringStartsWithIgnoreCase(line, "comment:")) {
                        try {
                            std::string fullData = line.substr(8);
                            std::regex split("(;([0-9]+);([0-9]+))$");
                            std::smatch match;
                            std::regex_search(fullData, match, split);
                            if (match.size() == 4) {
                                std::string comment = fullData.substr(0, fullData.size() - match[0].str().size());
                                int x = std::stoi(match[2].str());
                                int y = std::stoi(match[3].str());
                                comments.push_back(CommentData{ XY{x,y}, comment });
                            }
                        }
                        catch (std::exception&) {
                            logerr("error reading comment");
                        }
                    }
                    else if (stringStartsWithIgnoreCase(line, "+") && line.find(':') != std::string::npos) {
                        auto kv = splitString(line, ':');
                        if (kv[0].size() > 1) {
                            ssneKeyvals[kv[0].substr(1)] = kv[1];
                        }
                    }
                    //compat
                    else if (line.find(':') != std::string::npos) {
                        auto kv = splitString(line, ':');

                        if (kv[0] == "tiledim.x") {
                            ssneKeyvals["tile.dim.x"] = kv[1];
                        }
                        else if (kv[0] == "tiledim.y") {
                            ssneKeyvals["tile.dim.y"] = kv[1];
                        }
                    }
                }
                if (layers.size() > 0) {
                    ssn.overallDimensions = minImageDimensions;
                    Layer* imageLayer = new Layer(minImageDimensions.x, minImageDimensions.y);
                    imageLayer->name = "Split session layer";
                    for (int i = 0; i < layers.size(); i++) {
                        SplitSessionImage ssi = ssn.images[i];
                        Layer* layer = layers[i];
                        imageLayer->blit(layer, ssi.positionInOverallImage, { 0,0,layer->w, layer->h }, true);
                        delete layer;
                    }
                    MainEditor* newEditor = new MainEditor(imageLayer);
                    newEditor->splitSessionData = ssn;
                    newEditor->ssne = SessionEditorPrefs::deserializeFromKeyVals(ssneKeyvals);
                    newEditor->lastConfirmedSavePath = path;
                    newEditor->getCommentStack() = comments;
                    return newEditor;
                }

            }
            break;
            default:
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Unsupported split session file version"));
                f.close();
                return NULL;
            }
        }
    }
    return NULL;
}

bool saveSplitSession(PlatformNativePathString path, MainEditor* data)
{
    if (!data->splitSessionData.set) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "No split session data."));
        return false;
    }
    std::ofstream f(path);
    f << "voidsprite split session file v0\n";
    for (CommentData& comment : data->frames[0]->comments) {
        f << "comment:" << comment.data << ";" << comment.position.x << ";" << comment.position.y << "\n";
    }
    auto ssneKeyvals = data->ssne.serializeToKeyVals();
    for (auto& [k, v] : ssneKeyvals) {
        f << frmt("+{}:{}\n", k, v);
    }

    SplitSessionData ssn = data->splitSessionData;
    Layer* flat = data->flattenImage();
    for (SplitSessionImage& separateImage : ssn.images) {
        f << "#:"
            << separateImage.originalFileName
            << "|" << separateImage.positionInOverallImage.x << "|"
            << separateImage.positionInOverallImage.y << "\n";
        PlatformNativePathString subImageFile = convertStringOnWin32(separateImage.fileName);
        if (separateImage.exporter != NULL) {
            Layer* trimmed = flat->trim({ separateImage.positionInOverallImage.x, separateImage.positionInOverallImage.y,
                separateImage.dimensions.x, separateImage.dimensions.y });
            separateImage.exporter->exportData(subImageFile, trimmed);
            delete trimmed;
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "No exporter for split session image"));
        }
    }
    delete flat;
    g_addNotification(SuccessNotification("Success", "Saved split session."));
    return true;
}