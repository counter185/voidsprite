#include "UIAnchorSelect.h"
#include "UIStackPanel.h"

void UIAnchorSelect::chooseAnchor(Anchor anch)
{
	selectedAnchor = anch;
	updateButtons();
}

void UIAnchorSelect::updateButtons()
{
	static Fill checkedFills[] = {
		Fill::Gradient(0xD0808080, 0xD0000000, 0xD0000000, 0xD0000000),
		Fill::Gradient(0xD0808080, 0xD0808080, 0xD0000000, 0xD0000000),
		Fill::Gradient(0xD0000000, 0xD0808080, 0xD0000000, 0xD0000000),
		Fill::Gradient(0xD0808080, 0xD0000000, 0xD0808080, 0xD0000000),
		Fill::Gradient(0xD0808080, 0xD0808080, 0xD0808080, 0xD0808080),
		Fill::Gradient(0xD0000000, 0xD0808080, 0xD0000000, 0xD0808080),
		Fill::Gradient(0xD0000000, 0xD0000000, 0xD0808080, 0xD0000000),
		Fill::Gradient(0xD0000000, 0xD0000000, 0xD0808080, 0xD0808080),
		Fill::Gradient(0xD0000000, 0xD0000000, 0xD0000000, 0xD0808080),
	};
	static Fill uncheckedFill = FILL_BUTTON_CHECKED_DEFAULT;

	for (UIAnchorButton*& b : anchorButtons) {
		b->fill = b->thisAnchor == selectedAnchor ? checkedFills[selectedAnchor] : uncheckedFill;
	}
}

UIAnchorSelect::UIAnchorSelect()
{
	passThroughMouse = true;

	const int buttonW = 40;
	const int buttonH = 30;

	for (int i = 0; i < 9; i++) {
		UIAnchorButton* b = new UIAnchorButton();
		b->thisAnchor = (Anchor)i;
		b->onClickCallback = [this, i](...) { chooseAnchor((Anchor)i); };
		b->wxWidth = buttonW;
		b->wxHeight = buttonH;
		anchorButtons[i] = b;
	}
	
	subWidgets.addDrawable(UIStackPanel::Vertical(0, {
		UIStackPanel::Horizontal(0, {
			anchorButtons[0], anchorButtons[1], anchorButtons[2]
		}),
		UIStackPanel::Horizontal(0, {
			anchorButtons[3], anchorButtons[4], anchorButtons[5]
		}),
		UIStackPanel::Horizontal(0, {
			anchorButtons[6], anchorButtons[7], anchorButtons[8]
		}),
	}));
	updateButtons();
}

void UIAnchorButton::render(XY at)
{
	UIButton::render(at);

	SDL_Rect areaRect = offsetRect({ at.x,at.y, wxWidth, wxHeight }, -4);

	XY points[] = {
		{areaRect.x, areaRect.y},
		{areaRect.x + areaRect.w/2, areaRect.y},
		{areaRect.x + areaRect.w, areaRect.y},
		{areaRect.x, areaRect.y + areaRect.h/2},
		{areaRect.x + areaRect.w/2, areaRect.y + areaRect.h / 2},
		{areaRect.x + areaRect.w, areaRect.y + areaRect.h / 2},
		{areaRect.x, areaRect.y + areaRect.h},
		{areaRect.x + areaRect.w/2, areaRect.y + areaRect.h},
		{areaRect.x + areaRect.w, areaRect.y + areaRect.h}
	};

	SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xd0);

	switch (thisAnchor) {
		case ANCHOR_TOPLEFT:
			drawLine(points[ANCHOR_TOPLEFT], points[ANCHOR_TOPCENTER]);
			drawLine(points[ANCHOR_TOPLEFT], points[ANCHOR_MIDLEFT]);
			drawLine(points[ANCHOR_TOPLEFT], points[ANCHOR_MIDCENTER]);
			break;
		case ANCHOR_TOPCENTER:
			drawLine(points[ANCHOR_TOPCENTER], points[ANCHOR_TOPLEFT]);
			drawLine(points[ANCHOR_TOPCENTER], points[ANCHOR_TOPRIGHT]);
			drawLine(points[ANCHOR_TOPCENTER], points[ANCHOR_MIDCENTER]);
			break;
		case ANCHOR_TOPRIGHT:
			drawLine(points[ANCHOR_TOPRIGHT], points[ANCHOR_TOPCENTER]);
			drawLine(points[ANCHOR_TOPRIGHT], points[ANCHOR_MIDRIGHT]);
			drawLine(points[ANCHOR_TOPRIGHT], points[ANCHOR_MIDCENTER]);
			break;
		case ANCHOR_MIDLEFT:
			drawLine(points[ANCHOR_MIDLEFT], points[ANCHOR_TOPLEFT]);
			drawLine(points[ANCHOR_MIDLEFT], points[ANCHOR_BOTTOMLEFT]);
			drawLine(points[ANCHOR_MIDLEFT], points[ANCHOR_MIDCENTER]);
			break;
		case ANCHOR_MIDCENTER:
			drawLine(points[ANCHOR_MIDCENTER], points[ANCHOR_MIDLEFT]);
			drawLine(points[ANCHOR_MIDCENTER], points[ANCHOR_MIDRIGHT]);
			drawLine(points[ANCHOR_MIDCENTER], points[ANCHOR_TOPCENTER]);
			drawLine(points[ANCHOR_MIDCENTER], points[ANCHOR_BOTTOMCENTER]);
			break;
		case ANCHOR_MIDRIGHT:
			drawLine(points[ANCHOR_MIDRIGHT], points[ANCHOR_TOPRIGHT]);
			drawLine(points[ANCHOR_MIDRIGHT], points[ANCHOR_BOTTOMRIGHT]);
			drawLine(points[ANCHOR_MIDRIGHT], points[ANCHOR_MIDCENTER]);
			break;
		case ANCHOR_BOTTOMLEFT:
			drawLine(points[ANCHOR_BOTTOMLEFT], points[ANCHOR_MIDLEFT]);
			drawLine(points[ANCHOR_BOTTOMLEFT], points[ANCHOR_BOTTOMCENTER]);
			drawLine(points[ANCHOR_BOTTOMLEFT], points[ANCHOR_MIDCENTER]);
			break;
		case ANCHOR_BOTTOMCENTER:
			drawLine(points[ANCHOR_BOTTOMCENTER], points[ANCHOR_BOTTOMLEFT]);
			drawLine(points[ANCHOR_BOTTOMCENTER], points[ANCHOR_BOTTOMRIGHT]);
			drawLine(points[ANCHOR_BOTTOMCENTER], points[ANCHOR_MIDCENTER]);
			break;
		case ANCHOR_BOTTOMRIGHT:
			drawLine(points[ANCHOR_BOTTOMRIGHT], points[ANCHOR_BOTTOMCENTER]);
			drawLine(points[ANCHOR_BOTTOMRIGHT], points[ANCHOR_MIDRIGHT]);
			drawLine(points[ANCHOR_BOTTOMRIGHT], points[ANCHOR_MIDCENTER]);
			break;
	}
}
