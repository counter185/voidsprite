#include <fstream>
#include <regex>

#include "SplitSessionEditor.h"
#include "UILabel.h"
#include "FileIO.h"
#include "maineditor.h"
#include "Notification.h"
#include "mathops.h"
#include "FontRenderer.h"
#include "TooltipsLayer.h"
#include "ScrollingPanel.h"
#include "ScreenWideNavBar.h"
#include "PopupYesNo.h"
#include "PopupFilePicker.h"
#include "keybinds.h"

//TODO: standardize this loading process and move it somewhere else
SplitSessionData loadSplitSessionData(PlatformNativePathString path)
{
    std::string utf8path = convertStringToUTF8OnWin32(path);
    std::string fullDirectory = (utf8path.find('/') != std::string::npos || utf8path.find('\\') != std::string::npos) ? utf8path.substr(0, utf8path.find_last_of("\\/")) : "";
    if (fullDirectory.length() > 0) {
        fullDirectory += "/";
    }

    std::ifstream f(path);
    if (f.good() && f.is_open()) {
        SplitSessionData ssn;
        ssn.set = true;
        std::string line;
        std::getline(f, line);
        if (stringStartsWithIgnoreCase(line, "voidsprite split session file v")) {
            int version;
            try {
                version = std::stoi(line.substr(31));
            }
            catch (std::exception&) {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid split session file"));
                f.close();   //not needed apparently
                return { false };
            }

            switch (version) {
            case 0:
            {
                while (!f.eof()) {
                    std::getline(f, line);
                    if (stringStartsWithIgnoreCase(line, "#:")) {
                        //image properties in format: #:filename|x|y
                        try {
                            std::string originalFileName = line.substr(2, line.find('|') - 2);
                            std::string filename = fullDirectory + originalFileName;
                            int x = std::stoi(line.substr(line.find('|') + 1, line.find('|', line.find('|') + 1) - line.find('|') - 1));
                            int y = std::stoi(line.substr(line.find('|', line.find('|') + 1) + 1));
                            SplitSessionImage ssi;
                            ssi.fileName = filename;
                            ssi.originalFileName = originalFileName;
                            ssi.positionInOverallImage = XY{ x,y };
                            ssi.exporter = NULL;    //todo

                            FileImporter* foundImporter = NULL;
                            MainEditor* subsn = loadAnyIntoSession(filename, &foundImporter);
                            if (subsn != NULL) {
                                ssi.exporter = foundImporter->getCorrespondingExporter();
                                Layer* nlayer = subsn->flattenImage();
                                ssi.dimensions = { nlayer->w, nlayer->h };
                                ssi.loadedLayer = nlayer;
                                ssn.images.push_back(ssi);
                                if (x + nlayer->w > ssn.overallDimensions.x) {
                                    ssn.overallDimensions.x = x + nlayer->w;
                                }
                                if (y + nlayer->h > ssn.overallDimensions.y) {
                                    ssn.overallDimensions.y = y + nlayer->h;
                                }
                                delete subsn;
                            }
                            else {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to load split session fragment"));
                            }

                        }
                        catch (std::exception&) {
                            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to load split session fragment"));
                        }
                    }
                    else if (stringStartsWithIgnoreCase(line, "comment:")) {
                        try {
                            std::string fullData = line.substr(8);
                            std::regex split("(;([0-9]+);([0-9]+))$");
                            std::smatch match;
                            std::regex_search(fullData, match, split);
                            if (match.size() == 4) {
                                std::string comment = fullData.substr(0, fullData.size() - match[0].str().size());
                                int x = std::stoi(match[2].str());
                                int y = std::stoi(match[3].str());
                                //comments.push_back(CommentData{ XY{x,y}, comment });
                            }
                        }
                        catch (std::exception&) {
                            logerr("error reading comment");
                        }
                    }
                    else if (line.find(':')) {
                        std::string argName = line.substr(0, line.find(':'));
                        std::string argValue = line.substr(line.find(':') + 1);

                        if (argName == "tiledim.x") {
                            ssn.tileDimensions.x = std::stoi(argValue);
                        }
                        else if (argName == "tiledim.y") {
                            ssn.tileDimensions.y = std::stoi(argValue);
                        }
                    }
                }
                return ssn;

            }
            break;
            default:
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Unsupported split session file version"));
                f.close();
                return { false };
            }
        }
    }
    return { false };
}

SplitSessionEditor::SplitSessionEditor()
{
    setupWidgets();

}

SplitSessionEditor::SplitSessionEditor(PlatformNativePathString path)
{
    outputSPSNFilePath = convertStringToUTF8OnWin32(path);
    setupWidgets();
    if (std::filesystem::exists(path)) {
        tryLoadExistingSplitSession(path);
    }
}

SplitSessionEditor::~SplitSessionEditor()
{
    for (tempSplitSessionImage& tssi : loadedImgs) {
        if (tssi.loadedLayer != NULL) {
            delete tssi.loadedLayer;
        }
    }
}

void SplitSessionEditor::tick()
{
    c.lockToScreenBounds(0, 0, 0, 0);
}

void SplitSessionEditor::render()
{
    drawBackground();

    SDL_Rect canvasRect = c.getCanvasOnScreenRect();
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x90);
    SDL_RenderDrawRect(g_rd, &canvasRect);

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x10);
    if (c.scale >= 10) {
        c.drawTileGrid({ 1,1 });
    }

    XY hoverPixel = c.getTilePosAt({ g_mouseX, g_mouseY });
    SDL_Rect r = c.getTileScreenRectAt(hoverPixel, { 1,1 });
    SDL_RenderFillRect(g_rd, &r);

    for (tempSplitSessionImage &tssi : loadedImgs) {
        SDL_Rect r = c.canvasRectToScreenRect(
            {tssi.position.x, tssi.position.y, tssi.dims.x, tssi.dims.y});
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x90);
        SDL_RenderDrawRect(g_rd, &r);
        renderGradient(r, 0x20FFFFFF, 0x20FFFFFF, 0x20FFFFFF, 0x40FFFFFF);
        tssi.loadedLayer->render(r, 0x20);
        //SDL_RenderCopy(g_rd, tssi.loadedLayer->tex, NULL, &r);
    }

    if (dragging != -1) {
        tempSplitSessionImage& tssi = loadedImgs[dragging];
        XY diff = xySubtract(c.screenPointToCanvasPoint({g_mouseX, g_mouseY}),
                             draggingStartPixel);
        SDL_Rect r = c.canvasRectToScreenRect(
            { tssi.position.x + diff.x, tssi.position.y + diff.y, tssi.dims.x, tssi.dims.y });
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x90);
        SDL_RenderDrawRect(g_rd, &r);
    }
    else {
        drawTSSITooltips();
    }

    drawBottomBar();
    g_fnt->RenderString(frmt("Current canvas size: {}x{} ({}%)", c.dimensions.x, c.dimensions.y, c.scale * 100), 2, g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });

    BaseScreen::render();

}

void SplitSessionEditor::takeInput(SDL_Event evt)
{
    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }
    else if (evt.type == SDL_DROPFILE) {
        std::string filePath = evt.drop.data;
        tryAddFile(filePath);
        return;
    }

    LALT_TO_SUMMON_NAVBAR;

    if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt)) {
        if (!g_keybindManager.processKeybinds(evt, "splitsessioneditor", this)) {
            switch (evt.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (evt.button.button == SDL_BUTTON_MIDDLE) {
                    scrollingCanvas = true;
                }
                else if (evt.button.button == SDL_BUTTON_LEFT) {
                    dragging = findTSSIAt(mousePixelPos);
                    if (dragging != -1) {
                        draggingStartPixel = c.screenPointToCanvasPoint({ (int)evt.button.x, (int)evt.button.y });
                    }
                    else {
                        scrollingCanvas = true;
                    }
                }
                else if (evt.button.button == SDL_BUTTON_RIGHT) {
                    int i = findTSSIAt(mousePixelPos);
                    if (i >= 0 && i < loadedImgs.size()) {

                        PopupYesNo* newPopup = new PopupYesNo(
                            "Remove image?",
                            "Remove the image from the split session?\n " + loadedImgs[i].calcRelativePath);
                        newPopup->setCallbackListener(10 + i, this);
                        g_addPopup(newPopup);
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (evt.button.button == SDL_BUTTON_MIDDLE) {
                    scrollingCanvas = false;
                }
                else if (evt.button.button == SDL_BUTTON_LEFT) {
                    scrollingCanvas = false;
                    if (dragging != -1) {
                        XY endPoint = c.screenPointToCanvasPoint({(int)evt.button.x, (int)evt.button.y});
                        XY targetPosition = xyAdd(loadedImgs[dragging].position, xySubtract(endPoint, draggingStartPixel));
                        if (targetPosition.x < 0) {
                            int xOffset = -targetPosition.x;
                            for (int i = 0; i < loadedImgs.size(); i++) {
                                if (i != dragging) {
                                    loadedImgs[i].position.x += xOffset;
                                }
                            }
                            c.panCanvas({ -xOffset,0 });
                            targetPosition.x = 0;
                        }
                        if (targetPosition.y < 0) {
                            int yOffset = -targetPosition.y;
                            for (int i = 0; i < loadedImgs.size(); i++) {
                                if (i != dragging) {
                                    loadedImgs[i].position.y += yOffset;
                                }
                            }
                            c.panCanvas({ 0, -yOffset });
                            targetPosition.y = 0;
                        }
                        if (targetPosition.x >= 0 && targetPosition.y >= 0) {
                            loadedImgs[dragging].position = targetPosition;
                        }
                        else {
                            //should not happen now ever
                            g_addNotification(ErrorNotification("Image was placed out of bounds.", ""));
                        }
                        dragging = -1;
                        recalcCanvasDimensions();
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                calcMousePixelPos({ (int)(evt.motion.x), (int)(evt.motion.y) });
                if (scrollingCanvas) {
                    c.panCanvas({ (int)(evt.motion.xrel), (int)(evt.motion.yrel) });
                }
                break;
            case SDL_MOUSEWHEEL:
                c.zoomFromWheelInput(evt.wheel.y);
                break;
            }
        }
    }
}

void SplitSessionEditor::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex)
{
    if (evt_id == 0) {
        outputSPSNFilePath = convertStringToUTF8OnWin32(name);
        recalcRelativePaths();
    }
}

void SplitSessionEditor::eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex)
{
    if (evt_id == EVENT_SPLITSESSION_ADDIMAGE) {
        tryAddFile(convertStringToUTF8OnWin32(name));
    }
}

void SplitSessionEditor::eventPopupClosed(int evt_id, BasePopup* popup)
{
    if (evt_id >= 10 && evt_id - 10 < loadedImgs.size()) {
        if (((PopupYesNo*)popup)->result) {
            int index = evt_id - 10;
            auto& tssi = loadedImgs[index];
            if (tssi.loadedLayer != NULL) {
                delete tssi.loadedLayer;
            }
            loadedImgs.erase(loadedImgs.begin() + index);
            populateSubImagesList();
        }
    }
}

void SplitSessionEditor::drawBackground()
{
    renderGradient({ 0,0,g_windowW,g_windowH }, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF202020);
}

void SplitSessionEditor::drawTSSITooltips()
{
    TooltipsLayer localttp;

    int i = findTSSIAt(c.screenPointToCanvasPoint({ g_mouseX, g_mouseY }));
    for (int x = 0; x < loadedImgs.size(); x++) {
        tempSplitSessionImage& tssi = loadedImgs[x];
        if (i == x) {
            tssi.namePopupTimer.startIfNotStarted();
        }
        else {
            tssi.namePopupTimer.stop();
        }
        XY screenPos = c.canvasPointToScreenPoint(tssi.position);
        localttp.addTooltip(Tooltip{ 
            xySubtract(screenPos, {0,30}),
            tssi.calcRelativePath,
            {0xff, 0xff, 0xff, 0xff},
            XM1PW3P1(tssi.namePopupTimer.percentElapsedTime(500))
        });
    }

    localttp.renderAll();
}

void SplitSessionEditor::tryLoadExistingSplitSession(PlatformNativePathString path)
{
    auto ssn = loadSplitSessionData(path);
    if (ssn.set) {
        loadedImgs.clear();
        
        for (auto& ssi : ssn.images) {
            tempSplitSessionImage a;
            a.loadedLayer = ssi.loadedLayer;
            a.dims = ssi.dimensions;
            a.fullOriginalPath = ssi.fileName;
            a.position = ssi.positionInOverallImage;
            loadedImgs.push_back(a);
        }
        recalcCanvasDimensions();
        recalcRelativePaths();
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to load existing split session file"));
        return;
    }
}

void SplitSessionEditor::setupWidgets()
{
    c = Canvas();
    c.dimensions = { 0,0 };

    c.recenter();

    UILabel* l = new UILabel("Split sessions let you edit multiple images on one canvas.");
    l->position = {5, 50};
    l->color = { 0xB0,0xB0,0xB0,0xff };
    wxsManager.addDrawable(l);

    guideLabel = new UILabel("");
    guideLabel->position = { 5,90 };
    wxsManager.addDrawable(guideLabel);

    subImagesPanel = new ScrollingPanel();
    subImagesPanel->position = { 25, 120 };
    subImagesPanel->wxWidth = 350;
    subImagesPanel->wxHeight = 450;
    subImagesPanel->scrollVertically = true;
    subImagesPanel->scrollHorizontally = false;
    subImagesPanel->clipElementsToSize = false;
    wxsManager.addDrawable(subImagesPanel);

    navbar = new ScreenWideNavBar(this, {
        {
            SDL_SCANCODE_F,
            {
                "File",
                {SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, SDL_SCANCODE_C},
                {
                    {SDL_SCANCODE_C, { "Close",
                        [this]() {
                            closeThisScreen();
                        }
                    }},
                    {SDL_SCANCODE_S, { "Save",
                        [this]() {
                            this->trySave();
                        }
                    }},
                    {SDL_SCANCODE_D, { "Save and open in editor",
                        [this]() {
                            this->trySave(true);
                        }
                    }},
                    {SDL_SCANCODE_A, { "Add file...",
                        [this]() {
                            this->promptAddImageToSplitSession();
                        }
                    }},
                    {SDL_SCANCODE_F, { "Set output file location",
                        [this]() {
                            platformTrySaveOtherFile(this, {{".voidspsn", "Split session file"}}, "set split session file location", 0);
                        }
                    }}
                },
                g_iconNavbarTabFile
            }
        }
    }, {SDL_SCANCODE_F});
    wxsManager.addDrawable(navbar);

    recalcRelativePaths();
}

int SplitSessionEditor::findTSSIAt(XY canvasPosition)
{
    for (int x = loadedImgs.size(); x-- > 0;) {
        tempSplitSessionImage& tssi = loadedImgs[x];
        if (pointInBox(canvasPosition, { tssi.position.x, tssi.position.y,
                                        tssi.dims.x, tssi.dims.y })) {
            return x;
        }
    }
    return -1;
}

void SplitSessionEditor::recalcCanvasDimensions()
{
    XY maxDims = { 0,0 };
    for (tempSplitSessionImage &tssi : loadedImgs) {
        if (tssi.position.x + tssi.dims.x > maxDims.x) {
            maxDims.x = tssi.position.x + tssi.dims.x;
        }
        if (tssi.position.y + tssi.dims.y > maxDims.y) {
            maxDims.y = tssi.position.y + tssi.dims.y;
        }
    }
    c.dimensions = maxDims;
}

void SplitSessionEditor::recalcRelativePaths()
{
    guideLabel->setText(outputSPSNFilePath.size() > 0 ? outputSPSNFilePath : "No output file set.");
    for (tempSplitSessionImage& tssi : loadedImgs) {
        tssi.calcRelativePath = evalRelativePath(outputSPSNFilePath, tssi.fullOriginalPath);
        std::replace(tssi.calcRelativePath.begin(), tssi.calcRelativePath.end(), '\\', '/');
    }

    populateSubImagesList();
}

void SplitSessionEditor::populateSubImagesList()
{
    int y = 0;
    subImagesPanel->subWidgets.freeAllDrawables();
    for (int x = 0; x < loadedImgs.size(); x++) {
        tempSplitSessionImage& tssi = loadedImgs[x];
        UILabel* lbl = new UILabel();
        lbl->setText("-- " + tssi.calcRelativePath);
        lbl->position = {0, y};
        y += 30;
        //btn->wxWidth = 250;
        //btn->wxHeight = 30;
        //btn->setCallbackListener(x, this);
        subImagesPanel->subWidgets.addDrawable(lbl);
    }

    UILabel* lbl = new UILabel(" + [Drag images in to add them to the split session]");
    lbl->position = { 0, y };
    subImagesPanel->subWidgets.addDrawable(lbl);
    y += 30;
}

XY SplitSessionEditor::findNextAvailablePosition()
{
    XY p = { 0,0 };

    for (tempSplitSessionImage& tssi : loadedImgs) {
        if (tssi.position.x + tssi.dims.x > p.x) {
            p.x = tssi.position.x + tssi.dims.x;
        }
    }

    return p;
}

void SplitSessionEditor::tryAddFile(std::string path)
{
    FileImporter* usedImporter = NULL;
    MainEditor* newSsn = loadAnyIntoSession(path, &usedImporter);
    if (newSsn != NULL) {
        if (usedImporter->getCorrespondingExporter() != NULL) {
            tempSplitSessionImage tssi;
            tssi.loadedLayer = newSsn->flattenImage();
            tssi.dims = { tssi.loadedLayer->w, tssi.loadedLayer->h };
            tssi.fullOriginalPath = path;
            tssi.position = findNextAvailablePosition();
            loadedImgs.push_back(tssi);
            recalcCanvasDimensions();
            recalcRelativePaths();
        }
        else {
            g_addNotification(ErrorNotification("Error", "This format cannot be used in split sessions."));
        }
        delete newSsn;
    }
    else {
        g_addNotification(ErrorNotification("Error", "Failed to load the image"));
    }

}

void SplitSessionEditor::calcMousePixelPos(XY onScreenPos)
{
    mousePixelPos = c.screenPointToCanvasPoint(onScreenPos);
}

bool SplitSessionEditor::trySave(bool openSession)
{
    if (loadedImgs.size() == 0) {
        g_addNotification(ErrorNotification("Error", "No images added."));
        return false;
    }
    if (outputSPSNFilePath.size() > 0) {
        recalcRelativePaths();

        std::ofstream file(outputSPSNFilePath);
        if (file.good() && file.is_open()) {
            file << "voidsprite split session file v0\n";
            for (tempSplitSessionImage &tssi : loadedImgs) {
                file << "#:" << tssi.calcRelativePath << "|" << tssi.position.x << "|" << tssi.position.y << "\n";
            }
            file.close();
            g_addNotification(SuccessNotification("Success", "Split session file saved."));
            if (openSession) {
                MainEditor* newSsn = loadAnyIntoSession(outputSPSNFilePath);
                if (newSsn != NULL) {
                    g_addScreen(newSsn);
                    closeNextTick = true;
                }
                else {
                    g_addNotification(ErrorNotification("Error", "Failed to open split session."));
                }
            }
            return true;
        }
        else {
            g_addNotification(ErrorNotification("Error", "Failed to open output file."));
        }
    }
    else {
        g_addNotification(ErrorNotification("Error", "No output file path set."));
    }
    return false;
}

void SplitSessionEditor::promptAddImageToSplitSession() 
{
    PopupFilePicker::PlatformAnyImageWithMatchingExporterImportDialog(this, TL("vsp.popup.addtosplitsession"), EVENT_SPLITSESSION_ADDIMAGE);
}
