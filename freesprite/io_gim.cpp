
#include "io_base.h"
#include "io_gim.h"

GIMHeader headerBEtoLE(GIMHeader h) {
    return {
        BEtoLE16(h.sig), 
        BEtoLE32(h.version),
            h.formatStyle, // not flipped
            BEtoLE32(h.paddingMaybe)
    };
}

GIMBlock blockBEtoLE(GIMBlock b) {
    return {
        BEtoLE16(b.block_id), 
        BEtoLE16(b.block_unk), 
        BEtoLE32(b.block_size), 
        BEtoLE32(b.block_header_next),
        BEtoLE32(b.block_data_offset)
    };
}

GIMImageBlock imageBlockBEtoLE(GIMImageBlock b) {
    return {BEtoLE16(b.data_length),  BEtoLE16(b.b45_unk1),    BEtoLE16(b.image_format),
            BEtoLE16(b.pixel_order),  BEtoLE16(b.w),           BEtoLE16(b.h),
            BEtoLE16(b.bpp_align),    BEtoLE16(b.pitch_align), BEtoLE16(b.height_align),
            BEtoLE16(b.b45_unk2),     BEtoLE32(b.b45_unk3),    BEtoLE32(b.index_start),
            BEtoLE32(b.pixels_start), BEtoLE32(b.pixels_end),  BEtoLE32(b.plane_mask),
            BEtoLE16(b.level_type),   BEtoLE16(b.level_count), BEtoLE16(b.frame_type),
            BEtoLE16(b.frame_count)};
}

//most of this ported over from https://github.com/nickworonekin/puyotools/blob/master/src/PuyoTools.Core/Textures/Gim/GimTextureDecoder.cs#L313
//sorry
u32 calcSwizzledOffset(u64 pos, int stride) {

    int x = pos % stride;
    int y = pos / stride;

    int rowBlocks = stride / 16;

    int blockX = x / 16;
    int blockY = y / 8;

    int blockIndex = blockX + (blockY * rowBlocks);
    int blockAddress = blockIndex * 16 * 8;

    return blockAddress + (x - blockX * 16) + ((y - blockY * 8) * 16);
}

//http://homebrew.pixelbath.com/wiki/PSP_texture_swizzling
u32 unswizzle(u32 offset, u32 log2_w) {
    if (log2_w <= 4)
        return offset;

    u32 w_mask = (1 << log2_w) - 1;

    u32 mx = offset & 0xf;
    u32 my = offset & 0x70;

    u32 by = offset & (~7 << log2_w);
    u32 bx = offset & ((w_mask & 0xf) << 7);

    return by | (bx >> 3) | (my << (log2_w - 4)) | mx;
}

std::vector<GIMBlock> readBlockStack(FILE* f, bool BE) {
    std::vector<GIMBlock> ret;

    GIMBlock rootBlock;
    fread(&rootBlock, sizeof(GIMBlock), 1, f);
    if (BE) {
        rootBlock = blockBEtoLE(rootBlock);
    }

    rootBlock.block_data_offset += ftell(f) - 0x10; //set offset to be the global position in file
    ret.push_back(rootBlock);
    fseek(f, rootBlock.block_header_next - 0x10, SEEK_CUR);

    switch (rootBlock.block_id) {
        case 0x02:
            //picture(0x03) and fileinfo(0xFF)
            for (auto& i : {0,1}) {
                for (auto& nblock : readBlockStack(f, BE)) {
                    ret.push_back(nblock);
                }
            }
            break;
        case 0x03:
            //image(0x04) and palette(0x05)
            bool nonpalette = false;
            for (auto& i : {0, 1}) {
                if (nonpalette) {
                    break;
                }
                for (auto& nblock : readBlockStack(f, BE)) {
                    if (nblock.block_id == 0x04) {
                        u64 posdump = ftell(f);
                        fseek(f, nblock.block_data_offset, SEEK_SET);
                        GIMImageBlock imgBlock;
                        fread(&imgBlock, sizeof(GIMImageBlock), 1, f);
                        if (BE) {
                            imgBlock = imageBlockBEtoLE(imgBlock);
                        }
                        nonpalette = imgBlock.image_format != 0x04 
                            && imgBlock.image_format != 0x05
                            && imgBlock.image_format != 0x06
                            && imgBlock.image_format != 0x07;
                        fseek(f, posdump, SEEK_SET);
                    }
                    ret.push_back(nblock);
                }
            }
            break;
    }
    return ret;
}

Layer* readGIM(PlatformNativePathString path, uint64_t seek) {
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        GIMHeader header;
        fread(&header, sizeof(GIMHeader), 1, f);
        bool isBE = memcmp(&header.sig, ".GIM", 4) == 0;
        if (isBE) {
            header = headerBEtoLE(header);
        }
        std::vector<GIMBlock> blocks = readBlockStack(f, isBE);
        
        std::map<u16, GIMBlock> blockMap;

        for (GIMBlock& b : blocks) {
            logprintf("[GIM] Block ID: %x, next header offset: %x, data offset: %x, size: %x\n", b.block_id, b.block_header_next, b.block_data_offset, b.block_size);
            blockMap[b.block_id] = b;
        }
        
        std::vector<u32> palette;
        if (blockMap.contains(0x05)) {
            u64 block05Offset = blockMap[0x05].block_data_offset;
            fseek(f, block05Offset, SEEK_SET);
            GIMImageBlock imgBlock;
            fread(&imgBlock, sizeof(GIMImageBlock), 1, f);
            if (isBE) {
                imgBlock = imageBlockBEtoLE(imgBlock);
            }
            int nEntries = imgBlock.w * imgBlock.h;
            loginfo(std::format("[GIM] palette color format: {}", imgBlock.image_format));
            u32 next;
            int bpp = 
                imgBlock.image_format == 0x03 ? 4       //rgba8888
                : imgBlock.image_format == 0x02 ? 2     //rgba4444
                : imgBlock.image_format == 0x01 ? 2     //rgba5551
                : imgBlock.image_format == 0x00 ? 2     //rgb565
                : 0;
            u64 pixelStart = block05Offset + imgBlock.pixels_start;
            fseek(f, pixelStart, SEEK_SET);
            if (bpp == 0) {
                logerr("[GIM] unsupported palette color format");
            } else {
                for (int i = 0; i < nEntries; i++) {
                    fread(&next, bpp, 1, f);
                    switch (imgBlock.image_format) {
                        case 0x03: //rgba8888
                            next = PackRGBAtoARGB(next & 0xFF, (next >> 8) & 0xFF, (next >> 16) & 0xFF, (next >> 24) & 0xFF);
                            break;
                        case 0x02:
                            next = RGBA4444toARGB8888(next);
                            break;
                        case 0x01:
                            next = (RGB565toARGB8888(next) & 0xFFFFFF) | ((((next >> 15) & 1) != 0) ? 0xFF000000 : 0);
                            break;
                        case 0x00:
                            next = RGB565toARGB8888(next);
                            break;
                    }
                    palette.push_back(next);
                }
            }
        }
        else {
            logwarn("[GIM] No 0x05 palette block");
        }

        u64 block04Offset = blockMap[0x04].block_data_offset;
        fseek(f, block04Offset, SEEK_SET);
        GIMImageBlock imgBlock;
        fread(&imgBlock, sizeof(GIMImageBlock), 1, f);
        if (isBE) {
            imgBlock = imageBlockBEtoLE(imgBlock);
        }
        logprintf("image format: %x\n", imgBlock.image_format);
        logprintf("pixel order: %i\n", imgBlock.pixel_order);  //1 needs to be detiled or something
        loginfo(std::format("image size: {}x{}", imgBlock.w, imgBlock.h));
        bool deswizzle = imgBlock.pixel_order == 1;
        Layer* ret = NULL;
        u64 pixelDataSize = imgBlock.pixels_end - imgBlock.pixels_start;
        u64 pixelStart = block04Offset + imgBlock.pixels_start;
        switch (imgBlock.image_format) {
            case 0x03:  // rgba8888
                {
                    ret = new Layer(imgBlock.w, imgBlock.h);
                    u32* ppx = (u32*)ret->pixelData;
                    fseek(f, pixelStart, SEEK_SET);
                    for (u64 i = 0; i < pixelDataSize; i += 4) {
                        u64 indexInPixelData = i / 4;
                        //note: the deswizzling here is diabolical
                        //one day i will try again
                        u8 px[4];
                        fread(px, 4, 1, f);
                        ppx[indexInPixelData] = PackRGBAtoARGB(px[0], px[1], px[2], px[3]);
                    }
                }
                break;
            case 0x04: // index4
                {
                    ret = new LayerPalettized(imgBlock.w, imgBlock.h);
                    ((LayerPalettized*)ret)->palette = palette;
                    u32* ppx = (u32*)ret->pixelData;
                    fseek(f, pixelStart, SEEK_SET);
                    for (u64 i = 0; i < pixelDataSize; i++) {
                        u8 px;
                        fread(&px, 1, 1, f);
                        ppx[i++] = px & 0x0F;
                        ppx[i] = (px >> 4) & 0x0F;
                    }
                }
                break;
            case 0x05: // index8
                {
                    ret = new LayerPalettized(imgBlock.w, imgBlock.h);
                    ((LayerPalettized*)ret)->palette = palette;
                    u32* ppx = (u32*)ret->pixelData;
                    fseek(f, pixelStart, SEEK_SET);
                    for (u64 i = 0; i < pixelDataSize; i++) {
                        if (deswizzle) {
                            u64 swizzledOffset = calcSwizzledOffset(i, imgBlock.w);
                            fseek(f, pixelStart + swizzledOffset, SEEK_SET);
                        }
                        u8 px;
                        fread(&px, 1, 1, f);
                        ppx[i] = px;
                    }
                }
                break;
            default:
                logerr(std::format("[GIM] unsupported image format: {:02x}", imgBlock.image_format));
                break;
        }

        fclose(f);
        return ret;
    }
    return NULL;
}

bool writeGIM(PlatformNativePathString path, Layer* data) {
    return false;
}
