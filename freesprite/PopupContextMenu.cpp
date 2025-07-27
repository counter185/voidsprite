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

	int i = 0;
	for (NamedOperation& action : actions) {
		UIButton* button = new UIButton();
		button->text = action.name;
		button->onClickCallback = [this, action](UIButton*) {
			action.function();
			closePopup();
		};
		button->position = { 0,0 };
		itemsPanel->subWidgets.addDrawable(button);
		items.push_back(button);
	}

}

void PopupContextMenu::tick()
{
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

	XY epPos = xyAdd(itemsPanel->position, contentSize);
	if (epPos.x > g_windowW) {
		itemsPanel->position.x = g_windowW - contentSize.x;
	}
	if (epPos.y > g_windowH) {
		itemsPanel->position.y = g_windowH - contentSize.y;
	}
	originPoint = itemsPanel->position;
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
