
#include "PanelRPG2KTilemapPreview.h"
#if VSP_USE_LIBLCF
#include "UIButton.h"
#include "UILayerButton.h"

PanelRPG2KTilemapPreview::PanelRPG2KTilemapPreview(RPG2KTilemapPreviewScreen* caller)
{
    this->caller = caller;

    wxWidth = 280;
    wxHeight = 180;

    btnLL = new UILayerButton("Lower layer", NULL);
    btnLL->position = XY{ 5, 100 };
    btnLL->setCallbackListener(0, this);
    subWidgets.addDrawable(btnLL);

    btnUL = new UILayerButton("Upper layer", NULL);
    btnUL->position = XY{ 5, 65 };
    btnUL->setCallbackListener(1, this);
    subWidgets.addDrawable(btnUL);

    btnEL = new UILayerButton("Event layer", NULL);
    btnEL->position = XY{ 5, 30 };
    btnEL->setCallbackListener(2, this);
    subWidgets.addDrawable(btnEL);
}

void PanelRPG2KTilemapPreview::render(XY position)
{
    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    //SDL_SetRenderDrawColor(g_rd, 0x30, 0x30, 0x30, focused ? 0x80 : 0x30);
    //SDL_RenderFillRect(g_rd, &r);

    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, focused ? 0xa0 : 0x90 };
    SDL_Color colorBG2 = { 0x10, 0x10, 0x10, focused ? 0xa0 : 0x90 };
    renderGradient(r, sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1));
    if (focused) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    DraggablePanel::render(position);
}

void PanelRPG2KTilemapPreview::eventGeneric(int evt_id, int data1, int data2)
{
    if (data1 == 0) {
        //switch active layer to evt_id
    }
    else if (data1 == 1) {
        switch (evt_id) {
            case 0:
                caller->rdLowerLayer = !caller->rdLowerLayer;
                btnLL->hideButton->fill = caller->rdLowerLayer ? SDL_Color{ 0,0,0,0xd0 } : SDL_Color{ 0xff,0xff,0xff, 0xa0 };
                break;
            case 1:
                caller->rdUpperLayer = !caller->rdUpperLayer;
                btnUL->hideButton->fill = caller->rdUpperLayer ? SDL_Color{ 0,0,0,0xd0 } : SDL_Color{ 0xff,0xff,0xff, 0xa0 };
                break;
            case 2:
                caller->rdEventLayer = !caller->rdEventLayer;
                btnEL->hideButton->fill = caller->rdEventLayer ? SDL_Color{ 0,0,0,0xd0 } : SDL_Color{ 0xff,0xff,0xff, 0xa0 };
                break;
        }
    }
}

#endif