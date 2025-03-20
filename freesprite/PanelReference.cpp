#include "PanelReference.h"
#include "UIButton.h"
#include "UIDropdown.h"

PanelReference::PanelReference(Layer* t)
{
    previewTex = t;
    c.dimensions = { previewTex->w, previewTex->h };

    initWidgets();
}

PanelReference::~PanelReference()
{
    if (previewTex != NULL) {
        delete previewTex;
    }
}

bool PanelReference::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
    SDL_Rect canvasDraw = getCanvasDrawRect(thisPositionOnScreen);
    return Panel::isMouseIn(thisPositionOnScreen, mousePos) || (enabled && pointInBox(mousePos, canvasDraw));
}

void PanelReference::handleInput(SDL_Event evt, XY gPosOffset)
{
    SDL_Rect canvasDraw = getCanvasDrawRect(gPosOffset);
    if (currentMode == 0) {
        if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN && !subWidgets.mouseInAny(gPosOffset, { (int)evt.button.x, (int)evt.button.y }) && pointInBox({ (int)evt.button.x, (int)evt.button.y }, canvasDraw)) {
            dragging++;
        }
        else if (dragging > 0 && evt.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            dragging = 0;
        }
        else if (dragging && evt.type == SDL_EVENT_MOUSE_MOTION) {
            c.panCanvas({ (int)evt.motion.xrel, (int)evt.motion.yrel });
        }
        else {
            Panel::handleInput(evt, gPosOffset);
        }
    }
    else {
        dragging = 0;
        Panel::handleInput(evt, gPosOffset);
    }
}

void PanelReference::render(XY at)
{
    if (!enabled) {
        return;
    }
    
    SDL_Rect panelRect = { at.x, at.y, wxWidth, wxHeight };

    renderGradient(panelRect, 0x80000000, 0x80000000, 0x80000000, 0x80404040);
    
    if (thisOrParentFocused()) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ at.x, at.y }, { at.x, at.y + wxHeight }, XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300)));
        drawLine({ at.x, at.y }, { at.x + wxWidth, at.y }, XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300)));
    }

    SDL_Rect canvasDraw = getCanvasDrawRect(at);
    if (currentMode == 0) { //pixel-perfect
        g_pushClip(canvasDraw);
        c.lockToScreenBounds(0, 0, 0, 0, { canvasDraw.w, canvasDraw.h });
        SDL_Rect texDraw = c.getCanvasOnScreenRect();
        texDraw.x += at.x;
        texDraw.y += at.y;
        previewTex->render(texDraw);
        g_popClip();
    }
    else if (currentMode == 1) {    //fit
        previewTex->render(canvasDraw);
    }

    Panel::render(at);

}

void PanelReference::eventDropdownItemSelected(int evt_id, int index, std::string name)
{
    if (evt_id == 0) {
        currentMode = index;
        initWidgets();
    }
}

void PanelReference::initWidgets()
{
    subWidgets.freeAllDrawables();

    if (currentMode == 0) { //pixel-perfect
        wxWidth = 400;
        wxHeight = 300;
    }
    else if (currentMode == 1) {  //fit
        wxWidth = 400;
        wxHeight = (int)(400.0 / previewTex->w * previewTex->h);
    }

    c.recenter({ wxWidth, wxHeight });

    UIButton* closeBtn = new UIButton();
    closeBtn->wxWidth = 30;
    closeBtn->wxHeight = 20;
    closeBtn->position = { wxWidth - 5 - closeBtn->wxWidth, 5 };
    closeBtn->text = "X";
    closeBtn->onClickCallback = [&](UIButton* caller) {
        DrawableManager* wxs = getTopmostParent()->parentManager;
        wxs->removeDrawable(getTopmostParent());
        };
    subWidgets.addDrawable(closeBtn);

    std::vector<std::string> modes = {"Pixel-perfect", "Fit"};
    UIDropdown* modeSwitch = new UIDropdown(modes);
    modeSwitch->text = modes[currentMode];
    modeSwitch->lastClick.start();
    modeSwitch->wxWidth = 170;
    modeSwitch->position = { wxWidth - 40 - modeSwitch->wxWidth, 1 };
    modeSwitch->setCallbackListener(0, this);
    subWidgets.addDrawable(modeSwitch);
}

SDL_Rect PanelReference::getCanvasDrawRect(XY at)
{
    return SDL_Rect{ at.x + 5, at.y + 35, wxWidth - 10, wxHeight - 40 };
}
