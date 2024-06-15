#include "globals.h"
#include "FileIO.h"
#include "maineditor.h"
#include <png.h>
#include "libtga/tga.h"
#include "ddspp/ddspp.h"
#include "easybmp/EasyBMP.h"
#include "zip/zip.h"
#include "pugixml/pugixml.hpp"

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

Layer* _VTFseekToLargestMipmapAndRead(FILE* infile, int width, int height, int mipmapCount, int frames, int imageFormat)
{
    Layer* ret = NULL;
    int w = width ;
    int h = height ;
    for (int skipMMap = 0; skipMMap < mipmapCount-1; skipMMap++) {
        w = ixmax(1, w / 2);
        h = ixmax(1, h / 2);
        int seekBy =
            imageFormat == IMAGE_FORMAT_BGR888 ? w * h * 3
            : imageFormat == IMAGE_FORMAT_BGRA8888 ? w * h * 4
            : imageFormat == IMAGE_FORMAT_RGBA8888 ? w * h * 4
            : imageFormat == IMAGE_FORMAT_ARGB8888 ? w * h * 4
            : imageFormat == IMAGE_FORMAT_DXT1 ? (ixmax(w, 4) / 4) * (ixmax(4, h) / 4) * 8
            : imageFormat == IMAGE_FORMAT_DXT3 ? (ixmax(w, 4) / 4) * (ixmax(4, h) / 4) * 16
            : imageFormat == IMAGE_FORMAT_DXT5 ? (ixmax(w, 4) / 4) * (ixmax(4, h) / 4) * 16
            : 0;
        fseek(infile, seekBy * frames, SEEK_CUR);
    }
    //fseek(infile, 16, SEEK_CUR);

    switch (imageFormat) {
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
    default:
        printf("IMAGE FORMAT NOT IMPLEMENTED\n");
        break;
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
                layers->push_back(sizeCorrectLayer);
            } else {
                printf("NOOOOO LAYER IS NULL\n");
            }

            free(pngData);
            zip_entry_close(zip);
        }
    }
}

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
        Bytef* compressedData = (Bytef*)malloc(cDataSize);
        fread(compressedData, 1, cDataSize, f);

        uLongf decompressedSize = 768 + imgW * imgH;
        Bytef* decompBytes = (Bytef*)malloc(decompressedSize);
        int res = uncompress(decompBytes, &decompressedSize, compressedData, cDataSize);
        //hopefully res == Z_OK

        /* //write uncompressed xyz data to file:
        FILE* outf = NULL;
        fopen_s(&outf, "decompressedxyz.bin", "wb");
        fwrite(decompBytes, 1, cDataSize, outf);
        fclose(outf);
        */

        uint32_t colorPalette[256];
        int filePtr = 0;
        for (int c = 0; c < 256; c++) {
            colorPalette[c] = 0xFF000000 | (decompBytes[filePtr++] << 16) | (decompBytes[filePtr++] << 8) | (decompBytes[filePtr++]);
        }

        Layer* nLayer = new Layer(imgW, imgH);
        uint32_t* pxData = (uint32_t*)nLayer->pixelData;
        for (int x = 0; x < imgW * imgH; x++) {
            pxData[x] = colorPalette[decompBytes[filePtr++]];
        }

        nLayer->name = "XYZ Image";

        free(compressedData);
        free(decompBytes);

        fclose(f);
        return nLayer;
    }
    return NULL;
}


size_t readPNGBytes = 0;   //if you promise to tell noone
size_t PNGFileSize = 0;
void _readPNGDataFromMem(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if (io_ptr == NULL) {
        printf("WHY  IS io_ptr NULL\n");
        return;
    }

    char* inputStream = (char*)io_ptr;
    memcpy(outBytes, inputStream + readPNGBytes, byteCountToRead);
    readPNGBytes += byteCountToRead;
}

//todo: don't just copy code come on
Layer* readPNGFromMem(uint8_t* data, size_t dataSize) {
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    readPNGBytes = 0;
    PNGFileSize = dataSize;
    png_set_read_fn(png, (void*)data, _readPNGDataFromMem);
    //png_set_sig_bytes(png, kPngSignatureLength);
    png_read_info(png, info);

    uint32_t width = png_get_image_width(png, info);
    uint32_t height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    int numchannels = png_get_channels(png, info);
    png_bytepp rows = new png_bytep[height];
    for (int y = 0; y < height; y++) {
        rows[y] = new png_byte[png_get_rowbytes(png, info)];
    }
    png_read_image(png, rows);

    Layer* nlayer = new Layer(width, height);
    nlayer->name = "PNG Image";

    int imagePointer = 0;
    for (uint32_t y = 0; y < height; y++) {
        if (numchannels == 4) {
            memcpy(nlayer->pixelData + (y * numchannels * width), rows[y], width * numchannels);
        }
        else if (numchannels == 3) {
            int currentRowPointer = 0;
            for (uint32_t x = 0; x < width; x++) {
                nlayer->pixelData[imagePointer++] = 0xff;
                nlayer->pixelData[imagePointer++] = rows[y][currentRowPointer++];
                nlayer->pixelData[imagePointer++] = rows[y][currentRowPointer++];
                nlayer->pixelData[imagePointer++] = rows[y][currentRowPointer++];
            }
        }
        else {
            printf("WHAT\n");
            exit(0);
        }
    }

    if (numchannels == 4) {
        SDL_Surface* convSrf = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_ABGR8888);
        memcpy(convSrf->pixels, nlayer->pixelData, height * width * 4);
        SDL_ConvertPixels(width, height, SDL_PIXELFORMAT_ABGR8888, convSrf->pixels, convSrf->pitch, SDL_PIXELFORMAT_ARGB8888, nlayer->pixelData, width * 4);
        SDL_FreeSurface(convSrf);
    }

    for (uint32_t y = 0; y < height; y++) {
        delete[] rows[y];
    }
    delete[] rows;
    png_destroy_read_struct(&png, &info, NULL);

    return nlayer;
}
Layer* readPNG(PlatformNativePathString path, uint64_t seek)
{
    FILE* pngfile = platformOpenFile(path, PlatformFileModeRB);
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    png_init_io(png, pngfile);
    png_read_info(png, info);

    uint32_t width = png_get_image_width(png, info);
    uint32_t height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    int numchannels = png_get_channels(png, info);
    png_bytepp rows = new png_bytep[height];
    for (int y = 0; y < height; y++) {
        rows[y] = new png_byte[png_get_rowbytes(png, info)];
    }
    png_read_image(png, rows);

    Layer* nlayer = new Layer(width, height);
    nlayer->name = "PNG Image";
    /*if (numchannels != 4) {
        printf("hey!!!!!! don't do this yet\n");
    }*/

    int imagePointer = 0;
    for (uint32_t y = 0; y < height; y++) {
        if (numchannels == 4) {
            memcpy(nlayer->pixelData + (y * numchannels * width), rows[y], width * numchannels);
        }
        else if (numchannels == 3) {
            int currentRowPointer = 0;
            for (uint32_t x = 0; x < width; x++) {
                nlayer->pixelData[imagePointer++] = 0xff;
                nlayer->pixelData[imagePointer++] = rows[y][currentRowPointer++];
                nlayer->pixelData[imagePointer++] = rows[y][currentRowPointer++];
                nlayer->pixelData[imagePointer++] = rows[y][currentRowPointer++];
            }
        }
        else {
            printf("WHAT\n");
            exit(0);
        }
    }

    if (numchannels == 4) {
        SDL_Surface* convSrf = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_ABGR8888);
        memcpy(convSrf->pixels, nlayer->pixelData, height * width * 4);
        SDL_ConvertPixels(width, height, SDL_PIXELFORMAT_ABGR8888, convSrf->pixels, convSrf->pitch, SDL_PIXELFORMAT_ARGB8888, nlayer->pixelData, width * 4);
        SDL_FreeSurface(convSrf);
    }

    for (uint32_t y = 0; y < height; y++) {
        delete[] rows[y];
    }
    delete[] rows;
    png_destroy_read_struct(&png, &info, NULL);

    fclose(pngfile);

    return nlayer;
}

Layer* readTGA(std::string path, uint64_t seek) {
    SDL_Surface* tgasrf = IMG_Load(path.c_str());
    
    return new Layer(tgasrf);
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

        char ddsheader[4];
        fseek(texfile, 0x34, SEEK_SET);
        fread(&ddsheader, 4, 1, texfile);
        if (ddsheader[0] == 'D' && ddsheader[1] == 'D' && ddsheader[2] == 'S') {
            fclose(texfile);
            return readDDS(path, 0x34);
        }
        else {

            fseek(texfile, 0x38, SEEK_SET);
            uint8_t* tgaData = (uint8_t*)malloc(filesize - 0x38);
            fread(tgaData, filesize - 0x38, 1, texfile);
            fclose(texfile);
            SDL_RWops* tgarw = SDL_RWFromMem(tgaData, filesize - 0x38);
            //todo: dds
            SDL_Surface* tgasrf = IMG_LoadTGA_RW(tgarw);
            SDL_RWclose(tgarw);
            free(tgaData);
            return new Layer(tgasrf);
        }
    }
    else {
        return NULL;
    }
}

Layer* readSDLImage(std::string path, uint64_t seek)
{
    SDL_Surface* img = IMG_Load(path.c_str());

    return img == NULL ? NULL : new Layer(img);
}

Layer* readWiiGCTPL(PlatformNativePathString path, uint64_t seek)
{
    struct TPLImageOffset {
        uint32_t headerOffset;
        uint32_t paletteHeader;
    };
    struct TPLImageHeader {
        uint16_t height;
        uint16_t width;
        uint32_t format;
        uint32_t imageDataAddress;
        uint32_t wrapS;
        uint32_t wrapT;
        uint32_t minFilter;
        uint32_t magFilter;
        float LODBias;
        uint8_t edgeLODEnable;
        uint8_t minLOD;
        uint8_t maxLOD;
        uint8_t unpacked;
    };
    struct TPLPaletteHeader {
        uint16_t entryCount;
        uint8_t unpacked;
        uint8_t paddingByte;
        uint32_t paletteFormat;
        uint32_t paletteDataAddress;

        bool set;
    };
    struct TPLImage {
        TPLImageHeader imgHdr;
        TPLPaletteHeader pltHdr;
    };

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
        printf("[TPL] %i image(s)\n", nImages);

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
            printf("image: %i x %i, format: %x, address: %x\n", nImg.imgHdr.height, nImg.imgHdr.width, nImg.imgHdr.format, nImg.imgHdr.imageDataAddress);
            if (i.paletteHeader != NULL) {
                fseek(infile, i.paletteHeader, SEEK_SET);
                fread(&nImg.pltHdr, sizeof(TPLPaletteHeader), 1, infile);
            }
            nImg.pltHdr.set = i.paletteHeader != NULL;

            fseek(infile, nImg.imgHdr.imageDataAddress, SEEK_SET);
            switch (nImg.imgHdr.format) {
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
                                        uint16_t px;
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
                default:
                    printf("unsupported format\n");
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
        printf("Mapper: %i%i\n", header.flag7>>4, header.flag6 >> 4);
        bool trainerPresent = (header.flag6 >> 3) & 0b1;

        if (trainerPresent) {
            fseek(infile, 512, SEEK_CUR);
        }
        fseek(infile, 16384 * header.prgRoms, SEEK_CUR);
        uint8_t* chrRomData = (uint8_t*)malloc(8192 * (int)header.chrRoms);
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

        free(chrRomData);
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
                printf("format [%i] not supported\n", desc.format);
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
        struct VTFHEADER
        {
            char            signature[4];       // File signature ("VTF\0"). (or as little-endian integer, 0x00465456)
            unsigned int    version[2];         // version[0].version[1] (currently 7.2).
            unsigned int    headerSize;         // Size of the header struct  (16 byte aligned; currently 80 bytes) + size of the resources dictionary (7.3+).
            unsigned short  width;              // Width of the largest mipmap in pixels. Must be a power of 2.
            unsigned short  height;             // Height of the largest mipmap in pixels. Must be a power of 2.
            unsigned int    flags;              // VTF flags.
            unsigned short  frames;             // Number of frames, if animated (1 for no animation).
            unsigned short  firstFrame;         // First frame in animation (0 based). Can be -1 in environment maps older than 7.5, meaning there are 7 faces, not 6.
            unsigned char   padding0[4];        // reflectivity padding (16 byte alignment).
            float           reflectivity[3];    // reflectivity vector.
            unsigned char   padding1[4];        // reflectivity padding (8 byte packing).
            float           bumpmapScale;       // Bumpmap scale.
            int             highResImageFormat; // High resolution image format.
            unsigned char   mipmapCount;        // Number of mipmaps.
            int             lowResImageFormat;  // Low resolution image format (Usually DXT1).
            unsigned char   lowResImageWidth;   // Low resolution image width.
            unsigned char   lowResImageHeight;  // Low resolution image height.

            // 7.2+
            unsigned short  depth;              // Depth of the largest mipmap in pixels. Must be a power of 2. Is 1 for a 2D texture.

            // 7.3+
            unsigned char   padding2[3];        // depth padding (4 byte alignment).
            unsigned int    numResources;       // Number of resources this vtf has. The max appears to be 32.

            unsigned char   padding3[8];        // Necessary on certain compilers
        };
        struct VTF_RESOURCE_ENTRY
        {
            unsigned char	tag[3]; 		// A three-byte "tag" that identifies what this resource is.
            unsigned char	flags;			// Resource entry flags. The only known flag is 0x2, which indicates that no data chunk corresponds to this resource.
            unsigned int	offset;			// The offset of this resource's data in the file. 
        };
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

        printf("[VTF] VERSION: %i.%i\n", hdr.version[0], hdr.version[1]);
        printf("[VTF] LowRes IMAGE FORMAT: %i   WxH: %i x %i\n", hdr.lowResImageFormat, hdr.lowResImageWidth, hdr.lowResImageHeight);
        printf("[VTF] HiRes IMAGE FORMAT: %i   WxH: %i x %i\n", hdr.highResImageFormat, hdr.width, hdr.height);
        printf("[VTF] Mipmaps: %i\n", hdr.mipmapCount);

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
                printf("[VTF] Found resource: %i %i %i  offset: %x\n", vtfRes.tag[0], vtfRes.tag[1], vtfRes.tag[2], vtfRes.offset);
                resources.push_back(vtfRes);
            }
            printf("[VTF] numResources = %i\n", resources.size());

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
                        printf("IMAGE FORMAT NOT IMPLEMENTED\n");
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
                printf("IMAGE FORMAT NOT IMPLEMENTED\n");
                break;
            }*/
            fseek(infile, hdr.headerSize, SEEK_SET);
            fseek(infile, (hdr.lowResImageWidth / 4) * (hdr.lowResImageHeight / 4) * 8, SEEK_CUR);

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
    struct MSPHeader
    {
        uint16_t  Key1;             /* Magic number    */
        uint16_t  Key2;             /* Magic number    */
        uint16_t  Width;            /* Width of the bitmap in pixels   */
        uint16_t  Height;           /* Height of the bitmap in pixels   */
        uint16_t  XARBitmap;        /* X Aspect ratio of the bitmap   */
        uint16_t  YARBitmap;        /* Y Aspect ratio of the bitmap   */
        uint16_t  XARPrinter;       /* X Aspect ratio of the printer   */
        uint16_t  YARPrinter;       /* Y Aspect ratio of the printer   */
        uint16_t  PrinterWidth;     /* Width of the printer in pixels   */
        uint16_t  PrinterHeight;    /* Height of the printer in pixels   */
        uint16_t  XAspectCorr;      /* X aspect correction (unused)     */
        uint16_t  YAspectCorr;      /* Y aspect correction (unused)     */
        uint16_t  Checksum;         /* Checksum of previous 24 bytes   */
        uint16_t  Padding[3];       /* Unused padding    */
    };

    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        fseek(infile, 0, SEEK_END);
        uint64_t fileLength = ftell(infile);
        fseek(infile, 0, SEEK_SET);

        MSPHeader hdr;
        printf("%i\n", sizeof(MSPHeader));
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

MainEditor* readOpenRaster(PlatformNativePathString path)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        MainEditor* ret = NULL;
        //read .ora file using zip
        zip_t *zip = zip_cstream_open(f, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
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
                        char* name = (char*)malloc(nameLen+1);
                        memset(name, 0, nameLen + 1);
                        fread(name, nameLen, 1, infile);

                        Layer* newLayer = new Layer(dimensions.x, dimensions.y);
                        newLayer->name = std::string(name);
                        free(name);
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
                {
                    XY dimensions;
                    fread(&dimensions.x, 4, 1, infile);
                    fread(&dimensions.y, 4, 1, infile);

                    char metaHeader[13];
                    fread(metaHeader, 13, 1, infile);
                    // this should equal /VOIDSN.META/
                    if (memcmp(metaHeader, "/VOIDSN.META/", 13) != 0) {
						printf("INVALID META HEADER\n");
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
                        //WHAT
                        //HOW DOES THIS
                        int valSize;
                        fread(&valSize, 4, 1, infile);
                        std::string val;
                        val.resize(valSize);
                        fread(&val[0], valSize, 1, infile);
                        extData[key] = val;
                    }

                    std::vector<Layer*> layers;
                    int nlayers;
                    fread(&nlayers, 4, 1, infile);
                    for (int x = 0; x < nlayers; x++) {
                        int nameLen;
                        fread(&nameLen, 4, 1, infile);
                        char* name = (char*)malloc(nameLen+1);
                        memset(name, 0, nameLen + 1);
                        fread(name, nameLen, 1, infile);

                        Layer* newLayer = new Layer(dimensions.x, dimensions.y);
                        newLayer->name = std::string(name);

                        char colorKeySet;
                        fread(&colorKeySet, 1, 1, infile);
                        newLayer->colorKeySet = colorKeySet == '\1';
                        fread(&newLayer->colorKey, 4, 1, infile);

                        free(name);
                        fread(newLayer->pixelData, newLayer->w * newLayer->h, 4, infile);
                        layers.push_back(newLayer);
                    }
                    MainEditor* ret = new MainEditor(layers);
                    if (extData.contains("tile.dim.x")) { ret->tileDimensions.x = std::stoi(extData["tile.dim.x"]); }
                    if (extData.contains("tile.dim.y")) { ret->tileDimensions.y = std::stoi(extData["tile.dim.y"]); }
                    if (extData.contains("sym.x")) { ret->symmetryPositions.x = std::stoi(extData["sym.x"]); }
                    if (extData.contains("sym.y")) { ret->symmetryPositions.y = std::stoi(extData["sym.y"]); }
                    if (extData.contains("layer.selected")) { ret->selLayer = std::stoi(extData["layer.selected"]); }
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
                        int nextSC = commentsData.find_first_of(';');
                        int commentsCount = std::stoi(commentsData.substr(0, nextSC));
                        commentsData = commentsData.substr(nextSC + 1);
                        for (int x = 0; x < commentsCount; x++) {
                            CommentData newComment;
                            nextSC = commentsData.find_first_of(';');
                            newComment.position.x = std::stoi(commentsData.substr(0, nextSC));
                            commentsData = commentsData.substr(nextSC + 1);
                            nextSC = commentsData.find_first_of(';');
                            newComment.position.y = std::stoi(commentsData.substr(0, nextSC));
                            commentsData = commentsData.substr(nextSC + 1);
                            nextSC = commentsData.find_first_of(';');
                            newComment.data = commentsData.substr(0, nextSC);
                            std::replace(newComment.data.begin(), newComment.data.end(), '\1', ';');
                            commentsData = commentsData.substr(nextSC + 1);
                            ret->comments.push_back(newComment);
                        }
                    }
                    fclose(infile);
                    return ret;
                }
                break;
            default:
                printf("VOIDSN FILE v%i NOT SUPPORTED\n", voidsnversion);
                fclose(infile);
                return NULL;
        }

        fclose(infile);
    }
    return NULL;
}

bool writePNG(PlatformNativePathString path, Layer* data)
{
    // exports png
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        png_structp outpng = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_infop outpnginfo = png_create_info_struct(outpng);
        setjmp(png_jmpbuf(outpng));
        png_init_io(outpng, outfile);
        png_set_compression_level(outpng, Z_BEST_COMPRESSION);
        png_set_compression_mem_level(outpng, MAX_MEM_LEVEL);
        png_set_compression_buffer_size(outpng, 1024 * 1024);
        setjmp(png_jmpbuf(outpng));
        png_set_IHDR(outpng, outpnginfo, data->w, data->h, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_DEFAULT);
        setjmp(png_jmpbuf(outpng));
        png_write_info(outpng, outpnginfo);

        uint8_t* convertedToABGR = (uint8_t*)malloc(data->w * data->h * 4);
        SDL_ConvertPixels(data->w, data->h, SDL_PIXELFORMAT_ARGB8888, data->pixelData, data->w * 4, SDL_PIXELFORMAT_ABGR8888, convertedToABGR, data->w * 4);

        png_bytepp rows = new png_bytep[data->h];
        for (int y = 0; y < data->h; y++) {
            rows[y] = convertedToABGR + (y * data->w*4);
        }
        png_write_image(outpng, rows);
        delete[] rows;
        png_write_end(outpng, outpnginfo);

        png_destroy_write_struct(&outpng, &outpnginfo);
        free(convertedToABGR);
        fclose(outfile);
        return true;
    }
    return false;
}

bool writeVOIDSNv1(PlatformNativePathString path, XY projDimensions, std::vector<Layer*> data)
{
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
                printf("[VOIDSNv1] INVALID LAYER DIMENSIONS (THIS IS BAD)");
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
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        uint8_t voidsnVersion = 0x02;
        fwrite(&voidsnVersion, 1, 1, outfile);
        uint32_t nvalBuffer;

        nvalBuffer = editor->texW;
        fwrite(&nvalBuffer, 4, 1, outfile);
        nvalBuffer = editor->texH;
        fwrite(&nvalBuffer, 4, 1, outfile);

        fwrite(&editor->tileDimensions.x, 4, 1, outfile);
        fwrite(&editor->tileDimensions.y, 4, 1, outfile);

        nvalBuffer = editor->layers.size();
        fwrite(&nvalBuffer, 4, 1, outfile);

        for (Layer*& lr : editor->layers) {
            if (lr->w * lr->h != editor->texW * editor->texH) {
                printf("[VOIDSNv2] INVALID LAYER DIMENSIONS (THIS IS BAD)");
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
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        uint8_t voidsnVersion = 0x03;
        fwrite(&voidsnVersion, 1, 1, outfile);
        uint32_t nvalBuffer;

        nvalBuffer = editor->texW;
        fwrite(&nvalBuffer, 4, 1, outfile);
        nvalBuffer = editor->texH;
        fwrite(&nvalBuffer, 4, 1, outfile);

        //fwrite(&editor->tileDimensions.x, 4, 1, outfile);
        //fwrite(&editor->tileDimensions.y, 4, 1, outfile);

        std::string commentsData;
        commentsData += std::to_string(editor->comments.size()) + ';';
        for (CommentData& c : editor->comments) {
            commentsData += std::to_string(c.position.x) + ';';
            commentsData += std::to_string(c.position.y) + ';';
            std::string sanitizedData = c.data;
            std::replace(sanitizedData.begin(), sanitizedData.end(), ';', '\1');
            commentsData += sanitizedData + ';';
        }

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
            {"layer.visibility", layerVisibilityData}
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
            if (lr->w * lr->h != editor->texW * editor->texH) {
                printf("[VOIDSNv3] INVALID LAYER DIMENSIONS (THIS IS BAD)");
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

bool writeOpenRaster(PlatformNativePathString path, MainEditor* editor)
{
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
                xmls += std::format("  <layer opacity=\"1\" x=\"0\" name=\"{}\" y=\"0\" src=\"data/layer{}.png\" composite-op=\"svg:src-over\" visibility=\"{}\"/>\n", (*l)->name, i++, (*l)->hidden ? "hidden" : "visible");
            }
            xmls += "  </stack>\n";
            xmls += "</image>\n";
            zip_entry_write(zip, xmls.c_str(), xmls.size());
        }
        zip_entry_close(zip);

        zip_entry_open(zip, "mergedimage.png"); 
        {
            Layer* flat = editor->flattenImage();
#if _WIN32
            if (writePNG(L"temp.bin", flat)) {
#else
            if (writePNG("temp.bin", flat)) {
#endif
                zip_entry_fwrite(zip, "temp.bin");
            }
            else {
                printf("OPENRASTER: PNG WRITE ERROR\n");
            }
            delete flat;
        }
        zip_entry_close(zip);

        zip_entry_open(zip, "Thumbnails/thumbnail.png"); 
        {
            Layer* flat = editor->flattenImage();
            Layer* flatScaled = flat->copyScaled(XY{255,255});
            delete flat;
#if _WIN32
            if (writePNG(L"temp.bin", flatScaled)) {
#else
            if (writePNG("temp.bin", flatScaled)) {
#endif
                zip_entry_fwrite(zip, "temp.bin");
            }
            else {
                printf("OPENRASTER: PNG WRITE ERROR\n");
            }
            delete flatScaled;
        }
        zip_entry_close(zip);

        int i = 0;
        for (auto l = data.rbegin(); l != data.rend(); l++) {
#if _WIN32
            if (writePNG(L"temp.bin", *l)) {
#else
            if (writePNG("temp.bin", *l)) {
#endif
                std::string fname = std::format("data/layer{}.png", i++);
                zip_entry_open(zip, fname.c_str());
                zip_entry_fwrite(zip, "temp.bin");
                zip_entry_close(zip);
            }
            else {
                printf("OPENRASTER: PNG WRITE ERROR\n");
            }
        }

        zip_stream_copy(zip, (void**)&zipBuffer, &zipBufferSize);
    }
    zip_close(zip);

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {

        fwrite(zipBuffer, zipBufferSize, 1, f);
        fclose(f);
        free(zipBuffer);
        return true;
    }
    free(zipBuffer);
    return false;
}

bool writeXYZ(PlatformNativePathString path, Layer* data)
{
    std::vector<uint32_t> uniqueColors = data->getUniqueColors(true);
    if (uniqueColors.size() > 256) {
        printf("[XYZ] Too many colors\n");
        return false;
    }
    FILE* outfile = platformOpenFile(path, PlatformFileModeWB);
    if (outfile != NULL) {
        fwrite("XYZ1", 4, 1, outfile);
        fwrite(&data->w, 2, 1, outfile);
        fwrite(&data->h, 2, 1, outfile);
        uint8_t paletteData[256*3];
        int p = 0;
        for (uint32_t& a : uniqueColors) {
            uint32_t color = a;
            paletteData[p++] = (color >> 16) & 0xff;
            paletteData[p++] = (color >> 8) & 0xff;
            paletteData[p++] = color&0xff;
        }
        uint8_t* pxPalleteData = (uint8_t*)malloc(data->w * data->h);
        uint32_t* pixelData32 = (uint32_t*)data->pixelData;
        for (uint64_t x = 0; x < data->w * data->h; x++) {
            uint32_t pixel = pixelData32[x] | 0xff000000;
            int index = std::find(uniqueColors.begin(), uniqueColors.end(), pixel) - uniqueColors.begin();
            pxPalleteData[x] = (uint8_t)(index);
        }
        unsigned long dataLength = 256 * 3 + data->w * data->h;
        uint8_t* combined = (uint8_t*)malloc(dataLength);
        memcpy(combined, paletteData, 256 * 3);
        memcpy(combined+(256*3), pxPalleteData, data->w * data->h);
        uint8_t* dst = (uint8_t*)malloc(dataLength);
        compress((Bytef*)dst, &dataLength, combined, dataLength);
        fwrite(dst, dataLength, 1, outfile);

        free(combined);
        free(dst);
        free(pxPalleteData);
        fclose(outfile);
        return true;
    }

    return false;
}

bool writeBMP(PlatformNativePathString path, Layer* data) {

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

bool writeCaveStoryPBM(PlatformNativePathString path, Layer* data) {

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

bool writeTGA(PlatformNativePathString path, Layer* data) {
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
        fprintf(outfile, "uint8_t voidsprite_image_data[] = {\n");
        uint32_t* pxd = (uint32_t*)data->pixelData;
        uint64_t dp = 0;
        for (int y = 0; y < data->h; y++) {
            for (int x = 0; x < data->w; x++) {
                fprintf(outfile, "0x%08X,", pxd[dp++]);
            }
            fprintf(outfile, "\n");
        }
        fprintf(outfile, "};\n");

        fclose(outfile);
        return true;
    }
    return false;
}

bool writePythonNPArray(PlatformNativePathString path, Layer* data)
{
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