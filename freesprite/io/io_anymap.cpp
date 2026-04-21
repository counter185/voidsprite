
#include "io_base.h"
#include "io_anymap.h"

Layer* readAnymapPBM(PlatformNativePathString path, uint64_t seek)
{
    std::ifstream f(path, std::ios::binary);
    if (f.is_open()) {
        LayerPalettized* ret = NULL;
        std::vector<uint32_t> palette = { 0xFF000000, 0xFFFFFFFF };

        std::string line;
        std::getline(f, line);
        if (line == "P1") {
            //text pbm
            int w, h;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            f >> w >> h;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            ret = new LayerPalettized(w, h);
            ((LayerPalettized*)ret)->palette = palette;
            ret->name = "Anymap PBM layer";

            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    while (f.peek() == '#') {
                        std::getline(f, line);
                    }
                    char c;
                    f >> c;
                    ret->setPixel({ x,y }, c == '1' ? 0 : 1);
                }
            }
        }
        else if (line == "P4") {
            //binary pbm
            int w, h;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            f >> w >> h;
            std::getline(f, line);
            ret = new LayerPalettized(w, h);
            ((LayerPalettized*)ret)->palette = palette;
            ret->name = "Anymap PBM layer";
            uint64_t dataPointer = 0;
            while (!f.eof() && dataPointer < w * h) {
                uint8_t b;
                f.read((char*)&b, 1);
                for (int x = 0; x < 8; x++) {
                    ret->setPixel({ (int)(dataPointer % w), (int)(dataPointer / w) }, (b & (1 << (7 - x))) ? 0 : 1);
                    dataPointer++;
                    if (dataPointer >= w * h) {
                        break;
                    }
                }
            }
        }
        f.close();
        return ret;
    }
    return NULL;
}

Layer* readAnymapPGM(PlatformNativePathString path, uint64_t seek)
{
    std::ifstream f(path, std::ios::binary);
    if (f.is_open()) {
        LayerPalettized* ret = NULL;

        std::string line;
        std::getline(f, line);
        if (line == "P2") {
            //text pgm
            int w, h, cols;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            f >> w >> h;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            f >> cols;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            ret = new LayerPalettized(w, h);
            ((LayerPalettized*)ret)->palette = g_paletteByName(PALETTE_DEFAULT)->toRawColorList();
            ret->name = "Anymap PGM layer";

            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    while (f.peek() == '#') {
                        std::getline(f, line);
                    }
                    int c;
                    f >> c;
                    ret->setPixel({ x,y }, c);
                }
            }
        }
        else if (line == "P5") {
            //binary pgm
            int w, h, cols;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            f >> w >> h;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            f >> cols;
            std::getline(f, line);
            ret = new LayerPalettized(w, h);
            ((LayerPalettized*)ret)->palette = g_paletteByName(PALETTE_DEFAULT)->toRawColorList();
            ret->name = "Anymap PGM layer";
            uint64_t dataPointer = 0;
            while (!f.eof() && dataPointer < w * h) {
                uint8_t b;
                f.read((char*)&b, 1);
                ret->setPixel({ (int)(dataPointer % w), (int)(dataPointer / w) }, b);
                dataPointer++;
            }
        }
        f.close();
        return ret;
    }
    return NULL;
}

Layer* readAnymapPPM(PlatformNativePathString path, uint64_t seek)
{
    //TODO: CHANGE THIS TO USE FILE* AND FSCANF (text-based ppm loads very slowly and fscanf will be 2x faster)
    std::ifstream f(path, std::ios::binary);
    if (f.is_open()) {
        Layer* ret = NULL;

        std::string line;
        std::getline(f, line);
        if (line == "P3") {
            //text ppm
            int w, h, cols;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            f >> w >> h;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            f >> cols;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            ret = new Layer(w, h);
            ret->name = "Anymap PPM layer";

            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    u32 col = 0xFF000000;
                    for (int ch = 0; ch < 3; ch++) {
                        while (f.peek() == '#') {
                            std::getline(f, line);
                        }
                        int c;
                        f >> c;
                        col |= c << (16 - (ch * 8));
                    }
                    ret->setPixel({ x,y }, col);
                }
            }
        }
        else if (line == "P6") {
            //binary pgm
            int w, h, cols;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            f >> w >> h;
            while (f.peek() == '#') {
                std::getline(f, line);
            }
            f >> cols;
            std::getline(f, line);
            ret = new Layer(w, h);
            ret->name = "Anymap PPM layer";
            u32* ppx = ret->pixels32();
            u64 dataPointer = 0;
            while (!f.eof() && dataPointer < w * h) {
                u8 rgb[3];
                f.read((char*)rgb, 3);
                ppx[dataPointer++] = PackRGBAtoARGB(rgb[0], rgb[1], rgb[2], 255);
            }
        }
        f.close();
        return ret;
    }
    return NULL;
}

bool writeAnymapTextPBM(PlatformNativePathString path, Layer* data)
{
    auto uqColors = data->getUniqueColors();
    if (uqColors.size() > 2) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Too many colors. Anymap PBM requires 2."));
        return false;
    }
    std::sort(uqColors.begin(), uqColors.end());

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        fprintf(f, "P1\n");
        fprintf(f, "# File generated by voidsprite\n");
        fprintf(f, "%i %i\n", data->w, data->h);

        uint32_t* pxd = data->pixels32();
        for (int y = 0; y < data->h; y++) {
            for (int x = 0; x < data->w; x++) {
                uint32_t c = pxd[x + y * data->w];
                if (!data->isPalettized && (c & 0xFF000000) == 0) {
                    c = 0;
                }
                fprintf(f, "%ld ", 1 - (std::find(uqColors.begin(), uqColors.end(), c) - uqColors.begin()));
            }
            fprintf(f, "\n");
        }
        fclose(f);
        return true;
    }
    return false;
}

bool writeAnymapPGM(PlatformNativePathString path, Layer* data, OperationProgressReport* report, ParameterStore* params)
{
    bool binary = params == NULL || !params->hasParam("pgm.binary") ? false : params->getBool("pgm.binary");
    if (data->isPalettized) {
        if (((LayerPalettized*)data)->palette.size() > 256) {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Too many colors. Anymap PGM requires up to 256."));
            return false;
        }
    }

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        fprintf(f, binary ? "P5\n" : "P2\n");
        fprintf(f, "# File generated by voidsprite\n");
        fprintf(f, "%i %i\n", data->w, data->h);
        fprintf(f, "255\n");

        u32* pxd = data->pixels32();
        for (int y = 0; y < data->h; y++) {
            for (int x = 0; x < data->w; x++) {
                u32 c = pxd[x + y * data->w];
                if (!data->isPalettized) {
                    //slow as fuck but correct
                    u8 r = (c >> 16) & 0xff;
                    u8 g = (c >> 8) & 0xff;
                    u8 b = (c) & 0xff;
                    u8 grayScale = (r == g && g == b) ? r : (u8)(r * 0.2126 + g * 0.7152 + b * 0.0722);
                    if (binary) {
                        fwrite(&grayScale, 1, 1, f);
                    }
                    else {
                        fprintf(f, "%u ", grayScale);
                    }
                }
                else {
                    if (binary) {
                        fwrite(&c, 1, 1, f);
                    }
                    else {
                        fprintf(f, "%u ", c);
                    }

                }
            }
            if (!binary) {
                fprintf(f, "\n");
            }
        }
        fclose(f);
        return true;
    }
    return false;
}

bool writeAnymapTextPPM(PlatformNativePathString path, Layer* data)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        fprintf(f, "P3\n");
        fprintf(f, "# File generated by voidsprite\n");
        fprintf(f, "%i %i\n", data->w, data->h);
        fprintf(f, "255\n");

        u32* pxd = data->pixels32();
        for (int y = 0; y < data->h; y++) {
            for (int x = 0; x < data->w; x++) {
                u32 c = pxd[x + y * data->w];
                u8 r = (c >> 16) & 0xff;
                u8 g = (c >> 8) & 0xff;
                u8 b = (c) & 0xff;
                fprintf(f, "%u %u %u ", r, g, b);
            }
            fprintf(f, "\n");
        }
        fclose(f);
        return true;
    }
    return false;
}