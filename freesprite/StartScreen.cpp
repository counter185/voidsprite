#include "StartScreen.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "FileIO.h"
#include "PopupMessageBox.h"
#include "Notification.h"
#include "CustomTemplate.h"

void StartScreen::tick() {
    if (closeNextTick) {
        g_closeScreen(this);
    }
}

void StartScreen::render()
{
    renderBackground();

    SDL_Rect logoRect = SDL_Rect{ 4, g_windowH - 4 - 40 * 4, 128 * 4, 40 * 4 };
    SDL_RenderCopy(g_rd, g_mainlogo, NULL, &logoRect);
    g_fnt->RenderString(std::format("alpha@{}", __DATE__), 2, g_windowH - 20 - 20, SDL_Color{255,255,255,0x50});

    SDL_Rect bgr = SDL_Rect{ 0, 35, 560, 300 };
    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, 0xa0};
    SDL_Color colorBG2 = { 0x20, 0x20, 0x20, 0xa0};
    SDL_Color colorBG3 = { 0x10, 0x10, 0x10, 0xa0 };
    renderGradient(bgr, sdlcolorToUint32(colorBG3), sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1));
    /*if (focused) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }*/

    g_fnt->RenderString("voidsprite", 10, 40);

    g_fnt->RenderString("New image", 10, 80);

    bgr.x += bgr.w + 10;
    bgr.y += 40;
    bgr.h -= 40;
    renderGradient(bgr, sdlcolorToUint32(colorBG3), 0, sdlcolorToUint32(colorBG2), 0);

    wxsManager.renderAll();

    renderStartupAnim();
}

void StartScreen::takeInput(SDL_Event evt)
{
    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }

    LALT_TO_SUMMON_NAVBAR;

    if (evt.type == SDL_DROPFILE) {
        std::string filePath = evt.drop.data;
        tryLoadFile(filePath);
        return;
    }

    if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt)) {
        switch (evt.type) {
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (evt.button.button == SDL_BUTTON_LEFT) {
                    if (evt.button.down) {
                        SDL_Rect logoRect = SDL_Rect{ 4, g_windowH - 4 - 40 * 4, 128 * 4, 40 * 4 };
                        if (pointInBox({ (int)evt.button.x, (int)evt.button.y }, logoRect)) {
                            g_addNotification(Notification("voidsprite alpha", "by counter185 & contributors", 6000, g_iconNotifTheCreature, COLOR_INFO));
                        }
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                break;
            case SDL_MOUSEWHEEL:
                break;
            case SDL_KEYDOWN:
                if (evt.key.scancode == SDL_SCANCODE_V && g_ctrlModifier) {
                    tryOpenImageFromClipboard();
                }
                break;
        }
    }
}

void StartScreen::eventButtonPressed(int evt_id) {
    if (evt_id == 4) {
        switch (newImageTabs->openTab) {
        case 0:
            if (!tab0TextFieldW->textEmpty() && !tab0TextFieldH->textEmpty()) {
                try {
                    int newImgW = std::stoi(tab0TextFieldW->getText());
                    int newImgH = std::stoi(tab0TextFieldH->getText());
                    g_addScreen(new MainEditor(XY{ newImgW, newImgH }));
                }
                catch (std::out_of_range) {
                    g_addNotification(ErrorNotification("Error starting editor", "Invalid dimensions. Number out of range."));
                }
            }
            else {
                g_addNotification(ErrorNotification("Error starting editor", "Input the canvas dimensions."));
            }
            break;
        case 1:
            if (!tab1TextFieldCH->textEmpty() && !tab1TextFieldCW->textEmpty()
                && !tab1TextFieldCHX->textEmpty() && !tab1TextFieldCWX->textEmpty()) {
                try {
                    XY cellSize = XY{ std::stoi(tab1TextFieldCW->getText()) , std::stoi(tab1TextFieldCH->getText()) };
                    int newImgW = cellSize.x * std::stoi(tab1TextFieldCWX->getText());
                    int newImgH = cellSize.y * std::stoi(tab1TextFieldCHX->getText());
                    MainEditor* newMainEditor = new MainEditor(XY{ newImgW, newImgH });
                    newMainEditor->tileDimensions = cellSize;
                    g_addScreen(newMainEditor);
                }
                catch (std::out_of_range) {
                    g_addNotification(ErrorNotification("Error starting editor", "Invalid dimensions. Number out of range."));
                }
            }
            else {
                g_addNotification(ErrorNotification("Error starting editor", "Input the canvas dimensions."));
            }
            break;
        }
    }
    if (evt_id == 5) {
        switch (newImageTabs->openTab) {
        case 0:
            if (!tab0TextFieldW->textEmpty() && !tab0TextFieldH->textEmpty()) {
                try {
                    int newImgW = std::stoi(tab0TextFieldW->getText());
                    int newImgH = std::stoi(tab0TextFieldH->getText());
                    g_addScreen(new MainEditorPalettized(XY{ newImgW, newImgH }));
                }
                catch (std::out_of_range) {
                    g_addNotification(ErrorNotification("Error starting editor", "Invalid dimensions. Number out of range."));
                }
            }
            else {
                g_addNotification(ErrorNotification("Error starting editor", "Input the canvas dimensions."));
            }
            break;
        case 1:
            if (!tab1TextFieldCH->textEmpty() && !tab1TextFieldCW->textEmpty()
                && !tab1TextFieldCHX->textEmpty() && !tab1TextFieldCWX->textEmpty()) {
                try {
                    XY cellSize = XY{ std::stoi(tab1TextFieldCW->getText()) , std::stoi(tab1TextFieldCH->getText()) };
                    int newImgW = cellSize.x * std::stoi(tab1TextFieldCWX->getText());
                    int newImgH = cellSize.y * std::stoi(tab1TextFieldCHX->getText());
                    MainEditorPalettized* newMainEditor = new MainEditorPalettized(XY{ newImgW, newImgH });
                    newMainEditor->tileDimensions = cellSize;
                    g_addScreen(newMainEditor);
                }
                catch (std::out_of_range) {
                    g_addNotification(ErrorNotification("Error starting editor", "Invalid dimensions. Number out of range."));
                }
            }
            else {
                g_addNotification(ErrorNotification("Error starting editor", "Input the canvas dimensions."));
            }
            break;
        }
    }
    else if (evt_id >= 20) {
        int index = evt_id - 20;
        if (index < g_config.lastOpenFiles.size()) {
            tryLoadFile(g_config.lastOpenFiles[index]);
        }
    }
}

void StartScreen::eventFileSaved(int evt_id, PlatformNativePathString name, int importerIndex)
{
    if (evt_id == 0) {
        g_addScreen(new SplitSessionEditor(name));
    }
}

void StartScreen::eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex) {
    //wprintf(L"path: %s, index: %i\n", name.c_str(), importerIndex);
    importerIndex--;
    FileImporter* importer = g_fileImporters[importerIndex];
    void* importedData = importer->importData(name);
    if (importedData != NULL) {
        if (!importer->importsWholeSession()) {
            Layer* nlayer = (Layer*)importedData;
            g_addScreen(!nlayer->isPalettized ? new MainEditor(nlayer) : new MainEditorPalettized((LayerPalettized*)nlayer));
        }
        else {
            MainEditor* session = (MainEditor*)importedData;
            g_addScreen(session);
        }
    }
    else {
        g_addNotification(ErrorNotification("Error", "Failed to load file"));
    }
}

void StartScreen::eventDropdownItemSelected(int evt_id, int index, std::string name)
{
    if (evt_id == EVENT_STARTSCREEN_TEMPLATEPICKED) {
        BaseTemplate* templ = g_templates[index];
        MainEditor* newMainEditor = new MainEditor(templ->generate());
        std::vector<CommentData> templateComments = templ->placeComments();
        for (CommentData& comment : templateComments) {
            newMainEditor->comments.push_back(comment);
        }
        newMainEditor->tileDimensions = g_templates[index]->tileSize();
        newMainEditor->tileGridPaddingBottomRight = g_templates[index]->tilePadding();
        g_addScreen(newMainEditor);
    }
}

void StartScreen::populateLastOpenFiles()
{
    lastOpenFilesPanel->subWidgets.freeAllDrawables();

    UILabel* lbl = new UILabel();
    lbl->text = "Last open files";
    lbl->position = {5, 2};
    lastOpenFilesPanel->subWidgets.addDrawable(lbl);

    XY origin = { 10, 35 };

    int i = 0;
    for (std::string& lastPath : g_config.lastOpenFiles) {
        UIButton* button = new UIButton();
        button->position = origin;
        button->tooltip = lastPath;
        std::string s = lastPath;
        bool ellipsis = false;
        const int maxW = 600;
        while (s.size() > 3 && g_fnt->StatStringDimensions(s).x > maxW) {
            s = s.substr(1);
            ellipsis = true;
        }
        if (ellipsis) {
            s = "..." + s;
        }
        XY textDim = xyAdd({10, 0}, g_fnt->StatStringDimensions(s));
        button->text = s;
        button->wxWidth = textDim.x;
        button->setCallbackListener((i++) + 20, this);
        button->fill = Fill::Gradient(0x60000000, 0x60000000, 0x90909090, 0x90000000);
        origin.y += 30;
        lastOpenFilesPanel->subWidgets.addDrawable(button);
    }
}

void StartScreen::renderStartupAnim()
{
    if (startupAnimTimer.started) {
        double textAnimTime = startupAnimTimer.percentElapsedTime(4000, 200);
        double animTime = startupAnimTimer.percentElapsedTime(1300, 200);
        if (textAnimTime > 1.0) {
            startupAnimTimer.stop();
        }
        else {
            SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 255 * (1.0 - animTime));
            SDL_Rect bgRect = { 0, 0, g_windowW, g_windowH * (1.0 - XM1PW3P1(animTime)) };
            SDL_RenderFillRect(g_rd, &bgRect);
            //SDL_RenderFillRect(g_rd, NULL);

            if (animTime < 0.5) {
                SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
                drawLine({ 0, 30 }, { g_windowW, 30 }, XM1PW3P1(animTime / 0.5));
            }
            else {
                SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
                drawLine({ g_windowW, 30 }, { 0, 30 }, 1.0-XM1PW3P1((animTime - 0.5) / 0.5));
            }

            if (textAnimTime > 0.05) {
                std::string s = g_configWasLoaded ? "Welcome back" : "Welcome to voidsprite";
                XY fd = g_fnt->StatStringDimensions(s);
                g_fnt->RenderString(s, g_windowW - fd.x - 2, 0, { 255,255,255, (u8)(255 * (1.0 - XM1PW3P1((textAnimTime - 0.05) / 0.95))) });
            }
        }
    }
}

void StartScreen::renderBackground()
{
    uint32_t colorBG1 = 0xFF000000;//| (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x000000 : 0xDFDFDF);
    uint32_t colorBG2 = 0xFF000000 | 0x202020;//| (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x202020 : 0x808080);
    renderGradient({ 0,0, g_windowW, g_windowH }, colorBG1, colorBG1, colorBG1, colorBG2);

    struct StartScreenEffect {
        int type;
        XY pos;
        Timer64 timer;
    };
    static std::vector<StartScreenEffect> effects;

    if (!g_config.animatedBackground) {
        return;
    }

    while (effects.size() < 15) {
        StartScreenEffect e;
        e.type = rand() % 7;
        e.pos = { rand() % 960, rand() % 960 };
        e.timer.start();
        effects.push_back(e);
    }

    for (int x = 0; x < effects.size(); x++) {
        bool remove = false;
        XY normPosition = { g_windowW * (effects[x].pos.x / 960.0), g_windowH * (effects[x].pos.y / 960.0) };
        switch (effects[x].type) {
            case 0:
            {
                //long 2s line
                remove = effects[x].timer.percentElapsedTime(2000) == 1.0;
                XY normPosition2 = { g_windowW * ((effects[x].pos.x + 120) / 960.0), g_windowH * ((effects[x].pos.y - 120) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30 * (1.0 - effects[x].timer.percentElapsedTime(2000)));
                drawLine(normPosition, normPosition2, 1.0);
            }
            break;
            case 1:
            {
                //long 0.7s trail
                remove = effects[x].timer.percentElapsedTime(1000) == 1.0;
                XY normPosition2 = { g_windowW * ((effects[x].pos.x + 500) / 960.0), g_windowH * ((effects[x].pos.y - 500) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30 - 0x20 * effects[x].timer.percentElapsedTime(700));
                drawLine(normPosition, normPosition2, 1.0-XM1PW3P1(effects[x].timer.percentElapsedTime(700)));
            }
                break;
            case 2:
            {
                //short 3s line
                remove = effects[x].timer.percentElapsedTime(3000) == 1.0;
                XY normPosition2 = { g_windowW * ((effects[x].pos.x + 40) / 960.0), g_windowH * ((effects[x].pos.y - 40) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x20 * (1.0 - effects[x].timer.percentElapsedTime(3000)));
                drawLine(normPosition, normPosition2, 1.0);
            }
                break;
            case 3:
            {
                //mid length 2.5s line
                remove = effects[x].timer.percentElapsedTime(2500) == 1.0;
                XY normPosition2 = { g_windowW * ((effects[x].pos.x + 90) / 960.0), g_windowH * ((effects[x].pos.y - 90) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x20 * (1.0 - effects[x].timer.percentElapsedTime(2500)));
                drawLine(normPosition, normPosition2, 1.0);
            }
                break;
            case 4:
            {
                //short 1s star
                remove = effects[x].timer.percentElapsedTime(1000) == 1.0;
                XY normPosition2 = { g_windowW * ((effects[x].pos.x + 15) / 960.0), g_windowH * ((effects[x].pos.y - 15) / 960.0) };
                XY normPosition3 = { g_windowW * ((effects[x].pos.x - 15) / 960.0), g_windowH * ((effects[x].pos.y + 15) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x50 - 0x50 * effects[x].timer.percentElapsedTime(1000));
                drawLine(normPosition, normPosition2, 1.0 - XM1PW3P1(effects[x].timer.percentElapsedTime(1000)));
                drawLine(normPosition, normPosition3, 1.0 - XM1PW3P1(effects[x].timer.percentElapsedTime(1000)));
            }
                break;
            case 5:
            {
                //short 0.6s diamond
                remove = effects[x].timer.percentElapsedTime(600) == 1.0;
                XY posLeft = normPosition;
                XY posRight = { g_windowW * ((effects[x].pos.x + 30) / 960.0), g_windowH * ((effects[x].pos.y) / 960.0) };
                XY posUp = { g_windowW * ((effects[x].pos.x + 15) / 960.0), g_windowH * ((effects[x].pos.y - 12) / 960.0) };
                XY posDown = { g_windowW * ((effects[x].pos.x + 15) / 960.0), g_windowH * ((effects[x].pos.y + 12) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x24 - 0x24 * effects[x].timer.percentElapsedTime(600));
                drawLine(posLeft, posUp, 1.0 - XM1PW3P1(effects[x].timer.percentElapsedTime(600)));
                drawLine(posLeft, posDown, 1.0 - XM1PW3P1(effects[x].timer.percentElapsedTime(600)));
                drawLine(posRight, posUp, 1.0 - XM1PW3P1(effects[x].timer.percentElapsedTime(600)));
                drawLine(posRight, posDown, 1.0 - XM1PW3P1(effects[x].timer.percentElapsedTime(600)));
            }
                break;
            case 6:
            {
                //long 1.3s big diamond
                remove = effects[x].timer.percentElapsedTime(1300) == 1.0;
                XY posLeft = normPosition;
                XY posRight = { g_windowW * ((effects[x].pos.x + 60) / 960.0), g_windowH * ((effects[x].pos.y) / 960.0) };
                XY posUp = { g_windowW * ((effects[x].pos.x + 30) / 960.0), g_windowH * ((effects[x].pos.y - 24) / 960.0) };
                XY posDown = { g_windowW * ((effects[x].pos.x + 30) / 960.0), g_windowH * ((effects[x].pos.y + 24) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x19 - 0x19 * effects[x].timer.percentElapsedTime(1300));
                drawLine(posLeft, posUp, 1.0 - XM1PW3P1(effects[x].timer.percentElapsedTime(1300)));
                drawLine(posLeft, posDown, 1.0 - XM1PW3P1(effects[x].timer.percentElapsedTime(1300)));
                drawLine(posRight, posUp, 1.0 - XM1PW3P1(effects[x].timer.percentElapsedTime(1300)));
                drawLine(posRight, posDown, 1.0 - XM1PW3P1(effects[x].timer.percentElapsedTime(1300)));
            }
                break;
        }
        if (remove) {
            effects.erase(effects.begin() + x);
            x--;
            continue;
        }
    }

    auto timeNow = std::chrono::system_clock::now();
    std::time_t timeNowT = std::chrono::system_clock::to_time_t(timeNow);
    std::tm tmNow;
#ifdef _MSC_VER
    localtime_s(&tmNow, &timeNowT);
#else
    tmNow = *localtime(&timeNowT);
#endif

    int yearNow = tmNow.tm_year + 1900;
    int monthNow = tmNow.tm_mon;
    int dayNow = tmNow.tm_mday;

    int hourNow = tmNow.tm_hour;//std::chrono::duration_cast<std::chrono::hours>(timeNow.time_since_epoch()).count() % 24;
    int minuteNow = tmNow.tm_min;//std::chrono::duration_cast<std::chrono::minutes>(timeNow.time_since_epoch()).count() % 60;
    int secondNow = tmNow.tm_sec;//std::chrono::duration_cast<std::chrono::seconds>(timeNow.time_since_epoch()).count() % 60;
    int msNow = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch()).count() % 1000;

    int xOrigin = g_windowW - 10;
    int yOrigin = 40;

    //draw hour lines
    double sep = (g_windowH - (60 + yOrigin)) / (23.0);
    for (int x = 0; x < 24; x++) {
        XY lineP1 = { xOrigin, yOrigin + x * sep };
        XY lineP2 = { xOrigin - g_windowW/32, yOrigin + x * sep };

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x20);
        drawLine(lineP1, lineP2, 1.0);

        if (x == hourNow) {
            //progress line
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x42);
            drawLine(lineP1, lineP2, (minuteNow / 60.0));
        }
        
        else if (x < hourNow) {
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x29);
            drawLine(lineP1, lineP2, 1.0);
        }
    }

    //draw minute lines
    xOrigin -= g_windowW / 32 + 20;
    sep = (g_windowH - (60 + yOrigin)) / (59.0);
    for (int x = 0; x < 60; x++) {
        XY lineP1 = { xOrigin, yOrigin + x * sep };
        XY lineP2 = { xOrigin - g_windowW / 20, yOrigin + x * sep };
        if (x % 2) {
            lineP1 = xyAdd(lineP1, { -20, 0 });
            lineP2 = xyAdd(lineP2, { -20, 0 });
            XY t = lineP1;
            lineP1 = lineP2;
            lineP2 = t;
        }

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x1A);
        drawLine(lineP1, lineP2, 1.0);

        if (x == minuteNow) {
            //progress line
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x39);
            drawLine(lineP1, lineP2, (secondNow / 60.0));
        }
        else if (x < minuteNow) {
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x20);
            drawLine(lineP1, lineP2, 1.0);
        }
    }

    //draw second lines
    xOrigin -= g_windowW / 20 + 20 + 20;
    sep = (g_windowH - (60 + yOrigin)) / (59.0);
    for (int x = 0; x < 60; x++) {
        XY lineP1 = { xOrigin, yOrigin + x * sep };
        XY lineP2 = { xOrigin - g_windowW / 16, yOrigin + x * sep };
        if (x % 2) {
            lineP1 = xyAdd(lineP1, { -40, 0 });
            lineP2 = xyAdd(lineP2, { -40, 0 });
            XY t = lineP1;
            lineP1 = lineP2;
            lineP2 = t;
        }

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x13);
        drawLine(lineP1, lineP2, 1.0);

        if (x == secondNow) {
            //progress line
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x2F);
            drawLine(lineP1, lineP2, XM1PW3P1(msNow / 1000.0));
        }
        else if (x < secondNow) {
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x1A);
            drawLine(lineP1, lineP2, 1.0);
        }
    }

    //g_fnt->RenderString(std::format("{:04}-{:02}-{:02}", yearNow, monthNow, dayNow), g_windowW - 120, g_windowH - 90, SDL_Color{255,255,255,0x50});
    //g_fnt->RenderString(std::format("{:02}:{:02}:{:02}", hourNow, minuteNow, secondNow), g_windowW - 120, g_windowH - 70, SDL_Color{255,255,255,0x50});
}

void StartScreen::openImageLoadDialog()
{
    std::vector<std::pair<std::string, std::string>> filetypes;
    for (FileImporter*& f : g_fileImporters) {
        filetypes.push_back({ f->extension(), f->name()});
    }

    platformTryLoadOtherFile(this, filetypes, "open image", 0);
}

void StartScreen::tryLoadFile(std::string path)
{
    MainEditor* newSession = loadAnyIntoSession(path);
    if (newSession != NULL) {
        g_addScreen(newSession);
        g_tryPushLastFilePath(path);
    }
    else {
        g_addNotification(ErrorNotification("Error", "Failed to load file"));
    }
}

void StartScreen::tryOpenImageFromClipboard()
{
    Layer* l = platformGetImageFromClipboard();
    if (l != NULL) {
        g_addScreen(new MainEditor(l));
    }
    else {
        char* clipboard = SDL_GetClipboardText();
        Layer* foundLayer = NULL;
        if (clipboard != NULL) {
            std::string clipboardText = clipboard;
            //base64 png
            if (clipboardText.size() > 0) {
                if (clipboardText.find("iVBO") != std::string::npos) {
                    foundLayer = readPNGFromBase64String(clipboardText);
                }
            }
        }

        if (foundLayer != NULL) {
            g_addScreen(foundLayer->isPalettized ? new MainEditorPalettized((LayerPalettized*)foundLayer) : new MainEditor(foundLayer));
        }
        else {
            g_addNotification(ErrorNotification("Error", "No image in clipboard"));
        }
        SDL_free(clipboard);
    }
}
