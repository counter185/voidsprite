#include <regex>

#include "PopupListLAN.h"
#include "ScrollingPanel.h"
#include "UIButton.h"
#include "background_operation.h"
#include "StartScreen.h"
#include "network.h"

PopupListLAN::PopupListLAN()
{
    wxHeight = 330;
    lanScanThread = new std::thread(&PopupListLAN::lanScannerThread, this);

    listPanel = new ScrollingPanel();
    listPanel->position = XY{ 10, 40 };
    listPanel->wxWidth = wxWidth - 20;
    listPanel->wxHeight = 240;
    listPanel->scrollVertically = true;
    listPanel->scrollHorizontally = false;
    listPanel->bgColor = Fill::Solid(0x70101010);
    wxsManager.addDrawable(listPanel);

    UIButton* nbutton2 = actionButton(TL("vsp.cmn.cancel"));
    nbutton2->onClickCallback = [this](UIButton*) { this->closePopup(); };

    XY bodyEndpoint = makeTitleAndDesc(TL("vsp.launchpad.nav.findcollabonlan"), "");
}

PopupListLAN::~PopupListLAN()
{
    runThread = false;
    if (lanScanThread != NULL) {
        lanScanThread->join();
        delete lanScanThread;
        lanScanThread = NULL;
    }
}

void PopupListLAN::tick()
{
    BasePopup::tick();
    opQueue.process();
}

void PopupListLAN::populateList()
{
    listPanel->subWidgets.freeAllDrawables();
    addressListMutex.lock();
    int yNow = 0;
    for (auto& [addr,_] : addrsFound) {
        auto scPos = addr.find(';');
        if (scPos != std::string::npos) {
            std::string ip = addr.substr(0, scPos);
            std::string port = addr.substr(scPos + 1);
            UIButton* addrBtn = new UIButton(frmt("{} :{}", ip, port));
            addrBtn->position = XY{ 0, yNow };
            addrBtn->wxWidth = listPanel->wxWidth - 30;
            addrBtn->onClickCallback = [this, ip, port](UIButton*) {
                StartScreen::promptConnectToNetworkCanvas(ip.c_str(), port);
                g_startNewMainThreadOperation([this]() {
                    this->closePopup();
                });
            };
            listPanel->subWidgets.addDrawable(addrBtn);
            yNow += 30;
        }
    }
    addressListMutex.unlock();
}

void PopupListLAN::lanScannerThread()
{
#if VSP_NETWORKING
    NetBroadcastSocket* broadcastSocket = g_netInterfaces.front()->createBroadcastListener("");

    if (broadcastSocket == NULL) {
        logerr("Failed to create LAN broadcast socket");
        return;
    }
    
    DoOnReturn closeSocket([broadcastSocket]() {
        broadcastSocket->netIf->destroyBroadcastListener(broadcastSocket);
    });
    loginfo(frmt("Listening on {}", LAN_BROADCAST_PORT));

    while (runThread) {
        NetBroadcastMessage nmsg{};
        if (broadcastSocket->netIf->tryReceiveBroadcastMessage(broadcastSocket, &nmsg)) {

            if (nmsg.bytes.size() >= 10 && memcmp(nmsg.bytes.data(), "vspcollab:", 10) == 0) {
                std::string fullRequest;
                fullRequest.resize(nmsg.bytes.size());
                memcpy((u8*)fullRequest.data(), nmsg.bytes.data(), nmsg.bytes.size());

                std::smatch matchRequestData;
                if (std::regex_match(fullRequest, matchRequestData, std::regex("^vspcollab:([0-9]+)"))) {
                    int port = std::stoi(matchRequestData[1]);

                    std::string fullAddr = nmsg.srcAddress;
                    std::smatch s;
                    if (std::regex_match(fullAddr, s, std::regex(".*:([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})$"))) {
                        fullAddr = s[1];
                    }

                    fullAddr += frmt(";{}", port);

                    addressListMutex.lock();
                    if (!addrsFound[fullAddr]) {
                        addrsFound[fullAddr] = true;
                        opQueue.add([this, fullAddr]() {
                            loginfo(frmt("LAN session found on {}", fullAddr));
                            this->populateList();
                        });
                    }
                    addressListMutex.unlock();
                }
            }
        }
        SDL_Delay(250);
    }
#endif
}
