#pragma once
#include "BasePopup.h"

struct PopupSetNetworkCanvasData {
    std::string ip;
    int port = 6600;
    std::string username;
    u32 userColor = 0xFFFFFF;
    std::string password = "";
    NetInterface* netIf = NULL;
};

class PopupSetupNetworkCanvas :
    public BasePopup
{
public:
    UITextField* textboxIP, *textboxPort, *textboxUsername, *textboxPassword;
    UIButton* buttonSetUserColor;
    u32 userColor = 0xFFFFFF;
    int netInterfaceIndex = 0;

    bool rpcEnabled = false;
    UITextField* textboxExternalIP = NULL;
    bool rpcLobbyPrivate = false;

    std::function<void(PopupSetupNetworkCanvas*, PopupSetNetworkCanvasData)> onInputConfirmCallback = NULL;

    PopupSetupNetworkCanvas(std::string tt, std::string tx, bool ipField = true, bool portField = true, bool rpcOptions = false);

    void updateUserColorButton();
};

