#include "PanelSpritesheetPreview.h"
#include "UILabel.h"
#include "UITextField.h"
#include "maineditor.h"
#include "SpritesheetPreviewScreen.h"

PanelSpritesheetPreview::PanelSpritesheetPreview(SpritesheetPreviewScreen* caller) {
    this->caller = caller;

    wxWidth = 320;
    wxHeight = 200;
    borderColor = visualConfigHexU32("ui/panel/border");

    setupDraggable();
    setupCollapsible();
    addTitleText("Preview sprites");

    UILabel* msPerSpriteLabel = new UILabel("MS per sprite");
    msPerSpriteLabel->position = { 5, 40 };
    wxsTarget().addDrawable(msPerSpriteLabel);

    UINumberInputField* textfieldMSPerSprite = new UINumberInputField(&caller->msPerSprite);
    textfieldMSPerSprite->wxWidth = 150;
    textfieldMSPerSprite->position = { 140, 40 };
    textfieldMSPerSprite->validateFunction = [](int v) {return v > 0; };
    wxsTarget().addDrawable(textfieldMSPerSprite);
}

void PanelSpritesheetPreview::render(XY position)
{
    PanelUserInteractable::render(position);

    if (!collapsed) {
        XY previewPos = { 5,90 };
        XY callerPaddedTileSize = caller->caller->getPaddedTileDimensions();
        XY tileSize = {
            caller->canvas.scale * callerPaddedTileSize.x,
            caller->canvas.scale * callerPaddedTileSize.y,
        };
        wxWidth = ixmax(320, previewPos.x + tileSize.x + 10);
        wxHeight = ixmax(200, previewPos.y + tileSize.y + 10);
        caller->drawPreview(xyAdd(position, previewPos));
    }
}
