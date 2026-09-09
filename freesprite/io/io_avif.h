#pragma once

#include "../globals.h"
#include "../videoencoder.h"

struct avifEncoder;

std::string getLibAVIFVersion();

std::vector<u8> writeAVIFToMem(Layer* l, int quality);
SDL_Surface* readAVIFFromMem(u8* data, size_t dataSize);

MainEditor* readAVIF(PlatformNativePathString path, OperationProgressReport* progress);
bool writeAVIF(PlatformNativePathString path, MainEditor* editor, OperationProgressReport* progress, ParameterStore* params);

bool writeAVIFWithSDLImage(PlatformNativePathString path, MainEditor* data, int quality = 100);

#if VSP_USE_LIBAVIF
class AVIFVideoEncoder : public VideoEncoder {

	void startRecording(PlatformNativePathString path, int msPerFrame, int quality) override;
	void stopRecording() override;

	void submitFrame(Layer* l) override;
protected:
	PlatformNativePathString file;
	avifEncoder* encoder = NULL;
	int msPerFrame = 100;
};
#endif