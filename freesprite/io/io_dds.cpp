#include "io_base.h"
#include "io_dds.h"

#include "../ddspp/ddspp.h"
#include "../bc7enc_rdo/bc7decomp.h"

void DeXT1(Layer* ret, int width, int height, FILE* infile)
{
    uint32_t* pxd = ret->pixels32();
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
    uint32_t* pxd = ret->pixels32();
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
    u32* pxd = ret->pixels32();
    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 4) {
            //gonna be real i have no idea what's going on here
            u8 alpha0;
            u8 alpha1;
            fread(&alpha0, 1, 1, infile);
            fread(&alpha1, 1, 1, infile);

            u8 bits[6];
            fread(bits, 1, 6, infile);
            unsigned long alphaCode1 = bits[2] | (bits[3] << 8) | (bits[4] << 16) | (bits[5] << 24);
            u16 alphaCode2 = bits[0] | (bits[1] << 8);

            u16 color0;
            u16 color1;
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
                        ret->setPixel(XY{ x + i, y + j }, color);
                }
            }
        }
    }
}

void DeBC7(Layer* ret, int width, int height, FILE* infile)
{
    u32* pxd = ret->pixels32();
    memset(pxd, 0xcc, width * height * 4);
    for (int y = 0; y < height; y += 4) {
        for (int x = 0; x < width; x += 4) {
            u8 block[16];
            fread(block, 16, 1, infile);
            bc7decomp::color_rgba pixelData[16];
            bc7decomp::unpack_bc7(block, pixelData);

            for (int yy = 0; yy < 4; yy++) {
                if (y + yy >= height) {
                    break;
                }
                for (int xx = 0; xx < 4; xx++) {
                    if (x + xx < width) {
                        auto c = ARRAY2DPOINT(pixelData, xx, yy, 4);
                        ARRAY2DPOINT(pxd, x + xx, y + yy, width) = PackRGBAtoARGB(c.r, c.g, c.b, c.a);
                    }
                }
            }
        }
    }
}

Layer* readDDS(PlatformNativePathString path, uint64_t seek)
{
    Layer* ret = NULL;
    FILE* infile = platformOpenFile(path, PlatformFileModeRB);
    if (infile != NULL) {
        if (seek != 0) {
            fseek(infile, seek, SEEK_SET);
        }
        u8 header[0x80];
        fread(header, 0x80, 1, infile);

        //do we even like need ddspp
        ddspp::Descriptor desc;
        ddspp::Result decodeResult = ddspp::decode_header(header, desc);

        //handle DX10
        if (memcmp(header + 0x54, "DX10", 4) == 0) {
            DDS_DX10_HEADER dx10{};
            fread(&dx10, sizeof(DDS_DX10_HEADER), 1, infile);
            desc.format = (ddspp::DXGIFormat)dx10.dxgiFormat;
        }


        switch (desc.format) {
        case ddspp::BC1_UNORM:
            ret = new Layer(desc.width, desc.height);
            ret->name = "DDS DXT1 Layer";
            DeXT1(ret, desc.width, desc.height, infile);
            break;
        case ddspp::BC2_UNORM:
            //todo holy fucking shit please clean this up
            ret = new Layer(desc.width, desc.height);
            ret->name = "DDS DXT2/3 Layer";
            DeXT23(ret, desc.width, desc.width, infile);
            break;
        case ddspp::BC3_UNORM:
            //todo holy fucking shit please clean this up
            ret = new Layer(desc.width, desc.height);
            ret->name = "DDS DXT4/5 Layer";
            DeXT45(ret, desc.width, desc.width, infile);
            break;
        case ddspp::BC7_UNORM:
            ret = new Layer(desc.width, desc.height);
            ret->name = "DDS BC7 Layer";
            DeBC7(ret, desc.width, desc.width, infile);
            break;
        case ddspp::B8G8R8A8_UNORM:
        {
            ret = new Layer(desc.width, desc.height);
            ret->name = "DDS BGRA Layer";
            uint32_t* pxd = ret->pixels32();
            for (uint64_t d = 0; d < desc.width * desc.height; d++) {
                uint32_t bgra;
                fread(&bgra, 4, 1, infile);
                //bgra = BEtoLE32(bgra);
                pxd[d] = bgra;
            }
        }
        break;
        case ddspp::R8G8_SNORM:
        {
            ret = new Layer(desc.width, desc.height);
            ret->name = "DDS R8G8 Layer";
            uint32_t* pxd = ret->pixels32();
            for (uint64_t d = 0; d < desc.width * desc.height; d++) {
                u8 rg[2];
                fread(&rg, 2, 1, infile);
                pxd[d] = PackRGBAtoARGB(rg[0] + 0x80, rg[1] + 0x80, 0x80, 255);
            }
        }
        break;
        default:
            logerr(frmt("[DDS] format [{:x}] not supported", (u32)desc.format));
            break;
        }

        fclose(infile);
    }

    return ret;
}