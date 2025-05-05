#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

struct KeybindConf {
    std::string name = "";
    SDL_Scancode* target = NULL;
    SDL_Texture* icon = NULL;
};

class PopupGlobalConfig :
    public BasePopup, public EventCallbackListener
{
private:
    UILabel* languageCredit = NULL;
    GlobalConfig previousConfig;
    std::vector<std::pair<KeybindConf, UIButton*>> keybindButtons;
    bool bindingKey = false;
    int bindingKeyIndex = -1;
    std::vector<std::string> langLocNames;
    std::vector<SDL_Scancode> reservedKeys = {
        SDL_SCANCODE_LCTRL,
        SDL_SCANCODE_RCTRL,
        SDL_SCANCODE_LALT,
        SDL_SCANCODE_LEFTBRACKET,
        SDL_SCANCODE_RIGHTBRACKET,
        SDL_SCANCODE_Q,
        SDL_SCANCODE_E,
        SDL_SCANCODE_F2
    };
public:
    PopupGlobalConfig();

    void takeInput(SDL_Event evt) override;

    void eventButtonPressed(int evt_id) override;
    void eventTextInput(int evt_id, std::string text) override;
    void eventDropdownItemSelected(int evt_id, int index, std::string name) override;

    void updateKeybindButtonText(std::pair<KeybindConf, UIButton*> t);
    void updateLanguageCredit();

    UICheckbox* optionCheckbox(std::string name, std::string tooltip, bool* target, XY* position);
};

