#pragma once

#include "../globals.h"

std::string getLibAVIFVersion();

std::vector<u8> writeAVIFToMem(Layer* l, int quality);
SDL_Surface* readAVIFFromMem(u8* data, size_t dataSize);

MainEditor* readAVIF(PlatformNativePathString path, OperationProgressReport* progress);
bool writeAVIF(PlatformNativePathString path, MainEditor* editor);

bool writeAVIFWithSDLImage(PlatformNativePathString path, MainEditor* data);