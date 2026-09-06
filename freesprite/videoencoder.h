#pragma once

#include "globals.h"

class VideoEncoder {
public:
	~VideoEncoder() {}

	virtual void startRecording(PlatformNativePathString path) {}
	virtual void stopRecording() {}

	virtual void submitFrame(Layer* l) {}
};

