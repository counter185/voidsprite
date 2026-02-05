
#include <zlib.h>
#include "voidsn_read.h"


u32 modAlpha(u32 color, u8 alpha)
{
    return (color & 0x00FFFFFF) + (alpha << 24);
}

u32 alphaBlend(u32 colora, u32 colorb) {
    u32 a2 = (colorb & 0xFF000000) >> 24;
    u32 alpha = a2;
    if (alpha == 0) return colora;
    if (alpha == 255) return colorb;
    u32 a1 = (colora & 0xFF000000) >> 24;
    u32 nalpha = 0x100 - alpha;
    u32 rb1 = (nalpha * (colora & 0xFF00FF)) >> 8;
    u32 rb2 = (alpha * (colorb & 0xFF00FF)) >> 8;
    u32 g1 = (nalpha * (colora & 0x00FF00)) >> 8;
    u32 g2 = (alpha * (colorb & 0x00FF00)) >> 8;
    u32 anew = a1 + a2;
    if (anew > 255) { anew = 255; }
    return ((rb1 + rb2) & 0xFF00FF) + ((g1 + g2) & 0x00FF00) + (anew << 24);
}

bool stringStartsWithIgnoreCase(std::string c, std::string startsWith)
{
    if (c.size() < startsWith.size()) {
        return false;
    }
    auto otherString = c.substr(0, startsWith.size());
    std::transform(otherString.begin(), otherString.end(), otherString.begin(), ::tolower);
    std::transform(startsWith.begin(), startsWith.end(), startsWith.begin(), ::tolower);
    return otherString == startsWith;
}

u32 voidsnReadU32(IStream* f)
{
    u32 nvalBuffer;
    fread(&nvalBuffer, 4, 1, f);
    return nvalBuffer;
}

u64 voidsnReadU64(IStream* f)
{
    u64 nvalBuffer;
    fread(&nvalBuffer, 8, 1, f);
    return nvalBuffer;
}

std::string voidsnReadString(IStream* f)
{
    u32 nvalBuffer = voidsnReadU32(f);
    std::string ret;
    ret.resize(nvalBuffer);
    fread((void*)ret.data(), nvalBuffer, 1, f);
    return ret;
}

Layer* mergeLayerStack(std::vector<Layer*> layers) {
    if (layers.size() == 0) {
        return NULL;
    }
    else if (layers.size() == 1) {
        return layers.front();
    }
    int w = layers.front()->w;
    int h = layers.front()->h;
    Layer* ret = new Layer(w, h);
    ret->isPalettized = layers.front()->isPalettized;
    memset(ret->pixels32(), ret->isPalettized ? -1 : 0, w * h * 4);

    for (int i = 0; i < layers.size(); i++) {
        if (!layers[i]->hidden) {
            Layer* oldr = ret;
            ret = Layer::mergeLayers(ret, layers[i]);
            delete oldr;
            delete layers[i];
        }
    }
    return ret;
}

FILE* platformOpenFile(PlatformNativePathString path, PlatformNativePathString readMode) {
#ifdef _MSC_VER
    FILE* ff = NULL;
    _wfopen_s(&ff, path.c_str(), readMode.c_str());
    return ff;
#else
    return wfopen(path, readMode);
#endif
}


Layer* readVOIDSN(IStream* infile)
{
    if (infile != NULL) {
        fseek(infile, 0, SEEK_SET);
        uint8_t voidsnversion = 0;
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
                auto ret = mergeLayerStack(layers);
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
                    char* name = (char*)malloc(nameLen + 1);
                    memset(name, 0, nameLen + 1);
                    fread(name, nameLen, 1, infile);

                    Layer* newLayer = new Layer(dimensions.x, dimensions.y);
                    free(name);
                    fread(newLayer->pixels32(), newLayer->w * newLayer->h, 4, infile);
                    layers.push_back(newLayer);
                }
                auto ret = mergeLayerStack(layers);
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
                        //logerr("INVALID VOIDSN HEADER\n");
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
                    //logprintf("INVALID META HEADER\n");
                    fclose(infile);
                    return NULL;
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

                if (voidsnversion >= 7) {

                    int nframes = voidsnReadU32(infile);
                    //we only care about the first frame

                    int nFrameExtData = voidsnReadU32(infile);
                    std::map<std::string, std::string> frameExtData;
                    for (int x = 0; x < nFrameExtData; x++) {
                        std::string key = voidsnReadString(infile);
                        std::string val = voidsnReadString(infile);
                        frameExtData[key] = val;
                    }
                }

                int nlayers = voidsnReadU32(infile);

                std::vector<Layer*> layers;
                for (int x = 0; x < nlayers; x++) {

                    if (voidsnversion >= 6) {
                        int currentVariant = 0;
                        Layer* newLayer = new Layer(dimensions.x, dimensions.y);
                        newLayer->isPalettized = isPalettized;

                        std::vector<u32*> variantData;

                        u32 numLayerData = voidsnReadU32(infile);
                        for (u32 i = 0; i < numLayerData; i++) {
                            std::string dataName = voidsnReadString(infile);
                            u64 dataSize = voidsnReadU64(infile);
                            u8* data = (u8*)malloc(dataSize);
                            fread(data, dataSize, 1, infile);

                            if (dataName == "colorKey") {
                                newLayer->colorKey = *(u32*)data;
                            }
                            else if (dataName == "colorKeySet") {
                                newLayer->colorKeySet = data[0] == '\1';
                            }
                            else if (dataName == "currentVariant") {
                                currentVariant = *(u32*)data;
                            }
                            else if (dataName == "opacity") {
                                newLayer->layerAlpha = data[0];
                            }
                            else if (dataName == "hidden") {
                                newLayer->hidden = data[0] == '\1';
                            }
                            else if (stringStartsWithIgnoreCase(dataName, "variant.")) {
                                
                                u8* newpx = (u8*)malloc(dimensions.x * dimensions.y * 4);
                                u64 decompressedDataSize = dimensions.x * dimensions.y * 4;
                                uncompress(newpx, (uLongf*)&decompressedDataSize, data, dataSize);

                                variantData.push_back((u32*)newpx);
                            }
                            free(data);
                        }

                        bool valid = false;
                        if (currentVariant < variantData.size()) {
                            memcpy(newLayer->pixels32(), variantData[currentVariant], newLayer->w* newLayer->h * 4);
                            valid = true;
                        }
                        for (u32* d : variantData) {
                            free(d);
                        }

                        if (valid) {
                            layers.push_back(newLayer);
                        }
                        else {
                            delete newLayer;
                        }
                    }
                    else {
                        std::string layerName = voidsnReadString(infile);
                        Layer* newLayer = new Layer(dimensions.x, dimensions.y);
                        if (newLayer != NULL) {

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
                                uncompress((u8*)newLayer->pixels32(), (uLongf*)&dstLength, compressedData, compressedLength);
                                delete[] compressedData;
                            }
                            layers.push_back(newLayer);
                        }
                        else {
                            //logerr(frmt("Failed to allocate layer: {}", layerName));
                            //g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.mallocfail")));
                        }
                    }
                }
                /*if (isPalettized) {
                    std::vector<LayerPalettized*> indexedLayers;
                    //cast all layers to LayerPalettized*
                    std::transform(layers.begin(), layers.end(), std::back_inserter(indexedLayers), [](Layer* l) {
                        return (LayerPalettized*)l;
                    });
                    ret = new MainEditorPalettized(indexedLayers);
                }
                else {
                    ret = new MainEditor(layers);
                }*/
                
                /*
                if (extData.contains("edit.time")) { ret->editTime = std::stoull(extData["edit.time"]); }
                */
                if (extData.contains("layer.visibility")) {
                    std::string layerVisibilityData = extData["layer.visibility"];
                    for (int x = 0; x < nlayers && x < layerVisibilityData.size(); x++) {
                        layers[x]->hidden = layerVisibilityData[x] == '0';
                    }
                }
                if (extData.contains("layer.opacity")) {
                    std::string layerOpacityData = extData["layer.opacity"];
                    for (int x = 0; x < nlayers; x++) {
                        int nextSC = layerOpacityData.find_first_of(';');
                        layers[x]->layerAlpha = (uint8_t)std::stoi(layerOpacityData.substr(0, nextSC));
                        layerOpacityData = layerOpacityData.substr(nextSC + 1);
                    }
                }
                fclose(infile);
                Layer* ret = mergeLayerStack(layers);
                if (isPalettized) {
                    ret->applyPalette(palette);
                }
                return ret;
            }
            break;
            default:
                //logprintf("VOIDSN FILE v%i NOT SUPPORTED\n", voidsnversion);
                //g_addNotification(ErrorNotification(TL("vsp.cmn.error"), frmt("VOIDSN file v{} not supported", voidsnversion)));
                fclose(infile);
                return NULL;
        }

        fclose(infile);
    }
    return NULL;
}