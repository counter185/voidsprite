#pragma once

#if _WIN32
#include "discord_game_sdk/discord.h"

inline bool discordInit = false;
inline discord::Core* core;
inline discord::Activity currentActivity{};
inline std::string prevState = "", prevDetails = "";

inline void g_initRPC() {

    if (!discordInit) {
        core = {};
        discord::Result result = discord::Core::Create(1341694438560235613L, DiscordCreateFlags_NoRequireDiscord, &core);
        discordInit = result == discord::Result::Ok;
        if (!discordInit) {
            logprintf("[Discord Game SDK] init failed\n");
        }
    }
}

inline void g_deinitRPC() {
    if (discordInit) {
        delete core;
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
        if (prevState != state || prevDetails != details) {
            prevState = state;
            prevDetails = details;
            currentActivity.SetState(prevState.c_str());
            currentActivity.SetDetails(prevDetails.c_str());
            core->ActivityManager().UpdateActivity(currentActivity, [](discord::Result result) {
                if (result != discord::Result::Ok) {
                    logprintf("[Discord Game SDK] update failed\n");
                }
                });
        }
        core->RunCallbacks();
    }
}
#else
inline void g_initRPC() {}
inline void g_deinitRPC() {}
inline void g_updateRPC(std::string state, std::string details) {}
inline void g_initOrDeinitRPCBasedOnConfig() {}
#endif