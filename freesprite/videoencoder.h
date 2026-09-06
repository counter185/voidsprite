#pragma once

#include "globals.h"

class VideoEncoder {
protected:
	int framesWritten = 0;
public:

	~VideoEncoder() {}

	virtual void startRecording(PlatformNativePathString path, int msPerFrame, int quality) {}
	virtual void stopRecording() {}

	virtual void submitFrame(Layer* l) {}

	int getFramesWritten() { return framesWritten; }
};

