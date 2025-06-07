#include "StartScreen.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "FileIO.h"
#include "Notification.h"
#include "CustomTemplate.h"
#include "ScreenNonogramPlayer.h"
#include "LayerPalettized.h"
#include "ExtractDataScreen.h"

#include "PopupMessageBox.h"
#include "PopupFilePicker.h"

void StartScreen::tick() {
    if (closeNextTick) {
        g_closeScreen(this);
        return;
    }

    if (waitingForUpdateCheckInfo && updateCheckComplete) {
        if (!updateCheckFailed) {
            updateCheckFinished();
        }
        waitingForUpdateCheckInfo = false;
    }
}

void StartScreen::render()
{
    renderBackground();

    SDL_Rect logoRect = SDL_Rect{ 4, g_windowH - 4 - 40 * 4, 128 * 4, 40 * 4 };
    SDL_RenderCopy(g_rd, g_mainlogo, NULL, &logoRect);
    g_fnt->RenderString(std::format("alpha@{}\n{}", __DATE__, GIT_HASH), 6, g_windowH - 20 - 20, SDL_Color{255,255,255,0x50}, 14);

    if (g_config.checkUpdates && updateCheckComplete && !updateCheckFailed && latestHash != GIT_HASH) {
        static std::string tlUpdateAvailable = TL("vsp.launchpad.update.title");
        static std::string tlLatestVer = TL("vsp.launchpad.update.latestver");
        std::string desc = std::format("{} {}/{}/{} - {}", tlLatestVer, latestVersionYear, latestVersionMonth, latestVersionDay, latestHash.substr(0, 7));
        XY position = { logoRect.x, logoRect.y };
        double fadeInTimer = updateCheckTimer.percentElapsedTime(800, -400);
        double fadeInTimer2 = updateCheckTimer.percentElapsedTime(800);
        g_fnt->RenderString(tlUpdateAvailable, position.x + 14 - 30 * (1.0 - XM1PW3P1(fadeInTimer)), position.y, SDL_Color{ 0xFC,0xFF,0x84,(u8)(0x80 * fadeInTimer) }, 20);
        g_fnt->RenderString(desc, position.x + 14 - 30 * (1.0 - XM1PW3P1(fadeInTimer2)), position.y + 24, SDL_Color{ 0xFC,0xFF,0x84,(u8)(0x80 * fadeInTimer2) }, 16);
        XY textB = g_fnt->StatStringDimensions(desc, 16);
        XY p1 = { position.x + 5, position.y + 5 };
        XY p2 = xyAdd(p1, { 0, 24 + textB.y });
        SDL_SetRenderDrawColor(g_rd, 0xFC, 0xFF, 0x84, (u8)(0xd0 * fadeInTimer));
        drawLine(p1, p2, XM1PW3P1(fadeInTimer2));
    }

    SDL_Rect bgr = SDL_Rect{ 0, 35, ixmax(560,newImageTabs->getDimensions().x + newImageTabs->position.x + 5), 300 };
    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, 0xa0};
    SDL_Color colorBG2 = { 0x20, 0x20, 0x20, 0xa0};
    SDL_Color colorBG3 = { 0x10, 0x10, 0x10, 0xa0 };
    renderGradient(bgr, sdlcolorToUint32(colorBG3), sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1));

    bgr.x += bgr.w + 10;
    bgr.y += 40;
    bgr.h -= 40;
    renderGradient(bgr, sdlcolorToUint32(colorBG3), 0, sdlcolorToUint32(colorBG2), 0);

    wxsManager.renderAll();

    renderFileDropAnim();

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
        droppingFile = false;
        std::string filePath = evt.drop.data;
        if (stringEndsWithIgnoreCase(filePath, ".zlib")) {
            unZlibFile(convertStringOnWin32(filePath));
        }
        else if (stringEndsWithIgnoreCase(filePath, ".unzlib")) {
            zlibFile(convertStringOnWin32(filePath));
        }
        else if (stringEndsWithIgnoreCase(filePath, ".data")) {
            g_addScreen(new ExtractDataScreen(convertStringOnWin32(filePath)));
        }
        else {
            tryLoadFile(filePath);
        }
        return;
    }
    else if (evt.type == SDL_EVENT_DROP_BEGIN) {
        fileDropTimer.start();
        droppingFile = true;
    }
    else if (evt.type == SDL_EVENT_DROP_COMPLETE) {
        droppingFile = false;
    }
    else if (evt.type == SDL_EVENT_DROP_POSITION) {
        fileDropXY = { (int)evt.drop.x, (int)evt.drop.y };
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
                else if (evt.key.scancode == SDL_SCANCODE_N && g_ctrlModifier) {
                    ScreenNonogramPlayer::StartDebugGame();
                }
#if _DEBUG
                else if (evt.key.scancode == SDL_SCANCODE_INSERT && g_ctrlModifier && g_shiftModifier) {
                    throw std::exception("** user-initiated test crash");
                }
#endif
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
                    Layer* newLayer = Layer::tryAllocLayer(newImgW, newImgH);
                    if (newLayer != NULL) {
                        MainEditor* newMainEditor = new MainEditor(newLayer);
                        g_addScreen(newMainEditor);
                    }
                    else {
                        g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.cmn.error.mallocfail")));
                    }
                }
                catch (std::out_of_range&) {
                    g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.launchpad.error.oob")));
                }
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.launchpad.error.no_dims")));
            }
            break;
        case 1:
            if (!tab1TextFieldCH->textEmpty() && !tab1TextFieldCW->textEmpty()
                && !tab1TextFieldCHX->textEmpty() && !tab1TextFieldCWX->textEmpty()) {
                try {
                    XY cellSize = XY{ std::stoi(tab1TextFieldCW->getText()) , std::stoi(tab1TextFieldCH->getText()) };
                    int newImgW = cellSize.x * std::stoi(tab1TextFieldCWX->getText());
                    int newImgH = cellSize.y * std::stoi(tab1TextFieldCHX->getText());
                    Layer* newLayer = Layer::tryAllocLayer(newImgW, newImgH);
                    if (newLayer != NULL) {
                        MainEditor* newMainEditor = new MainEditor(newLayer);
                        newMainEditor->tileDimensions = cellSize;
                        g_addScreen(newMainEditor);
                    }
                    else {
                        g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.cmn.error.mallocfail")));
                    }
                }
                catch (std::out_of_range&) {
                    g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.launchpad.error.oob")));
                }
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.launchpad.error.no_dims")));
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
                    LayerPalettized* newLayer = LayerPalettized::tryAllocIndexedLayer(newImgW, newImgH);
                    if (newLayer != NULL) {
                        newLayer->palette = g_palettes[PALETTE_DEFAULT];
                        MainEditorPalettized* newMainEditor = new MainEditorPalettized(newLayer);
                        g_addScreen(newMainEditor);
                    }
                    else {
                        g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.cmn.error.mallocfail")));
                    }
                }
                catch (std::out_of_range&) {
                    g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.launchpad.error.oob")));
                }
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.launchpad.error.no_dims")));
            }
            break;
        case 1:
            if (!tab1TextFieldCH->textEmpty() && !tab1TextFieldCW->textEmpty()
                && !tab1TextFieldCHX->textEmpty() && !tab1TextFieldCWX->textEmpty()) {
                try {
                    XY cellSize = XY{ std::stoi(tab1TextFieldCW->getText()) , std::stoi(tab1TextFieldCH->getText()) };
                    int newImgW = cellSize.x * std::stoi(tab1TextFieldCWX->getText());
                    int newImgH = cellSize.y * std::stoi(tab1TextFieldCHX->getText());
                    LayerPalettized* newLayer = LayerPalettized::tryAllocIndexedLayer(newImgW, newImgH);
                    if (newLayer != NULL) {
                        newLayer->palette = g_palettes[PALETTE_DEFAULT];
                        MainEditorPalettized* newMainEditor = new MainEditorPalettized(newLayer);
                        newMainEditor->tileDimensions = cellSize;
                        g_addScreen(newMainEditor);
                    }
                    else {
                        g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.cmn.error.mallocfail")));
                    }
                }
                catch (std::out_of_range&) {
                    g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.launchpad.error.oob")));
                }
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.launchpad.error.no_dims")));
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
    if (importerIndex == -1) {
        tryLoadFile(convertStringToUTF8OnWin32(name));
    } else {
        importerIndex--;
        FileImporter* importer = g_fileImporters[importerIndex];
        void* importedData = importer->importData(name);
        if (importedData != NULL) {
            MainEditor* outSession = NULL;
            if (!importer->importsWholeSession()) {
                Layer* nlayer = (Layer*)importedData;
                outSession = !nlayer->isPalettized ? new MainEditor(nlayer) : new MainEditorPalettized((LayerPalettized*)nlayer);
            }
            else {
                outSession = (MainEditor*)importedData;
            }

            if (importer->getCorrespondingExporter() != NULL) {
                outSession->lastWasSaveAs = false;
                outSession->lastConfirmedSave = true;
                outSession->lastConfirmedSavePath = name;
                outSession->lastConfirmedExporter = importer->getCorrespondingExporter();
            }
            g_addScreen(outSession);
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.fileloadfail")));
        }
    }
}

void StartScreen::eventDropdownItemSelected(int evt_id, int index, std::string name)
{
    if (evt_id == EVENT_STARTSCREEN_TEMPLATEPICKED) {
        BaseTemplate* templ = g_templates[index];
        Layer* templateLayer = templ->generate();
        if (templateLayer == NULL) {
            g_addNotification(ErrorNotification(TL("vsp.launchpad.error.starteditor"), TL("vsp.launchpad.error.templatefail")));
            return;
        }
        MainEditor* newMainEditor = new MainEditor(templateLayer);
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

    UILabel* lbl = new UILabel(TL("vsp.launchpad.lastfiles"));
    lbl->fontsize = 22;
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
                std::string s = g_configWasLoaded ? TL("vsp.launchpad.welcomereturning") : TL("vsp.launchpad.welcome1sttime");
                XY fd = g_fnt->StatStringDimensions(s, 33);
                g_fnt->RenderString(s, g_windowW - fd.x - 2, 0, { 255,255,255, (u8)(255 * (1.0 - XM1PW3P1((textAnimTime - 0.05) / 0.95))) }, 33);
            }
        }
    }
}

void StartScreen::renderFileDropAnim()
{
    if (droppingFile) {
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xa0 * fileDropTimer.percentElapsedTime(200));
        SDL_Rect r = { 0,0,g_windowW,g_windowH };
        SDL_RenderFillRect(g_rd, &r);

        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 255);
        SDL_Rect r2 = { 0,0, g_windowW, (g_windowH / 6) * XM1PW3P1(fileDropTimer.percentElapsedTime(1500)) };
        SDL_RenderFillRect(g_rd, &r2);
        r2.y = g_windowH - r2.h;
        SDL_RenderFillRect(g_rd, &r2);

        g_fnt->RenderString("Drop an image here to open it in a new workspace...", 50, 50, { 255,255,255,(u8)(255 * fileDropTimer.percentElapsedTime(200)) }, 22);

        if (g_config.vfxEnabled) {
            for (int x = 0; x < 3; x++) {
                SDL_SetRenderDrawColor(g_rd, 255, 255, 255, (255 / (x + 1)) * fileDropTimer.percentLoopingTime(2000, x * 700));
                XY origin = fileDropXY;
                SDL_Rect r3 = offsetRect({ origin.x, origin.y,1,1 }, ixpow(4, x + 1) * XM1PW3P1(fileDropTimer.percentLoopingTime(1000)));
                SDL_RenderDrawRect(g_rd, &r3);
            }


            double tick1 = 1.0 - XM1PW3P1(fileDropTimer.percentLoopingTime(1500));
            double tick2 = 1.0 - XM1PW3P1(fileDropTimer.percentLoopingTime(1500, 700));
            double tick3 = 1.0 - XM1PW3P1(fileDropTimer.percentLoopingTime(1500, 700));
            double tick4 = 1.0 - XM1PW3P1(fileDropTimer.percentLoopingTime(1500));
            SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255 * tick1);
            drawLine(xyAdd(fileDropXY, { -60,-60 }), xyAdd(fileDropXY, { -20,-20 }), tick1);
            drawLine(xyAdd(fileDropXY, { -60, 60 }), xyAdd(fileDropXY, { -20, 20 }), tick1);
            drawLine(xyAdd(fileDropXY, { 60,-60 }), xyAdd(fileDropXY, { 20,-20 }), tick1);
            drawLine(xyAdd(fileDropXY, { 60, 60 }), xyAdd(fileDropXY, { 20, 20 }), tick1);

        }
    }
}

void StartScreen::renderBackground()
{
    static Fill backgroundFill = visualConfigFill("launchpad/bg");
    backgroundFill.fill({ 0,0,g_windowW,g_windowH });
    //uint32_t colorBG1 = 0xFF000000;//| (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x000000 : 0xDFDFDF);
    //uint32_t colorBG2 = 0xFF000000 | 0x202020;//| (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x202020 : 0x808080);
    //renderGradient({ 0,0, g_windowW, g_windowH }, colorBG1, colorBG1, colorBG1, colorBG2);

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

    static SDL_Color vfxColor = visualConfigColor("launchpad/effects_color");

    for (int x = 0; x < effects.size(); x++) {
        bool remove = false;
        XY normPosition = { g_windowW * (effects[x].pos.x / 960.0), g_windowH * (effects[x].pos.y / 960.0) };
        switch (effects[x].type) {
            case 0:
            {
                //long 2s line
                remove = effects[x].timer.percentElapsedTime(2000) == 1.0;
                XY normPosition2 = { g_windowW * ((effects[x].pos.x + 120) / 960.0), g_windowH * ((effects[x].pos.y - 120) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, vfxColor.r, vfxColor.g, vfxColor.b, 0x30 * (1.0 - effects[x].timer.percentElapsedTime(2000)));
                drawLine(normPosition, normPosition2, 1.0);
            }
            break;
            case 1:
            {
                //long 0.7s trail
                remove = effects[x].timer.percentElapsedTime(1000) == 1.0;
                XY normPosition2 = { g_windowW * ((effects[x].pos.x + 500) / 960.0), g_windowH * ((effects[x].pos.y - 500) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, vfxColor.r, vfxColor.g, vfxColor.b, 0x30 - 0x20 * effects[x].timer.percentElapsedTime(700));
                drawLine(normPosition, normPosition2, 1.0-XM1PW3P1(effects[x].timer.percentElapsedTime(700)));
            }
                break;
            case 2:
            {
                //short 3s line
                remove = effects[x].timer.percentElapsedTime(3000) == 1.0;
                XY normPosition2 = { g_windowW * ((effects[x].pos.x + 40) / 960.0), g_windowH * ((effects[x].pos.y - 40) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, vfxColor.r, vfxColor.g, vfxColor.b, 0x20 * (1.0 - effects[x].timer.percentElapsedTime(3000)));
                drawLine(normPosition, normPosition2, 1.0);
            }
                break;
            case 3:
            {
                //mid length 2.5s line
                remove = effects[x].timer.percentElapsedTime(2500) == 1.0;
                XY normPosition2 = { g_windowW * ((effects[x].pos.x + 90) / 960.0), g_windowH * ((effects[x].pos.y - 90) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, vfxColor.r, vfxColor.g, vfxColor.b, 0x20 * (1.0 - effects[x].timer.percentElapsedTime(2500)));
                drawLine(normPosition, normPosition2, 1.0);
            }
                break;
            case 4:
            {
                //short 1s star
                remove = effects[x].timer.percentElapsedTime(1000) == 1.0;
                XY normPosition2 = { g_windowW * ((effects[x].pos.x + 15) / 960.0), g_windowH * ((effects[x].pos.y - 15) / 960.0) };
                XY normPosition3 = { g_windowW * ((effects[x].pos.x - 15) / 960.0), g_windowH * ((effects[x].pos.y + 15) / 960.0) };
                SDL_SetRenderDrawColor(g_rd, vfxColor.r, vfxColor.g, vfxColor.b, 0x50 - 0x50 * effects[x].timer.percentElapsedTime(1000));
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
                SDL_SetRenderDrawColor(g_rd, vfxColor.r, vfxColor.g, vfxColor.b, 0x24 - 0x24 * effects[x].timer.percentElapsedTime(600));
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
                SDL_SetRenderDrawColor(g_rd, vfxColor.r, vfxColor.g, vfxColor.b, 0x19 - 0x19 * effects[x].timer.percentElapsedTime(1300));
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

    if (g_config.vfxEnabled && g_config.checkUpdates && g_config.animatedBackground != 0) {
        renderBGStars();
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

    static SDL_Color hourColor = visualConfigColor("launchpad/hours_color");
    static SDL_Color minuteColor = visualConfigColor("launchpad/minutes_color");
    static SDL_Color secondColor = visualConfigColor("launchpad/seconds_color");

    //draw hour lines
    double sep = (g_windowH - (60 + yOrigin)) / (23.0);
    for (int x = 0; x < 24; x++) {
        XY lineP1 = { xOrigin, yOrigin + x * sep };
        XY lineP2 = { xOrigin - g_windowW/32, yOrigin + x * sep };

        SDL_SetRenderDrawColor(g_rd, hourColor.r, hourColor.g, hourColor.b, 0x20);
        drawLine(lineP1, lineP2, 1.0);

        if (x == hourNow) {
            //progress line
            SDL_SetRenderDrawColor(g_rd, hourColor.r, hourColor.g, hourColor.b, 0x42);
            drawLine(lineP1, lineP2, (minuteNow / 60.0));
        }
        
        else if (x < hourNow) {
            SDL_SetRenderDrawColor(g_rd, hourColor.r, hourColor.g, hourColor.b, 0x29);
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

        SDL_SetRenderDrawColor(g_rd, minuteColor.r, minuteColor.g, minuteColor.b, 0x1A);
        drawLine(lineP1, lineP2, 1.0);

        if (x == minuteNow) {
            //progress line
            SDL_SetRenderDrawColor(g_rd, minuteColor.r, minuteColor.g, minuteColor.b, 0x39);
            drawLine(lineP1, lineP2, (secondNow / 60.0));
        }
        else if (x < minuteNow) {
            SDL_SetRenderDrawColor(g_rd, minuteColor.r, minuteColor.g, minuteColor.b, 0x20);
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

        SDL_SetRenderDrawColor(g_rd, secondColor.r, secondColor.g, secondColor.b, 0x13);
        drawLine(lineP1, lineP2, 1.0);

        if (x == secondNow) {
            //progress line
            SDL_SetRenderDrawColor(g_rd, secondColor.r, secondColor.g, secondColor.b, 0x2F);
            drawLine(lineP1, lineP2, XM1PW3P1(msNow / 1000.0));
        }
        else if (x < secondNow) {
            SDL_SetRenderDrawColor(g_rd, secondColor.r, secondColor.g, secondColor.b, 0x1A);
            drawLine(lineP1, lineP2, 1.0);
        }
    }

    //g_fnt->RenderString(std::format("{:04}-{:02}-{:02}", yearNow, monthNow, dayNow), g_windowW - 120, g_windowH - 90, SDL_Color{255,255,255,0x50});
    //g_fnt->RenderString(std::format("{:02}:{:02}:{:02}", hourNow, minuteNow, secondNow), g_windowW - 120, g_windowH - 70, SDL_Color{255,255,255,0x50});
}

void StartScreen::renderBGStars()
{
    int i = 0;
    for (LaunchpadBGStar& star : stars) {
        XY realPosition = bgSpaceTransform(star.pos);
        XY endPos = bgSpaceTransform(xyAdd(star.pos, { 2,2 }));

        const int blinkDistance = 12;
        XY center = {star.pos.x + 1, star.pos.y + 1};
        XY centerInBgSpace = bgSpaceTransform(center);
        XY p1 = bgSpaceTransform(xyAdd(center, { blinkDistance, -blinkDistance }));
        XY p2 = bgSpaceTransform(xyAdd(center, { -blinkDistance, blinkDistance }));

        double blinkTimer = star.timer.percentLoopingTime(1500, -star.blinkOffset);
        u8 alpha = 0x20 + star.opacity * (1.0 - blinkTimer);
        SDL_SetRenderDrawColor(g_rd, 0xfd, 0xff, 0xab, alpha);
        drawLine(realPosition, endPos, XM1PW3P1(star.timer.percentElapsedTime(1000)));

        SDL_SetRenderDrawColor(g_rd, 0xfd, 0xff, 0xab, alpha / 2);
        drawLine(p1, centerInBgSpace, 1.0 - XM1PW3P1(dxmin(blinkTimer, 0.3) / 0.3));
        drawLine(p2, centerInBgSpace, 1.0 - XM1PW3P1(dxmin(blinkTimer, 0.3) / 0.3));
        i++;
    }
}

void StartScreen::openImageLoadDialog()
{
    PopupFilePicker::PlatformAnyImageImportDialog(this, TL("vsp.popup.openimage"), 0);
}

void StartScreen::tryLoadFile(std::string path)
{
    MainEditor* newSession = loadAnyIntoSession(path);
    if (newSession != NULL) {
        g_addScreen(newSession);
        g_tryPushLastFilePath(path);
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.fileloadfail")));
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
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.launchpad.error.clipboard_no_image")));
        }
        SDL_free(clipboard);
    }
}

void StartScreen::updateCheckFinished()
{
    genBGStars();
    updateCheckTimer.start();
}

void StartScreen::genBGStars() {
    stars.clear();
    for (int x = 0; x < ixmin(githubStars, 200); x++) {
        LaunchpadBGStar s = {
            XY{ randomInt(0, 960), randomInt(0,960) },
            randomInt(1,3),
            randomInt(0x60,0xa0),
            randomInt(0, 1400),
        };
        stars.push_back(s);
        stars[stars.size()-1].timer.start();
    }
}

XY StartScreen::bgSpaceTransform(XY p)
{
    return XY {
        (int)((p.x / 960.0f) * g_windowW),
        (int)((p.y / 960.0f) * g_windowH)
    };
}
