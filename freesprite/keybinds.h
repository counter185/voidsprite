#pragma once
#include "globals.h"

#define KEY_UNASSIGNED SDL_SCANCODE_UNKNOWN

class KeyCombo {
private:
    std::function<void(void*)> action = NULL;
public:
    std::string displayName = "Keybind";
    SDL_Scancode key = KEY_UNASSIGNED;
    bool ctrl = false;
    bool shift = false;
    SDL_Texture* icon = NULL;

    KeyCombo() {}
    KeyCombo(std::string n, std::function<void(void*)> a) : displayName(n), action(a) {}
    KeyCombo(std::string n, SDL_Scancode k, bool c, bool s, std::function<void(void*)> a) : displayName(n), key(k), ctrl(c), shift(s), action(a) {}

    bool isHit(SDL_Event evt) {
        return evt.type == SDL_EVENT_KEY_DOWN
            && key != KEY_UNASSIGNED
            && evt.key.scancode == key
            && ctrl == g_ctrlModifier
            && shift == g_shiftModifier;
    }
    void execute(void* caller) {
        if (action) {
            action(caller);
        }
    }
    bool executeIfHit(SDL_Event evt, void* caller) {
        if (isHit(evt)) {
            execute(caller);
            return true;
        }
        return false;
    }

    std::string getKeyComboName() {
        if (key == KEY_UNASSIGNED) {
            return "Unassigned";
        }
        else {
            return std::format("{}{}{}", ctrl ? "Ctrl + " : "", shift ? "Shift + " : "", SDL_GetScancodeName(key));
        }
    }

    bool isUnassigned() {
        return key == KEY_UNASSIGNED;
    }

    void unassign() {
        key = KEY_UNASSIGNED;
        ctrl = false;
        shift = false;
    }
};

class KeybindRegion {
public:
    std::string displayName;
    std::string regionKey;
    std::vector<SDL_Scancode> reservedKeys;
    std::map<std::string, KeyCombo> keybinds;
    std::vector<std::string> orderInSettings;

    bool unassignAllWith(SDL_Scancode c, bool shift, bool ctrl) {
        bool ret = false;
        for (auto& [key, kc] : keybinds) {
            if (kc.key == c && kc.shift == shift && kc.ctrl == ctrl) {
                kc.unassign();
                ret = true;
            }
        }
        return ret;
    }
};

class KeybindManager {
public:
    std::vector<SDL_Scancode> globalReservedKeys;
    std::map<std::string, KeybindRegion> regions;

    void newRegion(std::string key, std::string displayName) {
        regions[key] = KeybindRegion();
        regions[key].regionKey = key;
        regions[key].displayName = displayName;
    }

    void addKeybind(std::string region, std::string key, KeyCombo kc) {
        regions[region].keybinds[key] = kc;
        regions[region].orderInSettings.push_back(key);
    }

    bool processKeybinds(SDL_Event evt, std::string inRegion, void* caller) {
        for (auto [id, kc] : regions[inRegion].keybinds) {
            if (kc.executeIfHit(evt, caller)) {
                return true;
            }
        }
        return false;
    }

    std::vector<std::string> serializeKeybinds() {
        std::vector<std::string> ret;
        for (auto [region, regionData] : regions) {
            for (auto [key, kc] : regionData.keybinds) {
                ret.push_back(std::format("{}/{}:{}+{}+{}", region, key, (int)kc.key, kc.ctrl ? "1" : "0", kc.shift ? "1" : "0"));
            }
        }
        return ret;
    }

    void deserializeKeybindLine(std::string line) {
        auto splitByColon = splitString(line, ':');
        std::string path = splitByColon[0];
        auto splitBySlash = splitString(path, '/');
        std::string region = splitBySlash[0];
        std::string keyName = splitBySlash[1];

        std::string fullKey = splitByColon[1];
        auto splitByPlus = splitString(fullKey, '+');
        int key = std::stoi(splitByPlus[0]);
        bool ctrl = splitByPlus[1] == "1";
        bool shift = splitByPlus[2] == "1";

        if (regions[region].keybinds.contains(keyName)) {
            regions[region].keybinds[keyName].key = (SDL_Scancode)key;
            regions[region].keybinds[keyName].ctrl = ctrl;
            regions[region].keybinds[keyName].shift = shift;
        }
    }

    void deserializeKeybinds(std::vector<std::string> lines) {
        for (auto line : lines) {
            deserializeKeybindLine(line);
        }
    }
};

inline KeybindManager g_keybindManager;

void g_initKeybinds();