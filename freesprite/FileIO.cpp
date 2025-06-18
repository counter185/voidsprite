
#if _WIN32
#include <windows.h>
#endif

#include <fstream>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "libtga/tga.h"
#include "ddspp/ddspp.h"
#include "easybmp/EasyBMP.h"
#include "zip/zip.h"
#include "pugixml/pugixml.hpp"
#include "astc_dec/astc_decomp.h"
#include "base64/base64.hpp"
#include "json/json.hpp"

#include "globals.h"
#include "Notification.h"
#include "EditorLayerPicker.h"
#include "splitsession.h"
#include "FileIO.h"
#include "maineditor.h"
#include "MainEditorPalettized.h"
#include "LayerPalettized.h"
#include "RPG2KTilemapPreviewScreen.h"
#include "PopupMessageBox.h"

using json = nlohmann::json;

enum VTFFORMAT
{
    IMAGE_FORMAT_NONE = -1,
    IMAGE_FORMAT_RGBA8888 = 0,
    IMAGE_FORMAT_ABGR8888 = 1,
    IMAGE_FORMAT_RGB888 = 2,
    IMAGE_FORMAT_BGR888 = 3,
    IMAGE_FORMAT_RGB565 = 4,
    IMAGE_FORMAT_I8 = 5,
    IMAGE_FORMAT_IA88 = 6,
    IMAGE_FORMAT_P8 = 7,
    IMAGE_FORMAT_A8 = 8,
    IMAGE_FORMAT_RGB888_BLUESCREEN = 9,
    IMAGE_FORMAT_BGR888_BLUESCREEN = 10,
    IMAGE_FORMAT_ARGB8888 = 11,
    IMAGE_FORMAT_BGRA8888 = 12,
    IMAGE_FORMAT_DXT1 = 13,
    IMAGE_FORMAT_DXT3 = 14,
    IMAGE_FORMAT_DXT5 = 15,
    IMAGE_FORMAT_BGRX8888,
    IMAGE_FORMAT_BGR565,
    IMAGE_FORMAT_BGRX5551,
    IMAGE_FORMAT_BGRA4444,
    IMAGE_FORMAT_DXT1_ONEBITALPHA,
    IMAGE_FORMAT_BGRA5551,
    IMAGE_FORMAT_UV88,
    IMAGE_FORMAT_UVWQ8888,
    IMAGE_FORMAT_RGBA16161616F,
    IMAGE_FORMAT_RGBA16161616,
    IMAGE_FORMAT_UVLX8888
};

std::string getAllLibsVersions() {
    std::string ret = "";
    ret += "SDL: " + std::to_string(SDL_MAJOR_VERSION) + "." + std::to_string(SDL_MINOR_VERSION) + "." +
           std::to_string(SDL_MICRO_VERSION) + "\n";
    ret += "SDL_image: " + std::to_string(SDL_IMAGE_MAJOR_VERSION) + "." + std::to_string(SDL_IMAGE_MINOR_VERSION) +
           "." + std::to_string(SDL_IMAGE_MICRO_VERSION) + "\n";
    ret += "SDL_ttf: " + std::to_string(SDL_TTF_MAJOR_VERSION) + "." + std::to_string(SDL_TTF_MINOR_VERSION) + "." +
           std::to_string(SDL_TTF_MICRO_VERSION) + "\n";

    ret += "json: " + std::to_string(NLOHMANN_JSON_VERSION_MAJOR) + "." + std::to_string(NLOHMANN_JSON_VERSION_MINOR) +
           "." + std::to_string(NLOHMANN_JSON_VERSION_PATCH) + "\n";
    ret += std::format("libpng: {}\n", getlibpngVersion());
    ret += "zlib: " ZLIB_VERSION "\n";
#if VOIDSPRITE_JXL_ENABLED
    ret += std::format("libjxl: {}\n", getlibjxlVersion());
#endif
    ret += "EasyBMP:" _EasyBMP_Version_String_ "\n";
    return ret;
}

std::map<std::string, std::string> parseINI(PlatformNativePathString path)
{
    std::ifstream infile(path);
    std::map<std::string, std::string> ret;
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == ';') continue;
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            if (value.size() >= 2 && value[0] == '\"' && value[value.size() - 1] == '\"') {
                value = value.substr(1, value.size() - 2);
            }
            ret[key] = value;
        }
    }
    return ret;
}

void detile(Layer* ret, XY tileSize) {

    Layer* t = new Layer(ret->w, ret->h);
    memcpy(t->pixelData, ret->pixelData, 4 * t->w * t->h);
    //t->blitTile(ret, { 0,1 }, { 1,0 }, tileSize);
    Detiler dt(2);
    for (int y = 0; y < ceil(ret->h / (double)tileSize.y); y++) {
        for (int x = 0; x < ceil(ret->w / (double)tileSize.x); x++) {
            XY tilePos = dt.next();
            t->blitTile(ret, { x * tileSize.x, y * tileSize.y }, tilePos, tileSize);
        }
    }
    
    memcpy(ret->pixelData, t->pixelData, 4 * t->w * t->h);
    delete t;
}

int DeASTC(Layer* ret, int width, int height, uint64_t fileLength, FILE* infile, int blockWidth, int blockHeight) {
    uint32_t* pxd = (uint32_t*)ret->pixelData;
    /*int skip = 232;
    int skipHowMany = 24;
    int skipCounter = 0;*/

    int astcErrors = 0;
    int blocksW = ceil((float)width / blockWidth);
    int blocksH = ceil((float)height / blockHeight);

    int x = 0;
    int y = 0;

    uint8_t astcData[16];

    while (y < blocksH && x < blocksW) {

        //for (int yyyyy = 0; yyyyy < 2; yyyyy++) {
            for (int xxxxx = 0; xxxxx < 2; xxxxx++) {

                //1x4 vertical strip
                for (int yyyy = 0; yyyy < 4; yyyy++) {

                    //2x2 deswizzle
                    for (int xx = 0; xx < 2; xx++) {
                        for (int yy = 0; yy < 2; yy++) {

                            fread(astcData, 1, 16, infile);
                            /*while (((uint64_t*)astcData)[0] == 0 && ((uint64_t*)astcData)[1] == 0 && ftell(infile) < fileLength) {
                                fread(astcData, 1, 16, infile);
                            }*/
                            uint8_t* rgbaData = (uint8_t*)tracked_malloc(4 * blockHeight * blockWidth);
                            bool success = basisu::astc::decompress(rgbaData, astcData, false, blockWidth, blockHeight);

                            if (!success) {
                                logprintf("ASTC decompression failed\n");
                                //astcErrors++;
                                //return;
                            }

                            int rgbaDataPointer = 0;

                            for (int yyy = 0; yyy < blockHeight; yyy++) {
                                for (int xxx = 0; xxx < blockWidth; xxx++) {
                                    //uint32_t colorNow = ((uint32_t*)rgbaData)[yy * blockWidth + xx];

                                    uint8_t r = rgbaData[rgbaDataPointer++];
                                    uint8_t g = rgbaData[rgbaDataPointer++];
                                    uint8_t b = rgbaData[rgbaDataPointer++];
                                    uint8_t a = rgbaData[rgbaDataPointer++];

                                    //if (y+yy > )
                                    ret->setPixel({ (x + xxxxx * 2 + xx) * blockWidth + xxx, (y + yyyy * 2 + yy) * blockHeight + yyy }, PackRGBAtoARGB(r, g, b, a));
                                    //ret->setPixel({ x + xx, y + yy }, PackRGBAtoARGB(rgbaData[rgbaDataPointer++], rgbaData[rgbaDataPointer++], rgbaData[rgbaDataPointer++], 255));
                                    //ret->setPixel({ x + xx,y + yy }, colorNow);
                                }
                            }
                            tracked_free(rgbaData);
                        }
                    }
                }
            }
        //}
        
        y += 8;
        if (y >= blocksH-8) {
            y = 0;
            x += 4;
        }

    }

    logprintf("[ASTC] at %lx / %lx\n", ftell(infile), fileLength); 
    return astcErrors;
}

LayerPalettized* De4BPPBitplane(int width, int height, uint8_t* input)
{
    LayerPalettized* ret = new LayerPalettized(width, height);
    uint8_t* colorTable = (uint8_t*)tracked_malloc(width * height);
    memset(colorTable, 0, width * height);
    uint64_t colorTablePointer = 0;
    uint8_t* inputPtr = input;

    // X 0 0 0 bitplane
    inputPtr = input;
    colorTablePointer = 0;
    for (uint64_t tileY = 0; tileY < height/8; tileY++) {
        for (uint64_t tileX = 0; tileX < width / 8; tileX++) {
            for (int posY = 0; posY < 8; posY++) {
                uint8_t byteBP0 = *inputPtr;
                uint8_t byteBP1 = *(inputPtr + 1);
                uint8_t byteBP2 = *(inputPtr + 16);
                uint8_t byteBP3 = *(inputPtr + 17);
                for (int posX = 0; posX < 8; posX++) {
                    uint8_t value0 = (((byteBP0 >> (7 - posX)) & 1) << 0);
                    uint8_t value1 = (((byteBP1 >> (7 - posX)) & 1) << 1);
                    uint8_t value2 = (((byteBP2 >> (7 - posX)) & 1) << 2);
                    uint8_t value3 = (((byteBP3 >> (7 - posX)) & 1) << 3);
                    XY pixelPosNow = {
                        tileX * 8 + posX,
                        tileY * 8 + posY
                    };
                    colorTable[pixelPosNow.x + pixelPosNow.y * width] = value0 | value1 | value2 | value3;
                }
                inputPtr += 2;
            }
            inputPtr += 16;
        }
    }

    uint32_t* pxd = (uint32_t*)ret->pixelData;
    for (uint64_t i = 0; i < width * height; i++) {
        //pxd[i] = PackRGBAtoARGB(colorTable[i], colorTable[i], colorTable[i], 255);
        pxd[i] = colorTable[i];
    }

    tracked_free(colorTable);
    return ret;
}

uint8_t* DecompressMarioPaintSRM(FILE* f)
{
    // my port of that one cursed lua script
    fseek(f, 0x7FE, SEEK_SET);
    uint8_t shortBuffer[2];
    fread(shortBuffer, 2, 1, f);
    uint16_t datasize = *(uint16_t*)shortBuffer;

    std::vector<uint16_t> maptable;
    for (int i = 0; i < 0x400; i++) {
        fread(shortBuffer, 2, 1, f);
        maptable.push_back(*(uint16_t*)shortBuffer);
    }
    std::vector<uint16_t> bin;
    for (int i = 0; i < datasize / 2 - 0x400; i++) {
        fread(shortBuffer, 2, 1, f);
        bin.push_back(*(uint16_t*)shortBuffer);
    }

    std::vector<uint16_t> lz;
    int huff_ptr = 0;
    for (int i = 0; i < bin.size(); i++) {
        uint16_t v = bin[i];
        for (int n = 15; n >= 0; n--) {
            huff_ptr = maptable[huff_ptr / 2 + (((v & (1 << n)) != 0) ? 1 : 0)];
            if (maptable[huff_ptr / 2] == 0) {
                lz.push_back(maptable[huff_ptr / 2 + 1]);
                huff_ptr = 0;
            }
        }
    }

    std::vector<uint8_t> out;
    int lz_ptr = 0;
    while ((lz_ptr+1) < lz.size()) {
        uint16_t lo = lz[lz_ptr++];
        uint16_t hi = lz[lz_ptr++];
        if (hi >= 0x80) {
            for (int i = 0; i < hi - 0x80; i++) {
                out.push_back(out[out.size() - lo]);
            }
        }
        else {
            for (int i = 0; i < hi * 0x100 + lo; i++) {
                out.push_back(lz[lz_ptr++]);
            }
        }
    }

    uint8_t* ret = (uint8_t*)tracked_malloc(out.size());
    memcpy(ret, out.data(), out.size());

    return ret;
}

void DeXT1(Layer* ret, int width, int height, FILE* infile)
{
    uint32_t* pxd = (uint32_t*)ret->pixelData;
    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 4) {
            // Extract color endpoints
            uint16_t color0;
            uint16_t color1;
            fread(&color0, 2, 1, infile);
            fread(&color1, 2, 1, infile);


            //https://github.com/Benjamin-Dobell/s3tc-dxt-decompression/blob/master/s3tc.cpp

            unsigned long temp = (color0 >> 11) * 255 + 16;
            unsigned char r0 = (unsigned char)((temp / 32 + temp) / 32);
            temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
            unsigned char g0 = (unsigned char)((temp / 64 + temp) / 64);
            temp = (color0 & 0x001F) * 255 + 16;
            unsigned char b0 = (unsigned char)((temp / 32 + temp) / 32);

            temp = (color1 >> 11) * 255 + 16;
            unsigned char r1 = (unsigned char)((temp / 32 + temp) / 32);
            temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
            unsigned char g1 = (unsigned char)((temp / 64 + temp) / 64);
            temp = (color1 & 0x001F) * 255 + 16;
            unsigned char b1 = (unsigned char)((temp / 32 + temp) / 32);

            uint8_t a = r1 == 0 && g1 == 0 && b1 == 0 ? 0 : 255;

            uint32_t code;
            fread(&code, 4, 1, infile);

            // Fill ARGB array
            for (int dy = 0; dy < 4; ++dy) {
                for (int dx = 0; dx < 4; ++dx) {
                    unsigned char positionCode = (code >> 2 * (4 * dy + dx)) & 0x03;
                    uint32_t color = 0;
                    if (color0 > color1)
                    {
                        switch (positionCode)
                        {
                        case 0:
                            color = PackRGBAtoARGB(r0, g0, b0, 255);
                            break;
                        case 1:
                            color = PackRGBAtoARGB(r1, g1, b1, 255);
                            break;
                        case 2:
                            color = PackRGBAtoARGB((2 * r0 + r1) / 3, (2 * g0 + g1) / 3, (2 * b0 + b1) / 3, 255);
                            break;
                        case 3:
                            color = PackRGBAtoARGB((r0 + 2 * r1) / 3, (g0 + 2 * g1) / 3, (b0 + 2 * b1) / 3, 255);
                            break;
                        }
                    }
                    else
                    {
                        switch (positionCode)
                        {
                        case 0:
                            color = PackRGBAtoARGB(r0, g0, b0, 255);
                            break;
                        case 1:
                            color = PackRGBAtoARGB(r1, g1, b1, 255);
                            break;
                        case 2:
                            color = PackRGBAtoARGB((r0 + r1) / 2, (g0 + g1) / 2, (b0 + b1) / 2, 255);
                            break;
                        case 3:
                            color = PackRGBAtoARGB(0, 0, 0, 0);
                            break;
                        }
                    }

                    ret->setPixel(XY{ x + dx, y + dy }, color);
                    //pxd[(y + dy) * desc.width + (x + dx)] = color;
                }
            }
        }
    }
}
void DeXT23(Layer* ret, int width, int height, FILE* infile) {
    uint32_t* pxd = (uint32_t*)ret->pixelData;
    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 4) {
            // Extract color endpoints

            //this may be wrong lmao
            uint8_t alphaData[16];
            uint16_t alphaPtr = 0;
            for (int z = 0; z < 4; z++) {
                uint16_t alphaByte;
                fread(&alphaByte, 2, 1, infile);
                for (int q = 0; q < 4; q++) {
                    //*0x11
                    alphaData[alphaPtr++] = (alphaByte & 0b1111) * 0x11;
                    alphaByte >>= 4;
                }
                //alphaPtr += 4;
            }
            alphaPtr = 0;
            //fread(alphaData, 2, 4, infile);

            uint16_t color0;
            uint16_t color1;
            fread(&color0, 2, 1, infile);
            fread(&color1, 2, 1, infile);


            //https://github.com/Benjamin-Dobell/s3tc-dxt-decompression/blob/master/s3tc.cpp

            unsigned long temp = (color0 >> 11) * 255 + 16;
            unsigned char r0 = (unsigned char)((temp / 32 + temp) / 32);
            temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
            unsigned char g0 = (unsigned char)((temp / 64 + temp) / 64);
            temp = (color0 & 0x001F) * 255 + 16;
            unsigned char b0 = (unsigned char)((temp / 32 + temp) / 32);

            temp = (color1 >> 11) * 255 + 16;
            unsigned char r1 = (unsigned char)((temp / 32 + temp) / 32);
            temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
            unsigned char g1 = (unsigned char)((temp / 64 + temp) / 64);
            temp = (color1 & 0x001F) * 255 + 16;
            unsigned char b1 = (unsigned char)((temp / 32 + temp) / 32);

            uint8_t a = r1 == 0 && g1 == 0 && b1 == 0 ? 0 : 255;

            // Decode 4-bit indices
            uint32_t code;
            fread(&code, 4, 1, infile);
            //Read4BitIndices(compressedData, indices);



            // Fill ARGB array
            for (int dy = 0; dy < 4; ++dy) {
                for (int dx = 0; dx < 4; ++dx) {
                    unsigned char positionCode = (code >> 2 * (4 * dy + dx)) & 0x03;
                    uint32_t color = 0;
                    if (color0 > color1)
                    {
                        switch (positionCode)
                        {
                        case 0: //0b00
                            color = PackRGBAtoARGB(r0, g0, b0, alphaData[alphaPtr++]);
                            break;
                        case 1: //0b01
                            color = PackRGBAtoARGB(r1, g1, b1, alphaData[alphaPtr++]);
                            break;
                        case 2: //0b10
                            color = PackRGBAtoARGB((2 * r0 + r1) / 3, (2 * g0 + g1) / 3, (2 * b0 + b1) / 3, alphaData[alphaPtr++]);
                            break;
                        case 3: //0b11
                            color = PackRGBAtoARGB((r0 + 2 * r1) / 3, (g0 + 2 * g1) / 3, (b0 + 2 * b1) / 3, alphaData[alphaPtr++]);
                            break;
                        }
                    }
                    else
                    {
                        switch (positionCode)
                        {
                        case 0:
                            color = PackRGBAtoARGB(r0, g0, b0, alphaData[alphaPtr++]);
                            break;
                        case 1:
                            color = PackRGBAtoARGB(r1, g1, b1, alphaData[alphaPtr++]);
                            break;
                        case 2:
                            color = PackRGBAtoARGB((r0 + r1) / 2, (g0 + g1) / 2, (b0 + b1) / 2, alphaData[alphaPtr++]);
                            break;
                        case 3:
                            color = PackRGBAtoARGB(0, 0, 0, alphaData[alphaPtr++]);
                            break;
                        }
                    }

                    ret->setPixel(XY{ x + dx, y + dy }, color);
                    //pxd[(y + dy) * desc.width + (x + dx)] = color;
                }
            }
        }
    }
}
void DeXT45(Layer* ret, int width, int height, FILE* infile) {
    uint32_t* pxd = (uint32_t*)ret->pixelData;
    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 4) {
            //gonna be real i have no idea what's going on here
            unsigned char alpha0;
            unsigned char alpha1;
            fread(&alpha0, 1, 1, infile);
            fread(&alpha1, 1, 1, infile);

            unsigned char bits[6];
            fread(bits, 1, 6, infile);
            unsigned long alphaCode1 = bits[2] | (bits[3] << 8) | (bits[4] << 16) | (bits[5] << 24);
            unsigned short alphaCode2 = bits[0] | (bits[1] << 8);

            unsigned short color0;
            unsigned short color1;
            fread(&color0, 2, 1, infile);
            fread(&color1, 2, 1, infile);

            unsigned long temp;

            temp = (color0 >> 11) * 255 + 16;
            unsigned char r0 = (unsigned char)((temp / 32 + temp) / 32);
            temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
            unsigned char g0 = (unsigned char)((temp / 64 + temp) / 64);
            temp = (color0 & 0x001F) * 255 + 16;
            unsigned char b0 = (unsigned char)((temp / 32 + temp) / 32);

            temp = (color1 >> 11) * 255 + 16;
            unsigned char r1 = (unsigned char)((temp / 32 + temp) / 32);
            temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
            unsigned char g1 = (unsigned char)((temp / 64 + temp) / 64);
            temp = (color1 & 0x001F) * 255 + 16;
            unsigned char b1 = (unsigned char)((temp / 32 + temp) / 32);

            unsigned long code;
            fread(&code, 4, 1, infile);

            for (int j = 0; j < 4; j++)
            {
                for (int i = 0; i < 4; i++)
                {
                    int alphaCodeIndex = 3 * (4 * j + i);
                    int alphaCode;

                    if (alphaCodeIndex <= 12)
                    {
                        alphaCode = (alphaCode2 >> alphaCodeIndex) & 0x07;
                    }
                    else if (alphaCodeIndex == 15)
                    {
                        alphaCode = (alphaCode2 >> 15) | ((alphaCode1 << 1) & 0x06);
                    }
                    else // alphaCodeIndex >= 18 && alphaCodeIndex <= 45
                    {
                        alphaCode = (alphaCode1 >> (alphaCodeIndex - 16)) & 0x07;
                    }

                    unsigned char finalAlpha;
                    if (alphaCode == 0)
                    {
                        finalAlpha = alpha0;
                    }
                    else if (alphaCode == 1)
                    {
                        finalAlpha = alpha1;
                    }
                    else
                    {
                        if (alpha0 > alpha1)
                        {
                            finalAlpha = ((8 - alphaCode) * alpha0 + (alphaCode - 1) * alpha1) / 7;
                        }
                        else
                        {
                            if (alphaCode == 6)
                                finalAlpha = 0;
                            else if (alphaCode == 7)
                                finalAlpha = 255;
                            else
                                finalAlpha = ((6 - alphaCode) * alpha0 + (alphaCode - 1) * alpha1) / 5;
                        }
                    }

                    unsigned char colorCode = (code >> 2 * (4 * j + i)) & 0x03;

                    uint32_t color;
                    switch (colorCode)
                    {
                    case 0:
                        color = PackRGBAtoARGB(r0, g0, b0, finalAlpha);
                        break;
                    case 1:
                        color = PackRGBAtoARGB(r1, g1, b1, finalAlpha);
                        break;
                    case 2:
                        color = PackRGBAtoARGB((2 * r0 + r1) / 3, (2 * g0 + g1) / 3, (2 * b0 + b1) / 3, finalAlpha);
                        break;
                    case 3:
                        color = PackRGBAtoARGB((r0 + 2 * r1) / 3, (g0 + 2 * g1) / 3, (b0 + 2 * b1) / 3, finalAlpha);
                        break;
                    }

                    if (x + i < width)
                        ret->setPixel(XY{x+i, y+j}, color);
                }
            }
        }
    }
}

u64 VTFgetImageSizeInBytesForFormat(int imageFormat, int w, int h) {
    return imageFormat == IMAGE_FORMAT_I8 ? w * h
        : imageFormat == IMAGE_FORMAT_A8 ? w * h
        : imageFormat == IMAGE_FORMAT_IA88 ? w * h * 2
        : imageFormat == IMAGE_FORMAT_RGB565 ? w * h * 2
        : imageFormat == IMAGE_FORMAT_BGR888 ? w * h * 3
        : imageFormat == IMAGE_FORMAT_RGB888 ? w * h * 3
        : imageFormat == IMAGE_FORMAT_BGRA8888 ? w * h * 4
        : imageFormat == IMAGE_FORMAT_RGBA8888 ? w * h * 4
        : imageFormat == IMAGE_FORMAT_ARGB8888 ? w * h * 4
        : imageFormat == IMAGE_FORMAT_ABGR8888 ? w * h * 4
        : imageFormat == IMAGE_FORMAT_RGBA16161616F ? w * h * 8
        : imageFormat == IMAGE_FORMAT_DXT1 ? (ixmax(w, 4) / 4) * (ixmax(4, h) / 4) * 8
        : imageFormat == IMAGE_FORMAT_DXT1_ONEBITALPHA ? (ixmax(w, 4) / 4) * (ixmax(4, h) / 4) * 8
        : imageFormat == IMAGE_FORMAT_DXT3 ? (ixmax(w, 4) / 4) * (ixmax(4, h) / 4) * 16
        : imageFormat == IMAGE_FORMAT_DXT5 ? (ixmax(w, 4) / 4) * (ixmax(4, h) / 4) * 16
        : 0;
}

Layer* _VTFseekToLargestMipmapAndRead(FILE* infile, int width, int height, int mipmapCount, int frames, int imageFormat)
{
    Layer* ret = NULL;
    int w = width ;
    int h = height ;
    for (int skipMMap = 0; skipMMap < mipmapCount-1; skipMMap++) {
        w = ixmax(1, w / 2);
        h = ixmax(1, h / 2);
        int seekBy = VTFgetImageSizeInBytesForFormat(imageFormat, w, h);
            
        fseek(infile, seekBy * frames, SEEK_CUR);
    }
    //fseek(infile, 16, SEEK_CUR);

    switch (imageFormat) {
    case IMAGE_FORMAT_A8:
        ret = new Layer(width, height);
        ret->name = "VTF A8 Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                u8 a;
                fread(&a, 1, 1, infile);
                pxp[dataP] = PackRGBAtoARGB(255,255,255,a);
            }
        }
        break;
    case IMAGE_FORMAT_I8:
        ret = new Layer(width, height);
        ret->name = "VTF I8 Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                u8 i;
                fread(&i, 1, 1, infile);
                pxp[dataP] = PackRGBAtoARGB(i,i,i,255);
            }
        }
        break;
    case IMAGE_FORMAT_IA88:
        ret = new Layer(width, height);
        ret->name = "VTF IA88 Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                u8 i;
                u8 a;
                fread(&i, 1, 1, infile);
                fread(&a, 1, 1, infile);
                pxp[dataP] = PackRGBAtoARGB(i,i,i,a);
            }
        }
        break;    
    case IMAGE_FORMAT_BGRA8888:
        ret = new Layer(width, height);
        ret->name = "VTF BGRA Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                fread(pxp + dataP, 4, 1, infile);
            }
        }
        break;
    case IMAGE_FORMAT_RGBA8888:
        ret = new Layer(width, height);
        ret->name = "VTF RGBA Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                uint8_t ch[4];
                fread(ch, 4, 1, infile);

                pxp[dataP] = PackRGBAtoARGB(ch[0], ch[1], ch[2], ch[3]);
            }
        }
        break;
    case IMAGE_FORMAT_ARGB8888:
        ret = new Layer(width, height);
        ret->name = "VTF ARGB Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                uint8_t ch[4];
                fread(ch, 4, 1, infile);

                pxp[dataP] = PackRGBAtoARGB(ch[3], ch[0], ch[1], ch[2]);
            }
        }
        break;
    case IMAGE_FORMAT_ABGR8888:
        ret = new Layer(width, height);
        ret->name = "VTF ABGR Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                uint8_t ch[4];
                fread(ch, 4, 1, infile);

                pxp[dataP] = PackRGBAtoARGB(ch[3], ch[2], ch[1], ch[0]);
            }
        }
        break;
    case IMAGE_FORMAT_RGB888:
        ret = new Layer(width, height);
        ret->name = "VTF RGB Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                u8 c[3];
                fread(c, 3, 1, infile);
                pxp[dataP] = PackRGBAtoARGB(c[0], c[1], c[2], 255);
            }
        }
        break;
    case IMAGE_FORMAT_RGB565:
        ret = new Layer(width, height);
        ret->name = "VTF RGB565 Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                u8 c[2];
                fread(c, 2, 1, infile);
                pxp[dataP] = BGR565toARGB8888(c[0] | (c[1] << 8));
            }
        }
        break;
    case IMAGE_FORMAT_BGR888:
        ret = new Layer(width, height);
        ret->name = "VTF BGR Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                fread(pxp + dataP, 3, 1, infile);
                pxp[dataP] |= 0xFF000000;
            }
        }
        break;
    case IMAGE_FORMAT_DXT1:
    case IMAGE_FORMAT_DXT1_ONEBITALPHA:
        ret = new Layer(width, height);
        ret->name = "VTF DXT1 Layer";
        DeXT1(ret, width, height, infile);
        break;
    case IMAGE_FORMAT_DXT3:
        ret = new Layer(width, height);
        ret->name = "VTF DXT3 Layer";
        DeXT23(ret, width, height, infile);
        break;
    case IMAGE_FORMAT_DXT5:
        ret = new Layer(width, height);
        ret->name = "VTF DXT5 Layer";
        DeXT45(ret, width, height, infile);
        break;
    case IMAGE_FORMAT_RGBA16161616F:
        ret = new Layer(width, height);
        //no idea how to make this work
        ret->name = "VTF RGBA16f Layer";
        {
            uint32_t* pxp = (uint32_t*)ret->pixelData;
            for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                u16 ch[4];
                fread(ch, 2, 4, infile);
                //cast these to 16 bit floats
                u8 r = (u8)(halfToFloat(ch[1]) * 255);
                u8 g = (u8)(halfToFloat(ch[2]) * 255);
                u8 b = (u8)(halfToFloat(ch[0]) * 255);
                u8 a = 255;//(u8)(halfToFloat(ch[0]) * 255);

                pxp[dataP] = PackRGBAtoARGB(r, g, b, a);
            }
        }
        break;
    default:
        logprintf("IMAGE FORMAT NOT IMPLEMENTED\n");
        break;
    }
    return ret;
}

std::vector<u8> decompressZlibWithoutUncompressedSize(u8* data, size_t dataSize)
{
    const u32 bufferSize = 65536;

    std::vector<u8> ret;
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = dataSize;
    strm.next_in = data;
    int ret2 = inflateInit(&strm);
    if (ret2 != Z_OK) {
        logprintf("inflateInit failed\n");
        return ret;
    }
    u8 out[bufferSize];
    u64 totalSize = 0;
    do {
        strm.avail_out = bufferSize;
        strm.next_out = out;
        ret2 = inflate(&strm, Z_NO_FLUSH);
        if (ret2 < 0) {
            logprintf("inflate error\n");
            break;
        }
        int nextDataSize = bufferSize - strm.avail_out;
        totalSize += nextDataSize;
        ret.resize(totalSize);
        memcpy(ret.data() + totalSize - nextDataSize, out, nextDataSize);
        //ret.insert(ret.end(), out, out + nextDataSize);
    } while (ret2 != Z_STREAM_END);
    inflateEnd(&strm);
    logprintf("total decompressed size: %lli\n", totalSize);
    return ret;
}

std::vector<u8> compressZlib(u8* data, size_t dataSize)
{
    uint64_t maxCompressedDataSize = compressBound(dataSize);
    uint64_t compressedDataSize = maxCompressedDataSize;
    std::vector<u8> compressedData;
    compressedData.resize(maxCompressedDataSize);
    int res = compress(compressedData.data(), (uLongf*)&compressedDataSize, data, dataSize);
    if (res != Z_OK) {
        logprintf("compress failed\n");
        return std::vector<u8>();
    }
    compressedData.resize(compressedDataSize);
    return compressedData;
}

std::vector<u8> base64ToBytes(std::string b64)
{
	std::string decoded = base64::from_base64(b64);
	std::vector<u8> ret;
	ret.resize(decoded.size());
	memcpy(ret.data(), decoded.data(), decoded.size());
	return ret;
}

void zlibFile(PlatformNativePathString path)
{
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile == NULL) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.fileloadfail")));
        return;
    }
    fseek(infile, 0, SEEK_END);
    uint64_t fileLength = ftell(infile);
    fseek(infile, 0, SEEK_SET);
    u8* fileBuffer = (u8*)tracked_malloc(fileLength);
    fread(fileBuffer, fileLength, 1, infile);
    fclose(infile);

    std::vector<u8> compressedData = compressZlib(fileBuffer, fileLength);

    FILE* outfile = platformOpenFile(path + convertStringOnWin32(".zlib"), PlatformFileModeWB);
    fwrite(compressedData.data(), compressedData.size(), 1, outfile);
    fclose(outfile);
    tracked_free(fileBuffer);
    if (compressedData.size() == 0) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to compress zlib file"));
    }
    else {
        g_addNotification(SuccessNotification("Success", "Zlib file compressed"));
    }
}

void unZlibFile(PlatformNativePathString path)
{
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile == NULL) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.fileloadfail")));
        return;
    }
    fseek(infile, 0, SEEK_END);
    uint64_t fileLength = ftell(infile);
    fseek(infile, 0, SEEK_SET);
    u8* fileBuffer = (u8*)tracked_malloc(fileLength);
    fread(fileBuffer, fileLength, 1, infile);
    fclose(infile);

    std::vector<u8> decompressedData = decompressZlibWithoutUncompressedSize(fileBuffer, fileLength);

    FILE* outfile = platformOpenFile(path + convertStringOnWin32(".unzlib"), PlatformFileModeWB);
    fwrite(decompressedData.data(), decompressedData.size(), 1, outfile);
    fclose(outfile);
    tracked_free(fileBuffer);
    if (decompressedData.size() == 0) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to decompress zlib file"));
    } else {
        g_addNotification(SuccessNotification("Success", "Zlib file decompressed"));
    }
}

json serializePixelStudioSession(MainEditor* data) {
    json o = json::object();
    o["Version"] = 2;
    o["Id"] = randomUUID();
    o["Name"] = "voidsprite Image";
    o["Width"] = data->canvas.dimensions.x;
    o["Height"] = data->canvas.dimensions.y;
    o["Type"] = 0;
    o["Background"] = true;
    o["BackgroundColor"] = { {"r", 0}, {"g", 0}, {"b", 0}, {"a", 0} };
    o["TileMode"] = false;
    o["TileFade"] = data->tileGridAlpha;
    o["ActiveClipIndex"] = 0;
    o["Clips"] = json::array();

    json clip = json::object();
    clip["Name"] = "Untitled";
    clip["LayerTypes"] = json::array();
    clip["ActiveFrameIndex"] = 0;
    clip["Frames"] = json::array();

    json frame = json::object();
    frame["Id"] = randomUUID();
    frame["Delay"] = 0.3;
    frame["ActiveLayerIndex"] = data->selLayer;
    frame["Layers"] = json::array();

    for (Layer*& l : data->layers) {
        json layer = json::object();
        layer["Id"] = randomUUID();
        layer["Transparency"] = l->layerAlpha / 255.0;
        layer["Hidden"] = l->hidden;
        layer["Linked"] = false;
        layer["Outline"] = 0;
        layer["Lock"] = 0;
        layer["Sx"] = 0;
        layer["Sy"] = 0;
        layer["Version"] = 1;

        json historyJson;
        historyJson["Actions"] = json::array();
        historyJson["Index"] = 0;

        std::string pixelDataPNGAsBase64 = "";
        std::vector<u8> pngData = writePNGToMem(l);
        std::string fileBuffer;
        fileBuffer.resize(pngData.size());
        memcpy(&fileBuffer[0], pngData.data(), pngData.size());
        pixelDataPNGAsBase64 = base64::to_base64(fileBuffer);
        historyJson["_source"] = pixelDataPNGAsBase64;

        layer["_historyJson"] = historyJson.dump();

        frame["Layers"].push_back(layer);
    }

    clip["Frames"].push_back(frame);

    o["Clips"].push_back(clip);
    return o;
}

MainEditor* deserializePixelStudioSession(json j)
{
    int pspversion = j["Version"].get<int>();
    logprintf("Version: %i\n", pspversion);

    if (pspversion != 2) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Unsupported Pixel Studio file version"));
        return NULL;
    }

    XY dimensions = { j["Width"].get<int>(), j["Height"].get<int>() };

    json clips = j["Clips"];
    json clip0 = clips[0];
    std::string name = clip0["Name"];
    logprintf("Clip Name: %s\n", name.c_str());
    int activeFrameIndex = clip0["ActiveFrameIndex"].get<int>();
    logprintf("Active Frame Index: %i\n", activeFrameIndex);

    bool showWarning = false;

    json frames = clip0["Frames"][0]["Layers"];
    std::vector<Layer*> layers;
    for (json& frame : frames) {
        std::string id = frame["Id"];
        bool hidden = frame["Hidden"];
        std::string history = frame["_historyJson"];

        json subJson = json::parse(history);
        std::string source = subJson["_source"];
        std::string base64ImageData = base64::from_base64(source);
        uint8_t* imageData = (uint8_t*)base64ImageData.c_str();
        Layer* nlayer = readPNGFromMem(imageData, base64ImageData.size());
        if (nlayer != NULL) {

            nlayer->name = "Pixel Studio Layer";
            //std::cout << subJson.dump(4) << std::endl;
            json actions = subJson["Actions"];
            int actionIndex = subJson["Index"];
            int nAction = 0;
            for (json& action : actions) {
                if (nAction++ >= actionIndex) {
                    break;
                }

                bool invalid = action["Invalid"];
                if (invalid) {
                    continue;
                }
                int tool = action["Tool"];

                std::string colorsB64 = base64::from_base64(std::string(action["Colors"]));
                std::vector<u32> colors;
                for (int c = 0; c < colorsB64.size() / 4; c++) {
                    u32 color = 0;
                    u8 r = colorsB64[c * 4];
                    u8 g = colorsB64[c * 4 + 1];
                    u8 b = colorsB64[c * 4 + 2];
                    u8 a = colorsB64[c * 4 + 3];
                    color = PackRGBAtoARGB(r, g, b, a);
                    colors.push_back(color);
                }

                std::string positionsB64 = base64::from_base64(std::string(action["Positions"]));
                std::vector<XY> positions;
                for (int p = 0; p < positionsB64.size() / 4; p++) {
                    u16 x = *(u16*)(positionsB64.c_str() + (p * 4));
                    u16 y = dimensions.y - 1 - *(u16*)(positionsB64.c_str() + (p * 4 + 2));
                    positions.push_back(XY{ x,y });
                }
#if _DEBUG
                loginfo(action.dump(4));
#endif
                switch (tool) {
                    //1px pencil
                case 0:
                {
                    json colorIndexes = action["ColorIndexes"];
                    int colIndex = 0;
                    for (XY& p : positions) {
                        nlayer->setPixel(p, colors[colorIndexes.size() > colIndex ? (int)colorIndexes[colIndex] : 0]);
                        colIndex++;
                    }
                }
                break;
                //color picker, has no values at all attached to it
                case 1:
                    break;
                    //eraser
                case 2:
                    //"eraser pen"
                case 19:
                    for (XY& p : positions) {
                        nlayer->setPixel(p, modAlpha(colors[0], 0));
                    }
                    break;
                    //paint bucket
                case 3:
                    for (XY& p : positions) {
                        nlayer->paintBucket(p, colors[0]);
                    }
                    break;
                    //erase selection
                case 6:
                {
                    std::string metaString = action["Meta"];
                    json meta = json::parse(metaString);
                    XY from = { meta["From"]["X"], dimensions.y - 1 - (int)meta["From"]["Y"] };
                    XY to = { meta["To"]["X"],  dimensions.y - 1 - (int)meta["To"]["Y"] };

                    nlayer->fillRect(from, to, 0x00000000);
                    /*for (XY& p : positions) {
                        nlayer->setPixel(p, 0x00000000);
                    }*/
                }
                    break;
                    //move
                case 10:
                {
                    std::string metaString = action["Meta"];
                    json meta = json::parse(metaString);
                    XY from = { meta["From"]["X"], dimensions.y - 1 - (int)meta["From"]["Y"] };
                    XY to = { meta["To"]["X"],  dimensions.y - 1 - (int)meta["To"]["Y"] };

                    SDL_Rect rect = { ixmin(from.x, to.x),ixmin(from.y, to.y),abs(from.x - to.x) + 1,abs(from.y - to.y) + 1};

                    XY blitAt = positions[1];
                    u32* pixelData = (u32*)tracked_malloc(rect.w * rect.h * 4);
                    for (int y = 0; y < rect.h; y++) {
                        for (int x = 0; x < rect.w; x++) {
                            pixelData[y * rect.w + x] = nlayer->getPixelAt({ rect.x + x, rect.y + y });
                        }
                    }

                    for (int y = 0; y < rect.h; y++) {
                        for (int x = 0; x < rect.w; x++) {
                            nlayer->setPixel({ rect.x + x, rect.y + y }, 0);
                        }
                    }

                    for (int y = 0; y < rect.h; y++) {
                        for (int x = 0; x < rect.w; x++) {
                            u32 srcColor = pixelData[y * rect.w + x];
                            if (srcColor >> 24 != 0) {
                                nlayer->setPixel({ blitAt.x + x, blitAt.y + y }, srcColor);
                            }
                        }
                    }
                    showWarning = true;
                    tracked_free(pixelData);
                }
                break;
                //flip x
                case 13:
                {
                    std::string metaString = action["Meta"];
                    json meta = json::parse(metaString);
                    XY from = { meta["From"]["X"], dimensions.y - 1 - (int)meta["From"]["Y"] };
                    XY to = { meta["To"]["X"],  dimensions.y - 1 - (int)meta["To"]["Y"] };

                    SDL_Rect rect = { ixmin(from.x, to.x),ixmin(from.y, to.y),abs(from.x - to.x) + 1,abs(from.y - to.y) + 1 };
                    nlayer->flipHorizontally(rect);
                }
                    break;
                //flip y
                case 14:
                {
                    std::string metaString = action["Meta"];
                    json meta = json::parse(metaString);
                    XY from = { meta["From"]["X"], dimensions.y - 1 - (int)meta["From"]["Y"] };
                    XY to = { meta["To"]["X"],  dimensions.y - 1 - (int)meta["To"]["Y"] };

                    SDL_Rect rect = { ixmin(from.x, to.x),ixmin(from.y, to.y),abs(from.x - to.x) + 1,abs(from.y - to.y) + 1 };
                    nlayer->flipVertically(rect);
                }
                    break;
                    //replace color
                case 18:
                    for (XY& p : positions) {
                        nlayer->replaceColor(nlayer->getPixelAt(p), colors[0]);
                    }
                    break;
                    //image paste
                case 20:
                {
                    std::string subsubJson = action["Meta"];
                    json subsubJsonJ = json::parse(subsubJson);
                    //std::cout << subsubJsonJ.dump(4) << std::endl;
                    std::string pixels = subsubJsonJ["Pixels"];
                    std::string pixelsb64 = base64::from_base64(pixels);
                    uint8_t* imageData = (uint8_t*)pixelsb64.c_str();
                    Layer* nnlayer = readPNGFromMem(imageData, pixelsb64.size());

                    XY rectFrom = { subsubJsonJ["Rect"]["From"]["X"], dimensions.y - 1 - (int)subsubJsonJ["Rect"]["From"]["Y"] };
                    XY rectTo = { subsubJsonJ["Rect"]["To"]["X"], dimensions.y - 1 - (int)subsubJsonJ["Rect"]["To"]["Y"] };

                    SDL_Rect dstRect = {
                        ixmin(rectFrom.x, rectTo.x),
                        ixmin(rectFrom.y, rectTo.y),
                        abs(rectFrom.x - rectTo.x),
                        abs(rectFrom.y - rectTo.y)
                    };

                    XY rectSourceFrom = { subsubJsonJ["RectSource"]["From"]["X"], subsubJsonJ["RectSource"]["From"]["Y"] };
                    XY rectSourceTo = { subsubJsonJ["RectSource"]["To"]["X"], subsubJsonJ["RectSource"]["To"]["Y"] };

                    SDL_Rect srcRect = {
                        ixmin(rectSourceFrom.x, rectSourceTo.x),
                        ixmin(rectSourceFrom.y, rectSourceTo.y),
                        abs(rectSourceFrom.x - rectSourceTo.x),
                        abs(rectSourceFrom.y - rectSourceTo.y)
                    };
                    if (srcRect.w == 0 || srcRect.h == 0) {
                        srcRect.w = nnlayer->w;
                        srcRect.h = nnlayer->h;
                    }

                    nlayer->blit(nnlayer, { dstRect.x, dstRect.y }, srcRect);
                    showWarning = true;
                    delete nnlayer;
                }
                break;
                //case 21:
                    //rotate selection
                    //break;
                case 24:
                    //adjust HSL,
                    //data is in `"Meta": "[-15660,0,0]",`
                    //hue: max is 32400
                    //saturation, min is -10000
                {
                    loginfo(action.dump(4));
                    std::string metaStr = action["Meta"];
                    json meta = json::parse(metaStr);
                    int hue = meta[0];
                    int saturation = meta[1];
                    int lightness = meta[2];
                    hsl shift = {
                        hue / 32400.0f * 180.0f,
                        saturation / 10000.0f,
                        lightness / 10000.0f
                    };
                    logprintf("hsl shift by  h:%lf s:%lf l:%lf\n", shift.h, shift.s, shift.l);
                    u32* px32 = (u32*)nlayer->pixelData;
                    for (u64 dataPtr = 0; dataPtr < nlayer->w * nlayer->h; dataPtr++) {
                        px32[dataPtr] = hslShiftPixelStudioCompat(px32[dataPtr], shift);
                    }
                    //nlayer->shiftLayerHSL(shift);

                }
                    break;
                default:
                    g_addNotification(ErrorNotification("PixelStudio Error", std::format("Tool {} not implemented", tool)));
                    logprintf("[pixel studio PSP] TOOL %i NOT IMPLEMENTED\n", tool);
                    logprintf("\trelevant position data:\n");
                    for (XY& p : positions) {
                        logprintf("\t%i, %i\n", p.x, p.y);
                    }
                    logprintf("\trelevant color data:\n");
                    for (u32& c : colors) {
                        logprintf("\t%x\n", c);
                    }
                    loginfo(action.dump(4));
                    showWarning = true;
                    break;
                }
            }

            layers.push_back(nlayer);
        }

        /*FILE* tempf = platformOpenFile(convertStringOnWin32(id + "temp.bin"), PlatformFileModeWB);
        fwrite(base64ImageData.c_str(), base64ImageData.size(), 1, tempf);
        fclose(tempf);*/
    }
    MainEditor* ret = new MainEditor(layers);
    if (showWarning) {
        PopupMessageBox* warningPopup = new PopupMessageBox("Warning",
            "This is a file in a Pixel Studio Pro format.\n"
            "This format requires the whole undo history to be reenacted,\nso importing may not work directly.\n"
            "If anything looks incorrect, we suggest you do the following: \n\n"
            "  1. Load the file with Pixel Studio Pro\n"
            "  2. Open the Functions menu [F key]\n"
            "  3. Click \"Resize canvas\"\n"
            "  4. Click \"Resize\" without changing any values.\n"
            "  5. Export this file again and load it with voidsprite.", { 700, 290 });
        g_addPopup(warningPopup);
    }
    return ret;
}

void _parseORAStacksRecursively(std::vector<Layer*>* layers, XY dimensions, pugi::xml_node rootNode, zip_t* zip, XY offset = {0,0}) {

    for (pugi::xml_node layerNode : rootNode.children()) {
        std::string nodeName = layerNode.name();
        if (nodeName == "stack"){
            _parseORAStacksRecursively(layers, dimensions, layerNode, zip, xyAdd(offset, XY{layerNode.attribute("x").as_int(), layerNode.attribute("y").as_int()}));
        } else if (nodeName == "layer") {
            XY layerOffset = xyAdd(offset, XY{layerNode.attribute("x").as_int(), layerNode.attribute("y").as_int()});
            const char* pngPath = layerNode.attribute("src").as_string();
            zip_entry_open(zip, pngPath);
            uint8_t* pngData = NULL;
            size_t pngSize;
            zip_entry_read(zip, (void**)&pngData, &pngSize);

            Layer* nlayer = readPNGFromMem(pngData, pngSize);
            if (nlayer != NULL) {
                Layer* sizeCorrectLayer = new Layer(dimensions.x, dimensions.y);
                sizeCorrectLayer->blit(nlayer, layerOffset);
                delete nlayer;

                sizeCorrectLayer->hidden = std::string(layerNode.attribute("visibility").as_string()) != "visible";
                sizeCorrectLayer->name = std::string(layerNode.attribute("name").as_string());
                layers->insert(layers->begin(), sizeCorrectLayer);
            } else {
                logprintf("NOOOOO LAYER IS NULL\n");
            }

            tracked_free(pngData);
            zip_entry_close(zip);
        }
    }
}

Layer* readTGA(PlatformNativePathString path, uint64_t seek) {

    return readSDLImage(path, seek);
    /*TGA* tga = TGAOpen(path.c_str(), "r");
    TGAData data;
    data.flags = TGA_IMAGE_DATA;
    TGAReadImage(tga, &data);
    TGAHeader* header = &tga->hdr;
    tbyte* img = data.img_data;

    Layer* nlayer = new Layer(header->width, header->height);
    uint32_t imagePointer = 0;
    uint32_t imgPointer = 0;
    for (int x = 0; x < header->width * header->height; x++) {
        nlayer->pixelData[imagePointer++] = img[imgPointer++];
        nlayer->pixelData[imagePointer++] = img[imgPointer++];
        nlayer->pixelData[imagePointer++] = img[imgPointer++];
        nlayer->pixelData[imagePointer++] = img[imgPointer++];
    }

    TGAClose(tga);*/
}

Layer* readBMP(PlatformNativePathString path, uint64_t seek)
{
    BMP nbmp;
    FILE* bmpf = platformOpenFile(path, PlatformFileModeRB);
    nbmp._ReadFromFile(bmpf);
    int w = nbmp.TellWidth();
    int h = nbmp.TellHeight();
    Layer* nlayer = new Layer(w, h);
    nlayer->name = "BMP Layer";
    unsigned long dataPtr = 0;
    uint32_t* pxData32 = (uint32_t*)nlayer->pixelData;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            RGBApixel px = nbmp.GetPixel(x,y);
            px.Alpha = 255; //TODO
            pxData32[dataPtr++] = (px.Alpha << 24) + (px.Red << 16) + (px.Green << 8) + px.Blue;
        }
    }
    return nlayer;
}

Layer* readAETEX(PlatformNativePathString path, uint64_t seek) {
    FILE* texfile = platformOpenFile(path, PlatformFileModeRB);
    if (texfile != NULL) {
        fseek(texfile, 0, SEEK_END);
        long filesize = ftell(texfile);

        fseek(texfile, 0, SEEK_SET);
        int magicNumber = 0;
        uint8_t a;
        for (int x = 0; x < 4; x++) {
            fread(&a, 1, 1, texfile);
            magicNumber += a;
        }
        if (magicNumber != 1) {
            fseek(texfile, 4, SEEK_SET);
            u8 zlibByte;
            fread(&zlibByte, 1, 1, texfile);
            if (zlibByte == 'x') {
                fseek(texfile, 4, SEEK_SET);
                u8* zlibData = (u8*)tracked_malloc(filesize - 4);
                fread(zlibData, filesize - 4, 1, texfile);
                auto decompressedData = decompressZlibWithoutUncompressedSize(zlibData, filesize - 4);
                tracked_free(zlibData);
                FILE* tempf = platformOpenFile(convertStringOnWin32("temp2.bin"), PlatformFileModeWB);
                fwrite(decompressedData.data(), decompressedData.size(), 1, tempf);
                fclose(tempf);
                auto ret = readAETEX(convertStringOnWin32("temp2.bin"), 0);
                std::filesystem::remove("temp2.bin");
                return ret;
            }
            else {
                g_addNotification(ErrorNotification("AETEX error", "Invalid magic number"));
                return NULL;
            }
        }
        else {
            //according to game decomps, magicNumber must == 1 or else it's considered the wrong format

            fseek(texfile, 4, SEEK_SET);
            uint8_t gameIDMaybe;
            fread(&gameIDMaybe, 1, 1, texfile);

            fseek(texfile, 8, SEEK_SET);
            uint8_t formatType;
            fread(&formatType, 1, 1, texfile);

            fseek(texfile, 0x2c, SEEK_SET);
            u32 r2dataStart;
            fread(&r2dataStart, 4, 1, texfile);

            u8 compressionMethod;
            fseek(texfile, 0x0C, SEEK_SET);
            fread(&compressionMethod, 1, 1, texfile);

            u16 imgW, imgH;
            fseek(texfile, 0x10, SEEK_SET);
            fread(&imgW, 2, 1, texfile);
            fread(&imgH, 2, 1, texfile);

            if (gameIDMaybe == 0x0a || gameIDMaybe == 0x0c) {
                //0x0a - runner2
                //0x0c - sportsfriends
                fseek(texfile, r2dataStart, SEEK_SET);
                if (gameIDMaybe == 0x0c) {
                    fread(&r2dataStart, 4, 1, texfile);
                    fseek(texfile, r2dataStart, SEEK_SET);
                }
                char ddsheader[4];
                fread(&ddsheader, 4, 1, texfile);
                logprintf("[AETEX] R2 texture, r2DataStart = %x\n", r2dataStart);

                if (ddsheader[0] == 'D' && ddsheader[1] == 'D' && ddsheader[2] == 'S') {
                    logprintf("[AETEX] DDS found\n");
                    fclose(texfile);
                    return readDDS(path, r2dataStart);
                }
                else {
                    fseek(texfile, r2dataStart, SEEK_SET);
                    Layer* ret = NULL;

                    if (compressionMethod == 0x81) {
                        uint8_t* rawData = (uint8_t*)tracked_malloc(filesize - r2dataStart);
                        fread(rawData, filesize - r2dataStart, 1, texfile);
                        SDL_IOStream* tgarw =
                            SDL_IOFromMem(rawData, filesize - r2dataStart);
                        SDL_Surface* tgasrf = IMG_LoadTGA_IO(tgarw);
                        SDL_CloseIO(tgarw);
                        if (tgasrf != NULL) {
                            ret = new Layer(tgasrf);
                            ret->name = "AETEX TGA Layer";
                        }
                        tracked_free(rawData);
                    }
                    else if (compressionMethod == 0x8B) {
                        ret = new Layer(imgW, imgH);
                        ret->name = "AETEX DXT1 Layer";
                        fseek(texfile, r2dataStart, SEEK_SET);
                        DeXT1(ret, imgW, imgH, texfile);
                    }
                    else if (compressionMethod == 0x8C) {
                        ret = new Layer(imgW, imgH);
                        ret->name = "AETEX DXT2/3 Layer";
                        fseek(texfile, r2dataStart, SEEK_SET);
                        DeXT23(ret, imgW, imgH, texfile);
                    }
                    else {
                        g_addNotification(ErrorNotification("AETEX error", std::format("Compression method {} unsupported", compressionMethod)));
                    }
                    fclose(texfile);

                    return ret;
                }
            }
            else {
                if (formatType == 0x80) {
                    logprintf("[AETEX] ASTC texture\n");
                    //fseek(texfile, 0x80, SEEK_SET); //replace this with u16 at 0x2C
                    //uint8_t* astcData = (uint8_t*)tracked_malloc(filesize - 0x80);
                    //fread(astcData, filesize - 0x80, 1, texfile);

                    uint16_t astcWidth, astcHeight;
                    fseek(texfile, 0x10, SEEK_SET);
                    fread(&astcWidth, 2, 1, texfile);
                    fread(&astcHeight, 2, 1, texfile);

                    /*for (int x = 2; x < 13; x++) {
                        for (int y = 2; y < 13; y++) {
                            Layer* nlayer = new Layer(astcWidth, astcHeight);
                            nlayer->name = "AETEX ASTC layer";

                            uint8_t* rgbaData = (uint8_t*)tracked_malloc(astcWidth * astcHeight * 4);

                            fseek(texfile, 0x80, SEEK_SET); //replace this with u16 at 0x2C
                            //bool success = basisu::astc::decompress(rgbaData, astcData, false, 12,12);
                            int errors = DeASTC(nlayer, astcWidth, astcHeight, texfile, x, y);
                            //logprintf("[AETEX] astc %s\n", success ? "success" : "failed");
                            //SDL_ConvertPixels(astcWidth, astcHeight, SDL_PIXELFORMAT_RGBA8888, rgbaData, astcWidth * 4, SDL_PIXELFORMAT_ARGB8888, nlayer->pixelData, astcWidth * 4);
                            tracked_free(rgbaData);
                            delete nlayer;

                            int totalBlocks = ceil(astcWidth / (float)x) * ceil(astcHeight / (float)y);
                            logprintf("%i : %i: %i / %i errors (%f%%)\n", x, y, errors, totalBlocks, (float)errors/totalBlocks * 100);
                        }
                    }

                    return NULL;*/

                    Layer* nlayer = new Layer(astcWidth, astcHeight);
                    nlayer->name = "AETEX ASTC layer";

                    uint8_t* rgbaData = (uint8_t*)tracked_malloc(astcWidth * astcHeight * 4);

                    fseek(texfile, 0x80, SEEK_SET); //replace this with u16 at 0x2C
                    //bool success = basisu::astc::decompress(rgbaData, astcData, false, 12,12);
                    DeASTC(nlayer, astcWidth, astcHeight, filesize, texfile);
                    //logprintf("[AETEX] astc %s\n", success ? "success" : "failed");
                    //SDL_ConvertPixels(astcWidth, astcHeight, SDL_PIXELFORMAT_RGBA8888, rgbaData, astcWidth * 4, SDL_PIXELFORMAT_ARGB8888, nlayer->pixelData, astcWidth * 4);
                    tracked_free(rgbaData);
                    //tracked_free(astcData);
                    return nlayer;
                }
                else {

                    fseek(texfile, 0x38, SEEK_SET);
                    uint8_t* tgaData = (uint8_t*)tracked_malloc(filesize - 0x38);
                    fread(tgaData, filesize - 0x38, 1, texfile);
                    fclose(texfile);
                    SDL_IOStream* tgarw = SDL_IOFromMem(tgaData, filesize - 0x38);
                    //todo: dds
                    SDL_Surface* tgasrf = IMG_LoadTGA_IO(tgarw);
                    SDL_CloseIO(tgarw);
                    tracked_free(tgaData);
                    return tgasrf == NULL ? NULL : new Layer(tgasrf);
                }
            }
        }
    }
    else {
        return NULL;
    }
}

Layer* readSDLImage(PlatformNativePathString path, uint64_t seek)
{
    std::string p = convertStringToUTF8OnWin32(path);
    SDL_Surface* img = IMG_Load(p.c_str());

    return img == NULL ? NULL : new Layer(img);
}

Layer* readWiiGCTPL(PlatformNativePathString path, uint64_t seek)
{
    std::vector<Layer*> layers;

    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        uint32_t version;
        uint32_t nImages;
        uint32_t imageTableOffset;

        fread(&version, 4,1, infile);
        fread(&nImages, 4,1, infile);
        fread(&imageTableOffset, 4,1, infile);
        nImages = BEtoLE32(nImages);
        imageTableOffset = BEtoLE32(imageTableOffset);
        logprintf("[TPL] %i image(s)\n", nImages);

        fseek(infile, imageTableOffset, SEEK_SET);

        std::vector<TPLImageOffset> images;
        std::vector<TPLImage> imageHdrs;
        for (int x = 0; x < nImages; x++) {
            images.push_back(TPLImageOffset());
            fread(&images[x].headerOffset, 4, 1, infile);
            images[x].headerOffset = BEtoLE32(images[x].headerOffset);
            fread(&images[x].paletteHeader, 4, 1, infile);
            images[x].paletteHeader = BEtoLE32(images[x].paletteHeader);
        }

        for (TPLImageOffset& i : images) {
            TPLImage nImg;
            fseek(infile, i.headerOffset, SEEK_SET);
            fread(&nImg.imgHdr, sizeof(TPLImageHeader), 1, infile);
            nImg.imgHdr.height = BEtoLE16(nImg.imgHdr.height);
            nImg.imgHdr.width = BEtoLE16(nImg.imgHdr.width);
            nImg.imgHdr.format = BEtoLE32(nImg.imgHdr.format);
            nImg.imgHdr.imageDataAddress = BEtoLE32(nImg.imgHdr.imageDataAddress);
            logprintf("image: %i x %i, format: %x, address: %x\n", nImg.imgHdr.height, nImg.imgHdr.width, nImg.imgHdr.format, nImg.imgHdr.imageDataAddress);
            if (i.paletteHeader != 0) {
                fseek(infile, i.paletteHeader, SEEK_SET);
                fread(&nImg.pltHdr, sizeof(TPLPaletteHeader), 1, infile);
            }
            nImg.pltHdr.set = i.paletteHeader != 0;

            fseek(infile, nImg.imgHdr.imageDataAddress, SEEK_SET);
            switch (nImg.imgHdr.format) {
                case 0x00:      //I4
                    {
                        Layer* newLayer = new Layer(nImg.imgHdr.width, nImg.imgHdr.height);
                        newLayer->name = "TPL I4 Layer";
                        //uint32_t* imgDataPtr = (uint32_t*)newLayer->pixelData;
                        int xBlocks = (int)ceil(newLayer->w / 8.0);
                        int yBlocks = (int)ceil(newLayer->h / 8.0);
                        for (int yb = 0; yb < yBlocks; yb++) {
                            for (int xb = 0; xb < xBlocks; xb++) {
                                for (int ybb = 0; ybb < 8; ybb++) {
                                    for (int xbb = 0; xbb < 4; xbb++) {
                                        u8 px;
                                        fread(&px, 1, 1, infile);
                                        u8 v1 = (px & 0b1111) * 0x11;
                                        u8 v2 = ((px & 0b11110000) >> 4) * 0x11;
                                        newLayer->setPixel(XY{ xb * 8 + xbb * 2, yb * 8 + ybb }, PackRGBAtoARGB(v2, v2, v2, 255));
                                        newLayer->setPixel(XY{ xb * 8 + xbb * 2 + 1, yb * 8 + ybb }, PackRGBAtoARGB(v1, v1, v1, 255));
                                    }
                                }
                            }
                        }
                        layers.push_back(newLayer);
                        //TODO: don't just break if there are multiple images
                        break;
                    }
                case 0x05:      //RGB5A3
                    {
                        Layer* newLayer = new Layer(nImg.imgHdr.width, nImg.imgHdr.height);
                        newLayer->name = "TPL RGB5A3 Layer";
                        //uint32_t* imgDataPtr = (uint32_t*)newLayer->pixelData;
                        int xBlocks = (int)ceil(newLayer->w / 4.0);
                        int yBlocks = (int)ceil(newLayer->h / 4.0);
                        for (int yb = 0; yb < yBlocks; yb++) {
                            for (int xb = 0; xb < xBlocks; xb++) {
                                for (int ybb = 0; ybb < 4; ybb++) {
                                    for (int xbb = 0; xbb < 4; xbb++) {
                                        u16 px;
                                        fread(&px, 2, 1, infile);
                                        px = BEtoLE16(px);
                                        newLayer->setPixel(XY{ xb * 4 + xbb, yb * 4 + ybb }, RGB5A3toARGB8888(px));
                                    }
                                }
                            }
                        }
                        layers.push_back(newLayer);
                        //TODO: don't just break if there are multiple images
                        break;
                    }
                    break;
                case 0x06:      //RGBA32
                    {
                        Layer* newLayer = new Layer(nImg.imgHdr.width, nImg.imgHdr.height);
                        newLayer->name = "TPL RGBA32 Layer";
                        int xBlocks = (int)ceil(newLayer->w / 4.0);
                        int yBlocks = (int)ceil(newLayer->h / 4.0);
                        for (int yb = 0; yb < yBlocks; yb++) {
                            for (int xb = 0; xb < xBlocks; xb++) {

                                u8 ablock[16];
                                u8 rblock[16];
                                u8 gblock[16];
                                u8 bblock[16];
                                for (int b = 0; b < 64; b++) {
                                    u8* lowTarget = b < 32 ? ablock : gblock;
                                    u8* highTarget = b < 32 ? rblock : bblock;
                                    int targetIndex = (b % 32) / 2;

                                    u8* whichTarget = (b % 2 == 0) ? lowTarget : highTarget;
                                    fread(whichTarget + targetIndex, 1, 1, infile);
                                }

                                for (int ybb = 0; ybb < 4; ybb++) {
                                    for (int xbb = 0; xbb < 4; xbb++) {
                                        int index = ybb * 4 + xbb;
                                        newLayer->setPixel(XY{ xb * 4 + xbb, yb * 4 + ybb }, 
                                            PackRGBAtoARGB(rblock[index], gblock[index], bblock[index], ablock[index]));
                                    }
                                }
                            }
                        }
                        layers.push_back(newLayer);
                        //TODO: don't just break if there are multiple images
                        break;
                    }
                    break;
                /*case 0x0e:      //DXT1
                    //todo: broken
                    {
                        Layer* newLayer = new Layer(nImg.imgHdr.width, nImg.imgHdr.height);
                        newLayer->name = "TPL CMPR Layer";
                        //int xBlocks = (int)ceil(newLayer->w / 4.0);
                        //int yBlocks = (int)ceil(newLayer->h / 4.0);
                        //u8* dxt1 = tracked_malloc(xBlocks * yBlocks * 4 * 4 / 2);
                        DeXT1(newLayer, newLayer->w, newLayer->h, infile);
                        //tracked_free(dxt1);
                        layers.push_back(newLayer);
                        break;
                    }*/
                default:
                    logprintf("unsupported format\n");
                    break;
            }

            imageHdrs.push_back(nImg);
        }

        fclose(infile);
    }
    return layers.size() != 0 ? layers[0] : NULL;
}

Layer* readNES(PlatformNativePathString path, uint64_t seek)
{
    struct NESHeader {
        uint8_t header[4];
        uint8_t prgRoms;
        uint8_t chrRoms;
        uint8_t flag6,flag7,flag8,flag9,flag10;
        uint8_t unusedPadding[5];
    };
    Layer* ret = NULL;
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        NESHeader header;
        fread(&header, sizeof(NESHeader), 1, infile);
        logprintf("Mapper: %i%i\n", header.flag7>>4, header.flag6 >> 4);
        bool trainerPresent = (header.flag6 >> 3) & 0b1;

        if (trainerPresent) {
            fseek(infile, 512, SEEK_CUR);
        }
        fseek(infile, 16384 * header.prgRoms, SEEK_CUR);
        uint8_t* chrRomData = (uint8_t*)tracked_malloc(8192 * (int)header.chrRoms);
        fread(chrRomData, 8192, header.chrRoms, infile);

        ret = new Layer(32 * 8, 16 * 8 * header.chrRoms);
        ret->name = "NES CHR-ROM Dump";
        int dataPointer = 0;

        for (int y = 0; y < 16 * header.chrRoms; y++) {
            for (int x = 0; x < 32; x++) {
                uint8_t part1[8];
                uint8_t part2[8];
                memcpy(part1, chrRomData + dataPointer, 8);
                memcpy(part2, chrRomData + dataPointer + 8, 8);
                for (int yy = 0; yy < 8; yy++) {
                    uint8_t p1p = part1[yy];
                    uint8_t p2p = part2[yy];
                    for (int xx = 0; xx < 8; xx++) {
                        uint8_t colorValue = (((p1p >> (7 - xx)) & 0b1) + (((p2p >> (7 - xx)) & 0b1) << 1)) * 0x55;
                        ret->setPixel(XY{ x * 8 + xx, y * 8 + yy }, 0xFF000000 + (colorValue<<16) + (colorValue<<8) + colorValue);
                    }
                }
                dataPointer += 0x10;
            }
        }

        //GET RID OF THIS LMAO
        /*FILE* f2 = platformOpenFile(L"temp.bin", L"wb");
        fwrite(chrRomData, 8192, header.chrRoms, f2);
        fclose(f2);*/

        tracked_free(chrRomData);
        fclose(infile);
    }
    return ret;
}

Layer* readDDS(PlatformNativePathString path, uint64_t seek)
{
    Layer* ret = NULL;
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        if (seek != 0) {
            fseek(infile, seek, SEEK_SET);
        }
        unsigned char header[0x80];
        fread(header, 0x80, 1, infile);
        ddspp::Descriptor desc;
        ddspp::Result decodeResult = ddspp::decode_header(header, desc);

        switch (desc.format) {
            case ddspp::BC1_UNORM:
            {
                ret = new Layer(desc.width, desc.height);
                ret->name = "DDS DXT1 Layer";
                DeXT1(ret, desc.width, desc.height, infile);
            }
                break;
            case ddspp::BC2_UNORM:
            {
                //todo holy fucking shit please clean this up
                ret = new Layer(desc.width, desc.height);
                ret->name = "DDS DXT2/3 Layer";
                DeXT23(ret, desc.width, desc.width, infile);
            }
                break;
            case ddspp::BC3_UNORM:
            {
                //todo holy fucking shit please clean this up
                ret = new Layer(desc.width, desc.height);
                ret->name = "DDS DXT4/5 Layer";
                DeXT45(ret, desc.width, desc.width, infile);
            }
                break;
            case ddspp::B8G8R8A8_UNORM:
            {
                ret = new Layer(desc.width, desc.height);
                ret->name = "DDS BGRA Layer";
                uint32_t* pxd = (uint32_t*)ret->pixelData;
                for (uint64_t d = 0; d < desc.width * desc.height; d++) {
                    uint32_t bgra;
                    fread(&bgra, 4, 1, infile);
                    //bgra = BEtoLE32(bgra);
                    pxd[d] = bgra;
                }
            }
                break;
            default:
                logprintf("format [%i] not supported\n", desc.format);
                break;
        }

        fclose(infile);
    }

    return ret;
}

Layer* readVTF(PlatformNativePathString path, uint64_t seek)
{
    Layer* ret = NULL;
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        
        VTFHEADER hdr;
        fread(hdr.signature, 1, 4, infile);
        fread(hdr.version, 4, 2, infile);
        fread(&hdr.headerSize, 4, 1, infile);
        int sizeofVTFHeader = sizeof(VTFHEADER);
        int hdrSize = ixmin(hdr.headerSize - 4 - 8 - 4, sizeof(VTFHEADER) - 4 - 8 - 4);
        fread(&hdr.width, 2, 1, infile);
        fread(&hdr.height, 2, 1, infile);
        fread(&hdr.flags, 4, 1, infile);
        fread(&hdr.frames, 2, 1, infile);
        fread(&hdr.firstFrame, 2, 1, infile);
        fread(hdr.padding0, 4, 1, infile);
        fread(hdr.reflectivity, 4, 3, infile);
        fread(hdr.padding1, 4, 1, infile);
        fread(&hdr.bumpmapScale, 4, 1, infile);
        fread(&hdr.highResImageFormat, 4, 1, infile);
        fread(&hdr.mipmapCount, 1, 1, infile);
        fread(&hdr.lowResImageFormat, 4, 1, infile);
        fread(&hdr.lowResImageWidth, 1, 1, infile);
        fread(&hdr.lowResImageHeight, 1, 1, infile);

        logprintf("[VTF] VERSION: %i.%i\n", hdr.version[0], hdr.version[1]);
        logprintf("[VTF] LowRes IMAGE FORMAT: %i   WxH: %i x %i\n", hdr.lowResImageFormat, hdr.lowResImageWidth, hdr.lowResImageHeight);
        logprintf("[VTF] HiRes IMAGE FORMAT: %i   WxH: %i x %i\n", hdr.highResImageFormat, hdr.width, hdr.height);
        logprintf("[VTF] Mipmaps: %i\n", hdr.mipmapCount);

        if (hdr.version[1] >= 2) {
            fread(&hdr.depth, 2, 1, infile);
        }
        if (hdr.version[1] >= 3) {
            fread(hdr.padding2, 1, 3, infile);
            fread(&hdr.numResources, 4, 1, infile);
            fread(hdr.padding3, 1, 8, infile);
            std::vector<VTF_RESOURCE_ENTRY> resources;
            for (int x = 0; x < hdr.numResources; x++) {
                VTF_RESOURCE_ENTRY vtfRes;
                fread(&vtfRes, sizeof(VTF_RESOURCE_ENTRY), 1, infile);
                logprintf("[VTF] Found resource: %i %i %i  offset: %x\n", vtfRes.tag[0], vtfRes.tag[1], vtfRes.tag[2], vtfRes.offset);
                resources.push_back(vtfRes);
            }
            logprintf("[VTF] numResources = %i\n", resources.size());

            for (VTF_RESOURCE_ENTRY& res : resources) {
                if (res.tag[0] == 0x01 && res.tag[1] == 0x00 && res.tag[2] == 0x00) {
                    //01 00 00 : lowres image data
                    /*
                    fseek(infile, res.offset, SEEK_SET);
                    switch (hdr.lowResImageFormat) {
                    case IMAGE_FORMAT_DXT1:
                        ret = new Layer(hdr.lowResImageWidth, hdr.lowResImageHeight);
                        DeXT1(ret, hdr.lowResImageWidth, hdr.lowResImageHeight, infile);
                        break;
                    default:
                        logprintf("IMAGE FORMAT NOT IMPLEMENTED\n");
                        break;
                    }*/
                }
                else if (res.tag[0] == 0x30 && res.tag[1] == 0x00 && res.tag[2] == 0x00) {
                    fseek(infile, res.offset, SEEK_SET);

                    ret = _VTFseekToLargestMipmapAndRead(infile, hdr.width, hdr.height, hdr.mipmapCount, hdr.frames, hdr.highResImageFormat);
                }
            }
        }
        else {
            //read low res image data by uncommenting below
            /*switch (hdr.lowResImageFormat) {
            case IMAGE_FORMAT_DXT1:
                ret = new Layer(hdr.lowResImageWidth, hdr.lowResImageHeight);
                DeXT1(ret, hdr.lowResImageWidth, hdr.lowResImageHeight, infile);
                break;
            default:
                logprintf("IMAGE FORMAT NOT IMPLEMENTED\n");
                break;
            }*/
            fseek(infile, hdr.headerSize, SEEK_SET);
            u64 lowResImageBytes = VTFgetImageSizeInBytesForFormat(hdr.lowResImageFormat, hdr.lowResImageWidth, hdr.lowResImageHeight);
            fseek(infile, lowResImageBytes, SEEK_CUR);

            ret = _VTFseekToLargestMipmapAndRead(infile, hdr.width, hdr.height, hdr.mipmapCount, hdr.frames, hdr.highResImageFormat);
        }
        //unfortunately freading straight into the struct makes it cut off at lowResImageFormat

        fclose(infile);
    }
    return ret;
}

Layer* readGCI(PlatformNativePathString path, uint64_t seek)
{
    enum GCIBIFlags {
        GCI_BANNER_NONE = 0b00,
        GCI_BANNER_CI8 = 0b01,
        GCI_BANNER_RGBA5A3 = 0b10,
        GCI_BANNER_11 = 0b11
    };
    enum GCIIconFormat {
        GCI_ICON_NONE = 0b00,
        GCI_ICON_CI8_SHARED = 0b01,
        GCI_ICON_RGB5A3 = 0b10,
        GCI_ICON_CI8_UNIQUE = 0b11
    };

    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        uint8_t biFlags;
        short iconFmt;
        int imageOffset;
        fseek(infile, 0x7, SEEK_SET);
        fread(&biFlags, 1, 1, infile);

        fseek(infile, 0x2c, SEEK_SET);
        fread(&imageOffset, 4, 1, infile);

        fseek(infile, 0x30, SEEK_SET);
        fread(&iconFmt, 2, 1, infile);

        if (biFlags == GCI_BANNER_CI8) {

        }
        else if (biFlags == GCI_BANNER_RGBA5A3) {
        }
        else {
            // ...
        }

        fclose(infile);
    }
    return NULL;
}

Layer* readMSP(PlatformNativePathString path, uint64_t seek)
{
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        fseek(infile, 0, SEEK_END);
        uint64_t fileLength = ftell(infile);
        fseek(infile, 0, SEEK_SET);

        MSPHeader hdr;
        //logprintf("%i\n", sizeof(MSPHeader));
        fread(&hdr, sizeof(MSPHeader), 1, infile);
        //fseek(infile, 1, SEEK_CUR);
        Layer* ret = new Layer(hdr.Width, hdr.Height);
        ret->name = "MSP1.0/2.0 Layer";
        uint32_t* pxd = (uint32_t*)ret->pixelData;
        uint64_t dataPointer = 0;
        while (dataPointer < hdr.Width*hdr.Height && ftell(infile) < fileLength) {
            uint8_t RunType;
            fread(&RunType, 1, 1, infile);
            if (RunType == 0){
                uint8_t RunCount;
                fread(&RunCount, 1, 1, infile);
                uint8_t RunValue;
                fread(&RunValue, 1, 1, infile);
                for (int x = 0; x < RunCount; x++) {
                    for (int bit = 0; bit < 8; bit++) {
                        pxd[dataPointer++] = 0xFF000000 | (0xFFFFFF * ((RunValue >> (7 - bit)) & 0b1));
                    }
                    //pxd[dataPointer++] = 0xFF000000 | (0x010101 * RunValue);
                }
            } else {
                for (int x = 0; x < RunType; x++) {
                    uint8_t RunValue;
                    fread(&RunValue, 1, 1, infile);
                    for (int bit = 0; bit < 8; bit++) {
                        pxd[dataPointer++] = 0xFF000000 | (0xFFFFFF * ((RunValue >> (7 - bit)) & 0b1));
                    }
                    //pxd[dataPointer++] = 0xFF000000 | (0x010101 * RunValue);
                }   
            }
        }

        fclose(infile);
        return ret;
    }
    return NULL;
}

Layer* readMarioPaintSRM(PlatformNativePathString path, uint64_t seek)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        try {
            //there is no way to differentiate an srm.....
            uint8_t* fullDecompressedData = DecompressMarioPaintSRM(f);

            LayerPalettized* l = De4BPPBitplane(256, 174, fullDecompressedData + 0x6000);
            l->name = "Mario Paint Layer";
            l->palette = g_palettes["Mario Paint"];

            tracked_free(fullDecompressedData);
            fclose(f);
            return l;
        }
        catch (std::exception&) {
            fclose(f);
            return NULL;
        }
    }
    return NULL;
}

Layer* readXComSPK(PlatformNativePathString path, uint64_t seek)
{
    //don't tell anyone
    uint32_t pal1[768] = { 0x00000000,0xFF000000,0xFF000000,0xFFFFA6D6,0xFFFFA6D6,0xFFFFA6D6,0xFFC6719C,0xFFC6719C,0xFFC6719C,0xFFFF8200,0xFFFF8200,0xFFFF8200,0xFFC65D00,0xFFC65D00,0xFFC65D00,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFC6C3C6,0xFFC6C3C6,0xFFC6C3C6,0xFF00FF00,0xFF00FF00,0xFF00FF00,0xFF00C300,0xFF00C300,0xFF00C300,0xFF00FFFF,0xFF00FFFF,0xFF00FFFF,0xFF00C3C6,0xFF00C3C6,0xFF00C3C6,0xFFFFFF00,0xFFFFFF00,0xFFFFFF00,0xFFC6C300,0xFFC6C300,0xFFC6C300,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFC60000,0xFFC60000,0xFFC60000,0xFF000000,0xFF000000,0xFF000000,0xFF008229,0xFF008229,0xFF008229,0xFF007929,0xFF007929,0xFF007929,0xFF007521,0xFF007521,0xFF007521,0xFF006D21,0xFF006D21,0xFF006D21,0xFF006921,0xFF006921,0xFF006921,0xFF006521,0xFF006521,0xFF006521,0xFF005D18,0xFF005D18,0xFF005D18,0xFF005918,0xFF005918,0xFF005918,0xFF005118,0xFF005118,0xFF005118,0xFF004D18,0xFF004D18,0xFF004D18,0xFF004910,0xFF004910,0xFF004910,0xFF004110,0xFF004110,0xFF004110,0xFF003C10,0xFF003C10,0xFF003C10,0xFF003410,0xFF003410,0xFF003410,0xFF003008,0xFF003008,0xFF003008,0xFF002C08,0xFF002C08,0xFF002C08,0xFF739E29,0xFF739E29,0xFF739E29,0xFF639621,0xFF639621,0xFF639621,0xFF5A8E21,0xFF5A8E21,0xFF5A8E21,0xFF4A8618,0xFF4A8618,0xFF4A8618,0xFF397D18,0xFF397D18,0xFF397D18,0xFF317510,0xFF317510,0xFF317510,0xFF296D10,0xFF296D10,0xFF296D10,0xFF186908,0xFF186908,0xFF186908,0xFF106108,0xFF106108,0xFF106108,0xFF085900,0xFF085900,0xFF085900,0xFF005100,0xFF005100,0xFF005100,0xFF004900,0xFF004900,0xFF004900,0xFF004108,0xFF004108,0xFF004108,0xFF003808,0xFF003808,0xFF003808,0xFF003008,0xFF003008,0xFF003008,0xFF002C08,0xFF002C08,0xFF002C08,0xFFADA600,0xFFADA600,0xFFADA600,0xFFA59A00,0xFFA59A00,0xFFA59A00,0xFF9C9200,0xFF9C9200,0xFF9C9200,0xFF948A00,0xFF948A00,0xFF948A00,0xFF8C7D00,0xFF8C7D00,0xFF8C7D00,0xFF7B7500,0xFF7B7500,0xFF7B7500,0xFF736D00,0xFF736D00,0xFF736D00,0xFF6B6500,0xFF6B6500,0xFF6B6500,0xFF635900,0xFF635900,0xFF635900,0xFF5A5100,0xFF5A5100,0xFF5A5100,0xFF524900,0xFF524900,0xFF524900,0xFF4A4100,0xFF4A4100,0xFF4A4100,0xFF393800,0xFF393800,0xFF393800,0xFF313000,0xFF313000,0xFF313000,0xFF292800,0xFF292800,0xFF292800,0xFF212000,0xFF212000,0xFF212000,0xFF94A631,0xFF94A631,0xFF94A631,0xFF8C9A29,0xFF8C9A29,0xFF8C9A29,0xFF849229,0xFF849229,0xFF849229,0xFF7B8A21,0xFF7B8A21,0xFF7B8A21,0xFF738221,0xFF738221,0xFF738221,0xFF637918,0xFF637918,0xFF637918,0xFF5A6D18,0xFF5A6D18,0xFF5A6D18,0xFF526518,0xFF526518,0xFF526518,0xFF4A5D10,0xFF4A5D10,0xFF4A5D10,0xFF425510,0xFF425510,0xFF425510,0xFF394D10,0xFF394D10,0xFF394D10,0xFF314108,0xFF314108,0xFF314108,0xFF293808,0xFF293808,0xFF293808,0xFF213008,0xFF213008,0xFF213008,0xFF212800,0xFF212800,0xFF212800,0xFF182000,0xFF182000,0xFF182000,0xFF7BF7DE,0xFF7BF7DE,0xFF7BF7DE,0xFF73E7DE,0xFF73E7DE,0xFF73E7DE,0xFF6BC7D6,0xFF6BC7D6,0xFF6BC7D6,0xFF5AA6C6,0xFF5AA6C6,0xFF5AA6C6,0xFF5286B5,0xFF5286B5,0xFF5286B5,0xFF4A69A5,0xFF4A69A5,0xFF4A69A5,0xFF425194,0xFF425194,0xFF425194,0xFF393884,0xFF393884,0xFF393884,0xFF393073,0xFF393073,0xFF393073,0xFF392863,0xFF392863,0xFF392863,0xFFDEFF00,0xFFDEFF00,0xFFDEFF00,0xFFA5D700,0xFFA5D700,0xFFA5D700,0xFF73AE00,0xFF73AE00,0xFF73AE00,0xFF4A8600,0xFF4A8600,0xFF4A8600,0xFF295D00,0xFF295D00,0xFF295D00,0xFF103800,0xFF103800,0xFF103800,0xFFCEBECE,0xFFCEBECE,0xFFCEBECE,0xFFBDAABD,0xFFBDAABD,0xFFBDAABD,0xFFAD9AAD,0xFFAD9AAD,0xFFAD9AAD,0xFF94869C,0xFF94869C,0xFF94869C,0xFF84758C,0xFF84758C,0xFF84758C,0xFF73657B,0xFF73657B,0xFF73657B,0xFF635563,0xFF635563,0xFF635563,0xFF524552,0xFF524552,0xFF524552,0xFF423442,0xFF423442,0xFF423442,0xFF312831,0xFF312831,0xFF312831,0xFF184973,0xFF184973,0xFF184973,0xFFDEFF00,0xFFDEFF00,0xFFDEFF00,0xFFBDEB00,0xFFBDEB00,0xFFBDEB00,0xFFA5D700,0xFFA5D700,0xFFA5D700,0xFF8CC700,0xFF8CC700,0xFF8CC700,0xFF73B200,0xFF73B200,0xFF73B200,0xFF63A200,0xFF63A200,0xFF63A200,0xFF5A9A00,0xFF5A9A00,0xFF5A9A00,0xFF529200,0xFF529200,0xFF529200,0xFF4A8A00,0xFF4A8A00,0xFF4A8A00,0xFF428600,0xFF428600,0xFF428600,0xFF427D00,0xFF427D00,0xFF427D00,0xFF397500,0xFF397500,0xFF397500,0xFF316D00,0xFF316D00,0xFF316D00,0xFF316900,0xFF316900,0xFF316900,0xFF296100,0xFF296100,0xFF296100,0xFF295900,0xFF295900,0xFF295900,0xFF215500,0xFF215500,0xFF215500,0xFF214D00,0xFF214D00,0xFF214D00,0xFF184500,0xFF184500,0xFF184500,0xFF183C00,0xFF183C00,0xFF183C00,0xFF103800,0xFF103800,0xFF103800,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFF63CFBD,0xFF63CFBD,0xFF63CFBD,0xFF39A694,0xFF39A694,0xFF39A694,0xFF188263,0xFF188263,0xFF188263,0xFF085942,0xFF085942,0xFF085942,0xFF003421,0xFF003421,0xFF003421,0xFFADBE52,0xFFADBE52,0xFFADBE52,0xFF849A39,0xFF849A39,0xFF849A39,0xFF637929,0xFF637929,0xFF637929,0xFF425518,0xFF425518,0xFF425518,0xFF213408,0xFF213408,0xFF213408,0xFFB59642,0xFFB59642,0xFFB59642,0xFFAD8E42,0xFFAD8E42,0xFFAD8E42,0xFFA58642,0xFFA58642,0xFFA58642,0xFF947D42,0xFF947D42,0xFF947D42,0xFF8C7542,0xFF8C7542,0xFF8C7542,0xFF846D42,0xFF846D42,0xFF846D42,0xFF7B6542,0xFF7B6542,0xFF7B6542,0xFF6B5D39,0xFF6B5D39,0xFF6B5D39,0xFF635539,0xFF635539,0xFF635539,0xFF5A4D31,0xFF5A4D31,0xFF5A4D31,0xFF524531,0xFF524531,0xFF524531,0xFF423C29,0xFF423C29,0xFF423C29,0xFF393429,0xFF393429,0xFF393429,0xFF312C21,0xFF312C21,0xFF312C21,0xFF292418,0xFF292418,0xFF292418,0xFF211C18,0xFF211C18,0xFF211C18,0xFF9CB2B5,0xFF9CB2B5,0xFF9CB2B5,0xFF94AAAD,0xFF94AAAD,0xFF94AAAD,0xFF8CA2A5,0xFF8CA2A5,0xFF8CA2A5,0xFF849694,0xFF849694,0xFF849694,0xFF7B8E8C,0xFF7B8E8C,0xFF7B8E8C,0xFF738284,0xFF738284,0xFF738284,0xFF6B797B,0xFF6B797B,0xFF6B797B,0xFF636D6B,0xFF636D6B,0xFF636D6B,0xFF526563,0xFF526563,0xFF526563,0xFF4A595A,0xFF4A595A,0xFF4A595A,0xFF425152,0xFF425152,0xFF425152,0xFF394542,0xFF394542,0xFF394542,0xFF313C39,0xFF313C39,0xFF313C39,0xFF293431,0xFF293431,0xFF293431,0xFF212829,0xFF212829,0xFF212829,0xFF182021,0xFF182021,0xFF182021,0xFF733084,0xFF733084,0xFF733084,0xFF6B2C7B,0xFF6B2C7B,0xFF6B2C7B,0xFF632873,0xFF632873,0xFF632873,0xFF5A246B,0xFF5A246B,0xFF5A246B,0xFF522063,0xFF522063,0xFF522063,0xFF4A1C52,0xFF4A1C52,0xFF4A1C52,0xFF42184A,0xFF42184A,0xFF42184A,0xFF391842,0xFF391842,0xFF391842,0xFF311439,0xFF311439,0xFF311439,0xFF291031,0xFF291031,0xFF291031,0xFF210C29,0xFF210C29,0xFF210C29,0xFF180821,0xFF180821,0xFF180821,0xFF100418,0xFF100418,0xFF100418,0xFF080408,0xFF080408,0xFF080408,0xFF000000,0xFF000000,0xFF000000,0xFF000000,0xFF000000,0xFF000000,0xFF002473,0xFF002473,0xFF002473,0xFF002073,0xFF002073,0xFF002073,0xFF002073,0xFF002073,0xFF002073,0xFF00206B,0xFF00206B,0xFF00206B,0xFF00206B,0xFF00206B,0xFF00206B,0xFF00206B,0xFF00206B,0xFF00206B,0xFF00206B,0xFF00206B,0xFF00206B,0xFF001C63,0xFF001C63,0xFF001C63,0xFF001C63,0xFF001C63,0xFF001C63,0xFF001C63,0xFF001C63,0xFF001C63,0xFF001C63,0xFF001C63,0xFF001C63,0xFF001C5A,0xFF001C5A,0xFF001C5A,0xFF001C5A,0xFF001C5A,0xFF001C5A,0xFF001C5A,0xFF001C5A,0xFF001C5A,0xFF00185A,0xFF00185A,0xFF00185A,0xFF001852,0xFF001852,0xFF001852,0xFF001852,0xFF001852,0xFF001852,0xFF001852,0xFF001852,0xFF001852,0xFF001852,0xFF001852,0xFF001852,0xFF00184A,0xFF00184A,0xFF00184A,0xFF00144A,0xFF00144A,0xFF00144A,0xFF00144A,0xFF00144A,0xFF00144A,0xFF00144A,0xFF00144A,0xFF00144A,0xFF001442,0xFF001442,0xFF001442,0xFF001442,0xFF001442,0xFF001442,0xFF001442,0xFF001442,0xFF001442,0xFF001042,0xFF001042,0xFF001042,0xFF001042,0xFF001042,0xFF001042,0xFF001039,0xFF001039,0xFF001039,0xFF001039,0xFF001039,0xFF001039,0xFF001039,0xFF001039,0xFF001039,0xFF001039,0xFF001039,0xFF001039,0xFF6B7D42,0xFF6B7D42,0xFF6B7D42,0xFF6B7542,0xFF6B7542,0xFF6B7542,0xFF636D39,0xFF636D39,0xFF636D39,0xFF636939,0xFF636939,0xFF636939,0xFF636131,0xFF636131,0xFF636131,0xFF5A5931,0xFF5A5931,0xFF5A5931,0xFF524D31,0xFF524D31,0xFF524D31,0xFF4A4529,0xFF4A4529,0xFF4A4529,0xFF4A3C29,0xFF4A3C29,0xFF4A3C29,0xFF423421,0xFF423421,0xFF423421,0xFF392C21,0xFF392C21,0xFF392C21,0xFF312818,0xFF312818,0xFF312818,0xFF292018,0xFF292018,0xFF292018,0xFF291818,0xFF291818,0xFF291818,0xFF211410,0xFF211410,0xFF211410,0xFF181010,0xFF181010,0xFF181010,0xFF42B26B,0xFF42B26B,0xFF42B26B,0xFF318E52,0xFF318E52,0xFF318E52,0xFF296D42,0xFF296D42,0xFF296D42,0xFF184D29,0xFF184D29,0xFF184D29,0xFF102C18,0xFF102C18,0xFF102C18,0xFF84BEE7,0xFF84BEE7,0xFF84BEE7,0xFF6BA2CE,0xFF6BA2CE,0xFF6BA2CE,0xFF5A8ABD,0xFF5A8ABD,0xFF5A8ABD,0xFF4271A5,0xFF4271A5,0xFF4271A5,0xFF315D8C,0xFF315D8C,0xFF315D8C,0xFF29457B,0xFF29457B,0xFF29457B,0xFF183463,0xFF183463,0xFF183463,0xFFFFFFA5,0xFFFFFFA5,0xFFFFFFA5,0xFFDEEB8C,0xFFDEEB8C,0xFFDEEB8C,0xFFC6D77B,0xFFC6D77B,0xFFC6D77B,0xFFA5C36B,0xFFA5C36B,0xFFA5C36B, };
    uint32_t pal2[768] = { 0x00000000,0xFF000000,0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFEFEBEF,0xFFEFEBEF,0xFFEFEBEF,0xFFDEDBDE,0xFFDEDBDE,0xFFDEDBDE,0xFFC6C7C6,0xFFC6C7C6,0xFFC6C7C6,0xFFB5B2B5,0xFFB5B2B5,0xFFB5B2B5,0xFFA5A2A5,0xFFA5A2A5,0xFFA5A2A5,0xFF8C8E8C,0xFF8C8E8C,0xFF8C8E8C,0xFF7B7D7B,0xFF7B7D7B,0xFF7B7D7B,0xFF6B696B,0xFF6B696B,0xFF6B696B,0xFF5A595A,0xFF5A595A,0xFF5A595A,0xFF424542,0xFF424542,0xFF424542,0xFF313431,0xFF313431,0xFF313431,0xFF212021,0xFF212021,0xFF212021,0xFF080C08,0xFF080C08,0xFF080C08,0xFF000000,0xFF000000,0xFF000000,0xFFFFD300,0xFFFFD300,0xFFFFD300,0xFFF7BA00,0xFFF7BA00,0xFFF7BA00,0xFFE7A200,0xFFE7A200,0xFFE7A200,0xFFDE8E00,0xFFDE8E00,0xFFDE8E00,0xFFCE7900,0xFFCE7900,0xFFCE7900,0xFFC66900,0xFFC66900,0xFFC66900,0xFFB55500,0xFFB55500,0xFFB55500,0xFFAD4900,0xFFAD4900,0xFFAD4900,0xFF9C3800,0xFF9C3800,0xFF9C3800,0xFF942C00,0xFF942C00,0xFF942C00,0xFF842000,0xFF842000,0xFF842000,0xFF7B1800,0xFF7B1800,0xFF7B1800,0xFF6B0C00,0xFF6B0C00,0xFF6B0C00,0xFF630400,0xFF630400,0xFF630400,0xFF520000,0xFF520000,0xFF520000,0xFF4A0000,0xFF4A0000,0xFF4A0000,0xFFFF797B,0xFFFF797B,0xFFFF797B,0xFFF7696B,0xFFF7696B,0xFFF7696B,0xFFE75D5A,0xFFE75D5A,0xFFE75D5A,0xFFDE5152,0xFFDE5152,0xFFDE5152,0xFFCE4542,0xFFCE4542,0xFFCE4542,0xFFC63C39,0xFFC63C39,0xFFC63C39,0xFFB53031,0xFFB53031,0xFFB53031,0xFFAD2829,0xFFAD2829,0xFFAD2829,0xFF9C2021,0xFF9C2021,0xFF9C2021,0xFF941818,0xFF941818,0xFF941818,0xFF841010,0xFF841010,0xFF841010,0xFF7B0C08,0xFF7B0C08,0xFF7B0C08,0xFF6B0400,0xFF6B0400,0xFF6B0400,0xFF630400,0xFF630400,0xFF630400,0xFF520000,0xFF520000,0xFF520000,0xFF4A0000,0xFF4A0000,0xFF4A0000,0xFFD6E763,0xFFD6E763,0xFFD6E763,0xFFBDD752,0xFFBDD752,0xFFBDD752,0xFFA5C74A,0xFFA5C74A,0xFFA5C74A,0xFF94BA39,0xFF94BA39,0xFF94BA39,0xFF7BAA31,0xFF7BAA31,0xFF7BAA31,0xFF6B9E29,0xFF6B9E29,0xFF6B9E29,0xFF5A8E21,0xFF5A8E21,0xFF5A8E21,0xFF4A7D18,0xFF4A7D18,0xFF4A7D18,0xFF397110,0xFF397110,0xFF397110,0xFF296108,0xFF296108,0xFF296108,0xFF185508,0xFF185508,0xFF185508,0xFF104500,0xFF104500,0xFF104500,0xFF083400,0xFF083400,0xFF083400,0xFF002800,0xFF002800,0xFF002800,0xFF001800,0xFF001800,0xFF001800,0xFF000C00,0xFF000C00,0xFF000C00,0xFFEFEFFF,0xFFEFEFFF,0xFFEFEFFF,0xFFDEDBEF,0xFFDEDBEF,0xFFDEDBEF,0xFFCECBDE,0xFFCECBDE,0xFFCECBDE,0xFFBDBAD6,0xFFBDBAD6,0xFFBDBAD6,0xFFADAAC6,0xFFADAAC6,0xFFADAAC6,0xFF9C9ABD,0xFF9C9ABD,0xFF9C9ABD,0xFF948EAD,0xFF948EAD,0xFF948EAD,0xFF847DA5,0xFF847DA5,0xFF847DA5,0xFF7B7194,0xFF7B7194,0xFF7B7194,0xFF6B6584,0xFF6B6584,0xFF6B6584,0xFF63597B,0xFF63597B,0xFF63597B,0xFF524D6B,0xFF524D6B,0xFF524D6B,0xFF4A4163,0xFF4A4163,0xFF4A4163,0xFF423452,0xFF423452,0xFF423452,0xFF312C4A,0xFF312C4A,0xFF312C4A,0xFF292439,0xFF292439,0xFF292439,0xFFFFFBFF,0xFFFFFBFF,0xFFFFFBFF,0xFFEFE7E7,0xFFEFE7E7,0xFFEFE7E7,0xFFDED3D6,0xFFDED3D6,0xFFDED3D6,0xFFCEC3BD,0xFFCEC3BD,0xFFCEC3BD,0xFFC6B2AD,0xFFC6B2AD,0xFFC6B2AD,0xFFB59E9C,0xFFB59E9C,0xFFB59E9C,0xFFA58E8C,0xFFA58E8C,0xFFA58E8C,0xFF9C827B,0xFF9C827B,0xFF9C827B,0xFF8C716B,0xFF8C716B,0xFF8C716B,0xFF7B655A,0xFF7B655A,0xFF7B655A,0xFF735552,0xFF735552,0xFF735552,0xFF634942,0xFF634942,0xFF634942,0xFF523C31,0xFF523C31,0xFF523C31,0xFF423029,0xFF423029,0xFF423029,0xFF392421,0xFF392421,0xFF392421,0xFF291C18,0xFF291C18,0xFF291C18,0xFFF7C74A,0xFFF7C74A,0xFFF7C74A,0xFFE7B242,0xFFE7B242,0xFFE7B242,0xFFD69E31,0xFFD69E31,0xFFD69E31,0xFFCE8A31,0xFFCE8A31,0xFFCE8A31,0xFFBD7929,0xFFBD7929,0xFFBD7929,0xFFB56921,0xFFB56921,0xFFB56921,0xFFA55918,0xFFA55918,0xFFA55918,0xFF9C4910,0xFF9C4910,0xFF9C4910,0xFF8C3C10,0xFF8C3C10,0xFF8C3C10,0xFF843008,0xFF843008,0xFF843008,0xFF732008,0xFF732008,0xFF732008,0xFF6B1800,0xFF6B1800,0xFF6B1800,0xFF5A1000,0xFF5A1000,0xFF5A1000,0xFF520800,0xFF520800,0xFF520800,0xFF420000,0xFF420000,0xFF420000,0xFF390000,0xFF390000,0xFF390000,0xFFCEBAAD,0xFFCEBAAD,0xFFCEBAAD,0xFFBDAE9C,0xFFBDAE9C,0xFFBDAE9C,0xFFB5A294,0xFFB5A294,0xFFB5A294,0xFFAD9684,0xFFAD9684,0xFFAD9684,0xFFA58E7B,0xFFA58E7B,0xFFA58E7B,0xFF9C826B,0xFF9C826B,0xFF9C826B,0xFF846152,0xFF846152,0xFF846152,0xFF7B4D42,0xFF7B4D42,0xFF7B4D42,0xFF733C39,0xFF733C39,0xFF733C39,0xFF6B3031,0xFF6B3031,0xFF6B3031,0xFF632831,0xFF632831,0xFF632831,0xFF5A2031,0xFF5A2031,0xFF5A2031,0xFF521831,0xFF521831,0xFF521831,0xFF4A1031,0xFF4A1031,0xFF4A1031,0xFF420C31,0xFF420C31,0xFF420C31,0xFF390831,0xFF390831,0xFF390831,0xFFADD3F7,0xFFADD3F7,0xFFADD3F7,0xFF94C3E7,0xFF94C3E7,0xFF94C3E7,0xFF84B2DE,0xFF84B2DE,0xFF84B2DE,0xFF73A2D6,0xFF73A2D6,0xFF73A2D6,0xFF6396CE,0xFF6396CE,0xFF6396CE,0xFF5A86C6,0xFF5A86C6,0xFF5A86C6,0xFF4A75B5,0xFF4A75B5,0xFF4A75B5,0xFF3969AD,0xFF3969AD,0xFF3969AD,0xFF315DA5,0xFF315DA5,0xFF315DA5,0xFF294D9C,0xFF294D9C,0xFF294D9C,0xFF184194,0xFF184194,0xFF184194,0xFF103484,0xFF103484,0xFF103484,0xFF082C7B,0xFF082C7B,0xFF082C7B,0xFF002073,0xFF002073,0xFF002073,0xFF00186B,0xFF00186B,0xFF00186B,0xFF001063,0xFF001063,0xFF001063,0xFFFFFF7B,0xFFFFFF7B,0xFFFFFF7B,0xFFF7EB6B,0xFFF7EB6B,0xFFF7EB6B,0xFFE7DB5A,0xFFE7DB5A,0xFFE7DB5A,0xFFDEC752,0xFFDEC752,0xFFDEC752,0xFFCEB642,0xFFCEB642,0xFFCEB642,0xFFC6A239,0xFFC6A239,0xFFC6A239,0xFFBD9231,0xFFBD9231,0xFFBD9231,0xFFAD8229,0xFFAD8229,0xFFAD8229,0xFFA57121,0xFFA57121,0xFFA57121,0xFF946118,0xFF946118,0xFF946118,0xFF8C5110,0xFF8C5110,0xFF8C5110,0xFF844108,0xFF844108,0xFF844108,0xFF733408,0xFF733408,0xFF733408,0xFF6B2800,0xFF6B2800,0xFF6B2800,0xFF5A1C00,0xFF5A1C00,0xFF5A1C00,0xFF521400,0xFF521400,0xFF521400,0xFFBDA25A,0xFFBDA25A,0xFFBDA25A,0xFFAD924A,0xFFAD924A,0xFFAD924A,0xFFA58242,0xFFA58242,0xFFA58242,0xFF947139,0xFF947139,0xFF947139,0xFF8C6129,0xFF8C6129,0xFF8C6129,0xFF7B5121,0xFF7B5121,0xFF7B5121,0xFF734518,0xFF734518,0xFF734518,0xFF6B3818,0xFF6B3818,0xFF6B3818,0xFF5A2C10,0xFF5A2C10,0xFF5A2C10,0xFF522008,0xFF522008,0xFF522008,0xFF421408,0xFF421408,0xFF421408,0xFF390C00,0xFF390C00,0xFF390C00,0xFF290800,0xFF290800,0xFF290800,0xFF210400,0xFF210400,0xFF210400,0xFF100000,0xFF100000,0xFF100000,0xFF080000,0xFF080000,0xFF080000,0xFFFFDFD6,0xFFFFDFD6,0xFFFFDFD6,0xFFEFCBC6,0xFFEFCBC6,0xFFEFCBC6,0xFFE7B6B5,0xFFE7B6B5,0xFFE7B6B5,0xFFD6A2A5,0xFFD6A2A5,0xFFD6A2A5,0xFFCE9294,0xFFCE9294,0xFFCE9294,0xFFBD828C,0xFFBD828C,0xFFBD828C,0xFFB5757B,0xFFB5757B,0xFFB5757B,0xFFA56973,0xFFA56973,0xFFA56973,0xFF9C596B,0xFF9C596B,0xFF9C596B,0xFF8C4963,0xFF8C4963,0xFF8C4963,0xFF7B3C5A,0xFF7B3C5A,0xFF7B3C5A,0xFF733052,0xFF733052,0xFF733052,0xFF63244A,0xFF63244A,0xFF63244A,0xFF5A1C42,0xFF5A1C42,0xFF5A1C42,0xFF4A1039,0xFF4A1039,0xFF4A1039,0xFF310029,0xFF310029,0xFF310029,0xFF63415A,0xFF63415A,0xFF63415A,0xFF633C5A,0xFF633C5A,0xFF633C5A,0xFF5A385A,0xFF5A385A,0xFF5A385A,0xFF523452,0xFF523452,0xFF523452,0xFF523452,0xFF523452,0xFF523452,0xFF4A304A,0xFF4A304A,0xFF4A304A,0xFF422C4A,0xFF422C4A,0xFF422C4A,0xFF392842,0xFF392842,0xFF392842,0xFF312842,0xFF312842,0xFF312842,0xFF312439,0xFF312439,0xFF312439,0xFF292039,0xFF292039,0xFF292039,0xFF211C31,0xFF211C31,0xFF211C31,0xFF211C31,0xFF211C31,0xFF211C31,0xFF181829,0xFF181829,0xFF181829,0xFF181429,0xFF181429,0xFF181429,0xFF101421,0xFF101421,0xFF101421,0xFFFFDFD6,0xFFFFDFD6,0xFFFFDFD6,0xFFEFEBEF,0xFFEFEBEF,0xFFEFEBEF,0xFFBDBABD,0xFFBDBABD,0xFFBDBABD,0xFF8C8A8C,0xFF8C8A8C,0xFF8C8A8C,0xFF5A595A,0xFF5A595A,0xFF5A595A,0xFF292C29,0xFF292C29,0xFF292C29,0xFFDEDB84,0xFFDEDB84,0xFFDEDB84,0xFFB5AA63,0xFFB5AA63,0xFFB5AA63,0xFF8C794A,0xFF8C794A,0xFF8C794A,0xFF635131,0xFF635131,0xFF635131,0xFF422C18,0xFF422C18,0xFF422C18,0xFF84B2DE,0xFF84B2DE,0xFF84B2DE,0xFF527DB5,0xFF527DB5,0xFF527DB5,0xFF29518C,0xFF29518C,0xFF29518C,0xFF082C63,0xFF082C63,0xFF082C63,0xFF001039,0xFF001039,0xFF001039,0xFF522C84,0xFF522C84,0xFF522C84,0xFF42246B,0xFF42246B,0xFF42246B,0xFF392063,0xFF392063,0xFF392063,0xFF291C5A,0xFF291C5A,0xFF291C5A,0xFF21185A,0xFF21185A,0xFF21185A,0xFF181452,0xFF181452,0xFF181452,0xFF10104A,0xFF10104A,0xFF10104A,0xFF080C4A,0xFF080C4A,0xFF080C4A,0xFF080C42,0xFF080C42,0xFF080C42,0xFF000C39,0xFF000C39,0xFF000C39,0xFF000C39,0xFF000C39,0xFF000C39,0xFF000C31,0xFF000C31,0xFF000C31,0xFF000C29,0xFF000C29,0xFF000C29,0xFF000C21,0xFF000C21,0xFF000C21,0xFF000C21,0xFF000C21,0xFF000C21,0xFF000C18,0xFF000C18,0xFF000C18,0xFF000000,0xFF000000,0xFF000000,0xFF000000,0xFF000000,0xFF000000,0xFFEFCBC6,0xFFEFCBC6,0xFFEFCBC6,0xFFC68284,0xFFC68284,0xFFC68284,0xFF9C5163,0xFF9C5163,0xFF9C5163,0xFF732852,0xFF732852,0xFF732852,0xFF4A1039,0xFF4A1039,0xFF4A1039,0xFFB596EF,0xFFB596EF,0xFFB596EF,0xFF8C65BD,0xFF8C65BD,0xFF8C65BD,0xFF633C8C,0xFF633C8C,0xFF633C8C,0xFF421C5A,0xFF421C5A,0xFF421C5A,0xFF210829,0xFF210829,0xFF210829,0xFFFFFFA5,0xFFFFFFA5,0xFFFFFFA5,0xFFDEEB8C,0xFFDEEB8C,0xFFDEEB8C,0xFFC6D77B,0xFFC6D77B,0xFFC6D77B,0xFFA5C36B,0xFFA5C36B,0xFFA5C36B, };
    uint32_t pal3[768] = { 0x00000000,0xFF000000,0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFEFEFEF,0xFFEFEFEF,0xFFEFEFEF,0xFFDEDBDE,0xFFDEDBDE,0xFFDEDBDE,0xFFCECBCE,0xFFCECBCE,0xFFCECBCE,0xFFBDBABD,0xFFBDBABD,0xFFBDBABD,0xFFADAAAD,0xFFADAAAD,0xFFADAAAD,0xFF9C9A9C,0xFF9C9A9C,0xFF9C9A9C,0xFF8C8A8C,0xFF8C8A8C,0xFF8C8A8C,0xFF7B797B,0xFF7B797B,0xFF7B797B,0xFF6B696B,0xFF6B696B,0xFF6B696B,0xFF5A595A,0xFF5A595A,0xFF5A595A,0xFF4A494A,0xFF4A494A,0xFF4A494A,0xFF393839,0xFF393839,0xFF393839,0xFF292829,0xFF292829,0xFF292829,0xFF181818,0xFF181818,0xFF181818,0xFFFFFF00,0xFFFFFF00,0xFFFFFF00,0xFFDED300,0xFFDED300,0xFFDED300,0xFFB5AA00,0xFFB5AA00,0xFFB5AA00,0xFF948200,0xFF948200,0xFF948200,0xFF6B5D00,0xFF6B5D00,0xFF6B5D00,0xFF4A3C00,0xFF4A3C00,0xFF4A3C00,0xFF635500,0xFF635500,0xFF635500,0xFF4A3C00,0xFF4A3C00,0xFF4A3C00,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFD60000,0xFFD60000,0xFFD60000,0xFFB50000,0xFFB50000,0xFFB50000,0xFF8C0000,0xFF8C0000,0xFF8C0000,0xFF630000,0xFF630000,0xFF630000,0xFF420000,0xFF420000,0xFF420000,0xFF5A0000,0xFF5A0000,0xFF5A0000,0xFF420000,0xFF420000,0xFF420000,0xFF00FF00,0xFF00FF00,0xFF00FF00,0xFF00D700,0xFF00D700,0xFF00D700,0xFF00B200,0xFF00B200,0xFF00B200,0xFF008A00,0xFF008A00,0xFF008A00,0xFF006500,0xFF006500,0xFF006500,0xFF004100,0xFF004100,0xFF004100,0xFF005900,0xFF005900,0xFF005900,0xFF004100,0xFF004100,0xFF004100,0xFF007DFF,0xFF007DFF,0xFF007DFF,0xFF0065D6,0xFF0065D6,0xFF0065D6,0xFF004DAD,0xFF004DAD,0xFF004DAD,0xFF00347B,0xFF00347B,0xFF00347B,0xFF002052,0xFF002052,0xFF002052,0xFF001029,0xFF001029,0xFF001029,0xFF001C4A,0xFF001C4A,0xFF001C4A,0xFF001029,0xFF001029,0xFF001029,0xFF00FFFF,0xFF00FFFF,0xFF00FFFF,0xFF00D7D6,0xFF00D7D6,0xFF00D7D6,0xFF00AEAD,0xFF00AEAD,0xFF00AEAD,0xFF008684,0xFF008684,0xFF008684,0xFF005D5A,0xFF005D5A,0xFF005D5A,0xFF003839,0xFF003839,0xFF003839,0xFF005552,0xFF005552,0xFF005552,0xFF003839,0xFF003839,0xFF003839,0xFFFF86D6,0xFFFF86D6,0xFFFF86D6,0xFFD661A5,0xFFD661A5,0xFFD661A5,0xFFA54584,0xFFA54584,0xFFA54584,0xFF7B2C5A,0xFF7B2C5A,0xFF7B2C5A,0xFF4A1839,0xFF4A1839,0xFF4A1839,0xFF210818,0xFF210818,0xFF210818,0xFF421429,0xFF421429,0xFF421429,0xFF210818,0xFF210818,0xFF210818,0xFFFF7100,0xFFFF7100,0xFFFF7100,0xFFD65500,0xFFD65500,0xFFD65500,0xFFAD3800,0xFFAD3800,0xFFAD3800,0xFF842400,0xFF842400,0xFF842400,0xFF5A1400,0xFF5A1400,0xFF5A1400,0xFF310800,0xFF310800,0xFF310800,0xFF521000,0xFF521000,0xFF521000,0xFF310800,0xFF310800,0xFF310800,0xFFFFAE5A,0xFFFFAE5A,0xFFFFAE5A,0xFFDE924A,0xFFDE924A,0xFFDE924A,0xFFBD7939,0xFFBD7939,0xFFBD7939,0xFF9C6129,0xFF9C6129,0xFF9C6129,0xFF7B4918,0xFF7B4918,0xFF7B4918,0xFF5A3410,0xFF5A3410,0xFF5A3410,0xFF6B4118,0xFF6B4118,0xFF6B4118,0xFF5A3410,0xFF5A3410,0xFF5A3410,0xFFBD79FF,0xFFBD79FF,0xFFBD79FF,0xFF9C5DDE,0xFF9C5DDE,0xFF9C5DDE,0xFF7B45BD,0xFF7B45BD,0xFF7B45BD,0xFF632C9C,0xFF632C9C,0xFF632C9C,0xFF4A1C7B,0xFF4A1C7B,0xFF4A1C7B,0xFF31105A,0xFF31105A,0xFF31105A,0xFF421873,0xFF421873,0xFF421873,0xFF31105A,0xFF31105A,0xFF31105A,0xFF84BAC6,0xFF84BAC6,0xFF84BAC6,0xFF6B9AA5,0xFF6B9AA5,0xFF6B9AA5,0xFF52798C,0xFF52798C,0xFF52798C,0xFF39596B,0xFF39596B,0xFF39596B,0xFF213C4A,0xFF213C4A,0xFF213C4A,0xFF102431,0xFF102431,0xFF102431,0xFF213442,0xFF213442,0xFF213442,0xFF102431,0xFF102431,0xFF102431,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFDEDBDE,0xFFDEDBDE,0xFFDEDBDE,0xFFB5B6B5,0xFFB5B6B5,0xFFB5B6B5,0xFF949294,0xFF949294,0xFF949294,0xFF6B6D6B,0xFF6B6D6B,0xFF6B6D6B,0xFF4A4D4A,0xFF4A4D4A,0xFF4A4D4A,0xFF636563,0xFF636563,0xFF636563,0xFF4A4D4A,0xFF4A4D4A,0xFF4A4D4A,0xFF18DF8C,0xFF18DF8C,0xFF18DF8C,0xFF10BA73,0xFF10BA73,0xFF10BA73,0xFF109652,0xFF109652,0xFF109652,0xFF087139,0xFF087139,0xFF087139,0xFF084D29,0xFF084D29,0xFF084D29,0xFF002C10,0xFF002C10,0xFF002C10,0xFF084521,0xFF084521,0xFF084521,0xFF002C10,0xFF002C10,0xFF002C10,0xFFE78263,0xFFE78263,0xFFE78263,0xFFBD614A,0xFFBD614A,0xFFBD614A,0xFF9C4131,0xFF9C4131,0xFF9C4131,0xFF732818,0xFF732818,0xFF732818,0xFF521408,0xFF521408,0xFF521408,0xFF310800,0xFF310800,0xFF310800,0xFF4A1008,0xFF4A1008,0xFF4A1008,0xFF310800,0xFF310800,0xFF310800,0xFFCEAACE,0xFFCEAACE,0xFFCEAACE,0xFFA586A5,0xFFA586A5,0xFFA586A5,0xFF846584,0xFF846584,0xFF846584,0xFF5A455A,0xFF5A455A,0xFF5A455A,0xFF392839,0xFF392839,0xFF392839,0xFF181018,0xFF181018,0xFF181018,0xFF312031,0xFF312031,0xFF312031,0xFF181018,0xFF181018,0xFF181018,0xFFC6A642,0xFFC6A642,0xFFC6A642,0xFFA58631,0xFFA58631,0xFFA58631,0xFF846921,0xFF846921,0xFF846921,0xFF634D10,0xFF634D10,0xFF634D10,0xFF423408,0xFF423408,0xFF423408,0xFF291C00,0xFF291C00,0xFF291C00,0xFF392C08,0xFF392C08,0xFF392C08,0xFF291C00,0xFF291C00,0xFF291C00,0xFFFF9694,0xFFFF9694,0xFFFF9694,0xFFD67173,0xFFD67173,0xFFD67173,0xFFA55152,0xFFA55152,0xFFA55152,0xFF7B3431,0xFF7B3431,0xFF7B3431,0xFF522021,0xFF522021,0xFF522021,0xFF290C08,0xFF290C08,0xFF290C08,0xFF421818,0xFF421818,0xFF421818,0xFF290C08,0xFF290C08,0xFF290C08,0xFFD62084,0xFFD62084,0xFFD62084,0xFFAD146B,0xFFAD146B,0xFFAD146B,0xFF8C0852,0xFF8C0852,0xFF8C0852,0xFF6B0439,0xFF6B0439,0xFF6B0439,0xFF420029,0xFF420029,0xFF420029,0xFF210010,0xFF210010,0xFF210010,0xFF390021,0xFF390021,0xFF390021,0xFF210010,0xFF210010,0xFF210010,0xFFA5A2A5,0xFFA5A2A5,0xFFA5A2A5,0xFF848284,0xFF848284,0xFF848284,0xFF636163,0xFF636163,0xFF636163,0xFF424542,0xFF424542,0xFF424542,0xFF212421,0xFF212421,0xFF212421,0xFF080808,0xFF080808,0xFF080808,0xFF181C18,0xFF181C18,0xFF181C18,0xFF080808,0xFF080808,0xFF080808,0xFF004500,0xFF004500,0xFF004500,0xFF003800,0xFF003800,0xFF003800,0xFF002C00,0xFF002C00,0xFF002C00,0xFF002000,0xFF002000,0xFF002000,0xFF001800,0xFF001800,0xFF001800,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF8CF7E7,0xFF8CF7E7,0xFF8CF7E7,0xFF7BE7D6,0xFF7BE7D6,0xFF7BE7D6,0xFF6BDBC6,0xFF6BDBC6,0xFF6BDBC6,0xFF63CBB5,0xFF63CBB5,0xFF63CBB5,0xFF52BEA5,0xFF52BEA5,0xFF52BEA5,0xFF4AAE94,0xFF4AAE94,0xFF4AAE94,0xFF39A284,0xFF39A284,0xFF39A284,0xFF31967B,0xFF31967B,0xFF31967B,0xFF004DA5,0xFF004DA5,0xFF004DA5,0xFF004194,0xFF004194,0xFF004194,0xFF003884,0xFF003884,0xFF003884,0xFF00307B,0xFF00307B,0xFF00307B,0xFF00286B,0xFF00286B,0xFF00286B,0xFF00205A,0xFF00205A,0xFF00205A,0xFF001852,0xFF001852,0xFF001852,0xFF001442,0xFF001442,0xFF001442,0xFF399EBD,0xFF399EBD,0xFF399EBD,0xFF297DAD,0xFF297DAD,0xFF297DAD,0xFF215D94,0xFF215D94,0xFF215D94,0xFF104584,0xFF104584,0xFF104584,0xFF082C6B,0xFF082C6B,0xFF082C6B,0xFF00185A,0xFF00185A,0xFF00185A,0xFF000442,0xFF000442,0xFF000442,0xFF000031,0xFF000031,0xFF000031,0xFF317129,0xFF317129,0xFF317129,0xFF296521,0xFF296521,0xFF296521,0xFF185D18,0xFF185D18,0xFF185D18,0xFF105510,0xFF105510,0xFF105510,0xFF084D08,0xFF084D08,0xFF084D08,0xFF004500,0xFF004500,0xFF004500,0xFF003C00,0xFF003C00,0xFF003C00,0xFF003400,0xFF003400,0xFF003400,0xFF004DC6,0xFF004DC6,0xFF004DC6,0xFF0049BD,0xFF0049BD,0xFF0049BD,0xFF0045B5,0xFF0045B5,0xFF0045B5,0xFF0045AD,0xFF0045AD,0xFF0045AD,0xFF0041AD,0xFF0041AD,0xFF0041AD,0xFF0041A5,0xFF0041A5,0xFF0041A5,0xFF003C9C,0xFF003C9C,0xFF003C9C,0xFF003894,0xFF003894,0xFF003894,0xFF003894,0xFF003894,0xFF003894,0xFF00348C,0xFF00348C,0xFF00348C,0xFF003484,0xFF003484,0xFF003484,0xFF00307B,0xFF00307B,0xFF00307B,0xFF002C7B,0xFF002C7B,0xFF002C7B,0xFF002C73,0xFF002C73,0xFF002C73,0xFF00286B,0xFF00286B,0xFF00286B,0xFF00286B,0xFF00286B,0xFF00286B,0xFF084D08,0xFF084D08,0xFF084D08,0xFF084900,0xFF084900,0xFF084900,0xFF084500,0xFF084500,0xFF084500,0xFF004100,0xFF004100,0xFF004100,0xFF003C00,0xFF003C00,0xFF003C00,0xFF003800,0xFF003800,0xFF003800,0xFF003400,0xFF003400,0xFF003400,0xFF003000,0xFF003000,0xFF003000,0xFF002C00,0xFF002C00,0xFF002C00,0xFF002800,0xFF002800,0xFF002800,0xFF002400,0xFF002400,0xFF002400,0xFF002000,0xFF002000,0xFF002000,0xFF001C00,0xFF001C00,0xFF001C00,0xFF001800,0xFF001800,0xFF001800,0xFF001400,0xFF001400,0xFF001400,0xFF001000,0xFF001000,0xFF001000,0xFF001442,0xFF001442,0xFF001442,0xFF082C63,0xFF082C63,0xFF082C63,0xFF184D8C,0xFF184D8C,0xFF184D8C,0xFF3975AD,0xFF3975AD,0xFF3975AD,0xFF639ECE,0xFF639ECE,0xFF639ECE,0xFF94CFF7,0xFF94CFF7,0xFF94CFF7,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFFFFFFA5,0xFFFFFFA5,0xFFFFFFA5,0xFFDEEB8C,0xFFDEEB8C,0xFFDEEB8C,0xFFC6D77B,0xFFC6D77B,0xFFC6D77B,0xFFA5C36B,0xFFA5C36B,0xFFA5C36B, };
    uint32_t pal4[768] = { 0x00000000,0xFF000000,0xFF000000,0xFFFFFFE7,0xFFFFFFE7,0xFFFFFFE7,0xFFFFFBDE,0xFFFFFBDE,0xFFFFFBDE,0xFFFFFBCE,0xFFFFFBCE,0xFFFFFBCE,0xFFFFF7C6,0xFFFFF7C6,0xFFFFF7C6,0xFFFFF3BD,0xFFFFF3BD,0xFFFFF3BD,0xFFFFEFAD,0xFFFFEFAD,0xFFFFEFAD,0xFFFFE7A5,0xFFFFE7A5,0xFFFFE7A5,0xFFFFDF9C,0xFFFFDF9C,0xFFFFDF9C,0xFFFFDB8C,0xFFFFDB8C,0xFFFFDB8C,0xFFFFD384,0xFFFFD384,0xFFFFD384,0xFFFFC773,0xFFFFC773,0xFFFFC773,0xFFFFBE6B,0xFFFFBE6B,0xFFFFBE6B,0xFFFFB263,0xFFFFB263,0xFFFFB263,0xFFFFA652,0xFFFFA652,0xFFFFA652,0xFFFF9A4A,0xFFFF9A4A,0xFFFF9A4A,0xFFFFEB00,0xFFFFEB00,0xFFFFEB00,0xFFFFD700,0xFFFFD700,0xFFFFD700,0xFFF7CB00,0xFFF7CB00,0xFFF7CB00,0xFFF7BA00,0xFFF7BA00,0xFFF7BA00,0xFFF7AA00,0xFFF7AA00,0xFFF7AA00,0xFFEF9A00,0xFFEF9A00,0xFFEF9A00,0xFFEF8A00,0xFFEF8A00,0xFFEF8A00,0xFFEF7D00,0xFFEF7D00,0xFFEF7D00,0xFFE76D00,0xFFE76D00,0xFFE76D00,0xFFE76100,0xFFE76100,0xFFE76100,0xFFDE5500,0xFFDE5500,0xFFDE5500,0xFFCE4900,0xFFCE4900,0xFFCE4900,0xFFC63C00,0xFFC63C00,0xFFC63C00,0xFFB53000,0xFFB53000,0xFFB53000,0xFFAD2800,0xFFAD2800,0xFFAD2800,0xFF9C2000,0xFF9C2000,0xFF9C2000,0xFFFFFFEF,0xFFFFFFEF,0xFFFFFFEF,0xFFF7F7E7,0xFFF7F7E7,0xFFF7F7E7,0xFFEFEFDE,0xFFEFEFDE,0xFFEFEFDE,0xFFE7EBDE,0xFFE7EBDE,0xFFE7EBDE,0xFFDEE3D6,0xFFDEE3D6,0xFFDEE3D6,0xFFD6DFCE,0xFFD6DFCE,0xFFD6DFCE,0xFFCED7C6,0xFFCED7C6,0xFFCED7C6,0xFFC6CFBD,0xFFC6CFBD,0xFFC6CFBD,0xFFBDCBB5,0xFFBDCBB5,0xFFBDCBB5,0xFFB5C3B5,0xFFB5C3B5,0xFFB5C3B5,0xFFADBAAD,0xFFADBAAD,0xFFADBAAD,0xFFA5B6A5,0xFFA5B6A5,0xFFA5B6A5,0xFF9CAEA5,0xFF9CAEA5,0xFF9CAEA5,0xFF9CAA9C,0xFF9CAA9C,0xFF9CAA9C,0xFF94A29C,0xFF94A29C,0xFF94A29C,0xFF8C9A94,0xFF8C9A94,0xFF8C9A94,0xFF84968C,0xFF84968C,0xFF84968C,0xFF7B8E8C,0xFF7B8E8C,0xFF7B8E8C,0xFF7B8A84,0xFF7B8A84,0xFF7B8A84,0xFF73827B,0xFF73827B,0xFF73827B,0xFF6B797B,0xFF6B797B,0xFF6B797B,0xFF637573,0xFF637573,0xFF637573,0xFF636D6B,0xFF636D6B,0xFF636D6B,0xFF5A656B,0xFF5A656B,0xFF5A656B,0xFF525D63,0xFF525D63,0xFF525D63,0xFF4A555A,0xFF4A555A,0xFF4A555A,0xFF4A4D52,0xFF4A4D52,0xFF4A4D52,0xFF42494A,0xFF42494A,0xFF42494A,0xFF394142,0xFF394142,0xFF394142,0xFF393842,0xFF393842,0xFF393842,0xFF313039,0xFF313039,0xFF313039,0xFF292C31,0xFF292C31,0xFF292C31,0xFFEFFF7B,0xFFEFFF7B,0xFFEFFF7B,0xFFD6EF6B,0xFFD6EF6B,0xFFD6EF6B,0xFFC6E363,0xFFC6E363,0xFFC6E363,0xFFADD752,0xFFADD752,0xFFADD752,0xFF9CCB42,0xFF9CCB42,0xFF9CCB42,0xFF84BE39,0xFF84BE39,0xFF84BE39,0xFF73AE31,0xFF73AE31,0xFF73AE31,0xFF63A229,0xFF63A229,0xFF63A229,0xFF529621,0xFF529621,0xFF529621,0xFF428A18,0xFF428A18,0xFF428A18,0xFF317D10,0xFF317D10,0xFF317D10,0xFF216D08,0xFF216D08,0xFF216D08,0xFF106100,0xFF106100,0xFF106100,0xFF085500,0xFF085500,0xFF085500,0xFF004900,0xFF004900,0xFF004900,0xFF003C00,0xFF003C00,0xFF003C00,0xFFEFEFFF,0xFFEFEFFF,0xFFEFEFFF,0xFFE7E3F7,0xFFE7E3F7,0xFFE7E3F7,0xFFD6D7EF,0xFFD6D7EF,0xFFD6D7EF,0xFFCECFE7,0xFFCECFE7,0xFFCECFE7,0xFFC6C3DE,0xFFC6C3DE,0xFFC6C3DE,0xFFBDBAD6,0xFFBDBAD6,0xFFBDBAD6,0xFFB5AECE,0xFFB5AECE,0xFFB5AECE,0xFFA5A2C6,0xFFA5A2C6,0xFFA5A2C6,0xFF9C9ABD,0xFF9C9ABD,0xFF9C9ABD,0xFF9492BD,0xFF9492BD,0xFF9492BD,0xFF8C86B5,0xFF8C86B5,0xFF8C86B5,0xFF8C82AD,0xFF8C82AD,0xFF8C82AD,0xFF8475A5,0xFF8475A5,0xFF8475A5,0xFF7B6D9C,0xFF7B6D9C,0xFF7B6D9C,0xFF736994,0xFF736994,0xFF736994,0xFF6B6194,0xFF6B6194,0xFF6B6194,0xFF6B5D8C,0xFF6B5D8C,0xFF6B5D8C,0xFF635584,0xFF635584,0xFF635584,0xFF5A4D7B,0xFF5A4D7B,0xFF5A4D7B,0xFF524573,0xFF524573,0xFF524573,0xFF52416B,0xFF52416B,0xFF52416B,0xFF4A386B,0xFF4A386B,0xFF4A386B,0xFF423463,0xFF423463,0xFF423463,0xFF42305A,0xFF42305A,0xFF42305A,0xFF392852,0xFF392852,0xFF392852,0xFF31244A,0xFF31244A,0xFF31244A,0xFF312042,0xFF312042,0xFF312042,0xFF291C39,0xFF291C39,0xFF291C39,0xFF211839,0xFF211839,0xFF211839,0xFF211431,0xFF211431,0xFF211431,0xFF181029,0xFF181029,0xFF181029,0xFF180C21,0xFF180C21,0xFF180C21,0xFFFFD3FF,0xFFFFD3FF,0xFFFFD3FF,0xFFF7C3F7,0xFFF7C3F7,0xFFF7C3F7,0xFFEFB2EF,0xFFEFB2EF,0xFFEFB2EF,0xFFE7A2E7,0xFFE7A2E7,0xFFE7A2E7,0xFFD692DE,0xFFD692DE,0xFFD692DE,0xFFCE86D6,0xFFCE86D6,0xFFCE86D6,0xFFC679C6,0xFFC679C6,0xFFC679C6,0xFFBD6DBD,0xFFBD6DBD,0xFFBD6DBD,0xFFB561B5,0xFFB561B5,0xFFB561B5,0xFFAD55AD,0xFFAD55AD,0xFFAD55AD,0xFF9C49A5,0xFF9C49A5,0xFF9C49A5,0xFF94419C,0xFF94419C,0xFF94419C,0xFF8C3894,0xFF8C3894,0xFF8C3894,0xFF84308C,0xFF84308C,0xFF84308C,0xFF7B2884,0xFF7B2884,0xFF7B2884,0xFF73207B,0xFF73207B,0xFF73207B,0xFFCEEFFF,0xFFCEEFFF,0xFFCEEFFF,0xFFC6E3F7,0xFFC6E3F7,0xFFC6E3F7,0xFFB5DBF7,0xFFB5DBF7,0xFFB5DBF7,0xFFADD3EF,0xFFADD3EF,0xFFADD3EF,0xFF9CCBE7,0xFF9CCBE7,0xFF9CCBE7,0xFF94C3DE,0xFF94C3DE,0xFF94C3DE,0xFF8CB6DE,0xFF8CB6DE,0xFF8CB6DE,0xFF84AED6,0xFF84AED6,0xFF84AED6,0xFF73A6CE,0xFF73A6CE,0xFF73A6CE,0xFF6B9EC6,0xFF6B9EC6,0xFF6B9EC6,0xFF6396C6,0xFF6396C6,0xFF6396C6,0xFF5A8EBD,0xFF5A8EBD,0xFF5A8EBD,0xFF5286B5,0xFF5286B5,0xFF5286B5,0xFF4A7DAD,0xFF4A7DAD,0xFF4A7DAD,0xFF4275AD,0xFF4275AD,0xFF4275AD,0xFF396DA5,0xFF396DA5,0xFF396DA5,0xFF39659C,0xFF39659C,0xFF39659C,0xFF315D94,0xFF315D94,0xFF315D94,0xFF295994,0xFF295994,0xFF295994,0xFF29518C,0xFF29518C,0xFF29518C,0xFF214984,0xFF214984,0xFF214984,0xFF184584,0xFF184584,0xFF184584,0xFF183C7B,0xFF183C7B,0xFF183C7B,0xFF103873,0xFF103873,0xFF103873,0xFF103473,0xFF103473,0xFF103473,0xFF082C6B,0xFF082C6B,0xFF082C6B,0xFF082863,0xFF082863,0xFF082863,0xFF00245A,0xFF00245A,0xFF00245A,0xFF00205A,0xFF00205A,0xFF00205A,0xFF001C52,0xFF001C52,0xFF001C52,0xFF00184A,0xFF00184A,0xFF00184A,0xFF00144A,0xFF00144A,0xFF00144A,0xFFFFDFD6,0xFFFFDFD6,0xFFFFDFD6,0xFFEFC7C6,0xFFEFC7C6,0xFFEFC7C6,0xFFDEB2AD,0xFFDEB2AD,0xFFDEB2AD,0xFFD69A9C,0xFFD69A9C,0xFFD69A9C,0xFFC68A8C,0xFFC68A8C,0xFFC68A8C,0xFFB57984,0xFFB57984,0xFFB57984,0xFFAD697B,0xFFAD697B,0xFFAD697B,0xFF9C5D6B,0xFF9C5D6B,0xFF9C5D6B,0xFF8C4D63,0xFF8C4D63,0xFF8C4D63,0xFF84415A,0xFF84415A,0xFF84415A,0xFF733452,0xFF733452,0xFF733452,0xFF632C4A,0xFF632C4A,0xFF632C4A,0xFF5A2042,0xFF5A2042,0xFF5A2042,0xFF4A1839,0xFF4A1839,0xFF4A1839,0xFF391031,0xFF391031,0xFF391031,0xFF310C29,0xFF310C29,0xFF310C29,0xFFFFE7BD,0xFFFFE7BD,0xFFFFE7BD,0xFFF7DBAD,0xFFF7DBAD,0xFFF7DBAD,0xFFF7D3A5,0xFFF7D3A5,0xFFF7D3A5,0xFFEFCB94,0xFFEFCB94,0xFFEFCB94,0xFFE7C38C,0xFFE7C38C,0xFFE7C38C,0xFFE7BA84,0xFFE7BA84,0xFFE7BA84,0xFFDEB273,0xFFDEB273,0xFFDEB273,0xFFD6A66B,0xFFD6A66B,0xFFD6A66B,0xFFCE9E63,0xFFCE9E63,0xFFCE9E63,0xFFCE965A,0xFFCE965A,0xFFCE965A,0xFFC68E52,0xFFC68E52,0xFFC68E52,0xFFBD864A,0xFFBD864A,0xFFBD864A,0xFFBD7D42,0xFFBD7D42,0xFFBD7D42,0xFFB57539,0xFFB57539,0xFFB57539,0xFFAD6D31,0xFFAD6D31,0xFFAD6D31,0xFFAD6529,0xFFAD6529,0xFFAD6529,0xFFD6B66B,0xFFD6B66B,0xFFD6B66B,0xFFCEAA63,0xFFCEAA63,0xFFCEAA63,0xFFC69E5A,0xFFC69E5A,0xFFC69E5A,0xFFBD925A,0xFFBD925A,0xFFBD925A,0xFFAD8652,0xFFAD8652,0xFFAD8652,0xFFA57D4A,0xFFA57D4A,0xFFA57D4A,0xFF9C7142,0xFF9C7142,0xFF9C7142,0xFF946942,0xFF946942,0xFF946942,0xFF8C5D39,0xFF8C5D39,0xFF8C5D39,0xFF845531,0xFF845531,0xFF845531,0xFF734D31,0xFF734D31,0xFF734D31,0xFF6B4129,0xFF6B4129,0xFF6B4129,0xFF633821,0xFF633821,0xFF633821,0xFF5A3021,0xFF5A3021,0xFF5A3021,0xFF522818,0xFF522818,0xFF522818,0xFF4A2418,0xFF4A2418,0xFF4A2418,0xFFA53800,0xFFA53800,0xFFA53800,0xFF942800,0xFF942800,0xFF942800,0xFF8C2000,0xFF8C2000,0xFF8C2000,0xFF7B1400,0xFF7B1400,0xFF7B1400,0xFF6B0C00,0xFF6B0C00,0xFF6B0C00,0xFF630400,0xFF630400,0xFF630400,0xFF520000,0xFF520000,0xFF520000,0xFF4A0000,0xFF4A0000,0xFF4A0000,0xFFCEFBC6,0xFFCEFBC6,0xFFCEFBC6,0xFFADE39C,0xFFADE39C,0xFFADE39C,0xFF84CF7B,0xFF84CF7B,0xFF84CF7B,0xFF6BBA5A,0xFF6BBA5A,0xFF6BBA5A,0xFF4AA242,0xFF4AA242,0xFF4AA242,0xFF318E29,0xFF318E29,0xFF318E29,0xFF187918,0xFF187918,0xFF187918,0xFF086508,0xFF086508,0xFF086508,0xFFF7DFCE,0xFFF7DFCE,0xFFF7DFCE,0xFFEFCFBD,0xFFEFCFBD,0xFFEFCFBD,0xFFDEC3AD,0xFFDEC3AD,0xFFDEC3AD,0xFFD6B29C,0xFFD6B29C,0xFFD6B29C,0xFFCEA694,0xFFCEA694,0xFFCEA694,0xFFBD9A84,0xFFBD9A84,0xFFBD9A84,0xFFB58E73,0xFFB58E73,0xFFB58E73,0xFFA5826B,0xFFA5826B,0xFFA5826B,0xFF9C7563,0xFF9C7563,0xFF9C7563,0xFF946952,0xFF946952,0xFF946952,0xFF84614A,0xFF84614A,0xFF84614A,0xFF7B5542,0xFF7B5542,0xFF7B5542,0xFF734D39,0xFF734D39,0xFF734D39,0xFF634129,0xFF634129,0xFF634129,0xFF5A3821,0xFF5A3821,0xFF5A3821,0xFF523021,0xFF523021,0xFF523021,0xFF9C96BD,0xFF9C96BD,0xFF9C96BD,0xFF7B7994,0xFF7B7994,0xFF7B7994,0xFF5A5D6B,0xFF5A5D6B,0xFF5A5D6B,0xFF393C42,0xFF393C42,0xFF393C42,0xFF181C21,0xFF181C21,0xFF181C21,0xFF8CCFBD,0xFF8CCFBD,0xFF8CCFBD,0xFF6BA69C,0xFF6BA69C,0xFF6BA69C,0xFF4A7D7B,0xFF4A7D7B,0xFF4A7D7B,0xFF295152,0xFF295152,0xFF295152,0xFF102829,0xFF102829,0xFF102829,0xFFFF00FF,0xFFFF00FF,0xFFFF00FF,0xFFFF00FF,0xFFFF00FF,0xFFFF00FF,0xFFFFFFA5,0xFFFFFFA5,0xFFFFFFA5,0xFFDEEB8C,0xFFDEEB8C,0xFFDEEB8C,0xFFC6D77B,0xFFC6D77B,0xFFC6D77B,0xFFA5C36B,0xFFA5C36B,0xFFA5C36B, };
    uint32_t pal5[768] = { 0x00000000,0xFF000000,0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFEFEBEF,0xFFEFEBEF,0xFFEFEBEF,0xFFDEDBDE,0xFFDEDBDE,0xFFDEDBDE,0xFFC6C7C6,0xFFC6C7C6,0xFFC6C7C6,0xFFB5B2B5,0xFFB5B2B5,0xFFB5B2B5,0xFFA5A2A5,0xFFA5A2A5,0xFFA5A2A5,0xFF8C8E8C,0xFF8C8E8C,0xFF8C8E8C,0xFF7B7D7B,0xFF7B7D7B,0xFF7B7D7B,0xFF6B696B,0xFF6B696B,0xFF6B696B,0xFF5A595A,0xFF5A595A,0xFF5A595A,0xFF424542,0xFF424542,0xFF424542,0xFF313431,0xFF313431,0xFF313431,0xFF212021,0xFF212021,0xFF212021,0xFF080C08,0xFF080C08,0xFF080C08,0xFF000000,0xFF000000,0xFF000000,0xFFFFD300,0xFFFFD300,0xFFFFD300,0xFFEFB600,0xFFEFB600,0xFFEFB600,0xFFE7A200,0xFFE7A200,0xFFE7A200,0xFFD68A00,0xFFD68A00,0xFFD68A00,0xFFC67500,0xFFC67500,0xFFC67500,0xFFB56100,0xFFB56100,0xFFB56100,0xFFAD5100,0xFFAD5100,0xFFAD5100,0xFF9C4100,0xFF9C4100,0xFF9C4100,0xFF8C3400,0xFF8C3400,0xFF8C3400,0xFF7B2400,0xFF7B2400,0xFF7B2400,0xFF731C00,0xFF731C00,0xFF731C00,0xFF631000,0xFF631000,0xFF631000,0xFF520800,0xFF520800,0xFF520800,0xFF4A0400,0xFF4A0400,0xFF4A0400,0xFF390000,0xFF390000,0xFF390000,0xFF290000,0xFF290000,0xFF290000,0xFFFF797B,0xFFFF797B,0xFFFF797B,0xFFEF696B,0xFFEF696B,0xFFEF696B,0xFFE75D5A,0xFFE75D5A,0xFFE75D5A,0xFFD64D4A,0xFFD64D4A,0xFFD64D4A,0xFFC64542,0xFFC64542,0xFFC64542,0xFFBD3839,0xFFBD3839,0xFFBD3839,0xFFAD2C29,0xFFAD2C29,0xFFAD2C29,0xFF9C2421,0xFF9C2421,0xFF9C2421,0xFF8C1C18,0xFF8C1C18,0xFF8C1C18,0xFF841410,0xFF841410,0xFF841410,0xFF730C08,0xFF730C08,0xFF730C08,0xFF630808,0xFF630808,0xFF630808,0xFF5A0400,0xFF5A0400,0xFF5A0400,0xFF4A0000,0xFF4A0000,0xFF4A0000,0xFF390000,0xFF390000,0xFF390000,0xFF310000,0xFF310000,0xFF310000,0xFFA5E384,0xFFA5E384,0xFFA5E384,0xFF8CD373,0xFF8CD373,0xFF8CD373,0xFF73C763,0xFF73C763,0xFF73C763,0xFF5ABA52,0xFF5ABA52,0xFF5ABA52,0xFF4AAE4A,0xFF4AAE4A,0xFF4AAE4A,0xFF39A242,0xFF39A242,0xFF39A242,0xFF319239,0xFF319239,0xFF319239,0xFF298639,0xFF298639,0xFF298639,0xFF187939,0xFF187939,0xFF187939,0xFF106D31,0xFF106D31,0xFF106D31,0xFF106131,0xFF106131,0xFF106131,0xFF085129,0xFF085129,0xFF085129,0xFF004529,0xFF004529,0xFF004529,0xFF003821,0xFF003821,0xFF003821,0xFF002C18,0xFF002C18,0xFF002C18,0xFF002018,0xFF002018,0xFF002018,0xFFD6E763,0xFFD6E763,0xFFD6E763,0xFFBDD752,0xFFBDD752,0xFFBDD752,0xFFA5C74A,0xFFA5C74A,0xFFA5C74A,0xFF94BA39,0xFF94BA39,0xFF94BA39,0xFF7BAA31,0xFF7BAA31,0xFF7BAA31,0xFF6B9E29,0xFF6B9E29,0xFF6B9E29,0xFF5A8E21,0xFF5A8E21,0xFF5A8E21,0xFF4A7D18,0xFF4A7D18,0xFF4A7D18,0xFF397110,0xFF397110,0xFF397110,0xFF296108,0xFF296108,0xFF296108,0xFF185508,0xFF185508,0xFF185508,0xFF104500,0xFF104500,0xFF104500,0xFF083400,0xFF083400,0xFF083400,0xFF002800,0xFF002800,0xFF002800,0xFF001800,0xFF001800,0xFF001800,0xFF000C00,0xFF000C00,0xFF000C00,0xFFFFFBFF,0xFFFFFBFF,0xFFFFFBFF,0xFFEFE3E7,0xFFEFE3E7,0xFFEFE3E7,0xFFDED3CE,0xFFDED3CE,0xFFDED3CE,0xFFCEBEBD,0xFFCEBEBD,0xFFCEBEBD,0xFFC6AEAD,0xFFC6AEAD,0xFFC6AEAD,0xFFB59E94,0xFFB59E94,0xFFB59E94,0xFFA58E84,0xFFA58E84,0xFFA58E84,0xFF947D73,0xFF947D73,0xFF947D73,0xFF8C6D63,0xFF8C6D63,0xFF8C6D63,0xFF7B5D5A,0xFF7B5D5A,0xFF7B5D5A,0xFF6B514A,0xFF6B514A,0xFF6B514A,0xFF5A4539,0xFF5A4539,0xFF5A4539,0xFF523831,0xFF523831,0xFF523831,0xFF422C29,0xFF422C29,0xFF422C29,0xFF312018,0xFF312018,0xFF312018,0xFF291810,0xFF291810,0xFF291810,0xFFF7C74A,0xFFF7C74A,0xFFF7C74A,0xFFE7B242,0xFFE7B242,0xFFE7B242,0xFFD69E31,0xFFD69E31,0xFFD69E31,0xFFCE8A31,0xFFCE8A31,0xFFCE8A31,0xFFBD7929,0xFFBD7929,0xFFBD7929,0xFFB56921,0xFFB56921,0xFFB56921,0xFFA55518,0xFFA55518,0xFFA55518,0xFF944910,0xFF944910,0xFF944910,0xFF8C3810,0xFF8C3810,0xFF8C3810,0xFF7B2C08,0xFF7B2C08,0xFF7B2C08,0xFF732008,0xFF732008,0xFF732008,0xFF631400,0xFF631400,0xFF631400,0xFF520C00,0xFF520C00,0xFF520C00,0xFF4A0400,0xFF4A0400,0xFF4A0400,0xFF390000,0xFF390000,0xFF390000,0xFF310000,0xFF310000,0xFF310000,0xFF8CB2CE,0xFF8CB2CE,0xFF8CB2CE,0xFF84A2BD,0xFF84A2BD,0xFF84A2BD,0xFF7392B5,0xFF7392B5,0xFF7392B5,0xFF6B82A5,0xFF6B82A5,0xFF6B82A5,0xFF5A719C,0xFF5A719C,0xFF5A719C,0xFF526594,0xFF526594,0xFF526594,0xFF4A5584,0xFF4A5584,0xFF4A5584,0xFF39497B,0xFF39497B,0xFF39497B,0xFF313C6B,0xFF313C6B,0xFF313C6B,0xFF293063,0xFF293063,0xFF293063,0xFF21245A,0xFF21245A,0xFF21245A,0xFF211C4A,0xFF211C4A,0xFF211C4A,0xFF181842,0xFF181842,0xFF181842,0xFF181031,0xFF181031,0xFF181031,0xFF100C29,0xFF100C29,0xFF100C29,0xFF100821,0xFF100821,0xFF100821,0xFFADD3F7,0xFFADD3F7,0xFFADD3F7,0xFF94BEE7,0xFF94BEE7,0xFF94BEE7,0xFF84AED6,0xFF84AED6,0xFF84AED6,0xFF739ACE,0xFF739ACE,0xFF739ACE,0xFF638ABD,0xFF638ABD,0xFF638ABD,0xFF5279AD,0xFF5279AD,0xFF5279AD,0xFF4269A5,0xFF4269A5,0xFF4269A5,0xFF315994,0xFF315994,0xFF315994,0xFF294984,0xFF294984,0xFF294984,0xFF183C7B,0xFF183C7B,0xFF183C7B,0xFF10306B,0xFF10306B,0xFF10306B,0xFF082463,0xFF082463,0xFF082463,0xFF081852,0xFF081852,0xFF081852,0xFF001042,0xFF001042,0xFF001042,0xFF000839,0xFF000839,0xFF000839,0xFF000429,0xFF000429,0xFF000429,0xFFFFFF7B,0xFFFFFF7B,0xFFFFFF7B,0xFFEFE76B,0xFFEFE76B,0xFFEFE76B,0xFFE7CF5A,0xFFE7CF5A,0xFFE7CF5A,0xFFD6BA4A,0xFFD6BA4A,0xFFD6BA4A,0xFFC6A242,0xFFC6A242,0xFFC6A242,0xFFBD8E39,0xFFBD8E39,0xFFBD8E39,0xFFAD7929,0xFFAD7929,0xFFAD7929,0xFF9C6521,0xFF9C6521,0xFF9C6521,0xFF8C5118,0xFF8C5118,0xFF8C5118,0xFF844110,0xFF844110,0xFF844110,0xFF733008,0xFF733008,0xFF733008,0xFF632008,0xFF632008,0xFF632008,0xFF5A1400,0xFF5A1400,0xFF5A1400,0xFF4A0800,0xFF4A0800,0xFF4A0800,0xFF390400,0xFF390400,0xFF390400,0xFF310000,0xFF310000,0xFF310000,0xFFBDA25A,0xFFBDA25A,0xFFBDA25A,0xFFAD924A,0xFFAD924A,0xFFAD924A,0xFFA58242,0xFFA58242,0xFFA58242,0xFF947139,0xFF947139,0xFF947139,0xFF8C6129,0xFF8C6129,0xFF8C6129,0xFF7B5121,0xFF7B5121,0xFF7B5121,0xFF734518,0xFF734518,0xFF734518,0xFF6B3818,0xFF6B3818,0xFF6B3818,0xFF5A2C10,0xFF5A2C10,0xFF5A2C10,0xFF522008,0xFF522008,0xFF522008,0xFF421408,0xFF421408,0xFF421408,0xFF390C00,0xFF390C00,0xFF390C00,0xFF290800,0xFF290800,0xFF290800,0xFF210400,0xFF210400,0xFF210400,0xFF100000,0xFF100000,0xFF100000,0xFF080000,0xFF080000,0xFF080000,0xFFFFDFD6,0xFFFFDFD6,0xFFFFDFD6,0xFFEFC7C6,0xFFEFC7C6,0xFFEFC7C6,0xFFDEB2AD,0xFFDEB2AD,0xFFDEB2AD,0xFFD69A9C,0xFFD69A9C,0xFFD69A9C,0xFFC68A8C,0xFFC68A8C,0xFFC68A8C,0xFFB57984,0xFFB57984,0xFFB57984,0xFFAD697B,0xFFAD697B,0xFFAD697B,0xFF9C5D6B,0xFF9C5D6B,0xFF9C5D6B,0xFF8C4D63,0xFF8C4D63,0xFF8C4D63,0xFF84415A,0xFF84415A,0xFF84415A,0xFF733452,0xFF733452,0xFF733452,0xFF632C4A,0xFF632C4A,0xFF632C4A,0xFF5A2042,0xFF5A2042,0xFF5A2042,0xFF4A1839,0xFF4A1839,0xFF4A1839,0xFF391031,0xFF391031,0xFF391031,0xFF310C29,0xFF310C29,0xFF310C29,0xFFDEC7FF,0xFFDEC7FF,0xFFDEC7FF,0xFFCEAEEF,0xFFCEAEEF,0xFFCEAEEF,0xFFBD96E7,0xFFBD96E7,0xFFBD96E7,0xFFAD82D6,0xFFAD82D6,0xFFAD82D6,0xFFA56DC6,0xFFA56DC6,0xFFA56DC6,0xFF9C5DB5,0xFF9C5DB5,0xFF9C5DB5,0xFF8C4DAD,0xFF8C4DAD,0xFF8C4DAD,0xFF843C9C,0xFF843C9C,0xFF843C9C,0xFF7B308C,0xFF7B308C,0xFF7B308C,0xFF73247B,0xFF73247B,0xFF73247B,0xFF6B1873,0xFF6B1873,0xFF6B1873,0xFF631063,0xFF631063,0xFF631063,0xFF520852,0xFF520852,0xFF520852,0xFF4A0439,0xFF4A0439,0xFF4A0439,0xFF390029,0xFF390029,0xFF390029,0xFF290021,0xFF290021,0xFF290021,0xFF42C7FF,0xFF42C7FF,0xFF42C7FF,0xFF39AEEF,0xFF39AEEF,0xFF39AEEF,0xFF3196E7,0xFF3196E7,0xFF3196E7,0xFF2982D6,0xFF2982D6,0xFF2982D6,0xFF216DC6,0xFF216DC6,0xFF216DC6,0xFF1859BD,0xFF1859BD,0xFF1859BD,0xFF1849AD,0xFF1849AD,0xFF1849AD,0xFF10349C,0xFF10349C,0xFF10349C,0xFF08288C,0xFF08288C,0xFF08288C,0xFF081884,0xFF081884,0xFF081884,0xFF001073,0xFF001073,0xFF001073,0xFF000463,0xFF000463,0xFF000463,0xFF00005A,0xFF00005A,0xFF00005A,0xFF00004A,0xFF00004A,0xFF00004A,0xFF080039,0xFF080039,0xFF080039,0xFF080031,0xFF080031,0xFF080031,0xFFEFEFFF,0xFFEFEFFF,0xFFEFEFFF,0xFFDEDBEF,0xFFDEDBEF,0xFFDEDBEF,0xFFC6C7DE,0xFFC6C7DE,0xFFC6C7DE,0xFFB5B6D6,0xFFB5B6D6,0xFFB5B6D6,0xFFA5A6C6,0xFFA5A6C6,0xFFA5A6C6,0xFF9492B5,0xFF9492B5,0xFF9492B5,0xFF8C86AD,0xFF8C86AD,0xFF8C86AD,0xFF7B759C,0xFF7B759C,0xFF7B759C,0xFF6B658C,0xFF6B658C,0xFF6B658C,0xFF635984,0xFF635984,0xFF635984,0xFF524D73,0xFF524D73,0xFF524D73,0xFF4A4163,0xFF4A4163,0xFF4A4163,0xFF39345A,0xFF39345A,0xFF39345A,0xFF31284A,0xFF31284A,0xFF31284A,0xFF292039,0xFF292039,0xFF292039,0xFF211831,0xFF211831,0xFF211831,0xFFA5BADE,0xFFA5BADE,0xFFA5BADE,0xFF8496B5,0xFF8496B5,0xFF8496B5,0xFF637994,0xFF637994,0xFF637994,0xFF42596B,0xFF42596B,0xFF42596B,0xFF293C4A,0xFF293C4A,0xFF293C4A,0xFF73BAFF,0xFF73BAFF,0xFF73BAFF,0xFF5292D6,0xFF5292D6,0xFF5292D6,0xFF316DAD,0xFF316DAD,0xFF316DAD,0xFF184D7B,0xFF184D7B,0xFF184D7B,0xFF083052,0xFF083052,0xFF083052,0xFF424D52,0xFF424D52,0xFF424D52,0xFF42454A,0xFF42454A,0xFF42454A,0xFFFFFFA5,0xFFFFFFA5,0xFFFFFFA5,0xFFDEEB8C,0xFFDEEB8C,0xFFDEEB8C,0xFFC6D77B,0xFFC6D77B,0xFFC6D77B,0xFFA5C36B,0xFFA5C36B,0xFFA5C36B, };

    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        LayerPalettized* ret = new LayerPalettized(320, 200);
        ret->palette = g_palettes[PALETTE_DEFAULT];
        ret->name = "SPK Layer";
        uint32_t* pxd = (uint32_t*)ret->pixelData;
        uint64_t layerPointer = 0;
        uint16_t a;
        bool reachedImageEnd = false;
        while (!reachedImageEnd && !feof(f)) {
            fread(&a, 2, 1, f);
            if (a == 0xFFFF) {
                uint16_t skip;
                fread(&skip, 2, 1, f);
                layerPointer += (int)skip*2;
            }
            else if (a == 0xFFFE) {
                uint16_t draw;
                fread(&draw, 2, 1, f);
                for (int x = 0; x < (int)draw*2; x++) {
                    uint8_t paletteindex;
                    fread(&paletteindex, 1, 1, f);
                    pxd[layerPointer++] = (int)paletteindex;
                    if (layerPointer >= (320 * 200)) {
                        reachedImageEnd = true;
                        break;
                    }
                }
            }
            else if (a == 0xFFFD) {
                break;
            }
            else {
                logprintf("????\n");
                delete ret;
                fclose(f);
                return NULL;
            }
        }
        fclose(f);
        return ret;
    }
    return NULL;
}

Layer* readXComBDY(PlatformNativePathString path, uint64_t seek)
{
    //don't tell anyone
    uint32_t pal1[768] = { 0x00000000,0xFF000000,0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFE7E3E7,0xFFE7E3E7,0xFFE7E3E7,0xFFBDBED6,0xFFBDBED6,0xFFBDBED6,0xFF8486B5,0xFF8486B5,0xFF8486B5,0xFF526D94,0xFF526D94,0xFF526D94,0xFF4A6994,0xFF4A6994,0xFF4A6994,0xFF101463,0xFF101463,0xFF101463,0xFF000000,0xFF000000,0xFF000000,0xFFFF1818,0xFFFF1818,0xFFFF1818,0xFFDE1010,0xFFDE1010,0xFFDE1010,0xFFB50C08,0xFFB50C08,0xFFB50C08,0xFF8C0808,0xFF8C0808,0xFF8C0808,0xFF6B0000,0xFF6B0000,0xFF6B0000,0xFFC60000,0xFFC60000,0xFFC60000,0xFF000000,0xFF000000,0xFF000000,0xFF9CAE00,0xFF9CAE00,0xFF9CAE00,0xFF9CAA00,0xFF9CAA00,0xFF9CAA00,0xFF9CA600,0xFF9CA600,0xFF9CA600,0xFF9CA200,0xFF9CA200,0xFF9CA200,0xFF9CA200,0xFF9CA200,0xFF9CA200,0xFF9C9E00,0xFF9C9E00,0xFF9C9E00,0xFF9C9A00,0xFF9C9A00,0xFF9C9A00,0xFF9C9A00,0xFF9C9A00,0xFF9C9A00,0xFF949200,0xFF949200,0xFF949200,0xFF948E00,0xFF948E00,0xFF948E00,0xFF948A00,0xFF948A00,0xFF948A00,0xFF8C8600,0xFF8C8600,0xFF8C8600,0xFF8C7D00,0xFF8C7D00,0xFF8C7D00,0xFF847900,0xFF847900,0xFF847900,0xFF847500,0xFF847500,0xFF847500,0xFF847108,0xFF847108,0xFF847108,0xFF7B6D08,0xFF7B6D08,0xFF7B6D08,0xFF7B6908,0xFF7B6908,0xFF7B6908,0xFF7B6508,0xFF7B6508,0xFF7B6508,0xFF736108,0xFF736108,0xFF736108,0xFF735D08,0xFF735D08,0xFF735D08,0xFF735908,0xFF735908,0xFF735908,0xFF6B5508,0xFF6B5508,0xFF6B5508,0xFF6B5108,0xFF6B5108,0xFF6B5108,0xFF6B4D08,0xFF6B4D08,0xFF6B4D08,0xFF634908,0xFF634908,0xFF634908,0xFF634508,0xFF634508,0xFF634508,0xFF5A4508,0xFF5A4508,0xFF5A4508,0xFF5A4108,0xFF5A4108,0xFF5A4108,0xFF5A3C08,0xFF5A3C08,0xFF5A3C08,0xFF523808,0xFF523808,0xFF523808,0xFF523808,0xFF523808,0xFF523808,0xFF9CC3D6,0xFF9CC3D6,0xFF9CC3D6,0xFF7BAABD,0xFF7BAABD,0xFF7BAABD,0xFF6396AD,0xFF6396AD,0xFF6396AD,0xFF4A7D9C,0xFF4A7D9C,0xFF4A7D9C,0xFF39698C,0xFF39698C,0xFF39698C,0xFF29557B,0xFF29557B,0xFF29557B,0xFF18416B,0xFF18416B,0xFF18416B,0xFF10305A,0xFF10305A,0xFF10305A,0xFF082C5A,0xFF082C5A,0xFF082C5A,0xFF082852,0xFF082852,0xFF082852,0xFF082452,0xFF082452,0xFF082452,0xFF002052,0xFF002052,0xFF002052,0xFF001C4A,0xFF001C4A,0xFF001C4A,0xFF00184A,0xFF00184A,0xFF00184A,0xFF001442,0xFF001442,0xFF001442,0xFF001442,0xFF001442,0xFF001442,0xFF63B2CE,0xFF63B2CE,0xFF63B2CE,0xFF529EBD,0xFF529EBD,0xFF529EBD,0xFF428AAD,0xFF428AAD,0xFF428AAD,0xFF39799C,0xFF39799C,0xFF39799C,0xFF31658C,0xFF31658C,0xFF31658C,0xFF295584,0xFF295584,0xFF295584,0xFF184573,0xFF184573,0xFF184573,0xFF183863,0xFF183863,0xFF183863,0xFF10305A,0xFF10305A,0xFF10305A,0xFF082C52,0xFF082C52,0xFF082C52,0xFF08284A,0xFF08284A,0xFF08284A,0xFF00244A,0xFF00244A,0xFF00244A,0xFF002042,0xFF002042,0xFF002042,0xFF001C39,0xFF001C39,0xFF001C39,0xFF001831,0xFF001831,0xFF001831,0xFF001429,0xFF001429,0xFF001429,0xFF5AC763,0xFF5AC763,0xFF5AC763,0xFF42BE63,0xFF42BE63,0xFF42BE63,0xFF21AE42,0xFF21AE42,0xFF21AE42,0xFF218239,0xFF218239,0xFF218239,0xFF217131,0xFF217131,0xFF217131,0xFF186131,0xFF186131,0xFF186131,0xFF104D29,0xFF104D29,0xFF104D29,0xFF5AB2DE,0xFF5AB2DE,0xFF5AB2DE,0xFF52A6DE,0xFF52A6DE,0xFF52A6DE,0xFF4A9ED6,0xFF4A9ED6,0xFF4A9ED6,0xFF4292D6,0xFF4292D6,0xFF4292D6,0xFF398AD6,0xFF398AD6,0xFF398AD6,0xFF317DD6,0xFF317DD6,0xFF317DD6,0xFF2975D6,0xFF2975D6,0xFF2975D6,0xFF2169CE,0xFF2169CE,0xFF2169CE,0xFF1861CE,0xFF1861CE,0xFF1861CE,0xFF3179B5,0xFF3179B5,0xFF3179B5,0xFF2969A5,0xFF2969A5,0xFF2969A5,0xFF215994,0xFF215994,0xFF215994,0xFF184984,0xFF184984,0xFF184984,0xFF103C73,0xFF103C73,0xFF103C73,0xFF082C63,0xFF082C63,0xFF082C63,0xFF082452,0xFF082452,0xFF082452,0xFF00204A,0xFF00204A,0xFF00204A,0xFF001C4A,0xFF001C4A,0xFF001C4A,0xFF001842,0xFF001842,0xFF001842,0xFF001439,0xFF001439,0xFF001439,0xFF001031,0xFF001031,0xFF001031,0xFF001031,0xFF001031,0xFF001031,0xFF000C29,0xFF000C29,0xFF000C29,0xFF000821,0xFF000821,0xFF000821,0xFF000821,0xFF000821,0xFF000821,0xFF000000,0xFF000000,0xFF000000,0xFFFFA6D6,0xFFFFA6D6,0xFFFFA6D6,0xFFC6719C,0xFFC6719C,0xFFC6719C,0xFFFF8200,0xFFFF8200,0xFFFF8200,0xFFC65D00,0xFFC65D00,0xFFC65D00,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFC6C3C6,0xFFC6C3C6,0xFFC6C3C6,0xFF00FF00,0xFF00FF00,0xFF00FF00,0xFF00C300,0xFF00C300,0xFF00C300,0xFF00FFFF,0xFF00FFFF,0xFF00FFFF,0xFF00C3C6,0xFF00C3C6,0xFF00C3C6,0xFFFFFF00,0xFFFFFF00,0xFFFFFF00,0xFFC6C300,0xFFC6C300,0xFFC6C300,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFC60000,0xFFC60000,0xFFC60000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFCED7AD,0xFFCED7AD,0xFFCED7AD,0xFF9CB294,0xFF9CB294,0xFF9CB294,0xFFE7A284,0xFFE7A284,0xFFE7A284,0xFFDE8E63,0xFFDE8E63,0xFFDE8E63,0xFFAD8A73,0xFFAD8A73,0xFFAD8A73,0xFF084D4A,0xFF084D4A,0xFF084D4A,0xFF004542,0xFF004542,0xFF004542,0xFF003839,0xFF003839,0xFF003839,0xFF4AB2CE,0xFF4AB2CE,0xFF4AB2CE,0xFF39A2C6,0xFF39A2C6,0xFF39A2C6,0xFF218AB5,0xFF218AB5,0xFF218AB5,0xFF187DAD,0xFF187DAD,0xFF187DAD,0xFF1071A5,0xFF1071A5,0xFF1071A5,0xFF398A8C,0xFF398A8C,0xFF398A8C,0xFF005194,0xFF005194,0xFF005194,0xFFDEFF00,0xFFDEFF00,0xFFDEFF00,0xFFFFFF7B,0xFFFFFF7B,0xFFFFFF7B,0xFFEFE76B,0xFFEFE76B,0xFFEFE76B,0xFFE7CF5A,0xFFE7CF5A,0xFFE7CF5A,0xFFD6BA4A,0xFFD6BA4A,0xFFD6BA4A,0xFFB5AE00,0xFFB5AE00,0xFFB5AE00,0xFF9C9A00,0xFF9C9A00,0xFF9C9A00,0xFF848600,0xFF848600,0xFF848600,0xFF6B7100,0xFF6B7100,0xFF6B7100,0xFF525D00,0xFF525D00,0xFF525D00,0xFF314900,0xFF314900,0xFF314900,0xFFF76900,0xFFF76900,0xFFF76900,0xFF9C4500,0xFF9C4500,0xFF9C4500,0xFF632800,0xFF632800,0xFF632800,0xFFBD7DCE,0xFFBD7DCE,0xFFBD7DCE,0xFF8C41A5,0xFF8C41A5,0xFF8C41A5,0xFF52285A,0xFF52285A,0xFF52285A,0xFF081C39,0xFF081C39,0xFF081C39,0xFF082042,0xFF082042,0xFF082042,0xFF08244A,0xFF08244A,0xFF08244A,0xFF082852,0xFF082852,0xFF082852,0xFF082C5A,0xFF082C5A,0xFF082C5A,0xFF083063,0xFF083063,0xFF083063,0xFF083863,0xFF083863,0xFF083863,0xFF083C6B,0xFF083C6B,0xFF083C6B,0xFF084573,0xFF084573,0xFF084573,0xFF08497B,0xFF08497B,0xFF08497B,0xFF085184,0xFF085184,0xFF085184,0xFF08619C,0xFF08619C,0xFF08619C,0xFF0875AD,0xFF0875AD,0xFF0875AD,0xFF08558C,0xFF08558C,0xFF08558C,0xFF08386B,0xFF08386B,0xFF08386B,0xFF08244A,0xFF08244A,0xFF08244A,0xFFB55D00,0xFFB55D00,0xFFB55D00,0xFFAD4500,0xFFAD4500,0xFFAD4500,0xFFA52C00,0xFFA52C00,0xFFA52C00,0xFF9C1800,0xFF9C1800,0xFF9C1800,0xFF941400,0xFF941400,0xFF941400,0xFF841008,0xFF841008,0xFF841008,0xFF731010,0xFF731010,0xFF731010,0xFF5A0C10,0xFF5A0C10,0xFF5A0C10,0xFF520818,0xFF520818,0xFF520818,0xFF420818,0xFF420818,0xFF420818,0xFF390421,0xFF390421,0xFF390421,0xFF290421,0xFF290421,0xFF290421,0xFF180021,0xFF180021,0xFF180021,0xFF180008,0xFF180008,0xFF180008,0xFF080000,0xFF080000,0xFF080000,0xFF000000,0xFF000000,0xFF000000,0xFF1055CE,0xFF1055CE,0xFF1055CE,0xFF084DB5,0xFF084DB5,0xFF084DB5,0xFF0845A5,0xFF0845A5,0xFF0845A5,0xFF003C94,0xFF003C94,0xFF003C94,0xFF003484,0xFF003484,0xFF003484,0xFF002C73,0xFF002C73,0xFF002C73,0xFF002863,0xFF002863,0xFF002863,0xFF00245A,0xFF00245A,0xFF00245A,0xFF00184A,0xFF00184A,0xFF00184A,0xFF001442,0xFF001442,0xFF001442,0xFF000C39,0xFF000C39,0xFF000C39,0xFF000831,0xFF000831,0xFF000831,0xFF000429,0xFF000429,0xFF000429,0xFF000021,0xFF000021,0xFF000021,0xFF000018,0xFF000018,0xFF000018,0xFF000010,0xFF000010,0xFF000010,0xFF104D8C,0xFF104D8C,0xFF104D8C,0xFF103884,0xFF103884,0xFF103884,0xFF08287B,0xFF08287B,0xFF08287B,0xFF081C6B,0xFF081C6B,0xFF081C6B,0xFF081063,0xFF081063,0xFF081063,0xFF00045A,0xFF00045A,0xFF00045A,0xFF080452,0xFF080452,0xFF080452,0xFF10044A,0xFF10044A,0xFF10044A,0xFF080442,0xFF080442,0xFF080442,0xFF080439,0xFF080439,0xFF080439,0xFF080029,0xFF080029,0xFF080029,0xFF000021,0xFF000021,0xFF000021,0xFF000018,0xFF000018,0xFF000018,0xFF000010,0xFF000010,0xFF000010,0xFF000000,0xFF000000,0xFF000000,0xFF000000,0xFF000000,0xFF000000,0xFFADFF00,0xFFADFF00,0xFFADFF00,0xFF94EF00,0xFF94EF00,0xFF94EF00,0xFF84DF00,0xFF84DF00,0xFF84DF00,0xFF73CF00,0xFF73CF00,0xFF73CF00,0xFF63C300,0xFF63C300,0xFF63C300,0xFF52B200,0xFF52B200,0xFF52B200,0xFF42A200,0xFF42A200,0xFF42A200,0xFF399600,0xFF399600,0xFF399600,0xFF218A00,0xFF218A00,0xFF218A00,0xFF107900,0xFF107900,0xFF107900,0xFF006D00,0xFF006D00,0xFF006D00,0xFF005D00,0xFF005D00,0xFF005D00,0xFF004D08,0xFF004D08,0xFF004D08,0xFF004108,0xFF004108,0xFF004108,0xFF003010,0xFF003010,0xFF003010,0xFF002410,0xFF002410,0xFF002410,0xFFADDBE7,0xFFADDBE7,0xFFADDBE7,0xFF29A2D6,0xFF29A2D6,0xFF29A2D6,0xFF188AC6,0xFF188AC6,0xFF188AC6,0xFF00457B,0xFF00457B,0xFF00457B,0xFF000821,0xFF000821,0xFF000821,0xFF18DF8C,0xFF18DF8C,0xFF18DF8C,0xFF10BA73,0xFF10BA73,0xFF10BA73,0xFF109652,0xFF109652,0xFF109652,0xFF087139,0xFF087139,0xFF087139,0xFF084D29,0xFF084D29,0xFF084D29,0xFF002C10,0xFF002C10,0xFF002C10,0xFF000429,0xFF000429,0xFF000429,0xFFFFFFA5,0xFFFFFFA5,0xFFFFFFA5,0xFFDEEB8C,0xFFDEEB8C,0xFFDEEB8C,0xFFC6D77B,0xFFC6D77B,0xFFC6D77B,0xFFA5C36B,0xFFA5C36B,0xFFA5C36B, };
    uint32_t pal2[768] = { 0x00000000,0xFF000000,0xFF000000,0xFFF7EFEF,0xFFF7EFEF,0xFFF7EFEF,0xFFDECFD6,0xFFDECFD6,0xFFDECFD6,0xFFC6B2BD,0xFFC6B2BD,0xFFC6B2BD,0xFFA592A5,0xFFA592A5,0xFFA592A5,0xFF8C7194,0xFF8C7194,0xFF8C7194,0xFF73517B,0xFF73517B,0xFF73517B,0xFF523463,0xFF523463,0xFF523463,0xFF39144A,0xFF39144A,0xFF39144A,0xFFFF1818,0xFFFF1818,0xFFFF1818,0xFFDE1010,0xFFDE1010,0xFFDE1010,0xFFB50C08,0xFFB50C08,0xFFB50C08,0xFF8C0808,0xFF8C0808,0xFF8C0808,0xFF6B0000,0xFF6B0000,0xFF6B0000,0xFFC60000,0xFFC60000,0xFFC60000,0xFFFF00FF,0xFFFF00FF,0xFFFF00FF,0xFFFFFFC6,0xFFFFFFC6,0xFFFFFFC6,0xFFEFF3BD,0xFFEFF3BD,0xFFEFF3BD,0xFFDEE3B5,0xFFDEE3B5,0xFFDEE3B5,0xFFCED7AD,0xFFCED7AD,0xFFCED7AD,0xFFBDCBA5,0xFFBDCBA5,0xFFBDCBA5,0xFFADBE9C,0xFFADBE9C,0xFFADBE9C,0xFF9CB294,0xFF9CB294,0xFF9CB294,0xFF94A68C,0xFF94A68C,0xFF94A68C,0xFF849A84,0xFF849A84,0xFF849A84,0xFF7B9273,0xFF7B9273,0xFF7B9273,0xFF73866B,0xFF73866B,0xFF73866B,0xFF637D63,0xFF637D63,0xFF637D63,0xFF5A755A,0xFF5A755A,0xFF5A755A,0xFF526D52,0xFF526D52,0xFF526D52,0xFF4A654A,0xFF4A654A,0xFF4A654A,0xFF425D42,0xFF425D42,0xFF425D42,0xFF395539,0xFF395539,0xFF395539,0xFF314D31,0xFF314D31,0xFF314D31,0xFF294529,0xFF294529,0xFF294529,0xFF213C21,0xFF213C21,0xFF213C21,0xFF183418,0xFF183418,0xFF183418,0xFF182C18,0xFF182C18,0xFF182C18,0xFF102410,0xFF102410,0xFF102410,0xFF081C08,0xFF081C08,0xFF081C08,0xFF081408,0xFF081408,0xFF081408,0xFFFFF3BD,0xFFFFF3BD,0xFFFFF3BD,0xFFFFEBA5,0xFFFFEBA5,0xFFFFEBA5,0xFFFFE38C,0xFFFFE38C,0xFFFFE38C,0xFFFFDB73,0xFFFFDB73,0xFFFFDB73,0xFFFFCF5A,0xFFFFCF5A,0xFFFFCF5A,0xFFFFC742,0xFFFFC742,0xFFFFC742,0xFFFFBA31,0xFFFFBA31,0xFFFFBA31,0xFFE7EFB5,0xFFE7EFB5,0xFFE7EFB5,0xFFCEDF9C,0xFFCEDF9C,0xFFCEDF9C,0xFFADD384,0xFFADD384,0xFFADD384,0xFF8CC773,0xFF8CC773,0xFF8CC773,0xFF6BB663,0xFF6BB663,0xFF6BB663,0xFF52AA5A,0xFF52AA5A,0xFF52AA5A,0xFF429E63,0xFF429E63,0xFF429E63,0xFF318E5A,0xFF318E5A,0xFF318E5A,0xFF29825A,0xFF29825A,0xFF29825A,0xFF21755A,0xFF21755A,0xFF21755A,0xFF106552,0xFF106552,0xFF106552,0xFF085952,0xFF085952,0xFF085952,0xFF08494A,0xFF08494A,0xFF08494A,0xFF003439,0xFF003439,0xFF003439,0xFF002031,0xFF002031,0xFF002031,0xFF001421,0xFF001421,0xFF001421,0xFFFFF3BD,0xFFFFF3BD,0xFFFFF3BD,0xFFF7DF9C,0xFFF7DF9C,0xFFF7DF9C,0xFFF7C384,0xFFF7C384,0xFFF7C384,0xFFEFA263,0xFFEFA263,0xFFEFA263,0xFFEF7D4A,0xFFEF7D4A,0xFFEF7D4A,0xFFE75531,0xFFE75531,0xFFE75531,0xFFE72818,0xFFE72818,0xFFE72818,0xFFDE0410,0xFFDE0410,0xFFDE0410,0xFFC60410,0xFFC60410,0xFFC60410,0xFFAD0010,0xFFAD0010,0xFFAD0010,0xFF940010,0xFF940010,0xFF940010,0xFF7B0010,0xFF7B0010,0xFF7B0010,0xFF630010,0xFF630010,0xFF630010,0xFF4A0008,0xFF4A0008,0xFF4A0008,0xFF310008,0xFF310008,0xFF310008,0xFF180000,0xFF180000,0xFF180000,0xFFFFFF94,0xFFFFFF94,0xFFFFFF94,0xFFC6E36B,0xFFC6E36B,0xFFC6E36B,0xFF8CCB4A,0xFF8CCB4A,0xFF8CCB4A,0xFF52B231,0xFF52B231,0xFF52B231,0xFF219A18,0xFF219A18,0xFF219A18,0xFF088218,0xFF088218,0xFF088218,0xFF006929,0xFF006929,0xFF006929,0xFFE7FFFF,0xFFE7FFFF,0xFFE7FFFF,0xFFD6F3F7,0xFFD6F3F7,0xFFD6F3F7,0xFFC6E7E7,0xFFC6E7E7,0xFFC6E7E7,0xFFB5DBDE,0xFFB5DBDE,0xFFB5DBDE,0xFFA5CFCE,0xFFA5CFCE,0xFFA5CFCE,0xFF94C3C6,0xFF94C3C6,0xFF94C3C6,0xFF84BABD,0xFF84BABD,0xFF84BABD,0xFF7BB2B5,0xFF7BB2B5,0xFF7BB2B5,0xFF6BAAAD,0xFF6BAAAD,0xFF6BAAAD,0xFF5AA2A5,0xFF5AA2A5,0xFF5AA2A5,0xFF529A9C,0xFF529A9C,0xFF529A9C,0xFF4A9294,0xFF4A9294,0xFF4A9294,0xFF398A8C,0xFF398A8C,0xFF398A8C,0xFF318284,0xFF318284,0xFF318284,0xFF29797B,0xFF29797B,0xFF29797B,0xFF217173,0xFF217173,0xFF217173,0xFF18696B,0xFF18696B,0xFF18696B,0xFF105D63,0xFF105D63,0xFF105D63,0xFF10555A,0xFF10555A,0xFF10555A,0xFF084D4A,0xFF084D4A,0xFF084D4A,0xFF004542,0xFF004542,0xFF004542,0xFF003839,0xFF003839,0xFF003839,0xFF003031,0xFF003031,0xFF003031,0xFF002829,0xFF002829,0xFF002829,0xFF002021,0xFF002021,0xFF002021,0xFFA5CFFF,0xFFA5CFFF,0xFFA5CFFF,0xFF84B6E7,0xFF84B6E7,0xFF84B6E7,0xFF639ECE,0xFF639ECE,0xFF639ECE,0xFF4A8AB5,0xFF4A8AB5,0xFF4A8AB5,0xFF39799C,0xFF39799C,0xFF39799C,0xFF296584,0xFF296584,0xFF296584,0xFF185973,0xFF185973,0xFF185973,0xFF104D6B,0xFF104D6B,0xFF104D6B,0xFF08415A,0xFF08415A,0xFF08415A,0xFF00384A,0xFF00384A,0xFF00384A,0xFF002C39,0xFF002C39,0xFF002C39,0xFF002431,0xFF002431,0xFF002431,0xFF002029,0xFF002029,0xFF002029,0xFF001C21,0xFF001C21,0xFF001C21,0xFF001818,0xFF001818,0xFF001818,0xFF001418,0xFF001418,0xFF001418,0xFFCEFFFF,0xFFCEFFFF,0xFFCEFFFF,0xFFB5F3F7,0xFFB5F3F7,0xFFB5F3F7,0xFF9CEBEF,0xFF9CEBEF,0xFF9CEBEF,0xFF8CDFE7,0xFF8CDFE7,0xFF8CDFE7,0xFF73D3DE,0xFF73D3DE,0xFF73D3DE,0xFF63C7DE,0xFF63C7DE,0xFF63C7DE,0xFF52BED6,0xFF52BED6,0xFF52BED6,0xFF4AB2CE,0xFF4AB2CE,0xFF4AB2CE,0xFF39A2C6,0xFF39A2C6,0xFF39A2C6,0xFF3196BD,0xFF3196BD,0xFF3196BD,0xFF218AB5,0xFF218AB5,0xFF218AB5,0xFF187DAD,0xFF187DAD,0xFF187DAD,0xFF1071A5,0xFF1071A5,0xFF1071A5,0xFF08659C,0xFF08659C,0xFF08659C,0xFF005994,0xFF005994,0xFF005994,0xFF005194,0xFF005194,0xFF005194,0xFFFF86FF,0xFFFF86FF,0xFFFF86FF,0xFFEF75F7,0xFFEF75F7,0xFFEF75F7,0xFFD665E7,0xFFD665E7,0xFFD665E7,0xFFC659DE,0xFFC659DE,0xFFC659DE,0xFFB54DCE,0xFFB54DCE,0xFFB54DCE,0xFF9C41C6,0xFF9C41C6,0xFF9C41C6,0xFF8C34B5,0xFF8C34B5,0xFF8C34B5,0xFF7B2CAD,0xFF7B2CAD,0xFF7B2CAD,0xFF6B209C,0xFF6B209C,0xFF6B209C,0xFF5A1894,0xFF5A1894,0xFF5A1894,0xFF4A1484,0xFF4A1484,0xFF4A1484,0xFF390C7B,0xFF390C7B,0xFF390C7B,0xFF29086B,0xFF29086B,0xFF29086B,0xFF210463,0xFF210463,0xFF210463,0xFF100052,0xFF100052,0xFF100052,0xFF08004A,0xFF08004A,0xFF08004A,0xFFFFF7C6,0xFFFFF7C6,0xFFFFF7C6,0xFFFFEB7B,0xFFFFEB7B,0xFFFFEB7B,0xFFFFE339,0xFFFFE339,0xFFFFE339,0xFFFFD700,0xFFFFD700,0xFFFFD700,0xFFE7AA00,0xFFE7AA00,0xFFE7AA00,0xFFCE8600,0xFFCE8600,0xFFCE8600,0xFFB56500,0xFFB56500,0xFFB56500,0xFF945100,0xFF945100,0xFF945100,0xFF733C00,0xFF733C00,0xFF733C00,0xFF5A2C00,0xFF5A2C00,0xFF5A2C00,0xFF422800,0xFF422800,0xFF422800,0xFF312000,0xFF312000,0xFF312000,0xFF181C00,0xFF181C00,0xFF181C00,0xFF081400,0xFF081400,0xFF081400,0xFF001000,0xFF001000,0xFF001000,0xFF000800,0xFF000800,0xFF000800,0xFFF7BAA5,0xFFF7BAA5,0xFFF7BAA5,0xFFE7A284,0xFFE7A284,0xFFE7A284,0xFFDE8E63,0xFFDE8E63,0xFFDE8E63,0xFFD67D42,0xFFD67D42,0xFFD67D42,0xFFCE6D29,0xFFCE6D29,0xFFCE6D29,0xFFBD6110,0xFFBD6110,0xFFBD6110,0xFFB55900,0xFFB55900,0xFFB55900,0xFFA54D00,0xFFA54D00,0xFFA54D00,0xFF944500,0xFF944500,0xFF944500,0xFF843800,0xFF843800,0xFF843800,0xFF6B3000,0xFF6B3000,0xFF6B3000,0xFF5A2800,0xFF5A2800,0xFF5A2800,0xFF4A2000,0xFF4A2000,0xFF4A2000,0xFF391800,0xFF391800,0xFF391800,0xFF291000,0xFF291000,0xFF291000,0xFF180800,0xFF180800,0xFF180800,0xFFF7FFFF,0xFFF7FFFF,0xFFF7FFFF,0xFFD6EBEF,0xFFD6EBEF,0xFFD6EBEF,0xFFBDD7D6,0xFFBDD7D6,0xFFBDD7D6,0xFFA5C3C6,0xFFA5C3C6,0xFFA5C3C6,0xFF8CAEAD,0xFF8CAEAD,0xFF8CAEAD,0xFF7B9A9C,0xFF7B9A9C,0xFF7B9A9C,0xFF638684,0xFF638684,0xFF638684,0xFF527573,0xFF527573,0xFF527573,0xFF426563,0xFF426563,0xFF426563,0xFF31595A,0xFF31595A,0xFF31595A,0xFF214D4A,0xFF214D4A,0xFF214D4A,0xFF104142,0xFF104142,0xFF104142,0xFF083431,0xFF083431,0xFF083431,0xFF002829,0xFF002829,0xFF002829,0xFF001C18,0xFF001C18,0xFF001C18,0xFF001010,0xFF001010,0xFF001010,0xFFEFB694,0xFFEFB694,0xFFEFB694,0xFFD6A684,0xFFD6A684,0xFFD6A684,0xFFBD9A7B,0xFFBD9A7B,0xFFBD9A7B,0xFFAD8A73,0xFFAD8A73,0xFFAD8A73,0xFF947D63,0xFF947D63,0xFF947D63,0xFF846D5A,0xFF846D5A,0xFF846D5A,0xFF6B5D4A,0xFF6B5D4A,0xFF6B5D4A,0xFF5A4D42,0xFF5A4D42,0xFF5A4D42,0xFF4A4539,0xFF4A4539,0xFF4A4539,0xFF393C31,0xFF393C31,0xFF393C31,0xFF293429,0xFF293429,0xFF293429,0xFF182C21,0xFF182C21,0xFF182C21,0xFF082418,0xFF082418,0xFF082418,0xFF001C18,0xFF001C18,0xFF001C18,0xFF001410,0xFF001410,0xFF001410,0xFF000C08,0xFF000C08,0xFF000C08,0xFFCE7994,0xFFCE7994,0xFFCE7994,0xFFBD718C,0xFFBD718C,0xFFBD718C,0xFFAD697B,0xFFAD697B,0xFFAD697B,0xFF9C6173,0xFF9C6173,0xFF9C6173,0xFF8C596B,0xFF8C596B,0xFF8C596B,0xFF7B5563,0xFF7B5563,0xFF7B5563,0xFF6B4D5A,0xFF6B4D5A,0xFF6B4D5A,0xFF5A4552,0xFF5A4552,0xFF5A4552,0xFF4A3C4A,0xFF4A3C4A,0xFF4A3C4A,0xFF393442,0xFF393442,0xFF393442,0xFF293039,0xFF293039,0xFF293039,0xFF182831,0xFF182831,0xFF182831,0xFF082029,0xFF082029,0xFF082029,0xFF001821,0xFF001821,0xFF001821,0xFF001010,0xFF001010,0xFF001010,0xFF000808,0xFF000808,0xFF000808,0xFFF7FBFF,0xFFF7FBFF,0xFFF7FBFF,0xFFDEE7F7,0xFFDEE7F7,0xFFDEE7F7,0xFFC6D3E7,0xFFC6D3E7,0xFFC6D3E7,0xFFA5BEDE,0xFFA5BEDE,0xFFA5BEDE,0xFF94A6D6,0xFF94A6D6,0xFF94A6D6,0xFF7B92C6,0xFF7B92C6,0xFF7B92C6,0xFF6382BD,0xFF6382BD,0xFF6382BD,0xFF526DB5,0xFF526DB5,0xFF526DB5,0xFF4259A5,0xFF4259A5,0xFF4259A5,0xFF31459C,0xFF31459C,0xFF31459C,0xFF213494,0xFF213494,0xFF213494,0xFF18248C,0xFF18248C,0xFF18248C,0xFF08147B,0xFF08147B,0xFF08147B,0xFF000873,0xFF000873,0xFF000873,0xFF00006B,0xFF00006B,0xFF00006B,0xFF6B7173,0xFF6B7173,0xFF6B7173, };
    uint32_t pal3[768] = { 0x00000000,0xFF000000,0xFF000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFE7E3E7,0xFFE7E3E7,0xFFE7E3E7,0xFFBDBED6,0xFFBDBED6,0xFFBDBED6,0xFF8486B5,0xFF8486B5,0xFF8486B5,0xFF526D94,0xFF526D94,0xFF526D94,0xFF292C52,0xFF292C52,0xFF292C52,0xFF101429,0xFF101429,0xFF101429,0xFF000000,0xFF000000,0xFF000000,0xFFFF1818,0xFFFF1818,0xFFFF1818,0xFFDE1010,0xFFDE1010,0xFFDE1010,0xFFB50C08,0xFFB50C08,0xFFB50C08,0xFF8C0808,0xFF8C0808,0xFF8C0808,0xFF6B0000,0xFF6B0000,0xFF6B0000,0xFF292829,0xFF292829,0xFF292829,0xFF181818,0xFF181818,0xFF181818,0xFFFFFF00,0xFFFFFF00,0xFFFFFF00,0xFFDED300,0xFFDED300,0xFFDED300,0xFFB5AA00,0xFFB5AA00,0xFFB5AA00,0xFF948200,0xFF948200,0xFF948200,0xFF6B5D00,0xFF6B5D00,0xFF6B5D00,0xFF4A3C00,0xFF4A3C00,0xFF4A3C00,0xFF635500,0xFF635500,0xFF635500,0xFF4A3C00,0xFF4A3C00,0xFF4A3C00,0xFFFF0000,0xFFFF0000,0xFFFF0000,0xFFD60000,0xFFD60000,0xFFD60000,0xFFB50000,0xFFB50000,0xFFB50000,0xFF8C0000,0xFF8C0000,0xFF8C0000,0xFF630000,0xFF630000,0xFF630000,0xFF420000,0xFF420000,0xFF420000,0xFF5A0000,0xFF5A0000,0xFF5A0000,0xFF420000,0xFF420000,0xFF420000,0xFF00FF00,0xFF00FF00,0xFF00FF00,0xFF00D700,0xFF00D700,0xFF00D700,0xFF00B200,0xFF00B200,0xFF00B200,0xFF008A00,0xFF008A00,0xFF008A00,0xFF006500,0xFF006500,0xFF006500,0xFF004100,0xFF004100,0xFF004100,0xFF005900,0xFF005900,0xFF005900,0xFF004100,0xFF004100,0xFF004100,0xFF007DFF,0xFF007DFF,0xFF007DFF,0xFF0065D6,0xFF0065D6,0xFF0065D6,0xFF004DAD,0xFF004DAD,0xFF004DAD,0xFF00347B,0xFF00347B,0xFF00347B,0xFF002052,0xFF002052,0xFF002052,0xFF001029,0xFF001029,0xFF001029,0xFF001C4A,0xFF001C4A,0xFF001C4A,0xFF001029,0xFF001029,0xFF001029,0xFF00FFFF,0xFF00FFFF,0xFF00FFFF,0xFF00D7D6,0xFF00D7D6,0xFF00D7D6,0xFF00AEAD,0xFF00AEAD,0xFF00AEAD,0xFF008684,0xFF008684,0xFF008684,0xFF005D5A,0xFF005D5A,0xFF005D5A,0xFF003839,0xFF003839,0xFF003839,0xFF005552,0xFF005552,0xFF005552,0xFF003839,0xFF003839,0xFF003839,0xFFFF86D6,0xFFFF86D6,0xFFFF86D6,0xFFD661A5,0xFFD661A5,0xFFD661A5,0xFFA54584,0xFFA54584,0xFFA54584,0xFF7B2C5A,0xFF7B2C5A,0xFF7B2C5A,0xFF4A1839,0xFF4A1839,0xFF4A1839,0xFF210818,0xFF210818,0xFF210818,0xFF421429,0xFF421429,0xFF421429,0xFF210818,0xFF210818,0xFF210818,0xFFFF7100,0xFFFF7100,0xFFFF7100,0xFFD65500,0xFFD65500,0xFFD65500,0xFFAD3800,0xFFAD3800,0xFFAD3800,0xFF842400,0xFF842400,0xFF842400,0xFF5A1400,0xFF5A1400,0xFF5A1400,0xFF310800,0xFF310800,0xFF310800,0xFF521000,0xFF521000,0xFF521000,0xFF310800,0xFF310800,0xFF310800,0xFFFFAE5A,0xFFFFAE5A,0xFFFFAE5A,0xFFDE924A,0xFFDE924A,0xFFDE924A,0xFFBD7939,0xFFBD7939,0xFFBD7939,0xFF9C6129,0xFF9C6129,0xFF9C6129,0xFF7B4918,0xFF7B4918,0xFF7B4918,0xFF5A3410,0xFF5A3410,0xFF5A3410,0xFF6B4118,0xFF6B4118,0xFF6B4118,0xFF5A3410,0xFF5A3410,0xFF5A3410,0xFFBD79FF,0xFFBD79FF,0xFFBD79FF,0xFF9C5DDE,0xFF9C5DDE,0xFF9C5DDE,0xFF7B45BD,0xFF7B45BD,0xFF7B45BD,0xFF632C9C,0xFF632C9C,0xFF632C9C,0xFF4A1C7B,0xFF4A1C7B,0xFF4A1C7B,0xFF31105A,0xFF31105A,0xFF31105A,0xFF421873,0xFF421873,0xFF421873,0xFF31105A,0xFF31105A,0xFF31105A,0xFF84BAC6,0xFF84BAC6,0xFF84BAC6,0xFF6B9AA5,0xFF6B9AA5,0xFF6B9AA5,0xFF52798C,0xFF52798C,0xFF52798C,0xFF39596B,0xFF39596B,0xFF39596B,0xFF213C4A,0xFF213C4A,0xFF213C4A,0xFF102431,0xFF102431,0xFF102431,0xFF213442,0xFF213442,0xFF213442,0xFF102431,0xFF102431,0xFF102431,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFDEDBDE,0xFFDEDBDE,0xFFDEDBDE,0xFFB5B6B5,0xFFB5B6B5,0xFFB5B6B5,0xFF949294,0xFF949294,0xFF949294,0xFF6B6D6B,0xFF6B6D6B,0xFF6B6D6B,0xFF4A4D4A,0xFF4A4D4A,0xFF4A4D4A,0xFF636563,0xFF636563,0xFF636563,0xFF4A4D4A,0xFF4A4D4A,0xFF4A4D4A,0xFF18DF8C,0xFF18DF8C,0xFF18DF8C,0xFF10BA73,0xFF10BA73,0xFF10BA73,0xFF109652,0xFF109652,0xFF109652,0xFF087139,0xFF087139,0xFF087139,0xFF084D29,0xFF084D29,0xFF084D29,0xFF002C10,0xFF002C10,0xFF002C10,0xFF084521,0xFF084521,0xFF084521,0xFF002C10,0xFF002C10,0xFF002C10,0xFFE78263,0xFFE78263,0xFFE78263,0xFFBD614A,0xFFBD614A,0xFFBD614A,0xFF9C4131,0xFF9C4131,0xFF9C4131,0xFF732818,0xFF732818,0xFF732818,0xFF521408,0xFF521408,0xFF521408,0xFF310800,0xFF310800,0xFF310800,0xFF4A1008,0xFF4A1008,0xFF4A1008,0xFF310800,0xFF310800,0xFF310800,0xFFCEAACE,0xFFCEAACE,0xFFCEAACE,0xFFA586A5,0xFFA586A5,0xFFA586A5,0xFF846584,0xFF846584,0xFF846584,0xFF5A455A,0xFF5A455A,0xFF5A455A,0xFF392839,0xFF392839,0xFF392839,0xFF181018,0xFF181018,0xFF181018,0xFF312031,0xFF312031,0xFF312031,0xFF181018,0xFF181018,0xFF181018,0xFFC6A642,0xFFC6A642,0xFFC6A642,0xFFA58631,0xFFA58631,0xFFA58631,0xFF846921,0xFF846921,0xFF846921,0xFF634D10,0xFF634D10,0xFF634D10,0xFF423408,0xFF423408,0xFF423408,0xFF291C00,0xFF291C00,0xFF291C00,0xFF392C08,0xFF392C08,0xFF392C08,0xFF291C00,0xFF291C00,0xFF291C00,0xFFFF9694,0xFFFF9694,0xFFFF9694,0xFFD67173,0xFFD67173,0xFFD67173,0xFFA55152,0xFFA55152,0xFFA55152,0xFF7B3431,0xFF7B3431,0xFF7B3431,0xFF522021,0xFF522021,0xFF522021,0xFF290C08,0xFF290C08,0xFF290C08,0xFF421818,0xFF421818,0xFF421818,0xFF290C08,0xFF290C08,0xFF290C08,0xFFFFFF7B,0xFFFFFF7B,0xFFFFFF7B,0xFFEFE76B,0xFFEFE76B,0xFFEFE76B,0xFFE7CF5A,0xFFE7CF5A,0xFFE7CF5A,0xFFD6BA4A,0xFFD6BA4A,0xFFD6BA4A,0xFFB5AE00,0xFFB5AE00,0xFFB5AE00,0xFF9C9A00,0xFF9C9A00,0xFF9C9A00,0xFF848600,0xFF848600,0xFF848600,0xFF6B7100,0xFF6B7100,0xFF6B7100,0xFF525D00,0xFF525D00,0xFF525D00,0xFF314900,0xFF314900,0xFF314900,0xFFF76900,0xFFF76900,0xFFF76900,0xFF9C4500,0xFF9C4500,0xFF9C4500,0xFF632800,0xFF632800,0xFF632800,0xFFBD7DCE,0xFFBD7DCE,0xFFBD7DCE,0xFF8C41A5,0xFF8C41A5,0xFF8C41A5,0xFF52285A,0xFF52285A,0xFF52285A,0xFF004500,0xFF004500,0xFF004500,0xFF003800,0xFF003800,0xFF003800,0xFF002C00,0xFF002C00,0xFF002C00,0xFF002000,0xFF002000,0xFF002000,0xFF001800,0xFF001800,0xFF001800,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF636163,0xFF8CF7E7,0xFF8CF7E7,0xFF8CF7E7,0xFF7BE7D6,0xFF7BE7D6,0xFF7BE7D6,0xFF6BDBC6,0xFF6BDBC6,0xFF6BDBC6,0xFF63CBB5,0xFF63CBB5,0xFF63CBB5,0xFF52BEA5,0xFF52BEA5,0xFF52BEA5,0xFF4AAE94,0xFF4AAE94,0xFF4AAE94,0xFF39A284,0xFF39A284,0xFF39A284,0xFF31967B,0xFF31967B,0xFF31967B,0xFF004DA5,0xFF004DA5,0xFF004DA5,0xFF004194,0xFF004194,0xFF004194,0xFF003884,0xFF003884,0xFF003884,0xFF00307B,0xFF00307B,0xFF00307B,0xFF00286B,0xFF00286B,0xFF00286B,0xFF00205A,0xFF00205A,0xFF00205A,0xFF001852,0xFF001852,0xFF001852,0xFF001442,0xFF001442,0xFF001442,0xFF399EBD,0xFF399EBD,0xFF399EBD,0xFF297DAD,0xFF297DAD,0xFF297DAD,0xFF215D94,0xFF215D94,0xFF215D94,0xFF104584,0xFF104584,0xFF104584,0xFF082C6B,0xFF082C6B,0xFF082C6B,0xFF00185A,0xFF00185A,0xFF00185A,0xFF000442,0xFF000442,0xFF000442,0xFF000031,0xFF000031,0xFF000031,0xFF317129,0xFF317129,0xFF317129,0xFF296521,0xFF296521,0xFF296521,0xFF185D18,0xFF185D18,0xFF185D18,0xFF105510,0xFF105510,0xFF105510,0xFF084D08,0xFF084D08,0xFF084D08,0xFF004500,0xFF004500,0xFF004500,0xFF003C00,0xFF003C00,0xFF003C00,0xFF003400,0xFF003400,0xFF003400,0xFF004DC6,0xFF004DC6,0xFF004DC6,0xFF0049BD,0xFF0049BD,0xFF0049BD,0xFF0045B5,0xFF0045B5,0xFF0045B5,0xFF0045AD,0xFF0045AD,0xFF0045AD,0xFF0041AD,0xFF0041AD,0xFF0041AD,0xFF0041A5,0xFF0041A5,0xFF0041A5,0xFF003C9C,0xFF003C9C,0xFF003C9C,0xFF003894,0xFF003894,0xFF003894,0xFF003894,0xFF003894,0xFF003894,0xFF00348C,0xFF00348C,0xFF00348C,0xFF003484,0xFF003484,0xFF003484,0xFF00307B,0xFF00307B,0xFF00307B,0xFF002C7B,0xFF002C7B,0xFF002C7B,0xFF002C73,0xFF002C73,0xFF002C73,0xFF00286B,0xFF00286B,0xFF00286B,0xFF00286B,0xFF00286B,0xFF00286B,0xFFC6AADE,0xFFC6AADE,0xFFC6AADE,0xFFB59ECE,0xFFB59ECE,0xFFB59ECE,0xFFA592BD,0xFFA592BD,0xFFA592BD,0xFF9486B5,0xFF9486B5,0xFF9486B5,0xFF8479A5,0xFF8479A5,0xFF8479A5,0xFF736D9C,0xFF736D9C,0xFF736D9C,0xFF63658C,0xFF63658C,0xFF63658C,0xFF5A5D84,0xFF5A5D84,0xFF5A5D84,0xFF4A4D7B,0xFF4A4D7B,0xFF4A4D7B,0xFF424573,0xFF424573,0xFF424573,0xFF39386B,0xFF39386B,0xFF39386B,0xFF292C63,0xFF292C63,0xFF292C63,0xFF21205A,0xFF21205A,0xFF21205A,0xFF101452,0xFF101452,0xFF101452,0xFF08084A,0xFF08084A,0xFF08084A,0xFF000021,0xFF000021,0xFF000021,0xFF001442,0xFF001442,0xFF001442,0xFF082C63,0xFF082C63,0xFF082C63,0xFF184D8C,0xFF184D8C,0xFF184D8C,0xFF3975AD,0xFF3975AD,0xFF3975AD,0xFF639ECE,0xFF639ECE,0xFF639ECE,0xFF94CFF7,0xFF94CFF7,0xFF94CFF7,0xFFA5A2A5,0xFFA5A2A5,0xFFA5A2A5,0xFF848684,0xFF848684,0xFF848684,0xFF636563,0xFF636563,0xFF636563,0xFF4A494A,0xFF4A494A,0xFF4A494A,0xFF292829,0xFF292829,0xFF292829,0xFF080808,0xFF080808,0xFF080808,0xFF181C18,0xFF181C18,0xFF181C18,0xFF080808,0xFF080808,0xFF080808,0xFFC6D77B,0xFFC6D77B,0xFFC6D77B,0xFFA5C36B,0xFFA5C36B,0xFFA5C36B, };


    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        LayerPalettized* ret = new LayerPalettized(320, 200);
        ret->palette = g_palettes[PALETTE_DEFAULT];
        ret->name = "BDY Layer";
        uint32_t* pxd = (uint32_t*)ret->pixelData;
        uint32_t* end = pxd + (320 * 200);
        bool reachedImageEnd = false;
        while (!reachedImageEnd && !feof(f)) {
            uint8_t a;
            fread(&a, 1, 1, f);
            if (a >= 129) {
                uint8_t pixel;
                fread(&pixel, 1, 1, f);
                for (int x = 0; x < 257 - a; x++) {
                    *(pxd++) = (uint32_t)pixel;
                    if (pxd >= end) {
                        reachedImageEnd = true;
                        break;
                    }
                }
            }
            else {
                for (uint32_t x = 0; x < a+1; x++) {
                    uint8_t pixel;
                    fread(&pixel, 1, 1, f);
                    *(pxd++) = (uint32_t)pixel;
                    if (pxd >= end) {
                        reachedImageEnd = true;
                        break;
                    }
                }
            }
        }

        return ret;
    }
    return NULL;
}

Layer* readXComSCR(PlatformNativePathString path, uint64_t seek)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        LayerPalettized* ret = new LayerPalettized(320, 200);
        uint32_t* pxd = (uint32_t*)ret->pixelData;
        ret->palette = g_palettes[PALETTE_DEFAULT];
        for (int x = 0; x < 320 * 200; x++) {
            uint8_t pixel;
            fread(&pixel, 1, 1, f);
            pxd[x] = pixel;
        }
        fclose(f);
        return ret;
    }
    return NULL;
}

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
            ((LayerPalettized*)ret)->palette = g_palettes[PALETTE_DEFAULT];
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
            ((LayerPalettized*)ret)->palette = g_palettes[PALETTE_DEFAULT];;
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
            uint64_t dataPointer = 0;
            while (!f.eof() && dataPointer < w * h) {
                uint8_t r,g,b;
                f.read((char*)&r, 1);
                f.read((char*)&g, 1);
                f.read((char*)&b, 1);
                ret->setPixel({ (int)(dataPointer % w), (int)(dataPointer / w) }, PackRGBAtoARGB(r,g,b, 255));
                dataPointer++;
            }
        }
        f.close();
        return ret;
    }
    return NULL;
}

Layer* readXBM(PlatformNativePathString path, uint64_t seek) {
    std::ifstream f(path);
    if (f.is_open()) {
        int w = -1, h = -1;
        LayerPalettized* ret = NULL;
        while (ret == NULL && !f.eof()) {
            std::string line;
            std::getline(f, line);

            if (line.find("#define") == 0) {
                line = line.substr(line.find(' ')+1);
                std::string defname = line.substr(0, line.find(' '));
                std::string value = line.substr(line.find(' ')+1);
                if (stringEndsWithIgnoreCase(defname, "_width")) {
                    w = std::stoi(value);
                }
                else if (stringEndsWithIgnoreCase(defname, "_height")) {
                    h = std::stoi(value);
                }
                else {
                    logprintf("[XBM] invalid define: %s\n", defname.c_str());
                }

                if (w >= 0 && h >= 0) {
                    ret = new LayerPalettized(w, h);
                    ret->name = "XBM Layer";
                    ret->palette = { 0xFF000000, 0xFFFFFFFF };
                }
            }
        }
        if (ret != NULL) {
            uint64_t dataPointer = 0;
            while (!f.eof() && dataPointer < w * h) {
                std::string nextData;
                f >> nextData;
                while (nextData.find("0x") == 0) {
                    uint8_t b = std::stoi(nextData, 0, 16);
                    for (int x = 0; x < ixmin(w, 8); x++) {
                        XY position = { dataPointer % w + x, dataPointer / w };
                        ret->setPixel(position, (b & 1) ? 0 : 1);
                        b >>= 1;

                        if (dataPointer >= w * h) {
                            break;
                        }
                    }
                    dataPointer += ixmin(w, 8);

                    if (nextData.find(",") != std::string::npos) {
                        nextData = nextData.substr(nextData.find(",")+1);
                    }
                    else {
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

Layer* readSR8(PlatformNativePathString path, uint64_t seek)
{
    //16x16
    //bytes are sequential
    //bytes are in format AIBBGGRR
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        LayerPalettized* l = new LayerPalettized(16, 16);
        l->name = "SR8 Layer";
        /*std::vector<u32> pal;
        for (int renderColor = 0; renderColor < 256; renderColor++) {
            uint8_t byte = renderColor;
            uint8_t a, i, r, g, b;
            a = (byte & 0b10000000) != 0 ? 255 : 0;
            i = (byte & 0b01000000) != 0 ? 33 : 0;
            r = (byte & 0b11) * 74 + i;
            g = ((byte >> 2) & 0b11) * 74 + i;
            b = ((byte >> 4) & 0b11) * 74 + i;
            pal.push_back((a << 24) + (r << 16) + (g << 8) + b);
        }
        l->palette = pal;*/
        l->palette = g_palettes["Slim Render (8-bit)"];

        u32* ppx = (u32*)l->pixelData;
        for (int dataPointer = 0; dataPointer < l->w * l->h; dataPointer++) {
            uint8_t byte;
            fread(&byte, 1, 1, f);
            ppx[dataPointer] = byte;
        }

        fclose(f);
        return l;
    }
    return NULL;
}

Layer* readVOID9SP(PlatformNativePathString path, uint64_t seek)
{
    auto nsp = read9SegmentPattern(path);
    if (nsp.first) {
        Layer* nlayer = new Layer(nsp.second.dimensions.x, nsp.second.dimensions.y);
        memcpy(nlayer->pixelData, nsp.second.pixelData, nlayer->w * nlayer->h * 4);
        tracked_free(nsp.second.pixelData);
        nlayer->name = "Pattern Layer";
        return nlayer;
    }
    return NULL;
}

Layer* readPS2ICN(PlatformNativePathString path, uint64_t seek)
{
    struct PS2IcnHeader {
        u32 iconFileID; // == 0x010000
        u32 animShapes;
        u32 textureType; // 0x07 is uncompressed
        u32 unknown;    // should be 0x3f800000
        u32 verts;
    };

    struct PS2IcnAnimationHeader {
        u32 idTag;
        u32 frameLength;
        u32 animSpeed;
        u32 playOffset;
        u32 numberOfFrames;
    };

    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        Layer* ret = NULL;
        PS2IcnHeader header;
        fread(&header, sizeof(PS2IcnHeader), 1, f);
        //std::reverse(&header.textureType, &header.textureType + 1);
        //std::reverse(&header.verts, &header.verts + 1);
        logprintf("[PS2ICN] texture type: %x\n", header.textureType);
        logprintf("[PS2ICN] verts: %x\n", header.verts);
        
        int sizeofVertexStruct =
            header.animShapes * 8   //vertex coordinates
            + 8 // normal coordinates
            + 8; // texture coordinates+color
        fseek(f, header.verts * sizeofVertexStruct, SEEK_CUR);

        PS2IcnAnimationHeader animHeader;
        fread(&animHeader, sizeof(PS2IcnAnimationHeader), 1, f);
        logprintf("[PS2ICN] num. anim frames: %x\n", animHeader.numberOfFrames);
        for (int x = 0; x < animHeader.numberOfFrames; x++) {
            fseek(f, 4, SEEK_CUR);
            u32 numberOfKeys;
            fread(&numberOfKeys, 4, 1, f);
            fseek(f, 8 + 8 * numberOfKeys, SEEK_CUR);
        }

        logprintf("[PS2ICN] texture segment start: %x\n", ftell(f));
        //aand we have arrived at the `Texture segment` just look at that view
        ret = new Layer(128, 128);
        ret->name = "PS2 ICN Layer";
        u32* ppx = (u32*)ret->pixelData;

        if (header.textureType == 0x07) {
            
            u16 px16;
            for (u64 pxPtr = 0; pxPtr < 128 * 128; pxPtr++) {
                fread(&px16, 2, 1, f);
                //std::reverse(&px16, &px16 + 1);
                u8 r = 8 * (px16 & 0x1f);
                u8 g = 8 * ((px16 >> 5) & 0x1f);
                u8 b = 8 * (px16 >> 10);
                u8 a = 255;
                ppx[pxPtr] = PackRGBAtoARGB(r, g, b, a);
            }
        }
        else {
            //g_addNotification(ErrorNotification("PS2 ICN error", std::format("Unsupported compression type: {}", header.textureType)));

            u16 sizeOfCompressedTextureData;
            u64 pxPtr = 0;
            fread(&sizeOfCompressedTextureData, 2, 1, f);
            sizeOfCompressedTextureData = BEtoLE16(sizeOfCompressedTextureData);
            logprintf("[PS2ICN] texture size: %x\n", sizeOfCompressedTextureData);
            while (pxPtr < 128 * 128 && !feof(f)) {
                u16 code;
                fread(&code, 2,1, f);
                code = BEtoLE16(code);
                if (code < 0xFF00) {
                    u16 data;
                    fread(&data, 2, 1, f);
                    data = BEtoLE16(data);
                    u8 r = 8 * (data & 0x1f);
                    u8 g = 8 * ((data >> 5) & 0x1f);
                    u8 b = 8 * (data >> 10);
                    for (u32 x = 0; x < data && pxPtr < 128 * 128; x++) {
                        ppx[pxPtr++] = PackRGBAtoARGB(r, g, b, 255);
                    }
                }
                else {
                    u16 dataLength = 0xFFFF - code;
                    for (u32 x = 0; x < dataLength && pxPtr < 128 * 128; x++) {
                        u16 data;
                        fread(&data, 2, 1, f);
                        data = BEtoLE16(data);
                        u8 r = 8 * (data & 0x1f);
                        u8 g = 8 * ((data >> 5) & 0x1f);
                        u8 b = 8 * (data >> 10);
                        ppx[pxPtr++] = PackRGBAtoARGB(r, g, b, 255);
                    }
                }
            }
        }

        fclose(f);
        return ret;
    }
    return NULL;
}

Layer* readNDSBanner(PlatformNativePathString path, uint64_t seek)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        u32 iconOffsetAddress = 0x68;
        fseek(f, iconOffsetAddress, SEEK_SET);
        u32 iconOffset = 0;
        //24-byte big endian
        fread(&iconOffset, 4, 1, f);
        //iconOffset = BEtoLE32(iconOffset);

        logprintf("[NDS] icon offset: %x\n", iconOffset);
        u32 bitmapStart = 0x20 + iconOffset;

        u8* pixelData = (u8*)tracked_malloc(32 * 32);

        fseek(f, bitmapStart, SEEK_SET);
        //u32* pxd = (u32*)ret->pixelData;
        for (int x = 0; x < 0x200; x++) {
            u8 pixel;
            fread(&pixel, 1, 1, f);
            pixelData[x] = pixel;
        }

        std::vector<u32> colorPalette;
        fseek(f, iconOffset + 0x220, SEEK_SET);
        for (int x = 0; x < 16; x++) {
            u16 color;
            fread(&color, 2, 1, f);
            u32 color32 = BGR555toARGB8888(color);
            colorPalette.push_back(color32);
        }
        colorPalette[0] &= 0x00FFFFFF;

        LayerPalettized* ret = new LayerPalettized(32, 32);
        ret->palette = colorPalette;
        ret->name = "NDS Banner Layer";

        for (int chunk = 0; chunk < 16; chunk++) {
            for (int pixelIndex = 0; pixelIndex < 32; pixelIndex++) {
                u8 pixel = pixelData[chunk * 32 + pixelIndex];
                XY position = {
                    ((chunk * 8) % 32) + ((pixelIndex * 2) % 8),
                    (chunk / 4) * 8 + (pixelIndex / 4)
                };

                u8 pxLow = pixel & 0xF;
                u8 pxHigh = (pixel >> 4) & 0xF;

                ret->setPixel(position, pxLow);
                ret->setPixel({ position.x + 1, position.y }, pxHigh);
            }
        }

        tracked_free(pixelData);
        fclose(f);
        return ret;
    }
    return NULL;
}

Layer* read3DSCXIIcon(PlatformNativePathString path, uint64_t seek)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        NCCHHeader header;
        fread(&header, sizeof(NCCHHeader), 1, f);
        ExeFSHeader exefs;
        fseek(f, header.exefsOffset * 0x200, SEEK_SET);
        fread(&exefs, sizeof(ExeFSHeader), 1, f);

        Layer* ret = NULL;
        for (ExeFSFileHeader& exefsfile : exefs.fileHeaders) {
            if (memcmp(exefsfile.fileName, "icon", 4) == 0) {
                fseek(f, exefsfile.fileOffset + 0xE00, SEEK_SET);
                u32 smdhMagic;
                fread(&smdhMagic, 4, 1, f);
                fseek(f, exefsfile.fileOffset + 0xE00 + 0x24C0, SEEK_SET);
                u16 rgb565Data[48 * 48];
                fread(rgb565Data, 2, 48 * 48, f);

                Detiler detiler(2);
                ret = new Layer(48, 48);
                ret->name = "3DS CXI Icon Layer";
                u32* pxd = (u32*)ret->pixelData;
                for (int x = 0; x < 48 * 48; x++) {
                    ret->setPixel(detiler.next(), RGB565toARGB8888(rgb565Data[x]));
                }
                break;
            }
        }
        fclose(f);
        return ret;
    }
    return NULL;
}

Layer* readGIF(PlatformNativePathString path, u64 seek)
{
    u8 IMAGE_SEPERATOR_MAGIC = 0x2C;
    u8 EXTENSION_INTRODUCER_MAGIC = 0x21;
    u8 GIF_TRAILER_MAGIC = 0x3B;

    const u8 LABEL_GC = 0xF9;
    const u8 LABEL_COMMENT = 0xFE;
    const u8 LABEL_APPLICATION = 0xFF;
    const u8 LABEL_PLAINTEXT = 0x01;

    struct GIFGCTFlags {
        u8 size : 3;
        u8 sort : 1;
        u8 colorRes : 3;
        u8 enabled : 1;
    };
    struct GIFLogicalScreenDescriptor {
        u16 width;
        u16 height;
        GIFGCTFlags gctFlags;
        u8 bgColorIndex;
        u8 pixelAspect;
    };
    struct GIFImageDescriptorFlags{
        u8 lctSize : 3;
        u8 reserved : 2;
        u8 lctSort : 1;
        u8 interlaceFlag : 1;
        u8 lctEnable : 1;
    };
    struct GIFImageDescriptor {
        u16 imageLeftPosition;
        u16 imageTopPosition;
        u16 imageWidth;
        u16 imageHeight;
        GIFImageDescriptorFlags flags;
    };

    struct GCE_Flags {
        u8 transparent : 1;
        u8 userInput : 1;
        u8 disposalMode : 3;
        u8 reserved : 3;
    };

    struct GIFGraphicControlExtension {
        u8 blockSize;
        GCE_Flags flags;
        u16 delay;
        u8 transparentColorIndex;
    };
    
    struct GIFApplicationExtension {
        u8 blockSize;
        char identifier[8];
        char authenticationCode[3];
    };

    struct GIFPlainTextExtension {
        u8 blockSize;
        u16 textGridLeftPos;
        u16 textGridTopPos;
        u16 textGridWidth;
        u16 textGridHeight;
        u8 charCellWidth;
        u8 charCellHeight;
        u8 textForegroundColorIndex;
        u8 textBackgroundColorIndex;
    };

    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        u8 magic[3];
        u8 version[3];
        fread(magic, 1, 3, f);
        fread(version, 1, 3, f);

        //magic should == "GIF"
        GIFLogicalScreenDescriptor lsd;
        fread(&lsd, sizeof(GIFLogicalScreenDescriptor), 1, f);
        std::vector<u32> gctColors;
        if (lsd.gctFlags.enabled) {
            int nColors = pow(2, lsd.gctFlags.size + 1);
            for (int x = 0; x < nColors; x++) {
                u8 color[3];
                fread(color, 1, 3, f);
                gctColors.push_back(PackRGBAtoARGB(color[0], color[1], color[2], 255));
            }
        }

        std::vector <GIFGraphicControlExtension> gces;
        std::vector <GIFApplicationExtension> gaes;
        std::vector <GIFPlainTextExtension> gptes;

        while (!feof(f)) {
            u8 blockIdentifier;
            fread(&blockIdentifier, 1, 1, f);
            if (blockIdentifier == IMAGE_SEPERATOR_MAGIC) {
                GIFImageDescriptor imgDesc;
                fread(&imgDesc, sizeof(GIFImageDescriptor), 1, f);
                std::vector<u32> lctColors;
                if (imgDesc.flags.lctEnable) {
                    int nColors = pow(2, imgDesc.flags.lctSize + 1);
                    for (int x = 0; x < nColors; x++) {
                        u8 color[3];
                        fread(color, 1, 3, f);
                        lctColors.push_back(PackRGBAtoARGB(color[0], color[1], color[2], 255));
                    }
                }
                u8 lzwMinCode;
                fread(&lzwMinCode, 1, 1, f);

                std::vector<std::vector<u8>> lzwCompressedData;
                //read subblocks
                while (true) {
                    u8 subBlockSize = 0;
                    fread(&subBlockSize, 1, 1, f);
                    if (subBlockSize == 0) {
                        break;
                    }
                    std::vector<u8> data;
                    data.resize(subBlockSize);
                    fread(data.data(), 1, subBlockSize, f);
                    lzwCompressedData.push_back(data);
                };

            }
            else if (blockIdentifier == EXTENSION_INTRODUCER_MAGIC) {
                u8 label;
                fread(&label, 1, 1, f);
                switch (label) {
                    case LABEL_GC:
                        GIFGraphicControlExtension gce;
                        fread(&gce, sizeof(GIFGraphicControlExtension), 1, f);
                        gces.push_back(gce);
                        break;
                    case LABEL_COMMENT: 
                        {
                            std::vector<std::vector<u8>> commentData;
                            //read subblocks
                            while (true) {
                                u8 subBlockSize = 0;
                                fread(&subBlockSize, 1, 1, f);
                                if (subBlockSize == 0) {
                                    break;
                                }
                                std::vector<u8> data;
                                data.resize(subBlockSize);
                                fread(data.data(), 1, subBlockSize, f);
                                commentData.push_back(data);
                            };
                        }
                        break;
                    case LABEL_APPLICATION: 
                        {
                            GIFApplicationExtension gae;
                            fread(&gae, sizeof(GIFApplicationExtension), 1, f);
                            std::vector<std::vector<u8>> applicationData;
                            //read subblocks
                            while (true) {
                                u8 subBlockSize = 0;
                                fread(&subBlockSize, 1, 1, f);
                                if (subBlockSize == 0) {
                                    break;
                                }
                                std::vector<u8> data;
                                data.resize(subBlockSize);
                                fread(data.data(), 1, subBlockSize, f);
                                applicationData.push_back(data);
                            };
                            gaes.push_back(gae);
                        }
                        break;
                    case LABEL_PLAINTEXT:
                        {
                            GIFPlainTextExtension gpe;
                            fread(&gpe, sizeof(GIFPlainTextExtension), 1, f);
                            std::vector<std::vector<u8>> ptd;
                            //read subblocks
                            while (true) {
                                u8 subBlockSize = 0;
                                fread(&subBlockSize, 1, 1, f);
                                if (subBlockSize == 0) {
                                    break;
                                }
                                std::vector<u8> data;
                                data.resize(subBlockSize);
                                fread(data.data(), 1, subBlockSize, f);
                                ptd.push_back(data);
                            };
                            gptes.push_back(gpe);
                        }
                        break;
                    default:
                        logprintf("UNKNOWN EXTENSION: %i\n", label);
                        break;
                }
            }
            else if (blockIdentifier == GIF_TRAILER_MAGIC) {
                break;
            }
            u8 blockTerminator;
            fread(&blockTerminator, 1, 1, f);
        }


        fclose(f);
    }
    return NULL;
}

Layer* readGXT(PlatformNativePathString path, u64 seek)
{
    struct GXTHeader {
        u32 magic;
        u16 versionMajor;
        u16 versionMinor;
        u32 embeddedTextureCount;
        u32 textureDataOffset;
        u32 totalTextureSize;
        u32 p4EntryPalettes;
        u32 p8EntryPalettes;
        u32 padding;
    };

    struct GXTTextureSpec {
        u32 textureOffset;
        u32 textureSize;
        u32 paletteIndex;
        u32 flagsUnused;
        u32 textureType;
        u32 textureBaseFormat;
        u16 width;
        u16 height;
        u16 mipmaps;
        u16 padding;
    };

    enum GXTTextureFormat {
        PVRT2BPP = 0x80000000,
        PVRT4BPP = 0x81000000,
        PVRTII2BPP = 0x82000000,
        PVRTII4BPP = 0x83000000,
        UBC1 = 0x85000000,
        UBC2 = 0x86000000,
        UBC3 = 0x87000000,
        PX1555 = 0x00040010,
        ARGB4444 = 0x10000000,
        ARGB8888 = 0x0C001000,
        XRGB888 = 0x0C005000,
        RGB888 = 0x98001000,
        RGB565 = 0x05001000,
        RGB555 = 0x04005000,
        RGB4444 = 0x02001000
    };

    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        Layer* ret = NULL;

        GXTHeader header;
        std::vector<GXTTextureSpec> textures;

        logprintf("%i\n", sizeof(GXTHeader));
        logprintf("%i\n", sizeof(GXTTextureSpec));

        fread(&header, sizeof(GXTHeader), 1, f);
        for (int x = 0; x < header.embeddedTextureCount; x++) {
            GXTTextureSpec spec;
            fread(&spec, sizeof(GXTTextureSpec), 1, f);
            textures.push_back(spec);

            logprintf("Texture %i\n", x);
            logprintf("Offset: %x\n", spec.textureOffset);
            logprintf("Size: %x\n", spec.textureSize);
            logprintf("Dimensions: %i %i\n", spec.width, spec.height);
            logprintf("Format: %x\n", spec.textureBaseFormat);
            logprintf("------------\n");
        }

        if (textures.size() > 0) {
            GXTTextureSpec firstTex = textures[0];
            ret = new Layer(firstTex.width, firstTex.height);
            fseek(f, firstTex.textureOffset, SEEK_SET);
            switch (firstTex.textureBaseFormat) {
                case UBC1:
                    DeXT1(ret, firstTex.width, firstTex.height, f);
                    detile(ret, {4,4});
                    break;
                case UBC2:
                    DeXT23(ret, firstTex.width, firstTex.height, f);
                    detile(ret, {4,4});
                    break;
            }
        }
        

        fclose(f);
        return ret;
    }

    return NULL;
}

Layer* readWinSHS(PlatformNativePathString path, u64 seek)
{
#if _WIN32
    IStorage* pStorage = nullptr;
    HRESULT hr = StgOpenStorage(path.c_str(), nullptr, STGM_READ | STGM_SHARE_EXCLUSIVE, nullptr, 0, &pStorage);
    if (FAILED(hr)) {
        g_addNotification(ErrorNotification("Win32 error", "StgOpenStorage failed"));
        return NULL;
    }

    // list all streams
    IEnumSTATSTG* pEnum = nullptr;
    hr = pStorage->EnumElements(0, nullptr, 0, &pEnum);
    if (FAILED(hr)) {
        pStorage->Release();
        g_addNotification(ErrorNotification("Win32 error", "EnumElements failed"));
        return NULL;
    }
    // find the stream with the image
    STATSTG stat2;
    while (pEnum->Next(1, &stat2, nullptr) == S_OK) {
        if (stat2.type == STGTY_STORAGE) {
            GUID bitmapImageGuid = { 0x0003000A, 0000, 0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} };
            if (IsEqualGUID(stat2.clsid, bitmapImageGuid)) {
                break;
            }
        }
    }
    IStorage* pStorage2;
    hr = pStorage->OpenStorage(stat2.pwcsName, nullptr, STGM_READ | STGM_SHARE_EXCLUSIVE, nullptr, 0, &pStorage2);
    if (FAILED(hr)) {
        pStorage->Release();
        g_addNotification(ErrorNotification("Win32 error", "OpenStorage failed"));
        return NULL;
    }
    IEnumSTATSTG* pEnum2 = nullptr;
    hr = pStorage2->EnumElements(0, nullptr, 0, &pEnum2);
    STATSTG stat3;
    while (pEnum2->Next(1, &stat3, nullptr) == S_OK) {
        if (stat3.type == STGTY_STREAM && std::wstring(stat3.pwcsName).find(L"Ole10Native") != std::wstring::npos) {
            break;
        }
    }

    IStream* pStream = nullptr;
    hr = pStorage2->OpenStream(stat3.pwcsName, nullptr, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);

    if (FAILED(hr)) {
        pStorage->Release();
        pStorage2->Release();
        g_addNotification(ErrorNotification("Win32 error", "OpenStream failed"));
        return NULL;
    }

    STATSTG stat;
    pStream->Stat(&stat, STATFLAG_NONAME);
    ULONG size = stat.cbSize.LowPart;

    u8* buffer = (u8*)tracked_malloc(size);
    ULONG bytesRead;
    hr = pStream->Read(buffer, size, &bytesRead);

    pStream->Release();
    pStorage->Release();
    pStorage2->Release();

    if (FAILED(hr)) {
        g_addNotification(ErrorNotification("Win32 error", "Failed to read stream"));
        tracked_free(buffer);
        return NULL;
    }
    FILE* temp = platformOpenFile(L"temp.bin", L"wb");
    fwrite(buffer + 4, 1, size - 4, temp);
    fclose(temp);
    tracked_free(buffer);
    Layer* l = readBMP(L"temp.bin", 0);
    l->name = "Windows Scrap Layer";
    std::filesystem::remove(L"temp.bin");
    return l;

#else
    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Format not supported on this platform"));
    return NULL;
#endif
}

MainEditor* readOpenRaster(PlatformNativePathString path)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        MainEditor* ret = NULL;
        //read .ora file using zip
        zip_t *zip = zip_cstream_open(f, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
        if (zip == NULL) {
            fclose(f);
            return NULL;
        }

        {
            pugi::xml_document doc;

            zip_entry_open(zip, "stack.xml");
            {
                char* zipBuffer;
                size_t zipBufferSize;
                zip_entry_read(zip, (void**)&zipBuffer, &zipBufferSize);

                //read xml from zipBuffer
                doc.load_string(zipBuffer);

            }
            zip_entry_close(zip);

            pugi::xml_node imgNode = doc.child("image");
            int w = imgNode.attribute("w").as_int();
            int h = imgNode.attribute("h").as_int();

            std::vector<Layer*> layers;
            _parseORAStacksRecursively(&layers, XY{w,h}, imgNode.child("stack"), zip);
            ret = new MainEditor(layers);
        }

        fclose(f);
        return ret;        
    }
    return NULL;
}

MainEditor* readPixelStudioPSP(PlatformNativePathString path)
{
    std::ifstream f(path);
    if (f.is_open()) {
        json j = json::parse(f);
        f.close();

        return deserializePixelStudioSession(j);

    }
    return NULL;
}

MainEditor* readPixelStudioPSX(PlatformNativePathString path)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        u64 fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);
        u8* fdata = (u8*)tracked_malloc(fileSize);
        fread(fdata, 1, fileSize, f);
        fclose(f);

        std::vector<u8> decompressed = decompressZlibWithoutUncompressedSize(fdata, fileSize);
        tracked_free(fdata);
        std::string jsonString(decompressed.begin(), decompressed.end());

        /*std::ofstream out("test.json");
        out << jsonString;
        out.close();*/

        try {
            json j = json::parse(jsonString);
            return deserializePixelStudioSession(j);
        }
        catch (std::exception&) {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to parse PSX JSON"));
            return NULL;
        }
    }
    return NULL;
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
                        fread(newLayer->pixelData, newLayer->w * newLayer->h, 4, infile);
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
                        char* name = (char*)tracked_malloc(nameLen+1);
                        memset(name, 0, nameLen + 1);
                        fread(name, nameLen, 1, infile);

                        Layer* newLayer = new Layer(dimensions.x, dimensions.y);
                        newLayer->name = std::string(name);
                        tracked_free(name);
                        fread(newLayer->pixelData, newLayer->w * newLayer->h, 4, infile);
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
                                fread(newLayer->pixelData, newLayer->w * newLayer->h, 4, infile);
                            }
                            else {
                                uint64_t compressedLength = 0;
                                fread(&compressedLength, 8, 1, infile);
                                uint8_t* compressedData = new uint8_t[compressedLength];
                                fread(compressedData, compressedLength, 1, infile);
                                uint64_t dstLength = newLayer->w * newLayer->h * 4;
                                uncompress(newLayer->pixelData, (uLongf*)&dstLength, compressedData, compressedLength);
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
                                fread(newLayer->pixelData, newLayer->w * newLayer->h, 4, infile);
                            }
                            else {
                                uint64_t compressedLength = 0;
                                fread(&compressedLength, 8, 1, infile);
                                uint8_t* compressedData = new uint8_t[compressedLength];
                                fread(compressedData, compressedLength, 1, infile);
                                uint64_t dstLength = newLayer->w * newLayer->h * 4;
                                uncompress(newLayer->pixelData, (uLongf*)&dstLength, compressedData, compressedLength);
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

Layer* loadAnyIntoFlat(std::string utf8path, FileImporter** outputFoundImporter)
{
    MainEditor* ssn = loadAnyIntoSession(utf8path, outputFoundImporter);
    if (ssn != NULL) {
        Layer* ret = ssn->flattenImage();
        delete ssn;
        return ret;
    }
    return NULL;
}

MainEditor* loadAnyIntoSession(std::string utf8path, FileImporter** outputFoundImporter)
{
    PlatformNativePathString fPath = convertStringOnWin32(utf8path);

    for (FileImporter*& importer : g_fileImporters) {
        if (stringEndsWithIgnoreCase(utf8path, importer->extension()) && importer->canImport(fPath)) {
            void* data = importer->importData(fPath);
            if (data != NULL) {
                MainEditor* session = NULL;
                if (importer->importsWholeSession()) {
                    session = (MainEditor*)data;
                }
                else {
                    Layer* l = (Layer*)data;
                    session = l->isPalettized ? new MainEditorPalettized((LayerPalettized*)l) : new MainEditor(l);
                }
                if (importer->getCorrespondingExporter() != NULL) {
                    session->lastWasSaveAs = false;
                    session->lastConfirmedSave = true;
                    session->lastConfirmedSavePath = fPath;
                    session->lastConfirmedExporter = importer->getCorrespondingExporter();
                }
                if (outputFoundImporter != NULL) {
                    *outputFoundImporter = importer;
                }
                return session;
            }
            else {
                logprintf("%s : load failed\n", importer->name().c_str());
            }
        }
    }
    return NULL;
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
            fwrite(lr->pixelData, lr->w * lr->h, 4, outfile);
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

            fwrite(lr->pixelData, lr->w * lr->h, 4, outfile);
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

            fwrite(lr->pixelData, lr->w * lr->h, 4, outfile);
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

            fwrite(lr->pixelData, lr->w * lr->h, 4, outfile);
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
            int res = compress(compressedData, (uLongf*)&compressedDataSize, lr->pixelData, lr->w * lr->h * 4);

            fwrite(&compressedDataSize, 8, 1, outfile);
            fwrite(compressedData, compressedDataSize, 1, outfile);
            delete[] compressedData;
        }

        fclose(outfile);
        return true;
    }
    return false;
}

bool writeOpenRaster(PlatformNativePathString path, MainEditor* editor)
{
    if (editor->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Palettized image export not implemented"));
        return false;
    }

    std::vector<Layer*> data = editor->layers;
    char* zipBuffer;
    size_t zipBufferSize;

    zip_t * zip = zip_stream_open(NULL, 0, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
    {
        zip_entry_open(zip, "mimetype");
        {
            zip_entry_write(zip, "image/openraster", sizeof("image/openraster"));
        }
        zip_entry_close(zip);
        
        zip_entry_open(zip, "stack.xml");
        {
            std::string xmls = "";
            xmls += std::format("<image version=\"0.0.1\" xres=\"72\" h=\"{}\" w=\"{}\" yres=\"72\">\n", data[0]->h, data[0]->w);
            xmls += std::format(" <stack opacity=\"1\" x=\"0\" name=\"root\" y=\"0\" isolation=\"isolate\" composite-op=\"svg:src-over\" visibility=\"visible\">\n");
            int i = 0;
            for (auto l = data.rbegin(); l != data.rend(); l++) {
                xmls += std::format("  <layer opacity=\"{}\" x=\"0\" name=\"{}\" y=\"0\" src=\"data/layer{}.png\" composite-op=\"svg:src-over\" visibility=\"{}\"/>\n", (*l)->layerAlpha / 255.0f, (*l)->name, i++, (*l)->hidden ? "hidden" : "visible");
            }
            xmls += "  </stack>\n";
            xmls += "</image>\n";
            zip_entry_write(zip, xmls.c_str(), xmls.size());
        }
        zip_entry_close(zip);

        zip_entry_open(zip, "mergedimage.png"); 
        {
            Layer* flat = editor->flattenImage();
            std::vector<u8> pngData = writePNGToMem(flat);
            zip_entry_write(zip, pngData.data(), pngData.size());
            delete flat;
        }
        zip_entry_close(zip);

        //todo: quantize it to 256 colors and save it as an 8bit png
        zip_entry_open(zip, "Thumbnails/thumbnail.png"); 
        {
            Layer* flat = editor->flattenImage();
            Layer* flatScaled = flat->copyScaled(XY{255,255});
            delete flat;
            std::vector<u8> pngData = writePNGToMem(flatScaled);
            zip_entry_write(zip, pngData.data(), pngData.size());
            delete flatScaled;
        }
        zip_entry_close(zip);

        int i = 0;
        for (auto l = data.rbegin(); l != data.rend(); l++) {
            std::vector<u8> pngData = writePNGToMem(*l);
            std::string fname = std::format("data/layer{}.png", i++);
            zip_entry_open(zip, fname.c_str());
            zip_entry_write(zip, pngData.data(), pngData.size());
            zip_entry_close(zip);
        }

        zip_stream_copy(zip, (void**)&zipBuffer, &zipBufferSize);
    }
    zip_close(zip);

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {

        fwrite(zipBuffer, zipBufferSize, 1, f);
        fclose(f);
        tracked_free(zipBuffer);
        return true;
    }
    tracked_free(zipBuffer);
    return false;
}

bool writePixelStudioPSP(PlatformNativePathString path, MainEditor* data)
{
    std::ofstream outfile(path);

    if (outfile.is_open()) {
        json o = serializePixelStudioSession(data);

        outfile << o.dump();
        outfile.close();
        return true;
    }
    return false;
}

bool writePixelStudioPSX(PlatformNativePathString path, MainEditor* data)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    //std::ofstream outfile(path);

    if (f != NULL) {
        json o = serializePixelStudioSession(data);
        std::string jsonString = o.dump();

        //compress with zlib and write to file
        unsigned long maxCompressedSize = compressBound(jsonString.size());
        unsigned long compressedSize = maxCompressedSize;
        uint8_t* compressedData = (uint8_t*)tracked_malloc(maxCompressedSize);
        compress(compressedData, &compressedSize, (const Bytef*)jsonString.c_str(), jsonString.size());
        fwrite(compressedData, compressedSize, 1, f);
        fclose(f);
        tracked_free(compressedData);

        //outfile.close();
        return true;
    }
    return false;
}

bool writeBMP(PlatformNativePathString path, Layer* data) {

    if (data->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Palettized image export not implemented"));
        return false;
    }

    BMP outbmp = BMP();
    outbmp.SetBitDepth(24);
    outbmp.SetSize(data->w, data->h);
    for (int y = 0; y < data->h; y++) {
        for (int x = 0; x < data->w; x++) {
            RGBApixel px;
            uint32_t pxx = data->getPixelAt(XY{ x,y });
            px.Alpha = 255;
            px.Red = pxx >> 16 & 0xff;
            px.Green = pxx >> 8 & 0xff;
            px.Blue = pxx & 0xff;
            outbmp.SetPixel(x, y, px);
        }
    }
    FILE* nfile = platformOpenFile(path, PlatformFileModeWB);
    return outbmp.WriteToFileP(nfile);
}

bool writeJPEG(PlatformNativePathString path, Layer* data)
{
    if (data->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Indexed image export not supported"));
        return false;
    }

    SDL_Surface* surface = SDL_CreateSurface(data->w, data->h, SDL_PIXELFORMAT_ARGB8888);
    if (surface == NULL) {
        logerr(std::format("[JPEG] failed to create surface: {}", SDL_GetError()));
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.mallocfail")));
        return false;
    }
    memcpy(surface->pixels, data->pixelData, data->w * data->h * 4);

    std::string u8path = convertStringToUTF8OnWin32(path);

    bool ret = IMG_SaveJPG(surface, u8path.c_str(), 100);

    SDL_FreeSurface(surface);

    return ret;
}

bool writeAVIF(PlatformNativePathString path, Layer* data)
{
    if (data->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Indexed image export not supported"));
        return false;
    }

    SDL_Surface* surface = SDL_CreateSurface(data->w, data->h, SDL_PIXELFORMAT_ARGB8888);
    if (surface == NULL) {
        logerr(std::format("[AVIF] failed to create surface: {}", SDL_GetError()));
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.mallocfail")));
        return false;
    }
    memcpy(surface->pixels, data->pixelData, data->w * data->h * 4);

    std::string u8path = convertStringToUTF8OnWin32(path);

    bool ret = IMG_SaveAVIF(surface, u8path.c_str(), 100);

    SDL_FreeSurface(surface);

    return ret;
}

bool writeCaveStoryPBM(PlatformNativePathString path, Layer* data) {

    if (data->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Indexed image export not implemented"));
        return false;
    }

    BMP outbmp = BMP();
    outbmp.SetBitDepth(24);
    outbmp.SetSize(data->w, data->h);
    for (int y = 0; y < data->h; y++) {
        for (int x = 0; x < data->w; x++) {
            RGBApixel px;
            uint32_t pxx = data->getPixelAt(XY{ x,y });
            px.Alpha = 255;
            px.Red = pxx >> 16 & 0xff;
            px.Green = pxx >> 8 & 0xff;
            px.Blue = pxx & 0xff;
            outbmp.SetPixel(x, y, px);
        }
    }
    FILE* nfile = platformOpenFile(path, PlatformFileModeWB);
    if (outbmp.WriteToFileP(nfile, false)) {
        fseek(nfile, 0, SEEK_END);
        fwrite("(C)Pixel", 8, 1, nfile);
        fclose(nfile);
        return true;
    }
    return false;
}

bool writeXBM(PlatformNativePathString path, Layer* data)
{
    auto uqColors = data->getUniqueColors();
    if (uqColors.size() > 2) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Too many colors. X Bitmap requires 2."));
        return false;
    }

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        fprintf(f, "#define voidsprite_export_width %i\n", data->w);
        fprintf(f, "#define voidsprite_export_height %i\n", data->h);
        fprintf(f, "static unsigned char voidsprite_export_bits[] = {\n");

        uint32_t* pxd = (uint32_t*)data->pixelData;
        for (int y = 0; y < data->h; y++) {
            for (int x = 0; x < (int)ceil(ixmax(data->w, 8) / 8.0); x++) {
                XY origin = { x * 8, y };
                uint8_t byte = 0;

                for (int bit = 0; bit < 8; bit++) {
                    XY position = xyAdd(origin, { bit, 0 });
                    if (pointInBox(position, {0,0,data->w, data->h})) {
                        uint32_t pxx = pxd[position.y * data->w + position.x];
                        /*if ((pxx & 0xFF000000) == 0) {
                            pxx = 0;
                        }*/
                        //1 - black, 0 - white
                        if (pxx != uqColors[1]) {
                            byte |= 1 << bit;
                        }
                    }
                }
                fprintf(f, "0x%02X,", byte);
            }
            fprintf(f, "\n");
        }

        fprintf(f, "};\n");
        fclose(f);
        return true;
    }

    return false;
}

bool writeTGA(PlatformNativePathString path, Layer* data) {
    if (data->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Palettized image export not implemented"));
        return false;
    }

    FILE* nfile = platformOpenFile(path, PlatformFileModeWB);
    if (nfile != NULL) {
        //copilot hallucinated all of this code i don't know if it even works
        uint8_t header[18];
        memset(header, 0, 18);
        header[2] = 2;  // uncompressed RGB
        header[12] = data->w & 0xff;    // width
        header[13] = (data->w >> 8) & 0xff;
        header[14] = data->h & 0xff;    // height
        header[15] = (data->h >> 8) & 0xff;
        header[16] = 24;    // 24 bit bitmap
        header[17] = 0x20;  // top-down, non-interlaced
        fwrite(header, 18, 1, nfile);
        for (int y = 0; y < data->h; y++) {
            for (int x = 0; x < data->w; x++) {
                uint32_t pxx = data->getPixelAt(XY{ x,y });
                uint8_t px[3];
                px[0] = pxx & 0xff;
                px[1] = (pxx >> 8) & 0xff;
                px[2] = (pxx >> 16) & 0xff;
                fwrite(px, 3, 1, nfile);
            }
        }
        
        fclose(nfile);
        return true;
    }
    return false;
}

bool writeCHeader(PlatformNativePathString path, Layer* data)
{
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {

        fprintf(outfile, "#pragma once\n");
        fprintf(outfile, "#include <stdint.h>\n");
        fprintf(outfile, "#include <stdlib.h>\n");
        fprintf(outfile, "//#include <SDL.h>\n\n");

        fprintf(outfile, "int voidsprite_image_w = %i;\n", data->w);
        fprintf(outfile, "int voidsprite_image_h = %i;\n", data->h);

        if (!data->isPalettized) {
            fprintf(outfile, "uint32_t voidsprite_image_data[] = {\n");
            uint32_t* pxd = (uint32_t*)data->pixelData;
            uint64_t dp = 0;
            for (int y = 0; y < data->h; y++) {
                for (int x = 0; x < data->w; x++) {
                    fprintf(outfile, "0x%08X,", pxd[dp++]);
                }
                fprintf(outfile, "\n");
            }
            fprintf(outfile, "};\n");
        }
        else {
            LayerPalettized* upcast = (LayerPalettized*)data;
            fprintf(outfile, "uint32_t voidsprite_palette data[%zu] = {\n", upcast->palette.size());
            int x = 0;
            for (uint32_t& col : upcast->palette) {
                fprintf(outfile, "0x%08X,", col);
                if (x++ % 16 == 0) {
                    fprintf(outfile, "\n");
                }
            }
            fprintf(outfile, "\n};\n\n");

            fprintf(outfile, "uint32_t voidsprite_image_data[] = {\n");
            uint32_t* pxd = (uint32_t*)data->pixelData;
            uint64_t dp = 0;
            for (int y = 0; y < data->h; y++) {
                for (int x = 0; x < data->w; x++) {
                    fprintf(outfile, "%i,", pxd[dp++]);
                }
                fprintf(outfile, "\n");
            }
            fprintf(outfile, "};\n");
        }
        fclose(outfile);
        return true;
    }
    return false;
}

bool writePythonNPArray(PlatformNativePathString path, Layer* data)
{
    if (data->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Palettized image export not implemented"));
        return false;
    }

    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        fprintf(outfile, "import numpy as np\n\n");
        fprintf(outfile, "#from PIL import Image\n\n");
        uint64_t dataPixelPointer = 0;
        fprintf(outfile, "voidsprite_image = np.array([\n");
        for (int y = 0; y < data->h; y++){
            fprintf(outfile, "    [");
            for (int x = 0; x < data->w; x++){
                uint8_t b = data->pixelData[dataPixelPointer++];
                uint8_t g = data->pixelData[dataPixelPointer++];
                uint8_t r = data->pixelData[dataPixelPointer++];
                uint8_t a = data->pixelData[dataPixelPointer++];
                fprintf(outfile, "[%i,%i,%i,%i],", r,g,b,a);
            }
            fprintf(outfile, "],\n");
        }
        fprintf(outfile, "], dtype=np.uint8)\n\n\n");

        fprintf(outfile, "#pilimage = Image.fromarray(voidsprite_image, mode='RGBA')");

        fclose(outfile);
        return true;
    }
    return false;
}

bool writeJavaBufferedImage(PlatformNativePathString path, Layer* data)
{
    if (data->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Palettized image export not implemented"));
        return false;
    }

    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {

        std::string javaHeader = std::format("\n\
import java.awt.image.BufferedImage;\n\
\n\
public class VoidspriteImage {{\n\
    public static int width = {};\n\
    public static int height = {};\n\
    // Class generated with voidsprite\n\
    // Warning! Images exported to java classes can be too big for javac!\n\
\n\
    public static BufferedImage toBufferedImage() {{\n\
        BufferedImage ret = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);\n\
        ret.setRGB(0, 0, width, height, rgbData, 0, width);\n\
        return ret;\n\
    }}\n\
\n\
    public static int[] rgbData = {{\n\
            ", data->w, data->h);

        std::string javaFooter = "\n\
    };\n\
}\n\
            ";
        fwrite(javaHeader.c_str(), 1, javaHeader.size(), outfile);
        for (int y = 0; y < data->h; y++) {
            for (int x = 0; x < data->w; x++) {
                uint32_t pxd = data->getPixelAt(XY{ x,y });
                fprintf(outfile, "0x%08X,", pxd);
            }
            fprintf(outfile, "\n");
        }
        fwrite(javaFooter.c_str(), 1, javaFooter.size(), outfile);

        fclose(outfile);
        return true;
    }
    return false;
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

        uint32_t* pxd = (uint32_t*)data->pixelData;
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

bool writeAnymapTextPGM(PlatformNativePathString path, Layer* data)
{
    if (data->isPalettized) {
        if (((LayerPalettized*)data)->palette.size() > 256) {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Too many colors. Anymap PGM requires up to 256."));
            return false;
        }
    }

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        fprintf(f, "P2\n");
        fprintf(f, "# File generated by voidsprite\n");
        fprintf(f, "%i %i\n", data->w, data->h);
        fprintf(f, "256\n");

        u32* pxd = (u32*)data->pixelData;
        for (int y = 0; y < data->h; y++) {
            for (int x = 0; x < data->w; x++) {
                u32 c = pxd[x + y * data->w];
                if (!data->isPalettized) {
                    //slow as fuck but correct
                    u8 r = (c>>16) & 0xff;
                    u8 g = (c >> 8) & 0xff;
                    u8 b = (c) & 0xff;
                    u8 grayScale = (r==g && g==b) ? r : (u8)(r * 0.2126 + g * 0.7152 + b * 0.0722);
                    //(f, "%u ", std::find(uqColors.begin(), uqColors.end(), c) - uqColors.begin());
                    fprintf(f, "%u ", grayScale);
                }
                else {
                    fprintf(f, "%u ", c);
                }
            }
            fprintf(f, "\n");
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

        u32* pxd = (u32*)data->pixelData;
        for (int y = 0; y < data->h; y++) {
            for (int x = 0; x < data->w; x++) {
                u32 c = pxd[x + y * data->w];
                u8 r = (c>>16) & 0xff;
                u8 g = (c >> 8) & 0xff;
                u8 b = (c) & 0xff;
                fprintf(f, "%u %u %u ", r,g,b);
            }
            fprintf(f, "\n");
        }
        fclose(f);
        return true;
    }
    return false;
}

bool writeSR8(PlatformNativePathString path, Layer* data)
{
    if (data->w != 16 || data->h != 16) {
        g_addNotification(ErrorNotification("Error exporting SR8", "Image size must be 16x16."));
        return false;
    }
    if (!data->isPalettized) {
        g_addNotification(ErrorNotification("Error exporting SR8", "RGB export not supported."));
        return false;
    }
    if (((LayerPalettized*)data)->palette.size() > 256) {
        g_addNotification(ErrorNotification("Error exporting SR8", "Invalid palette size. Must be 256 colors."));
        return false;
    }

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        u32* ppx = (u32*)data->pixelData;
        for (int x = 0; x < 16 * 16; x++) {
            u8 byte = ppx[x];
            fwrite(&byte, 1, 1, f);
        }

        fclose(f);
        return true;
    }
    return false;
}

bool writeCUR(PlatformNativePathString path, Layer* data)
{
    std::vector<u32> palette = data->isPalettized ? ((LayerPalettized*)data)->palette : data->getUniqueColors();
    if (palette.size() > 256) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Too many colors. CUR requires max. 256."));
        return false;
    }

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
#pragma pack(push, 1)
        struct CURHeader {
            u16 reserved = 0;
            u16 imageType = 2;
            u16 numImages = 1;
            u8 width;
            u8 height;
            u8 numPaletteColors = 0;
            u8 reserved2 = 0;
            u16 hotLeft = 0;
            u16 hotRight = 0;
            u32 imgSizeInBytes = 0;
            u32 imgDataOffset = 22;
            u32 numHeaderBytes = 40;
            u32 dibWidth;
            u32 dibHeight;
            u16 numColorPlanes = 1;
            u16 numBpp;
            u32 compression = 0;
            u32 sizeOfRawBitmap = 0;
            u32 printResW = 0;
            u32 printResH = 0;
            u32 dibNumPaletteColors = 0;
            u32 numImportantColors = 0;
        };
#pragma pack(pop)

        CURHeader h;
        h.width = data->w;
        h.height = data->h;
        h.dibWidth = data->w;
        h.dibHeight = data->h * 2;
        h.numBpp = 8;
        u32 imgSizeInb = 4 * 256 + data->w * data->h + data->h * (int)ceil(data->w / 8.0) + 40;
        h.imgSizeInBytes = imgSizeInb;
        fwrite(&h, sizeof(CURHeader), 1, f);

        for (int p = 0; p < 256; p++) {
            u32 c = p < palette.size() ? palette[p] : 0;
            u8 r = c >> 16 & 0xff;
            u8 g = c >> 8 & 0xff;
            u8 b = c & 0xff;
            u8 aPlaceHolder = 0x00;
            fwrite(&b, 1, 1, f);
            fwrite(&g, 1, 1, f);
            fwrite(&r, 1, 1, f);
            fwrite(&aPlaceHolder, 1, 1, f);
        }

        for (int y = data->h; y-->0; ) {
            for (int x = 0; x < data->w; x++) {
                u32 px = data->getPixelAt(XY{ x,y });
                
                u8 index = data->isPalettized ? px : ( std::find(palette.begin(), palette.end(), px) - palette.begin());
                fwrite(&index, 1, 1, f);
            }
        }

        u32 currentByte = 0;
        int currentBit = 7;
        for (int y = data->h; y-- > 0; ) {
            for (int x = 0; x < data->w; x++) {
                u32 px = data->getPixelAt(XY{ x,y });
                u32 pxx = data->isPalettized ? palette[px] : px;
                u8 a = pxx >> 24 & 0xff;
                if (a == 0) {
                    currentByte |= 1 << currentBit;
                }
                currentBit--;
                if (currentBit < 0) {
                    fwrite(&currentByte, 1, 1, f);
                    currentByte = 0;
                    currentBit = 7;
                }
            }
            if (currentBit != 7) {
                fwrite(&currentByte, 1, 1, f);
                currentByte = 0;
                currentBit = 7;
            }
        }

        fclose(f);
        return true;
    }
    return false;
}

bool writeVTF(PlatformNativePathString path, Layer* data)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        XY lowResDimensions = data->w > 32 || data->h > 32 ? XY{32, 32} : XY{data->w, data->h};
        Layer* lowResImage = data->copyScaled(lowResDimensions);

        VTFHEADER header;
        memset(&header, 0, sizeof(VTFHEADER));
        memcpy(&header.signature, "VTF\0", 4);
        header.version[0] = 7;
        header.version[1] = 1;
        header.headerSize = 64;
        header.width = data->w;
        header.height = data->h;
        header.flags = 1    // POINTSAMPLE
            | 0x100         // NOMIP
            | 0x200         // NOLOD
            | 0x2000        // 8BIT ALPHA
            ;
        header.frames = 1;
        header.firstFrame = 0;
        header.reflectivity[0] = header.reflectivity[1] = header.reflectivity[2] = 0.1796f; //i have no idea why
        header.bumpmapScale = 1;
        header.highResImageFormat = IMAGE_FORMAT_BGRA8888;
        header.mipmapCount = 1;
        header.lowResImageFormat = IMAGE_FORMAT_BGRA8888;
        header.lowResImageWidth = lowResDimensions.x;
        header.lowResImageHeight = lowResDimensions.y;

        fwrite(&header,64, 1, f);
        fwrite(lowResImage->pixelData, lowResDimensions.x * lowResDimensions.y, 4, f);
        fwrite(data->pixelData, data->w * data->h, 4, f);
        delete lowResImage;
        fclose(f);
        return true;
    }
    return false;
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

std::pair<bool, std::vector<uint32_t>> readPltJASCPAL(PlatformNativePathString name)
{
    std::ifstream f(name, std::ios::in);
    if (f.is_open()) {
        std::vector<uint32_t> newPalette;
        std::string line;
        std::getline(f, line);
        if (line.substr(0, 8) == "JASC-PAL") {
            f >> line;
            //should be 0100
            int count;
            f >> count;
            for (int x = 0; x < count; x++) {
                int r, g, b;
                f >> r >> g >> b;
                newPalette.push_back((0xff << 24) | (r << 16) | (g << 8) | b);
            }
            f.close();
            return { true, newPalette };
        }
        f.close();
    }
    return { false, {} };
}

std::pair<bool, std::vector<uint32_t>> readPltGIMPGPL(PlatformNativePathString name)
{
    std::ifstream f(name, std::ios::in);
    if (f.is_open()) {
        std::string magic = "";
        std::string name = "";
        int columns = 0;
        int columnNow = 0;
        int lineN = 0;
        bool oldFormat = false;
        std::vector<u32> ret;

        while (!f.eof()) {
            std::string line;
            std::getline(f, line);

            if (line.substr(0, 1) == "#") {
                continue;
            }
            lineN++;
            if (lineN == 1) {
                magic = line;
                if (magic != "GIMP Palette") {
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid GIMP palette file"));
                    f.close();
                    return { false, {} };
                }
            }
            else if (stringStartsWithIgnoreCase(line, "name: ")) {
                name = line.substr(6);
            }
            else if (lineN == 3 && stringStartsWithIgnoreCase(line, "columns: ")) {
                columns = std::stoi(line.substr(9));
            } 
            else if (line.size() > 3) {
                int r, g, b;
                std::istringstream iss(line);
                iss >> r >> g >> b;
                ret.push_back(PackRGBAtoARGB(r, g, b, 255));
            }
        }
        f.close();
        return { true, ret };
    }
    return { false, {} };
}

std::pair<bool, std::vector<uint32_t>> readPltHEX(PlatformNativePathString name)
{
    std::ifstream f(name, std::ios::in);
    if (f.is_open()) {
        std::pair<bool, std::vector<uint32_t>> ret;
        while (!f.eof()) {
            std::string line;
            std::getline(f, line);
            if (line.size() == 6 || line.size() == 8) {
                std::string r = line.substr(0, 2);
                std::string g = line.substr(2, 2);
                std::string b = line.substr(4, 2);
                std::string a = line.size() == 8 ? line.substr(6, 2) : "FF";
                int ri = std::stoi(r, 0, 16);
                int gi = std::stoi(g, 0, 16);
                int bi = std::stoi(b, 0, 16);
                int ai = std::stoi(a, 0, 16);
                ret.second.push_back(PackRGBAtoARGB(ri, gi, bi, ai));
            }
        }
        f.close();
        ret.first = ret.second.size() > 0;
        return ret;
    }
    return { false,{} };
}

std::pair<bool, std::vector<uint32_t>> readPltPDNTXT(PlatformNativePathString name)
{
    std::ifstream f(name, std::ios::in);
    if (f.is_open()) {
        std::pair<bool, std::vector<uint32_t>> ret;
        while (!f.eof()) {
            std::string line;
            std::getline(f, line);
            if (line.size() > 0 && line[0] == ';') {
                continue;
            }
            if (line.size() == 6 || line.size() == 8) {
                int idx = 0;
                std::string a = line.size() == 8 ? line.substr(idx++*2, 2) : "FF";
                std::string r = line.substr(idx++ * 2, 2);
                std::string g = line.substr(idx++ * 2, 2);
                std::string b = line.substr(idx++ * 2, 2);
                
                int ri = std::stoi(r, 0, 16);
                int gi = std::stoi(g, 0, 16);
                int bi = std::stoi(b, 0, 16);
                int ai = std::stoi(a, 0, 16);
                ret.second.push_back(PackRGBAtoARGB(ri, gi, bi, ai));
            }
        }
        f.close();
        ret.first = ret.second.size() > 0;
        return ret;
    }
    return { false,{} };
}

std::pair<bool, NineSegmentPattern> read9SegmentPattern(PlatformNativePathString path)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        char header[7];
        fread(header, 7, 1, f);
        if (strncmp(header, "VOID9SP", 7) == 0) {

            NineSegmentPattern ret;
            int* reads[] = {
                &ret.point1.x, &ret.point1.y, &ret.point2.x, &ret.point2.y, &ret.dimensions.x, &ret.dimensions.y
            };
            for (int*& r : reads) {
                u32 buffer;
                fread(&buffer, 4, 1, f);
                *r = buffer;
            }
            if (ret.dimensions.x > 0 && ret.dimensions.y > 0) {
                ret.pixelData = (u32*)tracked_malloc(4 * ret.dimensions.x * ret.dimensions.y, "9SP");
            }

            if (ret.pixelData != NULL) {
                fread(ret.pixelData, 4, ret.dimensions.x * ret.dimensions.y, f);
                fclose(f);
                return { true, ret };
            }
        }

        fclose(f);
    }
    return { false,{} };
}

bool write9SegmentPattern(PlatformNativePathString path, Layer* data, XY point1, XY point2)
{
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        fwrite("VOID9SP", 7, 1, outfile);
        u32 writes[] = {
            point1.x, point1.y, point2.x, point2.y, data->w, data->h
        };
        for (u32& w : writes) {
            fwrite(&w, 4, 1, outfile);
        }
        fwrite(data->pixelData, data->w * data->h, 4, outfile);

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
        XY minImageDimensions = { 0,0 };

        std::string line;
        std::getline(f, line);
        if (stringStartsWithIgnoreCase(line, "voidsprite split session file v")){
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
                                    ssi.dimensions = {nlayer->w, nlayer->h};
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
                                logprintf("error reading comment\n");
                            }
                        }
                        else if (line.find(':')) {
                            std::string argName = line.substr(0, line.find(':'));
                            std::string argValue = line.substr(line.find(':') + 1);

                            if (argName == "tiledim.x") {
                                ssn.tileDimensions.x = std::stoi(argValue);
                            }
                            else if (argName == "tiledim.y") {
                                ssn.tileDimensions.y = std::stoi(argValue);
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
                            imageLayer->blit(layer, ssi.positionInOverallImage, {0,0,layer->w, layer->h}, true);
                            delete layer;
                        }
                        MainEditor* newEditor = new MainEditor(imageLayer);
                        newEditor->splitSessionData = ssn;
                        newEditor->tileDimensions = ssn.tileDimensions;
                        newEditor->lastConfirmedSavePath = path;
                        newEditor->comments = comments;
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
    f << "tiledim.x:" << data->tileDimensions.x << "\n";
    f << "tiledim.y:" << data->tileDimensions.y << "\n";
    for (CommentData& comment : data->comments) {
        f << "comment:" << comment.data << ";" << comment.position.x << ";" << comment.position.y << "\n";
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
            Layer* trimmed = flat->trim({separateImage.positionInOverallImage.x, separateImage.positionInOverallImage.y, 
                separateImage.dimensions.x, separateImage.dimensions.y});
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

bool writeHTMLBase64(PlatformNativePathString path, Layer* data)
{
    if (data->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Palettized image export not implemented"));
        return false;
    }

    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);

    if (outfile != NULL) {

        std::string base64Buffer;
        {
            std::vector<u8> pngData = writePNGToMem(data);
            std::string fileBuffer;
            fileBuffer.resize(pngData.size());
            memcpy(fileBuffer.data(), pngData.data(), pngData.size());
            base64Buffer = base64::to_base64(fileBuffer);
        }

        std::string htmlPre = "<!DOCTYPE HTML>\n<html>\n\t<head>\n\t\t<title>voidsprite image</title>\n\t\t<!-- File exported with voidsprite -->\n\t</head>\n\t<body>\n\t\t<img src=\"data:image/png;base64, ";
        std::string htmlPost = "\"></img>\n\t</body>\n</html>";
        fwrite(htmlPre.c_str(), htmlPre.size(), 1, outfile);
        fwrite(base64Buffer.c_str(), base64Buffer.length(), 1, outfile);
        fwrite(htmlPost.c_str(), htmlPost.size(), 1, outfile);
        
        fclose(outfile);
        return true;
    }
    return false;
}
