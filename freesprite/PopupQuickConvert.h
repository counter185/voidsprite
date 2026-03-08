#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UIDropdown.h"

class PopupQuickConvert :
    public BasePopup, public EventCallbackListener
{
private:
    bool forceRGB = false;
public:
    FileExporter* currentExporter = NULL;

    PopupQuickConvert(std::string tt, std::string tx);

    void takeInput(SDL_Event evt) override;

    void onDropFileEvent(SDL_Event evt);

    static void doQuickConvert(MainEditor* session, PlatformNativePathString outPath, FileExporter* exporter = NULL, bool forceConvertRGB = false, OperationProgressReport* progress = NULL);
};

