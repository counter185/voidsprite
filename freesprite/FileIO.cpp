#include "FileIO.h"
#include "maineditor.h"
#include <png.h>
#include "libtga/tga.h"
#include "ddspp/ddspp.h"
#include "easybmp/EasyBMP.h"

Layer* readXYZ(PlatformNativePathString path)
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

Layer* readPNG(PlatformNativePathString path)
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

Layer* readBMP(PlatformNativePathString path)
{
    BMP nbmp;
    nbmp.ReadFromFileW(path);
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

Layer* readAETEX(PlatformNativePathString path) {
    FILE* texfile = platformOpenFile(path, PlatformFileModeRB);
    if (texfile != NULL) {
        fseek(texfile, 0, SEEK_END);
        long filesize = ftell(texfile);
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
    else {
        return NULL;
    }
}

Layer* readSDLImage(std::string path)
{
    SDL_Surface* img = IMG_Load(path.c_str());

    return img == NULL ? NULL : new Layer(img);
}

Layer* readWiiGCTPL(PlatformNativePathString path)
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

Layer* readDDS(PlatformNativePathString path)
{
    Layer* ret = NULL;
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        unsigned char header[0x80];
        fread(header, 0x80, 1, infile);
        ddspp::Descriptor desc;
        ddspp::Result decodeResult = ddspp::decode_header(header, desc);

        switch (desc.format) {
            case ddspp::BC1_UNORM:
            {
                ret = new Layer(desc.width, desc.height);
                ret->name = "DDS DXT1 Layer";
                uint32_t* pxd = (uint32_t*)ret->pixelData;
                for (int y = 0; y < desc.height; y += 4) {
                    for (int x = 0; x < desc.width; x += 4) {
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

                                ret->setPixel(XY{ x+dx, y + dy }, color);
                                //pxd[(y + dy) * desc.width + (x + dx)] = color;
                            }
                        }
                    }
                }
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
                printf("format not supported\n");
                break;
        }

        fclose(infile);
    }

    return ret;
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