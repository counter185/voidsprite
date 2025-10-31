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
    collapsed = false;
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

void PanelUserInteractable::setupResizable(XY minDimensions)
{
    resizable = true;
    minResizableSize = minDimensions;
}

void PanelUserInteractable::setupCloseButton(std::function<void()> callback)
{
    UIButton* closeButton = new UIButton("X");
    closeButton->position = { wxWidth - 25, 2 };
    closeButton->wxWidth = 20;
    closeButton->wxHeight = 20;
    closeButton->onClickCallback = [callback](UIButton* btn) {
        callback();
    };
    wxsTarget().addDrawable(closeButton);
}

UILabel* PanelUserInteractable::addTitleText(std::string title)
{
    XY labelPosition = { collapsible ? 30 : 5, 3 };
    UILabel* label = new UILabel(title, labelPosition);
    subWidgets.addDrawable(label);
    return label;
}

void PanelUserInteractable::renderResizeHandle(XY at)
{
    if (PointInResizeRange({ g_mouseX, g_mouseY })) {
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
        SDL_Rect handleRect = SDL_Rect{
            at.x + wxWidth - resizeDistance,
            at.y + wxHeight - resizeDistance,
            resizeDistance*2,
            resizeDistance*2
        };
        SDL_RenderDrawRect(g_rd, &handleRect);
    }
}

PanelUserInteractable::PanelUserInteractable()
{
    fillUnfocused = visualConfigFill("ui/panel/bg_unfocused");
    fillFocused = visualConfigFill("ui/panel/bg_focused");
    borderColor = visualConfigHexU32("ui/panel/border");
}

bool PanelUserInteractable::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
	return Panel::isMouseIn(thisPositionOnScreen, mousePos) || (resizable && PointInResizeRange(mousePos));
}

void PanelUserInteractable::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (!DrawableManager::processInputEventInMultiple({ subWidgets }, evt, gPosOffset)) {
        if (!resizable || !processResize(evt)) {
            if (!defaultInputAction(evt, gPosOffset)) {
                if (draggable) {
                    processDrag(evt);
                }
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

bool PanelUserInteractable::processResize(SDL_Event evt)
{
    evt = convertTouchToMouseEvent(evt);
    switch (evt.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (evt.button.button == SDL_BUTTON_LEFT) {
                if (PointInResizeRange({ g_mouseX, g_mouseY })) {
                    resizing = true;
                    return true;
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            resizing = false;
            return false;
        case SDL_MOUSEMOTION:
            if (resizing && (!collapsible || !collapsed)) {
                wxWidth = ixmin(g_windowW, ixmax(minResizableSize.x, wxWidth + (int)(evt.motion.xrel)));
                wxHeight = ixmin(g_windowH, ixmax(minResizableSize.x, wxHeight + (int)(evt.motion.yrel)));

                tryMoveOutOfOOB();
                return true;
            }
            break;
        /*case SDL_EVENT_FINGER_DOWN:
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
        */
    }
    return false;
}

bool PanelUserInteractable::PointInResizeRange(XY pos)
{
    return xyDistance(pos, xyAdd(position, getDimensions())) < resizeDistance;
}

void PanelUserInteractable::tryMoveOutOfOOB()
{
    XY dim = getDimensions();

    if (position.x < 0) {
        position.x = 0;
    }
    if (position.y < 0) {
        position.y = 0;
    }
    if (position.x + dim.x > g_windowW) {
        position.x = g_windowW - dim.x;
    }
    if (position.y + dim.y > g_windowH) {
        position.y = g_windowH - dim.y;
    }
}

void PanelUserInteractable::toggleCollapse() {
    bool newState = (collapsed = !collapsed);
    collapsePanel->enabled = !newState;
    subWidgets.forceUnfocus();
    focusTimer.start();
    collapseButton->text = newState ? "-" : "+";

    if (draggable) {
        tryMoveOutOfOOB();
    }
}