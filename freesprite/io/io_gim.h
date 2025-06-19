#pragma once
#include "../globals.h"

#pragma pack(push, 1)
struct GIMBlock {
    u16 block_id;
    u16 block_unk;
    u32 block_size;
    u32 block_header_next;
    u32 block_data_offset;
};
struct GIMHeader {
    u32 sig;
    u32 version;
    u32 formatStyle;
    u32 paddingMaybe;
};

struct GIMImageBlock {
    u16 data_length;
    u16 b45_unk1;
    u16 image_format;
    u16 pixel_order;
    u16 w;
    u16 h;
    u16 bpp_align;
    u16 pitch_align;
    u16 height_align;
    u16 b45_unk2;
    u32 b45_unk3;
    u32 index_start;
    u32 pixels_start;
    u32 pixels_end;
    //
    u32 plane_mask;
    u16 level_type;
    u16 level_count;
    u16 frame_type;
    u16 frame_count;
    // next is "optional user data"
};
#pragma pack(pop)

GIMHeader headerBEtoLE(GIMHeader h);
GIMBlock blockBEtoLE(GIMBlock b);
GIMImageBlock imageBlockBEtoLE(GIMImageBlock b);

Layer* readGIM(PlatformNativePathString path, uint64_t seek = 0);
bool writeGIM(PlatformNativePathString path, Layer* data);