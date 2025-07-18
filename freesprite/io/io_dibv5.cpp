#include "io_base.h"
#include "io_dibv5.h"
#include "io_png.h"

Layer* readDIBv5FromMem(u8* mem, u64 size)
{
    Layer* ret = NULL;
    if (mem != NULL && size > sizeof(vsp_BITMAPV5HEADER)) {
        vsp_BITMAPV5HEADER header = *((vsp_BITMAPV5HEADER*)mem);
        u32 startOffset = header.bV5Size;
        u16 bitDepth = header.bV5BitCount;
        logprintf("[DIBv5] Compression type: %i, bit depth: %i\n", header.bV5Compression, bitDepth);
        switch (header.bV5Compression) {
            case 0:         //BI_RGB
                ret = Layer::tryAllocLayer(header.bV5Width, header.bV5Height);
                if (ret != NULL) {
                    u32* ppx = ret->pixels32();
                    u8* srcpx = (u8*)mem + startOffset;
                    u64 y = 0;
                    for (u64 x = 0; x < ret->w * ret->h && (startOffset+y < size); x++) {
                        ppx[x] = PackRGBAtoARGB(
                            srcpx[y+2],
                            srcpx[y+1],
                            srcpx[y],
                            255
                        );
                        y += bitDepth / 8;
                    }
                    ret->flipVertically();  //tell noone lmao
                }
                break;
            case 3:         //BI_BITFIELDS
                {
                    u32 rMask = header.bV5RedMask;
                    u32 gMask = header.bV5GreenMask;
                    u32 bMask = header.bV5BlueMask;
                    u32 aMask = header.bV5AlphaMask;
                    ret = Layer::tryAllocLayer(header.bV5Width, header.bV5Height);
                    if (ret != NULL) {
                        u32* ppx = ret->pixels32();
                        u32* srcpx = (u32*)(mem + startOffset);
                        for (u64 x = 0; x < ret->w * ret->h; x++) {
                            u32 nextSrcPixel = srcpx[x];
                            ppx[x] = PackRGBAtoARGB(
                                (nextSrcPixel & rMask) / (rMask / 0xff),
                                (nextSrcPixel & gMask) / (gMask / 0xff),
                                (nextSrcPixel & bMask) / (bMask / 0xff),
                                aMask != 0 ? (nextSrcPixel & aMask) / (aMask / 0xff) : 0xff
                            );
                        }
                        ret->flipVertically();  //tell noone lmao
                    }
                }
                break;
            case 5:         //BI_PNG
                return readPNGFromMem(mem + startOffset, size - startOffset);
                break;
            default:
                logprintf("-- unsupported compression type\n");
                break;

        }
    }
    return ret;
}

std::vector<u8> writeDIBv5ToMem(Layer* image)
{
    std::vector<u8> ret;
    vsp_BITMAPV5HEADER header{};
    header.bV5Size = 0x7c;
    header.bV5Width = image->w;
    header.bV5Height = image->h;
    header.bV5Planes = 0;
    header.bV5BitCount = 32;
    header.bV5Compression = 3;
    header.bV5SizeImage = image->w * image->h * 4;
    header.bV5RedMask = 0x00FF0000;
    header.bV5GreenMask = 0x0000FF00;
    header.bV5BlueMask = 0x000000FF;
    header.bV5AlphaMask = 0xFF000000;
    header.bV5CSType = 0x73524742;      //sRGB
    header.bV5Intent = 4;   //no idea
    ret.resize(sizeof(vsp_BITMAPV5HEADER));
    memcpy(ret.data(), &header, sizeof(vsp_BITMAPV5HEADER));
    for (int y = image->h; y--> 0; ) {
        for (int x = 0; x < image->w; x++) {
            u32 color = image->isPalettized ? ((LayerPalettized*)image)->getVisualPixelAt({ x,y }) : image->getPixelAt({x,y});
            SDL_Color c = uint32ToSDLColor(color);
            ret.push_back(c.b);
            ret.push_back(c.g);
            ret.push_back(c.r);
            ret.push_back(c.a);
        }
    }
    return ret;
}

Layer* readDIBV5(PlatformNativePathString path, uint64_t seek)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        Layer* ret = NULL;
        
        fseek(f, 0, SEEK_END);
        u64 fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        u8* dibv5file = (u8*)tracked_malloc(fsize, "Temp.mem.");
        fread(dibv5file, 1, fsize, f);
        ret = readDIBv5FromMem(dibv5file, fsize);
        tracked_free(dibv5file);

        fclose(f);
        return ret;
    }
    return NULL;
}

bool writeDIBV5(PlatformNativePathString path, Layer* data)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        std::vector<u8> dibv5 = writeDIBv5ToMem(data);
        fwrite(dibv5.data(), 1, dibv5.size(), f);
        fclose(f);
        return true;
    }
    return false;
}
