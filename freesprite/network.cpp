#include "network.h"
#include "mathops.h"

#if VSP_NETWORKING
#include <SDL3_net/SDL_net.h>
#endif

#if VSP_STEAMWORKS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif


std::vector<NetInterface*> g_netInterfaces = {};
void g_initNet() {
	SteamNet* steamInterface = new SteamNet();
	if (steamInterface->init()) {
		g_netInterfaces.push_back(steamInterface);
	}
	else {
		delete steamInterface;
	}
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
	std::string str = std::string(msg.bytes.begin(), msg.bytes.begin() + ixmin(msg.bytes.size(), 8));
	loginfo(frmt("[{:x}] >> {} ({}b)", (u64)s, str, msgSize));
}

NetMessage SDLNet::readMessageFromSocket(NetSocket* s)
{
	NetMessage ret = {};
	u64 msgSize = 0;
	if (readNBytesFromSocketBlocking(s, 8, (u8*)&msgSize)) {
		ret.bytes.resize(msgSize);
		readNBytesFromSocketBlocking(s, msgSize, ret.bytes.data());
		std::string str = std::string(ret.bytes.begin(), ret.bytes.begin() + ixmin(ret.bytes.size(), 8));
		loginfo(frmt("[{:x}] << {} ({}b)", (u64)s, str, msgSize));
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
bool SteamNet::init()
{
	HMODULE m = LoadLibraryW(L"steam_api64.dll");
	if (m != NULL) {
		SteamAPI_InitSafe = (bool(_cdecl*)())GetProcAddress(m, "SteamAPI_InitSafe");
		SteamAPI_Shutdown = (void(_cdecl*)())GetProcAddress(m, "SteamAPI_Shutdown");
		SteamAPI_RunCallbacks = (void(_cdecl*)())GetProcAddress(m, "SteamAPI_RunCallbacks");

		if (SteamAPI_InitSafe != NULL) {
			return SteamAPI_InitSafe();
		}
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