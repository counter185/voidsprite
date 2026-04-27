#pragma once

#include "../globals.h"

#pragma pack(push, 1)
struct yoyoTexHeader {
	char magic[4];
	u16 w;
	u16 h;
	u32 len;
};
#pragma pack(pop)

const u8 QOI_INDEX = 0x00;
const u8 QOI_RUN_8 = 0x40;
const u8 QOI_RUN_16 = 0x60;
const u8 QOI_DIFF_8 = 0x80;
const u8 QOI_DIFF_16 = 0xc0;
const u8 QOI_DIFF_24 = 0xe0;

const u8 QOI_COLOR = 0xf0;
const u8 QOI_MASK_2 = 0xc0;
const u8 QOI_MASK_3 = 0xe0;
const u8 QOI_MASK_4 = 0xf0;

Layer* readYoYoTex(PlatformNativePathString path, u64 seek = 0);

Layer* readQoifFromMem(std::vector<u8>& bytes);