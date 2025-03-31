#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UIDropdown.h"

class PopupQuickConvert :
    public BasePopup, public EventCallbackListener
{
public:
    UICheckbox* checkForceRGB;
    UIDropdown* pickExportFormat;
    int exporterIndex = 0;

    PopupQuickConvert(std::string tt, std::string tx);

    void takeInput(SDL_Event evt) override;

    void eventButtonPressed(int evt_id) override {
        closePopup();
    }

    void onDropFileEvent(SDL_Event evt);
    void eventDropdownItemSelected(int evt_id, int index, std::string name);

    static void doQuickConvert(MainEditor* session, PlatformNativePathString outPath, FileExporter* exporter = NULL, bool forceConvertRGB = false);
};

