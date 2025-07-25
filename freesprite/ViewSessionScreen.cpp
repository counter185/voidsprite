#include "ViewSessionScreen.h"
#include "maineditor.h"

ViewSessionScreen::ViewSessionScreen(MainEditor* parent) : caller(parent) {
	c.scale = 1;
	c.dimensions = caller->canvas.dimensions;
	c.recenter();
}

BaseScreen* ViewSessionScreen::isSubscreenOf()
{
	return caller;
}

void ViewSessionScreen::render()
{
	c.dimensions = caller->canvas.dimensions;
	c.lockToScreenBounds(0, 0, 0, 0);

	background.fill({ 0,0,g_windowW, g_windowH });

	SDL_Rect canvasRenderRect = c.getCanvasOnScreenRect();
	for (Layer*& l : caller->layers) {
		l->render(canvasRenderRect, l->layerAlpha);
	}
}

void ViewSessionScreen::takeInput(SDL_Event evt)
{
    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }

    //LALT_TO_SUMMON_NAVBAR;

    if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt)) {
        switch (evt.type) {
        case SDL_MOUSEBUTTONDOWN:
            scrollingCanvas = true;
            break;
        case SDL_MOUSEBUTTONUP:
            scrollingCanvas = false;
            break;
        case SDL_MOUSEMOTION:
            if (scrollingCanvas) {
                c.panCanvas(XY{ (int)(evt.motion.xrel), (int)(evt.motion.yrel) });
            }
            break;
        case SDL_MOUSEWHEEL:
            c.zoom(evt.wheel.y);
            break;
        }
    }
}
