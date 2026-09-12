#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

struct AnimationRecorder {
    std::string name;
    std::string extension;
    std::string extensionDesc;
    std::function<VideoEncoder* ()> createFn;
};

std::vector<AnimationRecorder> g_getAnimRecorders();

class PopupSaveAnimation :
    public BasePopup, public EventCallbackListener
{
public:
    std::function<void(PopupSaveAnimation*, PlatformNativePathString)> onConfirmCallback = NULL;

    AnimationRecorder recorder;
    int msPerFrame = 100;
    int quality = 90;
    int scale = 1;
    int repeatTimes = 1;

    PopupSaveAnimation(std::string tt, std::string tx);

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex) override;
};

