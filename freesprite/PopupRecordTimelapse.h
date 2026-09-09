#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

struct TimelapseRecorder {
    std::string name;
    std::string extension;
    std::string extensionDesc;
    std::function<VideoEncoder*()> createFn;
};

class PopupRecordTimelapse :
    public BasePopup, public EventCallbackListener
{
protected:
    MainEditor* parent;
    int msPerFrame = 100;
    int quality = 90;
    int skipNFrames = 0;
    TimelapseRecorder recorder;
public:
    PopupRecordTimelapse(MainEditor* caller);

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex = -1) override;
};

