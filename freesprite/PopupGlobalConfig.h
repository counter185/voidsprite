#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "keybinds.h"
#include "UIButton.h"

class KeybindButton : public UIButton
{
public:
    std::string keybindText;
    SDL_Color keybindTextColor = { 255,255,255,0xa0 };

    void render(XY pos) override;
};

class PopupGlobalConfig :
    public BasePopup, public EventCallbackListener
{
private:
    ScrollingPanel* keybindsPanel = NULL;
    UILabel* languageCredit = NULL;
    GlobalConfig previousConfig;
    std::vector<std::string> previousKeybinds;

    Timer64 keyBindingTimer;
    bool bindingKey = false;
    KeyCombo* currentBindTarget = NULL;
    KeybindRegion* currentBindTargetRegion = NULL;
    KeybindButton* currentBindTargetButton = NULL;
    std::vector<SDL_Scancode> reservedKeysNow = {};

    std::vector<std::string> langLocNames;
public:
    PopupGlobalConfig();

    void render() override;
    void takeInput(SDL_Event evt) override;

    void eventButtonPressed(int evt_id) override;
    void eventTextInput(int evt_id, std::string text) override;
    void eventDropdownItemSelected(int evt_id, int index, std::string name) override;

    void createKeybindButtons();

    void updateKeybindButtonText(KeyCombo* keycombo, KeybindButton* btn);
    void updateLanguageCredit();

    UICheckbox* optionCheckbox(std::string name, std::string tooltip, bool* target, XY* position);
};

