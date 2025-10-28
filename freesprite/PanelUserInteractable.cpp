#include "PanelUserInteractable.h"
#include "UILabel.h"
#include "UIButton.h"

void PanelUserInteractable::drawPanelBackground(XY at)
{
    XY dim = getDimensions();
    SDL_Rect panelRect = { at.x, at.y, dim.x, dim.y };

    (focused ? fillFocused : fillUnfocused).fill(panelRect);
}

void PanelUserInteractable::setupDraggable()
{
    draggable = true;
}

void PanelUserInteractable::setupCollapsible()
{
    collapsible = true;
    Panel* subPanel = new Panel();
    subPanel->position = { 0,0 };
    subWidgets.addDrawable(subPanel);
    widgetsTarget = collapsePanel = subPanel;

    collapseButton = new UIButton();
    collapseButton->position = { 5, 5 };
    collapseButton->wxWidth = 20;
    collapseButton->wxHeight = 20;
    collapseButton->onClickCallback = [this](UIButton* btn) { toggleCollapse(); };
    collapseButton->text = "-";
    subWidgets.addDrawable(collapseButton);
}

UILabel* PanelUserInteractable::addTitleText(std::string title)
{
    XY labelPosition = { collapsible ? 30 : 5, 5 };
    UILabel* label = new UILabel(title, labelPosition);
    wxsTarget().addDrawable(label);
    return label;
}

void PanelUserInteractable::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (!DrawableManager::processInputEventInMultiple({ subWidgets }, evt, gPosOffset)) {
        if (!defaultInputAction(evt, gPosOffset)) {
            if (draggable) {
                processDrag(evt);
            }
        }
    }
}

void PanelUserInteractable::processDrag(SDL_Event evt) {
    switch (evt.type) {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        if (evt.button.button == SDL_BUTTON_LEFT) {
            dragging = evt.button.down;
        }
        break;
    case SDL_MOUSEMOTION:
        if (dragging) {
            wasDragged = true;
            position.x += (int)(evt.motion.xrel);
            position.y += (int)(evt.motion.yrel);

            tryMoveOutOfOOB();
        }
        break;
    case SDL_EVENT_FINGER_DOWN:
    case SDL_EVENT_FINGER_UP:
        dragging = evt.type == SDL_EVENT_FINGER_DOWN;
        break;
    case SDL_EVENT_FINGER_MOTION:
        if (dragging) {
            wasDragged = true;
            position.x += (int)(evt.tfinger.dx * g_windowW);
            position.y += (int)(evt.tfinger.dy * g_windowH);

            tryMoveOutOfOOB();
        }
        break;
    }
}

void PanelUserInteractable::tryMoveOutOfOOB()
{
    if (position.x < 0) {
        position.x = 0;
    }
    if (position.y < 0) {
        position.y = 0;
    }
    if (position.x + wxWidth > g_windowW) {
        position.x = g_windowW - wxWidth;
    }
    if (position.y + wxHeight > g_windowH) {
        position.y = g_windowH - wxHeight;
    }
}

void PanelUserInteractable::toggleCollapse() {
    bool newState = (collapsed = !collapsed);
    collapsePanel->enabled = newState;
    subWidgets.forceUnfocus();
    focusTimer.start();
    collapseButton->text = newState ? "-" : "+";
}