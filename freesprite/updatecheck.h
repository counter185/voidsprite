#pragma once

#include "globals.h"
#include "Notification.h"

#include "json/json.hpp"

using json = nlohmann::json;

#ifndef GIT_HASH
//change to literally any non-empty string to test the update checker
#define GIT_HASH ""
#endif

#ifndef GIT_BRANCH
#define GIT_BRANCH  UTF8_DIAMOND "dev"
#endif

#if _WIN32
#define PLATFORM "win64"
#elif __APPLE__
#define PLATFORM "mac"
#elif __ANDROID__
#define PLATFORM "android"
#else
#define PLATFORM "linux64"
#endif

inline std::atomic<bool> updateCheckComplete = false;
inline std::atomic<bool> updateCheckFailed = true;
inline std::atomic<int> githubStars = 0;
inline std::string latestHash = "";
inline int latestVersionYear = 0;
inline int latestVersionMonth = 0;
inline int latestVersionDay = 0;

inline bool didRunFinish(std::string commitHash) {
    try {
        std::string runsUrl = "https://api.github.com/repos/counter185/voidsprite/actions/runs";
        std::string runsData = platformFetchTextFile(runsUrl);
        json j = json::parse(runsData);
        if (j.is_object() && j.contains("workflow_runs")) {
            for (auto& runs : j["workflow_runs"]) {
                if (runs["head_sha"] == commitHash) {
                    if (runs["status"] == "in_progress") {
                        loginfo(std::format("updatecheck: run {} is currently in progress...", std::string(runs["head_sha"])));
                    }
                    return runs["status"] == "completed" && runs["conclusion"] == "success";
                }
            }
        }
    }
    catch (std::exception&) {
        logerr("Failed to get runs data");
    }
    return false;
}

inline void runUpdateCheck() {
    try {
        std::string repoInfoUrl = "https://api.github.com/repos/counter185/voidsprite";
        std::string repoInfoData = platformFetchTextFile(repoInfoUrl);

        json j = json::parse(repoInfoData);
        if (j.is_object() && j.contains("stargazers_count")) {
            githubStars = j["stargazers_count"].get<int>();
            loginfo(std::format("GitHub star count: {}", (int)githubStars));
        }
    }
    catch (std::exception&) {
        logerr("Failed to get repository data");
    }

    if (!std::string(GIT_HASH).empty()) {
        try {
            std::string artifactsUrl = "https://api.github.com/repos/counter185/voidsprite/actions/artifacts";
            std::string artifactsData = platformFetchTextFile(artifactsUrl);

            if (artifactsData.empty()) {
                latestHash = GIT_HASH;
            }
            else {
                json j = json::parse(artifactsData);
                if (j.is_object() && j.contains("artifacts")) {

                    std::string latestHashNow;
                    int latestYear = 0;
                    int latestMonth = 0;
                    int latestDay = 0;
                    int latestHr = 0;
                    int latestMin = 0;
                    for (auto& artifact : j["artifacts"]) {
                        try {
                            if (artifact.is_object()) {
                                std::string name = artifact["name"];
                                std::string branchName = artifact["workflow_run"]["head_branch"];
                                if (name.find(PLATFORM) == std::string::npos 
                                    || (branchName != "main" && branchName != GIT_BRANCH)) {
                                    continue;
                                }
                                std::string createdAt = artifact["created_at"];
                                std::string headSha = artifact["workflow_run"]["head_sha"];

                                auto timeSplit = splitString(createdAt, 'T');
                                auto date = splitString(timeSplit[0], '-');
                                auto time = splitString(timeSplit[1], ':');

                                int yearNow = std::stoi(date[0]);
                                int monthNow = std::stoi(date[1]);
                                int dayNow = std::stoi(date[2]);
                                int hourNow = std::stoi(time[0]);
                                int minuteNow = std::stoi(time[1]);
                                if (yearNow > latestYear ||
                                    (yearNow == latestYear && monthNow > latestMonth) ||
                                    (yearNow == latestYear && monthNow == latestMonth && dayNow > latestDay) ||
                                    (yearNow == latestYear && monthNow == latestMonth && dayNow == latestDay && hourNow > latestHr) ||
                                    (yearNow == latestYear && monthNow == latestMonth && dayNow == latestDay && hourNow == latestHr && minuteNow > latestMin)) {

                                    if (didRunFinish(headSha)) {
                                        latestHashNow = headSha;
                                        latestYear = yearNow;
                                        latestMonth = monthNow;
                                        latestDay = dayNow;
                                        latestHr = hourNow;
                                        latestMin = minuteNow;
                                    }
                                }
                            }
                        }
                        catch (std::exception&) {}
                    }

                    if (!latestHashNow.empty()) {
                        latestHash = latestHashNow;
                        latestVersionYear = latestYear;
                        latestVersionMonth = latestMonth;
                        latestVersionDay = latestDay;
                    }
                    else {
                        latestHash = GIT_HASH;
                    }
                }
            }
            updateCheckFailed = false;
        }
        catch (std::exception&) {
            logerr("Failed to get actions data");
            latestHash = GIT_HASH;
            updateCheckFailed = true;
        }
    }
    else {
        updateCheckFailed = false;
    }
    updateCheckComplete = true;
}