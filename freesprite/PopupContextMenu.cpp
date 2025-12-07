#include "PopupContextMenu.h"
#include "UIButton.h"
#include "Panel.h"

PopupContextMenu::PopupContextMenu(std::vector<NamedOperation> actions)
{
    usesWholeScreen = true;
    originPoint = { g_mouseX, g_mouseY };
    itemsPanel = new Panel();
    itemsPanel->position = originPoint;
    wxsManager.addDrawable(itemsPanel);
    wxsManager.forceFocusOn(itemsPanel);

    std::vector<UIButton*> genButtons;

    for (NamedOperation& action : actions) {
        UIButton* button = new UIButton();
        button->text = action.name;
        button->onClickCallback = [this, action](UIButton*) {
            action.function();
        };
        button->position = { 0,0 };
        genButtons.push_back(button);
    }
    addButtons(genButtons);
}

PopupContextMenu::PopupContextMenu(std::vector<UIButton*> actions)
{
    usesWholeScreen = true;
    itemsPanel = new Panel();
    setContextMenuOrigin({ g_mouseX, g_mouseY });
    wxsManager.addDrawable(itemsPanel);
    wxsManager.forceFocusOn(itemsPanel);
    addButtons(actions);
}

void PopupContextMenu::addButtons(std::vector<UIButton*> actions) {
    for (auto& a : actions) {
        auto wrappedCallback = a->onClickCallback;
        a->onClickCallback = [this, wrappedCallback](UIButton* btn) {
            if (wrappedCallback != NULL) {
                wrappedCallback(btn);
            }
            closePopup();
        };
        itemsPanel->subWidgets.addDrawable(a);
        items.push_back(a);
    }
}

void PopupContextMenu::tick()
{
    if (!wxsManager.anyFocused()) {
        finish();
        return;
    }

    double animTimer = XM1PW3P1(startTimer.percentElapsedTime(500));
    int lastEndpointY = 0;
    for (int x = 0; x < items.size(); x++) {
        Drawable* item = items[x];
        XY targetPosition = { 0, lastEndpointY };
        XY itemDimensions = item->getRenderDimensions();
        contentSize.x = ixmax(contentSize.x, itemDimensions.x);
        lastEndpointY += itemDimensions.y;

        item->position = { 0, (int)(animTimer * targetPosition.y) };
    }
    contentSize.y = lastEndpointY;

    if (!scrollable) {
        XY epPos = xyAdd(itemsPanel->position, contentSize);
        if (epPos.x > g_windowW) {
            itemsPanel->position.x = g_windowW - contentSize.x;
        }
        if (epPos.y > g_windowH) {
            itemsPanel->position.y = g_windowH - contentSize.y;
        }
        originPoint = itemsPanel->position;
    }
}

void PopupContextMenu::takeInput(SDL_Event evt)
{
    if (evt.type == SDL_EVENT_MOUSE_WHEEL) {
        if (scrollable) {
            int scrollAmount = evt.wheel.y * 30;
            itemsPanel->position = {
                itemsPanel->position.x,
                iclamp(scrollMin.y, itemsPanel->position.y + scrollAmount, scrollMax.y)
            };
        }
    }
    else if (evt.type == SDL_EVENT_FINGER_MOTION) {
        if (scrollable) {
            XY motionPos = { (int)(evt.tfinger.x * g_windowW), (int)(evt.tfinger.y * g_windowH) };
            XY motionDir = { (int)(evt.tfinger.dx * g_windowW), (int)(evt.tfinger.dy * g_windowH) };
            itemsPanel->position = {
                itemsPanel->position.x,
                iclamp(scrollMin.y, itemsPanel->position.y + motionDir.y, scrollMax.y)
            };
        }
    }
    else {
        BasePopup::takeInput(evt);
    }
}

void PopupContextMenu::defaultInputAction(SDL_Event evt) {
    if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN || evt.type == SDL_EVENT_FINGER_DOWN) {
        finish();
    }
}

void PopupContextMenu::playPopupCloseVFX()
{
    g_newVFX(VFX_POPUPCLOSE, 300, 0xFF000000, SDL_Rect{ originPoint.x, originPoint.y, contentSize.x, contentSize.y });
}

void PopupContextMenu::renderContextMenuBackground()
{
    double animTimer = XM1PW3P1(startTimer.percentElapsedTime(500));
    XY pleft = { 0, originPoint.y };
    XY pright = { g_windowW, originPoint.y };
    int upperY = originPoint.y - (originPoint.y * animTimer);
    int lowerY = originPoint.y + (g_windowH - originPoint.y) * animTimer;

    SDL_Rect upperRect = { 0, upperY, g_windowW, originPoint.y - upperY };
    SDL_Rect lowerRect = { 0, originPoint.y, g_windowW, lowerY - originPoint.y };

    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xD0);
    SDL_RenderFillRect(g_rd, &upperRect);
    SDL_RenderFillRect(g_rd, &lowerRect);
}

XY PopupContextMenu::getContentSize()
{
    XY ret = { 0,0 };
    for (auto& x : items) {
        XY itemDimensions = x->getRenderDimensions();
        ret.x = ixmax(ret.x, itemDimensions.x);
        ret.y += itemDimensions.y;
    }
    return ret;
}

void PopupContextMenu::setContextMenuOrigin(XY screenPos)
{
    XY contentSizeNow = getContentSize();
    originPoint = screenPos;
    XY itemsPanelPos = originPoint;
    if (itemsPanelPos.y + contentSizeNow.y > g_windowH) {
        itemsPanelPos.y = g_windowH - contentSizeNow.y;
    }
    if (itemsPanelPos.y < 0) {
        itemsPanelPos.y = 0;
    }
    itemsPanel->position = itemsPanelPos;
    scrollMax = originPoint;
    scrollMin = xySubtract(originPoint, { 0, contentSizeNow.y});
}
