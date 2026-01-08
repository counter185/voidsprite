#pragma once
inline std::map<int, VSPWindow*> g_windows;
inline Timer64 screenSwitchTimer;
inline SDL_Texture* screenPreviewFramebuffer = NULL;

int main_getLastFPS();

void main_renderScaleUp();
void main_renderScaleDown();
void main_switchToFavScreen();
void main_assignFavScreen();
void main_toggleFullscreen();

void main_newWindow();
void main_currentWorkspaceToNewWindow();
void main_attachCurrentWorkspaceToMainWindow();
void main_promptRenameCurrentWindow();