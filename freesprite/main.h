#pragma once
#include "DrawableManager.h"

inline int fav_screen = 0;
inline bool favourite = false;
inline int currentScreen = 0;
inline std::vector<BaseScreen*> screenStack;
inline std::vector<BasePopup*> popupStack;
inline Timer64 screenSwitchTimer;

inline DrawableManager overlayWidgets;
inline std::vector<ButtonStartScreenSession*> screenButtons;

inline SDL_Texture* screenPreviewFramebuffer = NULL;

void main_updateViewportScaler();
void main_renderScaleUp();
void main_renderScaleDown();
void main_switchToFavScreen();
void main_assignFavScreen();
void main_switchScreenLeft();
void main_switchScreenRight();
void main_toggleFullscreen();