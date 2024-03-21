#include "FileIO.h"
#include "maineditor.h"
#include <png.h>
#include "libtga/tga.h"

Layer* readXYZ(std::wstring path)
{
    FILE* f = NULL;
    _wfopen_s(&f, path.c_str(), L"rb");

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

        free(compressedData);
        free(decompBytes);

        fclose(f);
        return nLayer;
    }
    return NULL;
}

Layer* readPNG(std::wstring path)
{
    FILE* pngfile = NULL;
    _wfopen_s(&pngfile, path.c_str(), L"rb");
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
    /*if (numchannels != 4) {
        printf("hey!!!!!! don't do this yet\n");
    }*/

    int imagePointer = 0;
    for (int y = 0; y < height; y++) {
        if (numchannels == 4) {
            memcpy(nlayer->pixelData + (y * numchannels * width), rows[y], width * numchannels);
        }
        else if (numchannels == 3) {
            int currentRowPointer = 0;
            for (int x = 0; x < width; x++) {
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

    for (int y = 0; y < height; y++) {
        delete[] rows[y];
    }
    delete[] rows;
    png_destroy_read_struct(&png, &info, NULL);

    fclose(pngfile);

    return nlayer;
}

Layer* readTGA(std::string path) {
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

Layer* readAETEX(std::wstring path) {
    FILE* texfile = NULL;
    _wfopen_s(&texfile, path.c_str(), L"rb");
    if (texfile != NULL) {
        fseek(texfile, 0, SEEK_END);
        long filesize = ftell(texfile);
        fseek(texfile, 0x38, SEEK_SET);
        uint8_t* tgaData = (uint8_t*)malloc(filesize - 0x38);
        fread(tgaData, filesize - 0x38, 1, texfile);
        fclose(texfile);
        SDL_RWops* tgarw = SDL_RWFromMem(tgaData, filesize - 0x38);
        SDL_Surface* tgasrf = IMG_LoadTGA_RW(tgarw);
        SDL_RWclose(tgarw);
        free(tgaData);
        return new Layer(tgasrf);
    }
    else {
        return NULL;
    }
}

MainEditor* readVOIDSN(std::wstring path)
{
    FILE* infile = NULL;
    _wfopen_s(&infile, path.c_str(), L"rb");
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
                    MainEditor* ret = new MainEditor(layers[0]);
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

bool writePNG(std::wstring path, Layer* data)
{
    // exports png
    FILE* outfile;
    _wfopen_s(&outfile, path.c_str(), L"wb");
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

bool writeVOIDSNv1(std::wstring path, XY projDimensions, std::vector<Layer*> data)
{
    FILE* outfile = NULL;
    _wfopen_s(&outfile, path.c_str(), L"wb");
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
