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

    UILabel* titleLabel = new UILabel("Preview sprites");
    titleLabel->position = { 5, 2 };
    subWidgets.addDrawable(titleLabel);

	msPerSpriteLabel = new UILabel("MS per sprite");
    msPerSpriteLabel->position = { 5, 40 };
	subWidgets.addDrawable(msPerSpriteLabel);

	textfieldMSPerSprite = new UITextField();
	textfieldMSPerSprite->setText(std::to_string(caller->msPerSprite));
	textfieldMSPerSprite->isNumericField = true;
	textfieldMSPerSprite->wxWidth = 150;
    textfieldMSPerSprite->position = { 140, 40 };
	textfieldMSPerSprite->setCallbackListener(EVENT_SPRITEPREVIEW_SET_SPRITE_TICK, this);
    subWidgets.addDrawable(textfieldMSPerSprite);
}

void PanelSpritesheetPreview::render(XY position)
{
    XY previewPos = { 5,90 };
    XY callerPaddedTileSize = caller->caller->getPaddedTileDimensions();
    XY tileSize = { 
        caller->canvas.scale * callerPaddedTileSize.x,
        caller->canvas.scale * callerPaddedTileSize.y,
    };
    wxWidth = ixmax(320, previewPos.x + tileSize.x + 10);
    wxHeight = ixmax(200, previewPos.y + tileSize.y + 10);

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    //SDL_SetRenderDrawColor(g_rd, 0x30, 0x30, 0x30, focused ? 0x80 : 0x30);
    //SDL_RenderFillRect(g_rd, &r);

    u32 colorBG1 = PackRGBAtoARGB( 0x30, 0x30, 0x30, focused ? 0xa0 : 0x90 );
    u32 colorBG2 = PackRGBAtoARGB( 0x10, 0x10, 0x10, focused ? 0xa0 : 0x90 );
    renderGradient(r, colorBG2, colorBG1, colorBG1, colorBG1);
    if (focused) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    caller->drawPreview(xyAdd(position, previewPos));

    DraggablePanel::render(position);
}

void PanelSpritesheetPreview::eventTextInput(int evt_id, std::string data)
{
    if (evt_id == EVENT_SPRITEPREVIEW_SET_SPRITE_TICK) {
        try {
            caller->msPerSprite = std::stoi(data);
            textfieldMSPerSprite->bgColor = { 0,0,0,0xff };
        }
        catch (std::exception&) {
            caller->msPerSprite = -1;
            textfieldMSPerSprite->bgColor = { 80,0,0,0xff };
        }
    }
}
