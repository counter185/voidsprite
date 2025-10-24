#include "PanelReference.h"
#include "UIButton.h"
#include "UIDropdown.h"
#include "UISlider.h"
#include "UILabel.h"
#include "Layer.h"
#include "maineditor.h"

PanelReference::PanelReference(Layer* t, MainEditor* caller)
{
    previewTex = t;
    this->parent = caller;
    c.dimensions = { previewTex->w, previewTex->h };
    borderColor = visualConfigHexU32("ui/panel/border");

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
    SDL_Event evtc = convertTouchToMouseEvent(evt);
    SDL_Rect canvasDraw = getCanvasDrawRect(gPosOffset);
    if (currentMode == REFERENCE_PIXEL_PERFECT) {
        if (evtc.type == SDL_EVENT_MOUSE_BUTTON_DOWN 
            && !subWidgets.mouseInAny(gPosOffset, { (int)evtc.button.x, (int)evtc.button.y }) 
            && pointInBox({ (int)evtc.button.x, (int)evtc.button.y }, canvasDraw)) {
            dragging++;
        }
        else if (dragging > 0 && evtc.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            dragging = 0;
        }
        else if (dragging && evtc.type == SDL_EVENT_MOUSE_MOTION) {
            c.panCanvas({ (int)evtc.motion.xrel, (int)evtc.motion.yrel });
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
    if (currentMode == REFERENCE_PIXEL_PERFECT) { //pixel-perfect
        g_pushClip(canvasDraw);
        c.lockToScreenBounds(0, 0, 0, 0, { canvasDraw.w, canvasDraw.h });
        SDL_Rect texDraw = c.getCanvasOnScreenRect();
        texDraw.x += at.x;
        texDraw.y += at.y;
        previewTex->render(texDraw);
        g_popClip();
    }
    else if (currentMode == REFERENCE_FIT) {    //fit
        //previewTex->render(fitInside(canvasDraw, {0,0,previewTex->w, previewTex->h}));
        previewTex->render(canvasDraw);
    }

    Panel::render(at);

}

void PanelReference::eventDropdownItemSelected(int evt_id, int index, std::string name)
{
    if (evt_id == 0) {
        currentMode = (ReferencePanelMode)index;
        initWidgets();
    }
}

void PanelReference::initWidgets()
{
    subWidgets.freeAllDrawables();

    if (currentMode == REFERENCE_PIXEL_PERFECT) { //pixel-perfect
        wxWidth = 400;
        wxHeight = 300;
    }
    else if (currentMode == REFERENCE_FIT) {  //fit
        wxWidth = 400;
        wxHeight = (int)(400.0 / previewTex->w * previewTex->h);
    }
    else if (currentMode == REFERENCE_UNDER_CANVAS || currentMode == REFERENCE_OVER_CANVAS) {
        wxWidth = 400;
        wxHeight = 80;

        UISlider* opacitySlider = new UISlider();
        opacitySlider->setValue(0, 1, opacity);
        opacitySlider->onChangeValueCallback = [this](UISlider* target, float v) {
            opacity = v;
        };
        opacitySlider->position = { this->wxWidth / 2, 35 };
        opacitySlider->wxWidth = this->wxWidth / 2 - 5;
        opacitySlider->wxHeight = 20;
        subWidgets.addDrawable(opacitySlider);

        subWidgets.addDrawable(new UILabel(TL("vsp.cmn.opacity"), { 5, 35 }));
    }

    c.recenter({ wxWidth, wxHeight });

    UIButton* closeBtn = new UIButton();
    closeBtn->wxWidth = 30;
    closeBtn->wxHeight = 20;
    closeBtn->position = { wxWidth - 5 - closeBtn->wxWidth, 5 };
    closeBtn->text = "X";
    closeBtn->onClickCallback = [this](UIButton* caller) {
        if (parent != NULL) {
            parent->removeWidget(this);
        }
        else {
            DrawableManager* wxs = getTopmostParent()->parentManager;
            wxs->removeDrawable(getTopmostParent());
        }
    };
    subWidgets.addDrawable(closeBtn);

    std::vector<std::string> modes = {"Pixel-perfect", "Fit", "Under canvas", "Over canvas"};
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
    if (currentMode == REFERENCE_UNDER_CANVAS || currentMode == REFERENCE_OVER_CANVAS) {
        return SDL_Rect{ 0,0,0,0 };
    }
    return SDL_Rect{ at.x + 5, at.y + 35, wxWidth - 10, wxHeight - 40 };
}
