#pragma once
#include "globals.h"
#include <queue>
#include <mutex>

#define VSP_STEAMWORKS 1

class NetInterface;

struct NetSocket {
	NetInterface* netIf = 0;
	//for sdl_net this is a net_streamsocket
	void* nativePtr = 0;
};
struct NetBroadcastSocket {
	NetInterface* netIf = 0;
	//for sdl_net this is a net_datagramsocket
	void* nativePtr = 0;
};

struct NetServer {
	NetInterface* netIf = 0;
	//for sdl_net this is a net_server
	void* nativePtr = 0;
};

class NetMessage {
private:
	size_t readPtr = 0;
public:
	std::vector<u8> bytes;

	void reset() { bytes.clear(); readPtr = 0; }
	void seekToStart() { readPtr = 0; }

	void putBytes(u8* a, size_t count) {
		size_t sizeNow = bytes.size();
		bytes.resize(bytes.size() + count);
		memcpy(bytes.data() + sizeNow, a, count);
	}
	void putString(std::string a) {
		putBytes((u8*)a.data(), a.size());
	}
	void putU32(u32 a) {
		putBytes((u8*)&a, 4);
	}
	void putU64(u64 a) {
		putBytes((u8*)&a, 8);
	}

	void readBytes(u8* target, size_t count) {
		if (readPtr + count > bytes.size()) {
			logerr(frmt("** [NetMessage] out of bounds read (buffer={}b, readPtr={}, count={})", bytes.size(), readPtr, count));
		}
		else {
			memcpy(target, bytes.data() + readPtr, count);
			readPtr += count;
		}

	}
};

class NetBroadcastMessage : public NetMessage {
public:
	std::string srcAddress;
	int srcPort;
};

class NetInterface {
public:
	virtual std::string interfaceName() = 0;
	virtual std::string getDefaultUsername() { return "User"; }

	virtual bool init() = 0;
	virtual std::string getFriendlyAddress(NetSocket* s) = 0;
	virtual NetServer* createServer(std::string address, int port) = 0;
	virtual NetSocket* createClient(std::string address, int port) = 0;
	virtual NetSocket* acceptNextClient(NetServer* s) = 0;
	virtual void destroyServer(NetServer* s) = 0;
	virtual void destroySocket(NetSocket* s) = 0;
	virtual void writeMessageToSocket(NetSocket* s, NetMessage& msg) = 0;
	virtual NetMessage readMessageFromSocket(NetSocket* s) = 0;
	virtual bool messageAvailable(NetSocket* s) = 0;

	//for lan discovery:
	virtual NetBroadcastSocket* createBroadcastListener(std::string address) = 0;
	virtual void destroyBroadcastListener(NetBroadcastSocket* s) = 0;
	virtual bool tryReceiveBroadcastMessage(NetBroadcastSocket* s, NetBroadcastMessage* outMsg) = 0;
	virtual void broadcastMessage(NetMessage& msg) {}

	virtual void runCallbacks() {}
};

#if VSP_NETWORKING
class SDLNet : public NetInterface {
private:
	bool readNBytesFromSocketBlocking(NetSocket* s, int n, u8* dst);
public:
	std::string interfaceName() override { return "Manual networking (TCP/UDP)"; }

	bool init() override;
	std::string getFriendlyAddress(NetSocket* s) override;
	NetServer* createServer(std::string address, int port) override;
	NetSocket* createClient(std::string address, int port) override;
	NetSocket* acceptNextClient(NetServer* s) override;
	void destroyServer(NetServer* s) override;
	void destroySocket(NetSocket* s) override;
	void writeMessageToSocket(NetSocket* s, NetMessage& msg) override;
	NetMessage readMessageFromSocket(NetSocket* s) override;
	bool messageAvailable(NetSocket* s) override;

	NetBroadcastSocket* createBroadcastListener(std::string address) override;
	void destroyBroadcastListener(NetBroadcastSocket* s) override;
	bool tryReceiveBroadcastMessage(NetBroadcastSocket* s, NetBroadcastMessage* outMsg) override;
	void broadcastMessage(NetMessage& msg) override;
};
#endif

#if VSP_STEAMWORKS
/*typedef u64 SteamID;
typedef u64 SteamAPICall_t;
typedef u32 HSteamListenSocket;
#define SteamNetworkingConfigValue_t void
#define ISteamNetworking void
#define ISteamNetworkingSockets void
#define ISteamUser void*/

#include "steam/steam_api.h"

class SteamNet : public NetInterface {
private:
	std::mutex inboundConnectionsMutex;
	std::map<HSteamListenSocket, std::queue<HSteamNetConnection>> inboundConnections;
	STEAM_CALLBACK(SteamNet, handleSteamNetConnectionStatusChangedCallback, SteamNetConnectionStatusChangedCallback_t);
public:
	std::string interfaceName() override { return "Steam"; }
	std::string getDefaultUsername() override;

	bool init() override;
	std::string getFriendlyAddress(NetSocket* s) override { return "<steam socket>"; };
	NetServer* createServer(std::string address, int port) override;
	NetSocket* createClient(std::string address, int port) override;
	NetSocket* acceptNextClient(NetServer* s) override;
	void destroyServer(NetServer* s) override;
	void destroySocket(NetSocket* s) override;
	void writeMessageToSocket(NetSocket* s, NetMessage& msg) override;
	NetMessage readMessageFromSocket(NetSocket* s) override;
	bool messageAvailable(NetSocket* s) override { return true; };

	NetBroadcastSocket* createBroadcastListener(std::string address) override { return NULL; };
	void destroyBroadcastListener(NetBroadcastSocket* s) override {};
	bool tryReceiveBroadcastMessage(NetBroadcastSocket* s, NetBroadcastMessage* outMsg) override { return false; };
	void broadcastMessage(NetMessage& msg) override {};

	void runCallbacks() override;
};
#endif

extern std::vector<NetInterface*> g_netInterfaces;

void g_initNet();
std::string g_getAllNetworkLibsVersions();
void g_runNetworkCallbacks();