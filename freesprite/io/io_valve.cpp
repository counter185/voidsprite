#include "io_base.h"
#include "io_valve.h"

void DeXT1(Layer* ret, int width, int height, FILE* infile);
void DeXT23(Layer* ret, int width, int height, FILE* infile);
void DeXT45(Layer* ret, int width, int height, FILE* infile);

Layer* readValveSPR(PlatformNativePathString path, uint64_t seek)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        SPR_sprite spr;
        fread(&spr, sizeof(SPR_sprite), 1, f);
        if (spr.id != 0x50534449) {
            logerr(frmt("[SPR] Invalid file format {:08X}\n", spr.id));
            fclose(f);
            return NULL;
        }
        loginfo(frmt("[SPR] frame count: {}", spr.frameNum));
        loginfo(frmt("[SPR] sprite type: {}", spr.spriteType));
        loginfo(frmt("[SPR] transparency format: {}", (u32)spr.textFormat));
        loginfo(frmt("[SPR] colors in palette: {}", spr.paletteColorCount));
        //assuming version 2
        std::vector<u32> palette;
        for (short i = 0; i < spr.paletteColorCount; i++) {
            u8 r, g, b;
            fread(&r, 1, 1, f);
            fread(&g, 1, 1, f);
            fread(&b, 1, 1, f);
            palette.push_back(PackRGBAtoARGB(r, g, b, spr.textFormat == SPR_Transparency_IndexAlpha ? i : 255));
        }

        if (spr.textFormat == SPR_Transparency_AlphaTest && !palette.empty()) {
            palette[palette.size() - 1] &= 0xFFFFFF;
        }

        Layer* ret = NULL;

        for (u32 fr = 0; fr < spr.frameNum; fr++) {
            SPR_sprite_frame_header frame;
            fread(&frame, sizeof(SPR_sprite_frame_header), 1, f);

            LayerPalettized* l = LayerPalettized::tryAllocIndexedLayer(frame.width, frame.height);
            ret = l;
            if (l == NULL) {
                logerr(frmt("[SPR] Failed to allocate layer {}x{}\n", frame.width, frame.height));
            }
            else {
                l->name = "Valve SPR Layer";
                l->palette = palette;
                u32* ppx = l->pixels32();
                for (u32 y = 0; y < frame.height; y++) {
                    for (u32 x = 0; x < frame.width; x++) {
                        u8 px;
                        fread(&px, 1, 1, f);
                        ARRAY2DPOINT(ppx, x, y, l->w) = px;
                    }
                }
            }

            //todo: multiple frames once we get there
            break;
        }

        fclose(f);
        return ret;
    }
    return NULL;
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
    int w = width;
    int h = height;
    for (int skipMMap = 0; skipMMap < mipmapCount - 1; skipMMap++) {
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
                uint32_t* pxp = ret->pixels32();
                for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                    u8 a;
                    fread(&a, 1, 1, infile);
                    pxp[dataP] = PackRGBAtoARGB(255, 255, 255, a);
                }
            }
            break;
        case IMAGE_FORMAT_I8:
            ret = new Layer(width, height);
            ret->name = "VTF I8 Layer";
            {
                uint32_t* pxp = ret->pixels32();
                for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                    u8 i;
                    fread(&i, 1, 1, infile);
                    pxp[dataP] = PackRGBAtoARGB(i, i, i, 255);
                }
            }
            break;
        case IMAGE_FORMAT_IA88:
            ret = new Layer(width, height);
            ret->name = "VTF IA88 Layer";
            {
                uint32_t* pxp = ret->pixels32();
                for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                    u8 i;
                    u8 a;
                    fread(&i, 1, 1, infile);
                    fread(&a, 1, 1, infile);
                    pxp[dataP] = PackRGBAtoARGB(i, i, i, a);
                }
            }
            break;
        case IMAGE_FORMAT_BGRA8888:
            ret = new Layer(width, height);
            ret->name = "VTF BGRA Layer";
            {
                uint32_t* pxp = ret->pixels32();
                for (uint64_t dataP = 0; dataP < ret->w * ret->h; dataP++) {
                    fread(pxp + dataP, 4, 1, infile);
                }
            }
            break;
        case IMAGE_FORMAT_RGBA8888:
            ret = new Layer(width, height);
            ret->name = "VTF RGBA Layer";
            {
                uint32_t* pxp = ret->pixels32();
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
                uint32_t* pxp = ret->pixels32();
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
                uint32_t* pxp = ret->pixels32();
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
                uint32_t* pxp = ret->pixels32();
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
                uint32_t* pxp = ret->pixels32();
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
                uint32_t* pxp = ret->pixels32();
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
                uint32_t* pxp = ret->pixels32();
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
            logerr("IMAGE FORMAT NOT IMPLEMENTED");
            break;
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

bool writeVTF(PlatformNativePathString path, Layer* data)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        XY lowResDimensions = data->w > 32 || data->h > 32 ? XY{ 32, 32 } : XY{ data->w, data->h };
        Layer* lowResImage = data->copyCurrentVariantScaled(lowResDimensions);

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

        fwrite(&header, 64, 1, f);
        fwrite(lowResImage->pixels32(), lowResDimensions.x * lowResDimensions.y, 4, f);
        fwrite(data->pixels32(), data->w * data->h, 4, f);
        delete lowResImage;
        fclose(f);
        return true;
    }
    return false;
}