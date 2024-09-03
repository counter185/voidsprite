#include "Gamepad.h"
#include "Notification.h"

void Gamepad::TryCaptureGamepad()
{
	int n = SDL_NumJoysticks();
	if (n > 0) {
		gamepad = SDL_GameControllerOpen(0);
	}

	if (gamepad != NULL) {
		g_addNotification(Notification("Gamepad connected", SDL_GameControllerName(gamepad)));
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
				TryCaptureGamepad();
			}
			break;
		case SDL_CONTROLLERAXISMOTION:
			if (evt.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
				gamepadLSX = evt.caxis.value / 32768.0f;
			}
			if (evt.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
				gamepadLSY = evt.caxis.value / 32768.0f;
			}
			break;
	}
}

void Gamepad::SetLightbar(uint8_t r, uint8_t g, uint8_t b)
{
	if (gamepad != NULL) {
		SDL_GameControllerSetLED(gamepad, r, g, b);
	}
}
