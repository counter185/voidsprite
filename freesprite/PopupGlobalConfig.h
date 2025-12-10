#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "keybinds.h"
#include "UIButton.h"

class KeybindButton : public UIButton
{
public:
    std::string keybindText;
    std::string secondKeybindText;
    SDL_Color keybindTextColor = { 255,255,255,0xa0 };
    SDL_Color secondKeybindTextColor = { 255,255,255,0xa0 };

    void render(XY pos) override;
};

class PopupGlobalConfig :
    public BasePopup, public EventCallbackListener
{
private:
    ScrollingPanel* keybindsPanel = NULL;
    UIStackPanel* keybindsStack = NULL;
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

    void eventTextInput(int evt_id, std::string text) override;
    void eventDropdownItemSelected(int evt_id, int index, std::string name) override;

    void createKeybindButtons(std::string filterName);

    void updateKeybindButtonText(KeyCombo* keycombo, KeybindButton* btn);
    void updateLanguageCredit();

    void shellExtensionAction(bool install);

    UICheckbox* optionCheckbox(std::string name, std::string tooltip, bool* target, XY* position);
    static Panel* optionNumberInput(std::string name, std::string tooltip, int* target, int min, int max, XY* position);
};

