#pragma once

struct RPCLobbyInfo {
    std::string id;
    bool isPrivate;
    int currentSize;
    int maxSize;
    std::string joinSecret;
};

#if _WIN32
#include "StartScreen.h"
#include "Notification.h"
#include "background_operation.h"
#include "discord_game_sdk/discord.h"

inline bool discordInit = false;
inline discord::Core* core;
inline discord::Activity currentActivity{};
inline std::string prevState = "", prevDetails = "", prevLobbyID = "";
inline bool hasLobbyInfo = false;
inline RPCLobbyInfo lobbyInfoNow{};

inline void g_initRPC() {

    if (!discordInit) {
        core = {};
        discord::Result result = discord::Core::Create(1341694438560235613L, DiscordCreateFlags_NoRequireDiscord, &core);
        discordInit = result == discord::Result::Ok;
        if (!discordInit) {
            logwarn("[Discord Game SDK] init failed");
        }
        else {
            core->ActivityManager().RegisterCommand("voidsprite://");
            core->ActivityManager().OnActivityJoin.Connect([](char const* sec) {
                g_startNewMainThreadOperation([sec]() {
                    StartScreen::promptConnectToNetworkCanvas(sec);
                });
            });
            core->ActivityManager().OnActivityInvite.Connect([](discord::ActivityActionType, discord::User const& usr, discord::Activity const&) {
                g_addNotificationFromThread(Notification(frmt("{}", usr.GetUsername()), "invited you to a network canvas session", 5000));
            });
        }
    }
}

inline void g_deinitRPC() {
    if (discordInit) {
        try {
            delete core;
        }
        catch (std::exception& e) {
            //don't care
            logerr(frmt("error deinitializing discord RPC:\n {}", e.what()));
        }
        discordInit = false;
    }
}

inline void g_initOrDeinitRPCBasedOnConfig() {
    if (g_config.useDiscordRPC) {
        g_initRPC();
    }
    else {
        g_deinitRPC();
    }
}

inline void g_updateRPC(std::string state, std::string details) {
    if (discordInit) {
        if (prevState != state || prevDetails != details || prevLobbyID != lobbyInfoNow.id) {
            prevState = state;
            prevDetails = details;
            prevLobbyID = lobbyInfoNow.id;
            currentActivity = {};
            currentActivity.SetState(prevState.c_str());
            currentActivity.SetDetails(prevDetails.c_str());
            if (hasLobbyInfo) {
                currentActivity.GetParty().SetId(lobbyInfoNow.id.c_str());
                currentActivity.GetParty().SetPrivacy(lobbyInfoNow.isPrivate ? discord::ActivityPartyPrivacy::Private : discord::ActivityPartyPrivacy::Public);
                currentActivity.GetParty().GetSize().SetCurrentSize(lobbyInfoNow.currentSize);
                currentActivity.GetParty().GetSize().SetMaxSize(lobbyInfoNow.maxSize);
                currentActivity.SetSupportedPlatforms((u32)discord::ActivitySupportedPlatformFlags::Android 
                    | (u32)discord::ActivitySupportedPlatformFlags::Desktop);
                currentActivity.GetSecrets().SetJoin(lobbyInfoNow.joinSecret.c_str());
            }
            core->ActivityManager().UpdateActivity(currentActivity, [](discord::Result result) {
                if (result != discord::Result::Ok) {
                    logwarn("[Discord Game SDK] update failed");
                }
                });
        }
        core->RunCallbacks();
    }
}

inline void g_pushRPCLobbyInfo(RPCLobbyInfo inf) {
    hasLobbyInfo = true;
    lobbyInfoNow = inf;
}
inline bool g_lockRPCLobbyInfo() {
    bool now = hasLobbyInfo;
    hasLobbyInfo = true;
    return !now;
}
inline void g_clearRPCLobbyInfo() {
    hasLobbyInfo = false;
    lobbyInfoNow.id = "-";
}
#else
inline void g_initRPC() {}
inline void g_deinitRPC() {}
inline void g_updateRPC(std::string state, std::string details) {}
inline void g_initOrDeinitRPCBasedOnConfig() {}
inline void g_pushRPCLobbyInfo(RPCLobbyInfo inf) {}
inline bool g_lockRPCLobbyInfo() { return false; }
inline void g_clearRPCLobbyInfo() {}
#endif