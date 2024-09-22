#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UIDropdown.h"

class PopupQuickConvert :
    public BasePopup, public EventCallbackListener
{
public:
    std::string title = "";
    std::string text = "";
    

    UIDropdown* pickExportFormat;
    int exporterIndex = 0;

    PopupQuickConvert(std::string tt, std::string tx);

    void takeInput(SDL_Event evt) override;
    void render() override;

    void eventButtonPressed(int evt_id) override {
        closePopup();
    }

    void onDropFileEvent(SDL_Event evt);
    void eventDropdownItemSelected(int evt_id, int index, std::string name);

    MainEditor* loadSession(std::string path);
    Layer* loadFlat(std::string path);
};

