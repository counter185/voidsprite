#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

struct KeybindConf {
    std::string name = "";
    SDL_Keycode* target = NULL;
    SDL_Texture* icon = NULL;
};

class PopupGlobalConfig :
    public BasePopup, public EventCallbackListener
{
private:
    GlobalConfig previousConfig;
    std::vector<std::pair<KeybindConf, UIButton*>> keybindButtons;
    bool bindingKey = false;
    int bindingKeyIndex = -1;
    std::vector<SDL_Keycode> reservedKeys = {
        SDLK_LCTRL,
        SDLK_RCTRL,
        SDLK_LALT,
        SDLK_LEFTBRACKET,
        SDLK_RIGHTBRACKET,
        SDLK_Q,
        SDLK_E,
        SDLK_F2
    };
public:
    PopupGlobalConfig();

    void render() override;
    void takeInput(SDL_Event evt) override;

    void eventButtonPressed(int evt_id) override;
    void eventCheckboxToggled(int evt_id, bool checked) override;
    void eventTextInput(int evt_id, std::string text) override;
    void eventDropdownItemSelected(int evt_id, int index, std::string name) override;

    void updateKeybindButtonText(std::pair<KeybindConf, UIButton*> t);
};

