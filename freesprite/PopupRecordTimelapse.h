#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "PopupSaveAnimation.h"

class PopupRecordTimelapse :
    public BasePopup, public EventCallbackListener
{
protected:
    MainEditor* parent;
    int msPerFrame = 100;
    int quality = 90;
    int skipNFrames = 0;
    AnimationRecorder recorder;
public:
    PopupRecordTimelapse(MainEditor* caller);

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex = -1) override;
};

