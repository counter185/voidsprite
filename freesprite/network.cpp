#include "network.h"
#include "mathops.h"
#include "Notification.h"
#include "StartScreen.h"

#if VSP_NETWORKING
#include <SDL3_net/SDL_net.h>

#pragma comment(lib, "SDL3_net.lib")
#endif

#if VSP_STEAMWORKS
#include "steam/steam_api.h"
#include "steam/isteamuser.h"
#endif


std::vector<NetInterface*> g_netInterfaces = {};
void g_initNet() {
#if VSP_STEAMWORKS
    SteamNet* steamInterface = new SteamNet();
    if (steamInterface->init()) {
        g_netInterfaces.push_back(steamInterface);
    }
    else {
        logerr(frmt("Failed to initialize {}", steamInterface->interfaceName()));
        delete steamInterface;
    }
#endif

#if VSP_NETWORKING
    SDLNet* sdlNetInterface = new SDLNet();
    if (sdlNetInterface->init()) {
        g_netInterfaces.push_back(sdlNetInterface);
    }
    else {
        delete sdlNetInterface;
    }
#endif
}

std::string g_getAllNetworkLibsVersions()
{
    std::string ret = "";
#if VSP_NETWORKING
    ret += "SDL_net: " + std::to_string(SDL_NET_MAJOR_VERSION) + "." + std::to_string(SDL_NET_MINOR_VERSION) + "." +
        std::to_string(SDL_NET_MICRO_VERSION) + "\n";
#endif
    return ret;
}

void g_runNetworkCallbacks()
{
    for (auto*& netIf : g_netInterfaces) {
        netIf->runCallbacks();
    }
}

#if VSP_NETWORKING
bool SDLNet::init() {
    bool netInit = NET_Init();
    if (!netInit) {
        logerr("NET_Init failed: " + std::string(SDL_GetError()));
    }
    else {
        loginfo("Passed NET_Init");
    }
    return netInit;
}

std::string SDLNet::getFriendlyAddress(NetSocket* s) {
    NET_Address* addr = NET_GetStreamSocketAddress((NET_StreamSocket*)s->nativePtr);
    DoOnReturn a([&]() { NET_UnrefAddress(addr); });
    NET_GetAddressString(addr);
    int resolved = NET_WaitUntilResolved(addr, -1);
    if (resolved == 1) {
        const char* clientAddressStr = NET_GetAddressString(addr);
        return clientAddressStr != NULL ? std::string(clientAddressStr) : "<address unknown>";
    }
    return "<address unknown>";
}

NetServer* SDLNet::createServer(std::string address, int port)
{
    //todo: address
    NET_Server* s = NET_CreateServer(NULL, port, 0);

    if (s != NULL) {
        return new NetServer{ this, (void*)s };
    }
    return NULL;
}

NetSocket* SDLNet::createClient(std::string address, int port)
{
    NET_Address* addr = NET_ResolveHostname(address.c_str());
    if (NET_WaitUntilResolved(addr, 15000) == 1) {
        NET_StreamSocket* s = NET_CreateClient(addr, port, 0);
        if (NET_WaitUntilConnected(s, -1) == 1) {
            return new NetSocket{ this, s };
        }
        else {
            logerr(frmt("Connection to {} failed", address));
        }
    }
    else {
        logerr(frmt("Failed to resolve hostname: {}", address));
    }
    return NULL;
}

NetSocket* SDLNet::acceptNextClient(NetServer* s)
{
    NET_StreamSocket* socket = NULL;
    if (NET_AcceptClient((NET_Server*)s->nativePtr, &socket) && socket != NULL) {
        return new NetSocket{ this, (void*)socket };
    }
    return NULL;
}

void SDLNet::destroyServer(NetServer* s)
{
    NET_DestroyServer((NET_Server*)s->nativePtr);
    delete s;
}

void SDLNet::destroySocket(NetSocket* s)
{
    NET_DestroyStreamSocket((NET_StreamSocket*)s->nativePtr);
    delete s;
}

void SDLNet::writeMessageToSocket(NetSocket* s, NetMessage& msg)
{
    u64 msgSize = msg.bytes.size();
    NET_WriteToStreamSocket((NET_StreamSocket*)s->nativePtr, &msgSize,  8);
    NET_WriteToStreamSocket((NET_StreamSocket*)s->nativePtr, msg.bytes.data(), msgSize);
    //std::string str = std::string(msg.bytes.begin(), msg.bytes.begin() + ixmin(msg.bytes.size(), 8));
    //loginfo(frmt("[{:x}] >> {} ({}b)", (u64)s, str, msgSize));
}

NetMessage SDLNet::readMessageFromSocket(NetSocket* s)
{
    NetMessage ret = {};
    u64 msgSize = 0;
    if (readNBytesFromSocketBlocking(s, 8, (u8*)&msgSize)) {
        ret.bytes.resize(msgSize);
        readNBytesFromSocketBlocking(s, msgSize, ret.bytes.data());
        //std::string str = std::string(ret.bytes.begin(), ret.bytes.begin() + ixmin(ret.bytes.size(), 8));
        //loginfo(frmt("[{:x}] << {} ({}b)", (u64)s, str, msgSize));
    }
    return ret;
}

bool SDLNet::readNBytesFromSocketBlocking(NetSocket* s, int n, u8* dst)
{
    NET_StreamSocket* socket = (NET_StreamSocket*)s->nativePtr;
    int read = 0;
    while (read < n) {
        int bytesRead = NET_ReadFromStreamSocket(socket, dst + read, n - read);
        if (bytesRead == -1) {
            throw std::runtime_error("Failed to read from network socket");
            return false;
        }
        read += bytesRead;
    }
    return true;
}

bool SDLNet::messageAvailable(NetSocket* s)
{
    void* vsockets[] = { (void*)s->nativePtr };
    return NET_WaitUntilInputAvailable(vsockets, 1, 0) == 1;
}

bool SDLNet::socketConnected(NetSocket* s)
{
    return NET_GetConnectionStatus((NET_StreamSocket*)s->nativePtr) == NET_SUCCESS;
}

NetBroadcastSocket* SDLNet::createBroadcastListener(std::string address)
{
    //todo: address?
    NET_DatagramSocket* broadcastSocket = NET_CreateDatagramSocket(NULL, LAN_BROADCAST_PORT, 0);
    if (broadcastSocket != NULL) {
        return new NetBroadcastSocket{ this, broadcastSocket };
    }
    logerr("Failed to create LAN broadcast socket");
    return NULL;
}

void SDLNet::destroyBroadcastListener(NetBroadcastSocket* s)
{
    NET_DestroyDatagramSocket((NET_DatagramSocket*)s->nativePtr);
    delete s;
}

bool SDLNet::tryReceiveBroadcastMessage(NetBroadcastSocket* s, NetBroadcastMessage* outMsg)
{
    NET_Datagram* dgram = NULL;
    if (NET_ReceiveDatagram((NET_DatagramSocket*)s->nativePtr, &dgram) && dgram != NULL) {
        NetBroadcastMessage msg{};
        NET_WaitUntilResolved(dgram->addr, -1);
        msg.srcAddress = NET_GetAddressString(dgram->addr);
        msg.srcPort = dgram->port;
        msg.putBytes(dgram->buf, dgram->buflen);
        *outMsg = msg;
        NET_DestroyDatagram(dgram);
        return true;
    }
    return false;
}

void SDLNet::broadcastMessage(NetMessage& msg)
{
    auto adapters = platformGetNetworkAdapters();
    for (auto& adapter : adapters) {
        NET_Address* addr = NET_ResolveHostname(adapter.broadcastAddress.c_str());
        NET_Address* addr2 = NET_ResolveHostname(adapter.thisMachineAddress.c_str());
        NET_WaitUntilResolved(addr, -1);
        NET_WaitUntilResolved(addr2, -1);
        //loginfo(frmt("Broadcasting to {}...", NET_GetAddressString(addr)));
        if (addr != NULL) {
            DoOnReturn unrefAddr([addr]() {NET_UnrefAddress(addr); });
            DoOnReturn unrefAddr2([addr2]() {NET_UnrefAddress(addr2); });
            NET_DatagramSocket* sock = NET_CreateDatagramSocket(addr2, LAN_BROADCAST_PORT, 0);
            if (sock != NULL) {
                DoOnReturn freeSock([sock]() { NET_DestroyDatagramSocket(sock); });
                bool r = NET_SendDatagram(sock, addr, LAN_BROADCAST_PORT, (u8*)msg.bytes.data(), msg.bytes.size());
            }
        }
    }
}
#endif

#if VSP_STEAMWORKS
void SteamNet::handleSteamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* cb)
{
    if (cb->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting) {
        std::lock_guard<std::mutex> mtx(inboundConnectionsMutex);
        inboundConnections[cb->m_info.m_hListenSocket].push(cb->m_hConn);
    }
}
void SteamNet::handleLobbyEnter(LobbyEnter_t* cb)
{
    loginfo(frmt("[Steamworks] Entering lobby {}", cb->m_ulSteamIDLobby));
    currentLobbyID = cb->m_ulSteamIDLobby;
    runningLobby = true;
    if (SteamMatchmaking()->GetLobbyOwner(cb->m_ulSteamIDLobby) != SteamUser()->GetSteamID()) {
        StartScreen::promptConnectToNetworkCanvas(frmt("{}", SteamMatchmaking()->GetLobbyOwner(cb->m_ulSteamIDLobby).ConvertToUint64()), "");
        SteamMatchmaking()->LeaveLobby(cb->m_ulSteamIDLobby);
    }
}
void SteamNet::handleGameLobbyJoinRequested(GameLobbyJoinRequested_t* cb)
{
    SteamMatchmaking()->JoinLobby(cb->m_steamIDLobby);
}
std::string SteamNet::getDefaultUsername()
{
    return SteamFriends()->GetPersonaName();
}
bool SteamNet::init()
{
    /*HMODULE m = LoadLibraryW(L"steam_api64.dll");
    if (m != NULL) {
        SteamAPI_InitSafe = (decltype(SteamAPI_InitSafe))GetProcAddress(m, "SteamAPI_InitSafe");
        SteamAPI_Shutdown = (decltype(SteamAPI_Shutdown))GetProcAddress(m, "SteamAPI_Shutdown");
        SteamAPI_RunCallbacks = (decltype(SteamAPI_RunCallbacks))GetProcAddress(m, "SteamAPI_RunCallbacks");
        SteamAPI_SteamUser_v023 = (decltype(SteamAPI_SteamUser_v023))GetProcAddress(m, "SteamAPI_SteamUser_v023");
        SteamAPI_ISteamUser_GetSteamID = (decltype(SteamAPI_ISteamUser_GetSteamID))GetProcAddress(m, "SteamAPI_ISteamUser_GetSteamID");

        if (SteamAPI_InitSafe != NULL) {
            bool ret = SteamAPI_InitSafe();
            if (ret) {
                loginfo(frmt("Steam ID: {}", SteamAPI_ISteamUser_GetSteamID(SteamAPI_SteamUser_v023())));
            }
            return ret;
        }
    }*/

    bool ret = SteamAPI_Init();
    if (ret) {
        u64 steamID = SteamUser()->GetSteamID().ConvertToUint64();
        loginfo(frmt("[Steamworks] Logged in as Steam ID: {} ({})", steamID, SteamFriends()->GetPersonaName()));
        g_addNotificationFromThread(Notification("Connected to Steam", frmt("{} ({})", SteamFriends()->GetPersonaName(), steamID)));
    }

    return ret;
}

NetServer* SteamNet::createServer(std::string address, int port) { 
    if (runningLobby) {
        logerr("[Steamworks] Already running Steam lobby.");
        return NULL;
    }
    HSteamListenSocket socket = SteamNetworkingSockets()->CreateListenSocketP2P(port, 0, NULL);
    SteamNetworkingUtils()->InitRelayNetworkAccess();
    loginfo(frmt("[Steamworks] Server socket ID {}", socket));
    SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 100);
    std::lock_guard<std::mutex> mtx(inboundConnectionsMutex);
    NetServer* s = new NetServer{ this, (void*)socket };
    inboundConnections[socket] = {};
    return s;
}

NetSocket* SteamNet::createClient(std::string address, int port) {
    try {
        SteamNetworkingIdentity id{};
        id.SetSteamID64(std::stoull(address));

        HSteamNetConnection conn = SteamNetworkingSockets()->ConnectP2P(id, port, 0, NULL);
        loginfo(frmt("[Steamworks] Client socket ID {}", conn));
        return new NetSocket{ this, (void*)conn };
    }
    catch (std::exception&) {
        return NULL;
    }
}

NetSocket* SteamNet::acceptNextClient(NetServer* s) {
    std::lock_guard<std::mutex> mtx(inboundConnectionsMutex);
    auto& q = inboundConnections[(HSteamListenSocket)s->nativePtr];
    if (!q.empty()) {
        auto next = q.front();
        q.pop();
        EResult res = SteamNetworkingSockets()->AcceptConnection(next);
        if (res == k_EResultInvalidParam || res == k_EResultInvalidState) {
            return NULL;
        }
        else {
            return new NetSocket{ this, (void*)next };
        }
    }
    return NULL;
}

void SteamNet::destroyServer(NetServer* s) {
    loginfo(frmt("[Steamworks] Closing server socket {}", (HSteamListenSocket)s->nativePtr));
    bool r = SteamNetworkingSockets()->CloseListenSocket((HSteamListenSocket)s->nativePtr);
    delete s;
    if (runningLobby) {
        SteamMatchmaking()->LeaveLobby(currentLobbyID);
        runningLobby = false;
    }
}

void SteamNet::destroySocket(NetSocket* s) {
    SteamNetworkingSockets()->FlushMessagesOnConnection((HSteamNetConnection)s->nativePtr);
    SteamNetworkingSockets()->CloseConnection((HSteamNetConnection)s->nativePtr, ESteamNetConnectionEnd::k_ESteamNetConnectionEnd_App_Generic, NULL, false);
    delete s;
    if (runningLobby) {
        SteamMatchmaking()->LeaveLobby(currentLobbyID);
        runningLobby = false;
    }
}

void SteamNet::writeMessageToSocket(NetSocket* s, NetMessage& msg) {
    s64 pOutMsgNum = 0;
    EResult res = SteamNetworkingSockets()->SendMessageToConnection((HSteamNetConnection)s->nativePtr, msg.bytes.data(), msg.bytes.size(), k_nSteamNetworkingSend_Reliable, &pOutMsgNum);
    if (res > k_EResultOK) {
        logerr(frmt("[Steamworks] Error writing message: {}", (int)res));
        throw std::runtime_error("SendMessageToConnection failed");
    }
}

NetMessage SteamNet::readMessageFromSocket(NetSocket* s) {
    SteamNetworkingMessage_t* outMsgs[] = {NULL};
    int n = SteamNetworkingSockets()->ReceiveMessagesOnConnection((HSteamNetConnection)s->nativePtr, outMsgs, 1);
    if (n == -1) {
        throw std::runtime_error("ReceiveMessagesOnConnection invalid handle");
    }
    else if (n == 1) {
        NetMessage msg{};
        msg.putBytes((u8*)outMsgs[0]->GetData(), outMsgs[0]->GetSize());
        outMsgs[0]->Release();
        return msg;
    }
    else {
        return NetMessage{};
    }
}

bool SteamNet::socketConnected(NetSocket* s) {
    SteamNetConnectionInfo_t out{};
    if (SteamNetworkingSockets()->GetConnectionInfo((HSteamNetConnection)s->nativePtr, &out)) {
        auto connState = out.m_eState;
        return 
            connState == k_ESteamNetworkingConnectionState_Connecting
            || connState == k_ESteamNetworkingConnectionState_Connected
            || connState == k_ESteamNetworkingConnectionState_FindingRoute;
    }
    return false;
}

void SteamNet::runCallbacks()
{
    if (SteamAPI_RunCallbacks != NULL) {
        SteamAPI_RunCallbacks();
    }
}


#endif