#include "ExtractDataScreen.h"
#include "UILabel.h"
#include "UIDropdown.h"
#include "UITextField.h"

ExtractDataParametersPanel::ExtractDataParametersPanel(ExtractDataScreen* parent) : caller(parent) {

    wxWidth = 260;
    wxHeight = 240;

    subWidgets.addDrawable(new UILabel(TL("vsp.extractdata.title"), { 2,2 }, 18));

    XY layoutPos = { 5, 30 };

    std::vector<std::pair<std::string, std::string>> formatOptions;
    for (auto& it : pixelReaders) {
        formatOptions.push_back({ it.first, "" });
    }
    UIDropdown* formatDropdown = new UIDropdown(formatOptions);
    formatDropdown->position = layoutPos;
    formatDropdown->setTextToSelectedItem = true;
    formatDropdown->text = caller->getCurrentPixelFormat();
    formatDropdown->onDropdownItemSelectedCallback = [this](UIDropdown*, int, std::string selectedItem) {
        caller->setCurrentPixelFormat(selectedItem);
    };
    subWidgets.addDrawable(formatDropdown);
    layoutPos.y += 30;


    std::vector<std::pair<std::string, std::string>> pixelOrderOptions = {
        { TL("vsp.extractdata.pixelorder.xtheny"), "" },
        { TL("vsp.extractdata.pixelorder.ythenx"), "" },
    };
    UIDropdown* pixelOrderDropdown = new UIDropdown(pixelOrderOptions);
    pixelOrderDropdown->position = layoutPos;
    pixelOrderDropdown->setTextToSelectedItem = true;
    pixelOrderDropdown->text = pixelOrderOptions[caller->getCurrentPixelOrder()].first;
    pixelOrderDropdown->onDropdownItemSelectedCallback = [this](UIDropdown*, int index, std::string sel) {
        caller->setCurrentPixelOrder((PixelOrder)index);
    };
    subWidgets.addDrawable(pixelOrderDropdown);
    layoutPos.y += 30;


    UITextField* offsetField = new UITextField();
    offsetField->isNumericField = true;
    offsetField->setText(std::to_string(caller->getCurrentFileOffset()));
    offsetField->position = layoutPos;
    offsetField->wxWidth = 150;
    offsetField->onTextChangedCallback = [this](UITextField* field, std::string text) {
        u64 newOffset = 0;
        try {
            newOffset = std::stoull(text);
        }
        catch (std::exception&) {}
        caller->setCurrentFileOffset(newOffset);
    };
    subWidgets.addDrawable(offsetField);
    layoutPos.y += 35;

    UITextField* wField = new UITextField();
    wField->isNumericField = true;
    wField->setText(std::to_string(caller->getLayerWidth()));
    wField->position = layoutPos;
    wField->wxWidth = 100;
    wField->onTextChangedCallback = [this](UITextField* field, std::string text) {
        int newWidth = 0;
        try {
            newWidth = std::stoi(text);
        }
        catch (std::exception&) {}
        caller->setLayerWidth(newWidth);
    };
    subWidgets.addDrawable(wField);

    UITextField* hField = new UITextField();
    hField->isNumericField = true;
    hField->setText(std::to_string(caller->getLayerHeight()));
    hField->position = { layoutPos.x + 110, layoutPos.y };
    hField->wxWidth = 100;
    hField->onTextChangedCallback = [this](UITextField* field, std::string text) {
        int newHeight = 0;
        try {
            newHeight = std::stoi(text);
        }
        catch (std::exception&) {}
        caller->setLayerHeight(newHeight);
    };
    subWidgets.addDrawable(hField);
    layoutPos.y += 35;
}

void ExtractDataParametersPanel::render(XY at)
{
    SDL_Rect r = { at.x, at.y, wxWidth, wxHeight };
    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, focused ? 0xa0 : 0x90 };
    SDL_Color colorBG2 = { 0x10, 0x10, 0x10, focused ? 0xa0 : 0x90 };
    renderGradient(r, sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1));
    if (thisOrParentFocused()) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    DraggablePanel::render(at);
}


void ExtractDataScreen::render()
{
    renderBackground();
    c.lockToScreenBounds();
    //todo: multithread this
    if (pixelDataChanged) {
        processLayer();
        pixelDataChanged = false;
    }
    SDL_Rect canvasRect = c.getCanvasOnScreenRect();
    dataLayer->render(canvasRect);

    for (int x = 1; x < 8; x++) {
        SDL_Rect offs = offsetRect(canvasRect, x);
        u8 a = 255 / x;
        SDL_SetRenderDrawColor(g_rd, a, a, a, a);
        SDL_RenderDrawRect(g_rd, &offs);
    }

    XY endpos = c.canvasPointToScreenPoint(onCanvasEndPosition);
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x40);
    drawLine(endpos, xyAdd(endpos, { 10,0 }));
    drawLine(endpos, xyAdd(endpos, { 0,10 }));

    drawLine(xyAdd(endpos, {3,3}), xyAdd(endpos, { 8,3 }));
    drawLine(xyAdd(endpos, {3,3}), xyAdd(endpos, { 3,8 }));

    BaseScreen::render();
}

void ExtractDataScreen::takeInput(SDL_Event evt)
{
    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }

    //LALT_TO_SUMMON_NAVBAR;

    if (evt.type == SDL_DROPFILE) {
        PlatformNativePathString p = convertStringOnWin32(evt.drop.data);
        //todo
        return;
    }

    if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt)) {
        switch (evt.type) {
        case SDL_MOUSEWHEEL:
            c.zoom(evt.wheel.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                pan = evt.button.down;
            }
            break;
        case SDL_MOUSEMOTION:
            if (pan) {
                c.panCanvas(XY{ (int)(evt.motion.xrel), (int)(evt.motion.yrel) });
            }
            break;
        }
    }
}

void ExtractDataScreen::processLayer()
{
    fseek(fileHandle, fileOffset, SEEK_SET);
    dataLayer->fillRect({ 0,0 }, { (int)layerWidth, (int)layerHeight }, 0x00000000);
    XY nextPixelWritePos = { 0,0 };
    u32* ppx = (u32*)dataLayer->pixelData;


    std::function<void(Layer*,XY&)> pixelOrderFn = 
        currentPixelOrder == PO_XthenY ? [](Layer* l, XY& pos) { pos.x++; if (pos.x % l->w == 0) { pos.y++; pos.x = 0; } }
        : currentPixelOrder == PO_YthenX ? [](Layer* l, XY& pos) { pos.y++; if (pos.y % l->h == 0) { pos.x++; pos.y = 0; } }
        : [](Layer* l, XY& pos) {};

    std::function<void(u32)> pushPixelFn = [&](u32 pixel) {
        if (nextPixelWritePos.x < layerWidth && nextPixelWritePos.y < layerHeight) {
            u64 dataPtr = nextPixelWritePos.y * layerWidth + nextPixelWritePos.x;
            ppx[dataPtr] = pixel;
            dataPtr++;
        }
        pixelOrderFn(dataLayer, nextPixelWritePos);
    };
    while (!feof(fileHandle) && pointInBox(nextPixelWritePos, {0,0, (int)layerWidth, (int)layerHeight})) {
        if (pixelReaders.contains(currentPixelFormat)) {
            pixelReaders[currentPixelFormat](fileHandle, pushPixelFn);
        }
    }
    onCanvasEndPosition = nextPixelWritePos;
}

void ExtractDataScreen::renderBackground()
{
    Fill::Gradient(0xFF000000, 0xFF000000, 0xFF000000, 0xFF303030).fill({ 0, 0, g_windowW, g_windowH });
}