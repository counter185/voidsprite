#pragma once
#include "DrawableManager.h"

inline int fav_screen = 0;
inline bool favourite = false;
inline std::map<int, VSPWindow*> g_windows;
inline Timer64 screenSwitchTimer;

inline SDL_Texture* screenPreviewFramebuffer = NULL;

void main_renderScaleUp();
void main_renderScaleDown();
void main_switchToFavScreen();
void main_assignFavScreen();
void main_toggleFullscreen();

void main_newWindow(std::string title);