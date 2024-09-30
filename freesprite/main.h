#pragma once
#include "DrawableManager.h"

inline int fav_screen = 0;
inline bool favourite = false;
inline int currentScreen = 0;
inline std::vector<BaseScreen*> screenStack;
inline Timer64 screenSwitchTimer;

inline DrawableManager overlayWidgets;
inline std::vector<ButtonStartScreenSession*> screenButtons;
