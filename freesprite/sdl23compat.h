#pragma once

//we're going two-way compat:fire:
#if SDL_MAJOR_VERSION != 2
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

#define KEYCODE(a) a.key.scancode
#define DOWN(a) a.down

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
#else


#define KEYCODE(a) a.key.keysym.sym
#define DOWN(a) a.state

struct SDL_FColor
{
    float r;
    float g;
    float b;
    float a;
};

#define SDL_MICRO_VERSION SDL_PATCHLEVEL
#define SDL_IMAGE_MICRO_VERSION SDL_IMAGE_PATCHLEVEL
#define SDL_TTF_MICRO_VERSION SDL_TTF_PATCHLEVEL
#define SDL_Gamepad SDL_GameController
#define SDL_GetGamepadName SDL_GameControllerName
#define SDL_SetGamepadLED SDL_GameControllerSetLED
#define SDL_RenderLine SDL_RenderDrawLine      
#define SDL_GetTicks SDL_GetTicks64          
#define SDL_DestroySurface SDL_FreeSurface         
#define SDL_EVENT_KEY_UP SDL_KEYUP               
#define SDL_EVENT_KEY_DOWN SDL_KEYDOWN             
#define SDL_EVENT_QUIT SDL_QUIT                
#define SDL_EVENT_MOUSE_WHEEL SDL_MOUSEWHEEL          
#define SDL_EVENT_MOUSE_MOTION SDL_MOUSEMOTION             
#define SDL_EVENT_MOUSE_BUTTON_UP SDL_MOUSEBUTTONUP           
#define SDL_EVENT_MOUSE_BUTTON_DOWN SDL_MOUSEBUTTONDOWN             
#define SDL_EVENT_GAMEPAD_ADDED SDL_CONTROLLERDEVICEADDED           
#define SDL_EVENT_GAMEPAD_REMOVED SDL_CONTROLLERDEVICEREMOVED             
#define SDL_EVENT_FINGER_MOTION SDL_FINGERMOTION            
#define SDL_EVENT_FINGER_DOWN SDL_FINGERDOWN          
#define SDL_EVENT_FINGER_UP SDL_FINGERUP            
#define SDL_EVENT_GAMEPAD_AXIS_MOTION SDL_CONTROLLERAXISMOTION            
#define SDL_EVENT_TEXT_INPUT SDL_TEXTINPUT           
#define SDL_EVENT_DROP_FILE SDL_DROPFILE            
#define SDL_GamepadConnected SDL_GameControllerGetAttached           
#define SDL_SetRenderClipRect SDL_RenderSetClipRect      
#define SDL_SCALEMODE_NEAREST SDL_ScaleModeNearest
#define TTF_FontHasGlyph TTF_GlyphIsProvided32
#define TTF_GetGlyphMetrics TTF_GlyphMetrics32
#define SDL_IOStream SDL_RWops
#define SDL_IOFromMem SDL_RWFromMem
#define SDL_CloseIO SDL_RWclose
#define IMG_LoadTGA_IO IMG_LoadTGA_RW


#define SDL_PROP_TEXTINPUT_TYPE_NUMBER ""
#define SDL_PropertiesID uint8_t
bool SDL_SetBooleanProperty(SDL_PropertiesID props, const char* name, bool value) {
    return true;
}

bool SDL_StartTextInputWithProperties(SDL_Window* window, SDL_PropertiesID props) {
    SDL_StartTextInput();
    return true;
}

void SDL_StopTextInput(SDL_Window* wd) {
    SDL_StopTextInput();
}

#endif