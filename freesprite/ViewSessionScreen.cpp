#include "ViewSessionScreen.h"
#include "maineditor.h"
#include "ScreenWideNavBar.h"
#include "PopupYesNo.h"
#include "TooltipsLayer.h"

void ViewSessionScreen::setupNavbar()
{
    navbar = new ScreenWideNavBar(this, 
    {
        {SDL_SCANCODE_F, makeNavbarSection(TL("vsp.nav.file"), g_iconNavbarTabFile, {
            {SDL_SCANCODE_X, { TL("vsp.cmn.close"), [this]() { closeThisScreen(); }}}
        })},
        {SDL_SCANCODE_V, makeNavbarSection(TL("vsp.nav.view"), g_iconNavbarTabView, {
            {SDL_SCANCODE_C, { "Recenter canvas", [this]() { c.recenter(); }}}
        })},
    }, { SDL_SCANCODE_F, SDL_SCANCODE_V });
    wxsManager.addDrawable(navbar);
}

ViewSessionScreen::ViewSessionScreen(MainEditor* parent) : caller(parent) {
    c.scale = 1;
    c.dimensions = caller->canvas.dimensions;
    c.recenter();
    this->setupNavbar();
    lastInputTimer.start();
}

TransparentViewSessionScreen::TransparentViewSessionScreen(MainEditor* parent)
    : ViewSessionScreen(parent)
{
    background = Fill::None();
    this->setupNavbar();
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
    for (Layer*& l : caller->getLayerStack()) {
        if (!l->hidden) {
            l->render(canvasRenderRect, l->layerAlpha);
        }
    }

    if (lastInputTimer.elapsedTime() < 2000) {
        BaseScreen::render();
    }
}

void ViewSessionScreen::defaultInputAction(SDL_Event evt) 
{ 
    c.takeInput(evt); 
}

void ViewSessionScreen::resetInputTimerOnInput(SDL_Event evt)
{
    if (evt.type == SDL_EVENT_MOUSE_MOTION
        || evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN
        || evt.type == SDL_EVENT_KEY_DOWN)
    {
        lastInputTimer.start();
    }
}

void TransparentViewSessionScreen::render()
{
    static Fill fillFocused = Fill::ThreePointVerticalGradient(0x40000000, 0x40000000, 0x10000000, 0x10000000, 0x30000000, 0x30000000);
    static Fill fillUnfocused = Fill::None();

    background = (lastInputTimer.elapsedTime() < 2000) ? fillFocused : fillUnfocused;

    ViewSessionScreen::render();

    if (g_windowFocused && parentTransparentWindow->ignoreMouse) {
        g_ttp->addTooltip(Tooltip{ {50,50}, "Press any key to exit Overlay mode." });
    }
}

void TransparentViewSessionScreen::defaultInputAction(SDL_Event evt)
{
    if (!c.takeInput(evt)) {
        switch (evt.type) {
            case SDL_KEYDOWN:
                if (parentTransparentWindow->ignoreMouse) {
                    parentTransparentWindow->setIgnoreMouseInput(false);
                }
                else {
                    if (evt.key.key == SDLK_SPACE) {
                        promptEnableOverlayMode();
                    }
                }
                break;
        }
    }
}

void TransparentViewSessionScreen::setupNavbar()
{
    if (navbar != NULL) {
        wxsManager.removeDrawable(navbar);
    }
    navbar = new ScreenWideNavBar(this,
    {
        {SDL_SCANCODE_F, makeNavbarSection(TL("vsp.nav.file"), g_iconNavbarTabFile, {
            {SDL_SCANCODE_X, { TL("vsp.cmn.close"), [this]() { closeThisScreen(); }}}
        })},
        {SDL_SCANCODE_V, makeNavbarSection(TL("vsp.nav.view"), g_iconNavbarTabView, {
            {SDL_SCANCODE_T, { "Enable Overlay mode", [this]() { promptEnableOverlayMode(); }}},
            {SDL_SCANCODE_C, { "Recenter canvas", [this]() { c.recenter(); }}},
        })},
    }, { SDL_SCANCODE_F, SDL_SCANCODE_V });
    wxsManager.addDrawable(navbar);
}

void TransparentViewSessionScreen::promptEnableOverlayMode()
{
    PopupYesNo* p = new PopupYesNo(
        "Enable Overlay mode?",
#if VSP_PLATFORM == VSP_PLATFORM_WIN32
        "This window will ignore mouse input and be shown above all other windows.\n\n"
        "To exit Overlay mode, click this window on the Taskbar and press any key on the keyboard."
#else
        "This window will be shown above all other windows.\n\n"
        "To exit Overlay mode, switch to this window and press any key on the keyboard."
#endif
    );
    p->onFinishCallback = [this](PopupYesNo* p, bool a) { if (a) { enableOverlayMode(); } };
    g_addPopup(p);
}

void TransparentViewSessionScreen::enableOverlayMode()
{
    parentTransparentWindow->setIgnoreMouseInput(true);
}
