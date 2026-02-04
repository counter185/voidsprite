#include "BaseScreen.h"
#include "multiwindow.h"

void BaseScreen::renderWithBlurPanelsIfEnabled(std::function<void()> renderCode)
{
    if (g_currentWindow->blurBuffer->enabled) {
        g_currentWindow->blurBuffer->pushFullscreenBuffer();
        renderCode();
        g_currentWindow->blurBuffer->renderFullscreenBufferToScreen();
        g_currentWindow->blurBuffer->popAndApplyFullscreenBuffer();
        g_currentWindow->blurBuffer->blurBehindAllPanels(wxsManager.drawablesList);
#if _DEBUG
        if (g_debugConfig.debugBlurBehind) {
            g_currentWindow->blurBuffer->renderBlurBehind(SDL_Rect{ 0, 0, g_windowW, g_windowH });
        }
#endif
    }
    else {
        renderCode();
    }
}
