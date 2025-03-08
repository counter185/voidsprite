#pragma once
#define SDL_RenderDrawLine SDL_RenderLine
#define SDL_GetTicks64 SDL_GetTicks
#define SDL_FreeSurface SDL_DestroySurface
#define SDL_KEYUP SDL_EVENT_KEY_UP
#define SDL_KEYDOWN SDL_EVENT_KEY_DOWN
#define SDL_QUIT SDL_EVENT_QUIT
#define SDL_MOUSEWHEEL SDL_EVENT_MOUSE_WHEEL
#define SDL_MOUSEMOTION SDL_EVENT_MOUSE_MOTION
#define SDL_MOUSEBUTTONUP SDL_EVENT_MOUSE_BUTTON_UP
#define SDL_MOUSEBUTTONDOWN SDL_EVENT_MOUSE_BUTTON_DOWN
#define SDL_CONTROLLERDEVICEADDED SDL_EVENT_GAMEPAD_ADDED
#define SDL_CONTROLLERDEVICEREMOVED SDL_EVENT_GAMEPAD_REMOVED
#define SDL_FINGERMOTION SDL_EVENT_FINGER_MOTION
#define SDL_FINGERDOWN SDL_EVENT_FINGER_DOWN
#define SDL_FINGERUP SDL_EVENT_FINGER_UP
#define SDL_CONTROLLERAXISMOTION SDL_EVENT_GAMEPAD_AXIS_MOTION
#define SDL_TEXTINPUT SDL_EVENT_TEXT_INPUT
#define SDL_DROPFILE SDL_EVENT_DROP_FILE
#define SDL_GameControllerGetAttached SDL_GamepadConnected
#define SDL_RenderSetClipRect SDL_SetRenderClipRect

inline bool SDL_RenderDrawRect(SDL_Renderer* rd, SDL_Rect* r) {
    SDL_FRect fr = r != NULL ? SDL_FRect{(float)r->x, (float)r->y, (float)r->w, (float)r->h} : SDL_FRect{0, 0, 0, 0};
    return SDL_RenderRect(rd, r != NULL ? &fr : NULL);
}

inline bool SDL_RenderFillRect(SDL_Renderer* rd, SDL_Rect* r) {
    SDL_FRect fr = r != NULL ? SDL_FRect{(float)r->x, (float)r->y, (float)r->w, (float)r->h} : SDL_FRect{0, 0, 0, 0};
    return SDL_RenderFillRect(rd, r != NULL ? &fr : NULL);
}

inline bool SDL_RenderCopy(SDL_Renderer* rd, SDL_Texture* tex, SDL_Rect* srcr, SDL_Rect* dstr) {
    SDL_FRect srcfr = srcr != NULL ? SDL_FRect{(float)srcr->x, (float)srcr->y, (float)srcr->w, (float)srcr->h}
                                   : SDL_FRect{0, 0, 0, 0};
    SDL_FRect dstfr = dstr != NULL ? SDL_FRect{(float)dstr->x, (float)dstr->y, (float)dstr->w, (float)dstr->h}
                                   : SDL_FRect{0, 0, 0, 0};
    return SDL_RenderTexture(rd, tex, srcr == NULL ? NULL : &srcfr, dstr == NULL ? NULL : &dstfr);
}