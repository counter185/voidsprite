#include <fstream>
#include <zlib.h>

#include "io_base.h"
#include "io_rpgm.h"
#include "io_png.h"

#include "../RPG2KTilemapPreviewScreen.h"

#include "lcf/lmu/reader.h"
#include "lcf/ldb/reader.h"

Layer* readXYZ(PlatformNativePathString path, uint64_t seek)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);

    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        long filesize = ftell(f);
        fseek(f, 0, SEEK_SET);

        char header[4];
        fread(header, 1, 4, f);
        short imgW, imgH;
        fread(&imgW, 2, 1, f);
        fread(&imgH, 2, 1, f);

        long cDataSize = filesize - (4 + 2 + 2);
        Bytef* compressedData = (Bytef*)tracked_malloc(cDataSize);
        fread(compressedData, 1, cDataSize, f);

        uLongf decompressedSize = 768 + imgW * imgH;
        Bytef* decompBytes = (Bytef*)tracked_malloc(decompressedSize);
        int res = uncompress(decompBytes, &decompressedSize, compressedData, cDataSize);
        //hopefully res == Z_OK

        if (res != Z_OK) {
            g_addNotification(ErrorNotification("XYZ import failed", TL("vsp.cmn.error.decompressfail")));
            logprintf("[XYZ] uncompress failed\n");
            tracked_free(compressedData);
            tracked_free(decompBytes);
            fclose(f);
            return NULL;
        }

        LayerPalettized* nLayer = new LayerPalettized(imgW, imgH);

        //uint32_t colorPalette[256];
        int filePtr = 0;
        for (int c = 0; c < 256; c++) {
            //colorPalette[c] = 0xFF000000 | (decompBytes[filePtr++] << 16) | (decompBytes[filePtr++] << 8) | (decompBytes[filePtr++]);
            u8 r = decompBytes[filePtr++];
            u8 g = decompBytes[filePtr++];
            u8 b = decompBytes[filePtr++];
            nLayer->palette.push_back(PackRGBAtoARGB(r, g, b,255));
        }

        uint32_t* pxData = nLayer->pixels32();
        for (int x = 0; x < imgW * imgH; x++) {
            //pxData[x] = colorPalette[decompBytes[filePtr++]];
            pxData[x] = decompBytes[filePtr++];
        }

        nLayer->name = TL("vsp.layer.xyz");

        tracked_free(compressedData);
        tracked_free(decompBytes);

        fclose(f);
        return nLayer;
    }
    return NULL;
}


bool writeXYZ(PlatformNativePathString path, Layer* data)
{
    std::vector<uint32_t> uniqueColors;
    if (data->isPalettized) {
        if (((LayerPalettized*)data)->palette.size() > 256) {
            g_addNotification(ErrorNotification("XYZ export failed", "Too many colors in palette"));
            logprintf("[XYZ] Too many colors\n");
            return false;
        }
    }
    else {
        uniqueColors = data->getUniqueColors(true);
        if (uniqueColors.size() > 256) {
            g_addNotification(ErrorNotification("XYZ export failed", "Your image has more than 256 colors"));
            logprintf("[XYZ] Too many colors\n");
            return false;
        }
    }
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        fwrite("XYZ1", 4, 1, outfile);
        fwrite(&data->w, 2, 1, outfile);
        fwrite(&data->h, 2, 1, outfile);

        //write palette data
        uint8_t paletteData[256 * 3];
        int p = 0;
        if (data->isPalettized) {
            for (uint32_t& a : ((LayerPalettized*)data)->palette) {
                uint32_t color = a;
                paletteData[p++] = (color >> 16) & 0xff;
                paletteData[p++] = (color >> 8) & 0xff;
                paletteData[p++] = color & 0xff;
            }
        }
        else {
            for (uint32_t& a : uniqueColors) {
                uint32_t color = a;
                paletteData[p++] = (color >> 16) & 0xff;
                paletteData[p++] = (color >> 8) & 0xff;
                paletteData[p++] = color & 0xff;
            }
        }

        //write pixel data
        uint8_t* pxPalleteData = (uint8_t*)tracked_malloc(data->w * data->h);
        uint32_t* pixelData32 = data->pixels32();
        if (data->isPalettized) {
            for (uint64_t x = 0; x < data->w * data->h; x++) {
                pxPalleteData[x] = (uint8_t)(pixelData32[x]);
            }
        }
        else {
            for (uint64_t x = 0; x < data->w * data->h; x++) {
                uint32_t pixel = pixelData32[x] | 0xff000000;
                int index = std::find(uniqueColors.begin(), uniqueColors.end(), pixel) - uniqueColors.begin();
                pxPalleteData[x] = (uint8_t)(index);
            }
        }
        unsigned long dataLength = 256 * 3 + data->w * data->h;
        uint8_t* combined = (uint8_t*)tracked_malloc(dataLength);
        memcpy(combined, paletteData, 256 * 3);
        memcpy(combined + (256 * 3), pxPalleteData, data->w * data->h);
        uint8_t* dst = (uint8_t*)tracked_malloc(dataLength);
        compress((Bytef*)dst, &dataLength, combined, dataLength);
        fwrite(dst, dataLength, 1, outfile);

        tracked_free(combined);
        tracked_free(dst);
        tracked_free(pxPalleteData);
        fclose(outfile);
        return true;
    }

    return false;
}


MainEditor* readLMU(PlatformNativePathString path)
{
    MainEditor* ret = NULL;

    std::ifstream lmuFile(path, std::ios::binary);
    if (lmuFile.is_open()) {
        std::unique_ptr<lcf::rpg::Map> a = lcf::LMU_Reader::Load(lmuFile);
        int chipsetIndex = a.get()->chipset_id;
        PlatformNativePathString pathDir = path.substr(0, path.find_last_of(convertStringOnWin32("/\\")));
        PlatformNativePathString ldbPath = pathDir + convertStringOnWin32("/RPG_RT.ldb");

        if (std::filesystem::exists(ldbPath)) {
            std::ifstream ldbFile(ldbPath, std::ios::binary);
            if (ldbFile.is_open()) {
                std::unique_ptr<lcf::rpg::Database> db = lcf::LDB_Reader::Load(ldbFile);
                if (db.get()->chipsets.size() > chipsetIndex) {
                    //chipset_name is the file name
                    loginfo(frmt("chipset_name = {}", std::string(db.get()->chipsets[chipsetIndex - 1].chipset_name)));
                    loginfo(frmt("name = {}", std::string(db.get()->chipsets[chipsetIndex - 1].name)));

                    PlatformNativePathString chipsetPath = pathDir + convertStringOnWin32("/ChipSet/") + convertStringOnWin32(shiftJIStoUTF8(std::string(db.get()->chipsets[chipsetIndex - 1].chipset_name)));
                    Layer* l = NULL;
                    l = readXYZ(chipsetPath + convertStringOnWin32(".xyz"));
                    if (l == NULL) {
                        l = readPNG(chipsetPath + convertStringOnWin32(".png"));
                    }

                    if (l != NULL) {
                        l->name = shiftJIStoUTF8(std::string(db.get()->chipsets[chipsetIndex - 1].name));
                        ret = l->isPalettized ? new MainEditorPalettized((LayerPalettized*)l) : new MainEditor(l);

                        ret->tileDimensions = { 16,16 };

                        if (ret->isPalettized) {
                            //assume the first color is alpha because this seems to always be the case so far
                            ((MainEditorPalettized*)ret)->setPaletteIndex(0, ((MainEditorPalettized*)ret)->palette[0] & 0xFFFFFF);
                        }

                        RPG2KTilemapPreviewScreen* mapPreview = new RPG2KTilemapPreviewScreen(ret);
                        if (mapPreview->LoadLMU(path)) {
                            ret->hintOpenScreensInInteractiveMode.push_back(mapPreview);
                        }
                        else {
                            delete mapPreview;
                        }
                    }
                    else {
                        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to load Chipset"));
                    }
                }
                else {
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Chipset index not in database"));
                }
                ldbFile.close();
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to read LDB"));
            }
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "LDB not found"));
        }
        lmuFile.close();
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to read LMU"));
    }

    return ret;
}
