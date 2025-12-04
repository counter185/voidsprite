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
                if (gamepad == NULL) {
                    logerr(frmt("Error opening gamepad {}:\n {}", SDL_GetGamepadNameForID(ids[x]), SDL_GetError()));
                }
                else {
                    loginfo(frmt("Connected gamepad: {}", SDL_GetGamepadName(gamepad)));
                }
            }
        }
        SDL_free(ids);
    }

    if (gamepad != NULL) {
        g_addNotification(Notification("Gamepad connected", SDL_GetGamepadName(gamepad)));
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
