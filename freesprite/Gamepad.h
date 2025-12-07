#pragma once
#include "globals.h"

class Gamepad
{
public:
    float gamepadLSX = 0, gamepadLSY = 0;

    bool gamepadConnected = false;
    SDL_Gamepad* gamepad = NULL;
    void TryCaptureGamepad();
    void TakeEvent(SDL_Event evt);
    void SetLightbar(uint8_t r, uint8_t g, uint8_t b);

    static u32 GetColorForGamepadName(std::string name);
};

