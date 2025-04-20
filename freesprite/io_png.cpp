//libpng must be the first include or else it will cry
#if _WIN32
#include "libpng/png.h"
#else
#include <libpng/png.h>
#endif

#include "io_base.h"
#include "io_png.h"

#include "base64/base64.hpp"

std::string getlibpngVersion()
{
    return PNG_LIBPNG_VER_STRING;
}

size_t readPNGBytes = 0;   //if you promise to tell noone
size_t PNGFileSize = 0;

void _readPNGDataFromMem(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if (io_ptr == NULL) {
        logerr("WHY  IS io_ptr NULL");
        return;
    }

    char* inputStream = (char*)io_ptr;
    memcpy(outBytes, inputStream + readPNGBytes, byteCountToRead);
    readPNGBytes += byteCountToRead;
}

Layer* readPNGFromBase64String(std::string b64)
{
    auto seekTo = b64.find("iVBO");
    if (seekTo != std::string::npos) {
        try {
            std::string pixelData = b64.substr(seekTo);
            std::string pixelsb64 = base64::from_base64(pixelData);
            uint8_t* imageData = (uint8_t*)pixelsb64.c_str();
            return readPNGFromMem(imageData, pixelsb64.size());
        }
        catch (std::exception&) {
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

void addPNGText(png_structp outpng, png_infop outpnginfo, std::string key, std::string text) {
    png_text pngt;
    pngt.key = (char*)key.c_str();
    pngt.text = (char*)text.c_str();
    pngt.compression = PNG_TEXT_COMPRESSION_NONE;
    png_set_text(outpng, outpnginfo, &pngt, 1);
}

Layer* readPNG(png_structp png, png_infop info) {
    uint32_t width = png_get_image_width(png, info);
    uint32_t height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    Layer* ret;

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        LayerPalettized* ret2 = new LayerPalettized(width, height);
        ret2->name = "PNG Image";
        png_colorp palette;
        int num_palette;
        png_get_PLTE(png, info, &palette, &num_palette);

        for (int x = 0; x < num_palette; x++) {
            ret2->palette.push_back(0xFF000000 | (palette[x].red << 16) | (palette[x].green << 8) | palette[x].blue);
        }

        if (png_get_valid(png, info, PNG_INFO_tRNS)) {
            png_bytep trns;
            int num_trns;
            png_color_16p trns16p;
            png_get_tRNS(png, info, &trns, &num_trns, &trns16p);

            for (int x = 0; x < num_trns; x++) {
                ret2->palette[x] = (ret2->palette[x] & 0x00FFFFFF) | (trns[x] << 24);
            }
        }

        png_bytepp rows = new png_bytep[height];
        for (int y = 0; y < height; y++) {
            rows[y] = new png_byte[png_get_rowbytes(png, info)];
        }
        png_read_image(png, rows);

        uint32_t* pxData = (uint32_t*)ret2->pixelData;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                pxData[y * width + x] =
                    bit_depth == 1 ? (rows[y][x / 8] >> (7 - (x % 8))) & 0b1
                    : bit_depth == 2 ? (rows[y][x / 4] >> (2 * (3 - (x % 4)))) & 0b11
                    : bit_depth == 4 ? (rows[y][x / 2] >> (x % 2 == 0 ? 4 : 0)) & 0b1111
                    : rows[y][x];   //todo, more bit depths
                //: are those all of them?
            }
        }
        for (int y = 0; y < height; y++) {
            delete[] rows[y];
        }
        delete[] rows;
        ret = ret2;
    }
    else {
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
            png_set_expand_gray_1_2_4_to_8(png);
        if (png_get_valid(png, info, PNG_INFO_tRNS))
            png_set_tRNS_to_alpha(png);
        if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY)
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

        ret = new Layer(width, height);
        ret->name = "PNG Image";

        int imagePointer = 0;
        for (uint32_t y = 0; y < height; y++) {
            if (numchannels == 4) {
                memcpy(ret->pixelData + (y * numchannels * width), rows[y], width * numchannels);
            }
            else if (numchannels == 3) {
                int currentRowPointer = 0;
                for (uint32_t x = 0; x < width; x++) {
                    ret->pixelData[imagePointer++] = 0xff;
                    ret->pixelData[imagePointer++] = rows[y][currentRowPointer++];
                    ret->pixelData[imagePointer++] = rows[y][currentRowPointer++];
                    ret->pixelData[imagePointer++] = rows[y][currentRowPointer++];
                }
            }
            else {
                logprintf("WHAT\n");
                delete ret;
                png_destroy_read_struct(&png, &info, NULL);
                return NULL;
            }
        }

        if (numchannels == 4) {
            SDL_Surface* convSrf = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_ABGR8888);
            memcpy(convSrf->pixels, ret->pixelData, height * width * 4);
            SDL_ConvertPixels(width, height, SDL_PIXELFORMAT_ABGR8888, convSrf->pixels, convSrf->pitch, SDL_PIXELFORMAT_ARGB8888, ret->pixelData, width * 4);
            SDL_FreeSurface(convSrf);
        }

        for (uint32_t y = 0; y < height; y++) {
            delete[] rows[y];
        }
        delete[] rows;
    }
    if (g_config.saveLoadFlatImageExtData) {
        png_textp text_ptr;
        int num_text;
        png_get_text(png, info, &text_ptr, &num_text);
        std::map<std::string, std::string> extdata;
        for (int x = 0; x < num_text; x++) {
            std::string key = text_ptr[x].key;
            if (stringStartsWithIgnoreCase(key, "vsp/")) {
                extdata[key.substr(4)] = text_ptr[x].text;
            }
        }
        ret->importExportExtdata = extdata;
    }

    png_destroy_read_struct(&png, &info, NULL);
    return ret;
}

Layer* readPNGFromMem(uint8_t* data, size_t dataSize) {

    if (dataSize < 8) {
        logprintf("PNG data too small\n");
        return NULL;
    }
    else if (png_sig_cmp(data, 0, 8)) {
        logprintf("Not a PNG file\n");
        return NULL;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    readPNGBytes = 0;
    PNGFileSize = dataSize;
    png_set_read_fn(png, (void*)data, _readPNGDataFromMem);
    //png_set_sig_bytes(png, kPngSignatureLength);
    png_read_info(png, info);

    return readPNG(png, info);
}

//todo: condense this and writePNG into one function
std::vector<u8> writePNGToMem(Layer* data)
{
    if (data->isPalettized && ((LayerPalettized*)data)->palette.size() > 256) {
        //g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Too many colors in palette"));
        return {};
    }

    std::vector<u8> ret = {};
    png_rw_ptr writeFunc = [](png_structp png_ptr, png_bytep data, png_size_t length) {
        std::vector<u8>* ret = (std::vector<u8>*)png_get_io_ptr(png_ptr);
        ret->insert(ret->end(), data, data + length);
        };
    png_structp outpng = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop outpnginfo = png_create_info_struct(outpng);
    setjmp(png_jmpbuf(outpng));
    png_set_write_fn(outpng, &ret, writeFunc, [](png_structp) {});
    png_set_compression_level(outpng, Z_BEST_COMPRESSION);
    png_set_compression_mem_level(outpng, MAX_MEM_LEVEL);
    png_set_compression_buffer_size(outpng, 1024 * 1024);

    if (g_config.saveLoadFlatImageExtData) {
        addPNGText(outpng, outpnginfo, "Software", "voidsprite - libpng " PNG_LIBPNG_VER_STRING);
        auto extmap = data->importExportExtdata;
        for (auto& kv : extmap) {
            addPNGText(outpng, outpnginfo, "vsp/" + kv.first, kv.second);
        }
    }

    setjmp(png_jmpbuf(outpng));

    if (!data->isPalettized) {
        png_set_IHDR(outpng, outpnginfo, data->w, data->h, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_DEFAULT);
        setjmp(png_jmpbuf(outpng));
        png_write_info(outpng, outpnginfo);

        uint8_t* convertedToABGR = (uint8_t*)tracked_malloc(data->w * data->h * 4);
        SDL_ConvertPixels(data->w, data->h, SDL_PIXELFORMAT_ARGB8888, data->pixelData, data->w * 4, SDL_PIXELFORMAT_ABGR8888, convertedToABGR, data->w * 4);

        png_bytepp rows = new png_bytep[data->h];
        for (int y = 0; y < data->h; y++) {
            rows[y] = convertedToABGR + (y * data->w * 4);
        }
        png_write_image(outpng, rows);
        delete[] rows;
        tracked_free(convertedToABGR);
    }
    else {
        LayerPalettized* pltLayer = (LayerPalettized*)data;
        png_set_IHDR(outpng, outpnginfo, data->w, data->h, 8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_DEFAULT);
        setjmp(png_jmpbuf(outpng));

        png_colorp plt = new png_color[pltLayer->palette.size()];
        memset(plt, 0, pltLayer->palette.size() * sizeof(png_color));
        png_bytep trns = new png_byte[pltLayer->palette.size()];

        for (int x = 0; x < pltLayer->palette.size(); x++) {
            plt[x].red = (pltLayer->palette[x] >> 16) & 0xff;
            plt[x].green = (pltLayer->palette[x] >> 8) & 0xff;
            plt[x].blue = pltLayer->palette[x] & 0xff;
            trns[x] = (pltLayer->palette[x] >> 24) & 0xff;
        }

        png_set_PLTE(outpng, outpnginfo, plt, pltLayer->palette.size());
        png_set_tRNS(outpng, outpnginfo, trns, pltLayer->palette.size(), NULL);
        png_write_info(outpng, outpnginfo);

        int32_t* pixelData32 = (int32_t*)pltLayer->pixelData;
        png_bytepp rows = new png_bytep[data->h];
        for (int y = 0; y < data->h; y++) {
            png_bytep row = new png_byte[data->w];
            for (int x = 0; x < data->w; x++) {
                row[x] = pixelData32[x + y * data->w];
            }
            rows[y] = row;
        }
        png_write_image(outpng, rows);
        delete[] plt;
        delete[] trns;
        for (int y = 0; y < data->h; y++) {
            delete[] rows[y];
        }
        delete[] rows;

    }
    png_write_end(outpng, outpnginfo);

    png_destroy_write_struct(&outpng, &outpnginfo);
    return ret;
}

Layer* readPNG(PlatformNativePathString path, uint64_t seek)
{
    FILE* pngfile = platformOpenFile(path, PlatformFileModeRB);
    if (pngfile != NULL) {
        u8 sig[8];
        fread(sig, 1, 8, pngfile);
        if (png_sig_cmp(sig, 0, 8)) {
            logprintf("Not a PNG file\n");
            return NULL;
        }
        fseek(pngfile, 0, SEEK_SET);

        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_infop info = png_create_info_struct(png);
        png_init_io(png, pngfile);
        png_read_info(png, info);

        Layer* ret = readPNG(png, info);
        fclose(pngfile);
        return ret;
    }
    return NULL;
}

bool writePNG(PlatformNativePathString path, Layer* data)
{
    if (data->isPalettized && ((LayerPalettized*)data)->palette.size() > 256) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Too many colors in palette"));
        return false;
    }

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

        if (g_config.saveLoadFlatImageExtData) {
            addPNGText(outpng, outpnginfo, "Software", "voidsprite - libpng " PNG_LIBPNG_VER_STRING);
            auto extmap = data->importExportExtdata;
            for (auto& kv : extmap) {
                addPNGText(outpng, outpnginfo, "vsp/" + kv.first, kv.second);
            }
        }

        setjmp(png_jmpbuf(outpng));

        if (!data->isPalettized) {
            png_set_IHDR(outpng, outpnginfo, data->w, data->h, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_DEFAULT);
            setjmp(png_jmpbuf(outpng));
            png_write_info(outpng, outpnginfo);

            uint8_t* convertedToABGR = (uint8_t*)tracked_malloc(data->w * data->h * 4);
            SDL_ConvertPixels(data->w, data->h, SDL_PIXELFORMAT_ARGB8888, data->pixelData, data->w * 4, SDL_PIXELFORMAT_ABGR8888, convertedToABGR, data->w * 4);

            png_bytepp rows = new png_bytep[data->h];
            for (int y = 0; y < data->h; y++) {
                rows[y] = convertedToABGR + (y * data->w * 4);
            }
            png_write_image(outpng, rows);
            delete[] rows;
            tracked_free(convertedToABGR);
        }
        else {
            LayerPalettized* pltLayer = (LayerPalettized*)data;
            png_set_IHDR(outpng, outpnginfo, data->w, data->h, 8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_DEFAULT);
            setjmp(png_jmpbuf(outpng));

            png_colorp plt = new png_color[pltLayer->palette.size()];
            memset(plt, 0, pltLayer->palette.size() * sizeof(png_color));
            png_bytep trns = new png_byte[pltLayer->palette.size()];

            for (int x = 0; x < pltLayer->palette.size(); x++) {
                plt[x].red = (pltLayer->palette[x] >> 16) & 0xff;
                plt[x].green = (pltLayer->palette[x] >> 8) & 0xff;
                plt[x].blue = pltLayer->palette[x] & 0xff;
                trns[x] = (pltLayer->palette[x] >> 24) & 0xff;
            }

            png_set_PLTE(outpng, outpnginfo, plt, pltLayer->palette.size());
            png_set_tRNS(outpng, outpnginfo, trns, pltLayer->palette.size(), NULL);
            png_write_info(outpng, outpnginfo);

            int32_t* pixelData32 = (int32_t*)pltLayer->pixelData;
            png_bytepp rows = new png_bytep[data->h];
            for (int y = 0; y < data->h; y++) {
                png_bytep row = new png_byte[data->w];
                for (int x = 0; x < data->w; x++) {
                    row[x] = pixelData32[x + y * data->w];
                }
                rows[y] = row;
            }
            png_write_image(outpng, rows);
            delete[] plt;
            delete[] trns;
            for (int y = 0; y < data->h; y++) {
                delete[] rows[y];
            }
            delete[] rows;

        }
        png_write_end(outpng, outpnginfo);

        png_destroy_write_struct(&outpng, &outpnginfo);
        fclose(outfile);
        return true;
    }
    return false;
}