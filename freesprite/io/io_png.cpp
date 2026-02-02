//libpng must be the first include or else it will cry
#include "../libpng/png.h"

#include <zlib.h>


#include "io_base.h"
#include "io_png.h"

#include "../base64/base64.hpp"

std::string getlibpngVersion()
{
    return PNG_LIBPNG_VER_STRING;
}

struct PNGReadContext {
    u8* data;
    size_t readPNGBytes;
    size_t PNGFileSize;
};

void _readPNGDataFromMem(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if (io_ptr == NULL) {
        logerr("WHY  IS io_ptr NULL");
        return;
    }

    PNGReadContext* ctx = (PNGReadContext*)io_ptr;
    memcpy(outBytes, ctx->data + ctx->readPNGBytes, byteCountToRead);
    ctx->readPNGBytes += byteCountToRead;
}

std::vector<PNGChunk> loadPNGChunksFromMem(std::vector<u8>& data)
{
    std::vector<PNGChunk> ret;
    size_t pos = 8; //skip signature
    while (pos + 8 <= data.size()) {
        PNGChunk chunk;
        chunk.length = beU32(*(u32*)(data.data() + pos));
        memcpy(chunk.type, data.data() + pos + 4, 4);
        chunk.type[4] = '\0';
        pos += 8;
        if (pos + chunk.length + 4 > data.size()) {
            logerr("[PNG] chunk extends past the end");
            break;
        }
        chunk.data.resize(chunk.length);
        if (chunk.length > 0) {
            memcpy(chunk.data.data(), &data[pos], chunk.length);
        }
        pos += chunk.length;
        chunk.crc = beU32(*(u32*)(data.data() + pos));
        pos += 4;
        ret.push_back(chunk);
    }
    return ret;
}

void calcCRCAndSize(PNGChunk* chunk)
{
    chunk->length = (u32)chunk->data.size();

    u32 crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (const Bytef*)chunk->type, 4);
    if (chunk->length > 0) {
        crc = crc32(crc, (const Bytef*)chunk->data.data(), chunk->length);
    }
    chunk->crc = crc;
}

void writeChunkToFile(FILE* f, PNGChunk* chunk)
{
    u32 beLength = beU32(chunk->length);
    fwrite(&beLength, 4, 1, f);
    fwrite(chunk->type, 1, 4, f);
    if (chunk->length > 0) {
        fwrite(chunk->data.data(), 1, chunk->length, f);
    }
    u32 beCRC = beU32(chunk->crc);
    fwrite(&beCRC, 4, 1, f);
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
        catch (std::exception& e) {
            logerr(frmt("Error reading base64 string:\n {}", e.what()));
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

Layer* _readPNG(png_structp png, png_infop info) {
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
        for (u32 y = 0; y < height; y++) {
            rows[y] = new png_byte[png_get_rowbytes(png, info)];
        }
        png_read_image(png, rows);

        uint32_t* pxData = ret2->pixels32();
        for (u32 y = 0; y < height; y++) {
            for (u32 x = 0; x < width; x++) {
                pxData[y * width + x] =
                    bit_depth == 1 ? (rows[y][x / 8] >> (7 - (x % 8))) & 0b1
                    : bit_depth == 2 ? (rows[y][x / 4] >> (2 * (3 - (x % 4)))) & 0b11
                    : bit_depth == 4 ? (rows[y][x / 2] >> (x % 2 == 0 ? 4 : 0)) & 0b1111
                    : rows[y][x];   //todo, more bit depths
                //: are those all of them?
            }
        }
        for (u32 y = 0; y < height; y++) {
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
        for (u32 y = 0; y < height; y++) {
            rows[y] = new png_byte[png_get_rowbytes(png, info)];
        }
        png_read_image(png, rows);

        ret = new Layer(width, height);
        ret->name = "PNG Image";
        u8* pixelData = ret->pixels8();

        int imagePointer = 0;
        for (uint32_t y = 0; y < height; y++) {
            if (numchannels == 4) {
                memcpy(pixelData + (y * numchannels * width), rows[y], width * numchannels);
            }
            else if (numchannels == 3) {
                int currentRowPointer = 0;
                for (uint32_t x = 0; x < width; x++) {
                    pixelData[imagePointer++] = 0xff;
                    pixelData[imagePointer++] = rows[y][currentRowPointer++];
                    pixelData[imagePointer++] = rows[y][currentRowPointer++];
                    pixelData[imagePointer++] = rows[y][currentRowPointer++];
                }
            }
            else {
                logerr("WHAT\n");
                delete ret;
                png_destroy_read_struct(&png, &info, NULL);
                return NULL;
            }
        }

        if (numchannels == 4) {
            SDL_Surface* convSrf = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_ABGR8888);
            memcpy(convSrf->pixels, ret->pixels32(), height * width * 4);
            SDL_ConvertPixels(width, height, SDL_PIXELFORMAT_ABGR8888, convSrf->pixels, convSrf->pitch, SDL_PIXELFORMAT_ARGB8888, ret->pixels32(), width * 4);
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
        logerr("PNG data too small");
        return NULL;
    }
    else if (png_sig_cmp(data, 0, 8)) {
        logerr("Not a PNG file");
        return NULL;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    PNGReadContext readCtx{
        .data = data,
        .readPNGBytes = 0,
        .PNGFileSize = dataSize
    };
    png_set_read_fn(png, (void*)&readCtx, _readPNGDataFromMem);
    //png_set_sig_bytes(png, kPngSignatureLength);
    png_read_info(png, info);

    return _readPNG(png, info);
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
        SDL_ConvertPixels(data->w, data->h, SDL_PIXELFORMAT_ARGB8888, data->pixels32(), data->w * 4, SDL_PIXELFORMAT_ABGR8888, convertedToABGR, data->w * 4);

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

        int32_t* pixelData32 = (int32_t*)pltLayer->pixels32();
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

        Layer* ret = _readPNG(png, info);
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
            SDL_ConvertPixels(data->w, data->h, SDL_PIXELFORMAT_ARGB8888, data->pixels32(), data->w * 4, SDL_PIXELFORMAT_ABGR8888, convertedToABGR, data->w * 4);

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

            int32_t* pixelData32 = (int32_t*)pltLayer->pixels32();
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

bool writeAPNG(PlatformNativePathString path, MainEditor* data)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {

        Layer* flat = data->flattenFrame(data->frames.front());
        std::vector<u8> frame1PNG = writePNGToMem(flat);
        std::vector<PNGChunk> frame1Chunks = loadPNGChunksFromMem(frame1PNG);
        delete flat;

        //write header
        u8 pngHeader[8] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
        fwrite(pngHeader, 1, 8, f);
        //copy all chunks that come before IDAT
        for (auto& chunk : frame1Chunks) {
            if (std::string(chunk.type) == "IDAT") {
                break;
            }
            writeChunkToFile(f, &chunk);
        }

        PNGChunk actlChunk;
        memcpy(actlChunk.type, "acTL", 5);
        actlChunk.data.resize(8);
        u32* actlData = (u32*)actlChunk.data.data();
        actlData[0] = beU32(data->frames.size()); //num frames
        actlData[1] = beU32(0); //num loops (0 means infinite)
        calcCRCAndSize(&actlChunk);
        writeChunkToFile(f, &actlChunk);

        int sequenceNumber = 0;

        PNGChunk frame1fcTLChunk;
        memcpy(frame1fcTLChunk.type, "fcTL", 5);
        frame1fcTLChunk.data.resize(sizeof(PNGfcTLChunk));
        PNGfcTLChunk* fcTLData = (PNGfcTLChunk*)frame1fcTLChunk.data.data();
        fcTLData->sequenceNumber = beU32(sequenceNumber++);
        fcTLData->width = beU32(data->canvas.dimensions.x);
        fcTLData->height = beU32(data->canvas.dimensions.y);
        fcTLData->xOffset = beU32(0);
        fcTLData->yOffset = beU32(0);
        fcTLData->delayNum = beU16(data->frameAnimMSPerFrame);
        fcTLData->delayDen = beU16(1000);
        fcTLData->disposeOp = 0; //APNG_DISPOSE_OP_NONE
        fcTLData->blendOp = 0; //APNG_BLEND_OP_SOURCE
        calcCRCAndSize(&frame1fcTLChunk);
        writeChunkToFile(f, &frame1fcTLChunk);

        //copy all idat chunks
        for (auto& chunk : frame1Chunks) {
            if (std::string(chunk.type) == "IDAT") {
                writeChunkToFile(f, &chunk);
            }
        }

        //write remaining frames
        for (int i = 1; i < data->frames.size(); i++) {
            Layer* flatFrame = data->flattenFrame(data->frames[i]);
            std::vector<u8> framePNG = writePNGToMem(flatFrame);

            PNGChunk fcTLChunk = frame1fcTLChunk;
            PNGfcTLChunk* fcTLData = (PNGfcTLChunk*)fcTLChunk.data.data();
            fcTLData->sequenceNumber = beU32(sequenceNumber++);
            calcCRCAndSize(&fcTLChunk);
            writeChunkToFile(f, &fcTLChunk);

            std::vector<PNGChunk> frameChunks = loadPNGChunksFromMem(framePNG);
            for (auto& chunk : frameChunks) {
                if (std::string(chunk.type) == "IDAT") {
                    memcpy(chunk.type, "fdAT", 5);
                    for (int x = 0; x < 4; x++) { chunk.data.insert(chunk.data.begin(), 0); } //make space for sequence number
                    u32* seqNumPtr = (u32*)chunk.data.data();
                    *seqNumPtr = beU32(sequenceNumber++);
                    calcCRCAndSize(&chunk);
                    writeChunkToFile(f, &chunk);
                }
            }
            delete flatFrame;

        }

        PNGChunk iendChunk;
        memcpy(iendChunk.type, "IEND", 5);
        iendChunk.data.resize(0);
        calcCRCAndSize(&iendChunk);
        writeChunkToFile(f, &iendChunk);

        fclose(f);
        return true;
    }
    return false;
}
