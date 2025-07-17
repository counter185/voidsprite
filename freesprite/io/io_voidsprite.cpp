#include "io_base.h"
#include "io_voidsprite.h"

#include "../EditorLayerPicker.h"

#include <zlib.h>


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
            nvalBuffer = lr->name.size();
            fwrite(&nvalBuffer, 4, 1, outfile);
            fwrite(lr->name.c_str(), nvalBuffer, 1, outfile);

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

        for (auto& extDPair : extData) {
            nvalBuffer = extDPair.first.size();
            fwrite(&nvalBuffer, 4, 1, outfile);
            fwrite(extDPair.first.c_str(), nvalBuffer, 1, outfile);
            nvalBuffer = extDPair.second.size();
            fwrite(&nvalBuffer, 4, 1, outfile);
            fwrite(extDPair.second.c_str(), nvalBuffer, 1, outfile);
        }

        nvalBuffer = editor->layers.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : editor->layers) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logprintf("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
            }
            nvalBuffer = lr->name.size();
            fwrite(&nvalBuffer, 4, 1, outfile);
            fwrite(lr->name.c_str(), nvalBuffer, 1, outfile);

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

        for (auto& extDPair : extData) {
            nvalBuffer = extDPair.first.size();
            fwrite(&nvalBuffer, 4, 1, outfile);
            fwrite(extDPair.first.c_str(), nvalBuffer, 1, outfile);
            nvalBuffer = extDPair.second.size();
            fwrite(&nvalBuffer, 4, 1, outfile);
            fwrite(extDPair.second.c_str(), nvalBuffer, 1, outfile);
        }

        nvalBuffer = editor->layers.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : editor->layers) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logprintf("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
            }
            nvalBuffer = lr->name.size();
            fwrite(&nvalBuffer, 4, 1, outfile);
            fwrite(lr->name.c_str(), nvalBuffer, 1, outfile);

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
        uint32_t nvalBuffer;

        nvalBuffer = editor->canvas.dimensions.x;
        fwrite(&nvalBuffer, 4, 1, outfile);
        nvalBuffer = editor->canvas.dimensions.y;
        fwrite(&nvalBuffer, 4, 1, outfile);

        //fwrite(&editor->tileDimensions.x, 4, 1, outfile);
        //fwrite(&editor->tileDimensions.y, 4, 1, outfile);

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

        nvalBuffer = extData.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (auto& extDPair : extData) {
            nvalBuffer = extDPair.first.size();
            fwrite(&nvalBuffer, 4, 1, outfile);
            fwrite(extDPair.first.c_str(), nvalBuffer, 1, outfile);
            nvalBuffer = extDPair.second.size();
            fwrite(&nvalBuffer, 4, 1, outfile);
            fwrite(extDPair.second.c_str(), nvalBuffer, 1, outfile);
        }

        nvalBuffer = editor->layers.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : editor->layers) {
            if (lr->w * lr->h != editor->canvas.dimensions.x * editor->canvas.dimensions.y) {
                logprintf("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
            }
            nvalBuffer = lr->name.size();
            fwrite(&nvalBuffer, 4, 1, outfile);
            fwrite(lr->name.c_str(), nvalBuffer, 1, outfile);

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
        {
            XY dimensions;
            fread(&dimensions.x, 4, 1, infile);
            fread(&dimensions.y, 4, 1, infile);

            char metaHeader[13];
            fread(metaHeader, 13, 1, infile);
            // this should equal /VOIDSN.META/
            if (memcmp(metaHeader, "/VOIDSN.META/", 13) != 0) {
                logprintf("INVALID META HEADER\n");
            }
            int nExtData;
            fread(&nExtData, 4, 1, infile);
            std::map<std::string, std::string> extData;
            for (int x = 0; x < nExtData; x++) {
                int keySize;
                fread(&keySize, 4, 1, infile);
                std::string key;
                key.resize(keySize);
                fread(&key[0], keySize, 1, infile);
                int valSize;
                fread(&valSize, 4, 1, infile);
                std::string val;
                val.resize(valSize);
                fread(&val[0], valSize, 1, infile);
                extData[key] = val;
            }

            bool isPalettized = extData.contains("palette.enabled") && extData.contains("palette.colors") && std::stoi(extData["palette.enabled"]) == 1;

            int nlayers;
            fread(&nlayers, 4, 1, infile);

            MainEditor* ret;
            if (!isPalettized) {
                std::vector<Layer*> layers;
                for (int x = 0; x < nlayers; x++) {
                    int nameLen;
                    fread(&nameLen, 4, 1, infile);
                    char* name = (char*)tracked_malloc(nameLen + 1);
                    memset(name, 0, nameLen + 1);
                    fread(name, nameLen, 1, infile);

                    Layer* newLayer = new Layer(dimensions.x, dimensions.y);
                    newLayer->name = std::string(name);

                    char colorKeySet;
                    fread(&colorKeySet, 1, 1, infile);
                    newLayer->colorKeySet = colorKeySet == '\1';
                    fread(&newLayer->colorKey, 4, 1, infile);

                    tracked_free(name);

                    //voidsn version 5+ uses zlib compression
                    if (voidsnversion < 5) {
                        fread(newLayer->pixels32(), newLayer->w * newLayer->h, 4, infile);
                    }
                    else {
                        uint64_t compressedLength = 0;
                        fread(&compressedLength, 8, 1, infile);
                        uint8_t* compressedData = new uint8_t[compressedLength];
                        fread(compressedData, compressedLength, 1, infile);
                        uint64_t dstLength = newLayer->w * newLayer->h * 4;
                        uncompress(newLayer->pixels8(), (uLongf*)&dstLength, compressedData, compressedLength);
                        delete[] compressedData;
                    }

                    layers.push_back(newLayer);
                }
                ret = new MainEditor(layers);
            }
            else {
                std::vector<uint32_t> palette;
                std::string paletteString = extData["palette.colors"];
                int nextSC = paletteString.find_first_of(';');
                int paletteColors = std::stoi(paletteString.substr(0, nextSC));
                paletteString = paletteString.substr(nextSC + 1);
                for (int x = 0; x < paletteColors; x++) {
                    nextSC = paletteString.find_first_of(';');
                    palette.push_back(std::stoul(paletteString.substr(0, nextSC), NULL, 16));
                    paletteString = paletteString.substr(nextSC + 1);
                }

                std::vector<LayerPalettized*> layers;
                for (int x = 0; x < nlayers; x++) {
                    int nameLen;
                    fread(&nameLen, 4, 1, infile);
                    char* name = (char*)tracked_malloc(nameLen + 1);
                    memset(name, 0, nameLen + 1);
                    fread(name, nameLen, 1, infile);

                    LayerPalettized* newLayer = new LayerPalettized(dimensions.x, dimensions.y);
                    newLayer->name = std::string(name);

                    char colorKeySet;
                    fread(&colorKeySet, 1, 1, infile);
                    newLayer->colorKeySet = colorKeySet == '\1';
                    fread(&newLayer->colorKey, 4, 1, infile);

                    tracked_free(name);

                    if (voidsnversion < 5) {
                        fread(newLayer->pixels32(), newLayer->w * newLayer->h, 4, infile);
                    }
                    else {
                        uint64_t compressedLength = 0;
                        fread(&compressedLength, 8, 1, infile);
                        uint8_t* compressedData = new uint8_t[compressedLength];
                        fread(compressedData, compressedLength, 1, infile);
                        uint64_t dstLength = newLayer->w * newLayer->h * 4;
                        uncompress(newLayer->pixels8(), (uLongf*)&dstLength, compressedData, compressedLength);
                        delete[] compressedData;
                    }

                    newLayer->palette = palette;
                    layers.push_back(newLayer);
                }

                ret = new MainEditorPalettized(layers);
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