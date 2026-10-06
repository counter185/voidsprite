#pragma once
inline std::map<int, VSPWindow*> g_windows;
inline Timer64 screenSwitchTimer;
inline SDL_Texture* screenPreviewFramebuffer = NULL;
inline int lastFrameCount = 0;

int main_getLastFPS();
bool main_anyPopupsOpen();
void main_handleIPCMessage(std::string msg);

void main_renderScaleUp();
void main_renderScaleDown();
void main_switchToFavScreen();
void main_assignFavScreen();
void main_toggleFullscreen();

void main_newWindow();
VSPWindow* main_newSingleScreenTransparentWindow(BaseScreen* screen);
void main_currentWorkspaceToNewWindow();
void main_attachCurrentWorkspaceToMainWindow();
void main_promptRenameCurrentWindow();
void main_updateFramelessWindows();