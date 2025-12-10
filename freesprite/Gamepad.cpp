#include "Gamepad.h"
#include "Notification.h"

void Gamepad::TryCaptureGamepad()
{
    int n;
    SDL_JoystickID* ids = SDL_GetGamepads(&n);
    
    if (ids != NULL) {
        if (n > 0) {
            for (int x = 0; x < n && gamepad == NULL; x++) {
                gamepad = SDL_OpenGamepad(ids[x]);
                auto* gamepadNewName = SDL_GetGamepadName(gamepad);
                
                if (gamepad == NULL) {
                    auto* gamepadIDName = SDL_GetGamepadNameForID(ids[x]);
                    logerr(frmt("Error opening gamepad {}:\n {}", (gamepadIDName != NULL ? gamepadIDName : ""), SDL_GetError()));
                }
                else {
                    if (gamepadNewName != NULL) {
                        gamepadName = std::string(gamepadNewName);
                    }
                    loginfo(frmt("Connected gamepad: {}", gamepadName));
                }
            }
        }
        SDL_free(ids);
    }

    if (gamepad != NULL) {
        std::string name = gamepadName;
        g_addNotification(Notification("Gamepad connected", name, 5000, NULL, uint32ToSDLColor(GetColorForGamepadName(name))));
    }
    
    gamepadConnected = gamepad != NULL;
}

void Gamepad::TakeEvent(SDL_Event evt)
{
    switch (evt.type) {
        case SDL_CONTROLLERDEVICEADDED:
            if (!gamepadConnected) {
                TryCaptureGamepad();
            }
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            if (gamepad == NULL || !SDL_GameControllerGetAttached(gamepad)) {
                gamepad = NULL;
                gamepadConnected = false;
                g_addNotification(Notification("Gamepad disconnected", ""));
                TryCaptureGamepad();
            }
            break;
        case SDL_CONTROLLERAXISMOTION:
            if (evt.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX) {
                gamepadLSX = evt.gaxis.value / 32768.0f;
            }
            if (evt.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTY) {
                gamepadLSY = evt.gaxis.value / 32768.0f;
            }
            break;
    }
}

void Gamepad::SetLightbar(uint8_t r, uint8_t g, uint8_t b)
{
    if (gamepad != NULL) {
        SDL_SetGamepadLED(gamepad, r, g, b);
    }
}

u32 Gamepad::GetColorForGamepadName(std::string name)
{
    if (stringContainsIgnoreCase(name, "ps2")
        || stringContainsIgnoreCase(name, "ps3")
        || stringContainsIgnoreCase(name, "ps4")
        || stringContainsIgnoreCase(name, "dualsense")
        || stringContainsIgnoreCase(name, "dualshock")
        || stringContainsIgnoreCase(name, "ps vita")) {
        return 0xFF0000FF;
    }
    if (stringContainsIgnoreCase(name, "xbox")
        || stringContainsIgnoreCase(name, "razer")) {
        return 0xFF00FF00;
    }
    if (stringContainsIgnoreCase(name, "switch")
        || stringContainsIgnoreCase(name, "joy-con")
        || stringContainsIgnoreCase(name, "nintendo")
        || stringContainsIgnoreCase(name, "wii u")) {
        return 0xFFFF0000;
    }
    if (stringContainsIgnoreCase(name, "steam")) {
        return 0xFF5CCEFF;
    }
    return 0xFFFFFFFF;
}
