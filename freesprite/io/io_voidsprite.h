#pragma once

#include "../globals.h"

void voidsnWriteU32(FILE* f, u32 num);
void voidsnWriteU64(FILE* f, u64 num);
void voidsnWriteString(FILE* f, std::string str);
u32 voidsnReadU32(FILE* f);
u64 voidsnReadU64(FILE* f);
std::string voidsnReadString(FILE* f);

bool writeVOIDSNv1(PlatformNativePathString, XY projDimensions, std::vector<Layer*> data);
bool writeVOIDSNv2(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv3(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv4(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv5(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv6(PlatformNativePathString path, MainEditor* editor);

MainEditor* readVOIDSN(PlatformNativePathString path);

std::pair<bool, std::vector<uint32_t>> readPltVOIDPLT(PlatformNativePathString name);
bool writePltVOIDPLT(PlatformNativePathString path, std::vector<u32> palette);