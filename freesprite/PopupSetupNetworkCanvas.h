#pragma once
#include "BasePopup.h"

struct PopupSetNetworkCanvasData {
    std::string ip;
    int port = 6600;
    std::string username;
    u32 userColor = 0xFFFFFF;
};

class PopupSetupNetworkCanvas :
    public BasePopup
{
public:
    UITextField* textboxIP, *textboxPort, *textboxUsername;
    UIButton* buttonSetUserColor;
    u32 userColor = 0xFFFFFF;
    std::function<void(PopupSetupNetworkCanvas*, PopupSetNetworkCanvasData)> onInputConfirmCallback = NULL;

    PopupSetupNetworkCanvas(std::string tt, std::string tx, bool ipField = true, bool portField = true);

    void updateUserColorButton();
};

