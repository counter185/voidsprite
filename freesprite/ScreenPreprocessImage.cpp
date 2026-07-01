#include "json/json.hpp"

#include "ScreenPreprocessImage.h"
#include "BaseFilter.h"
#include "Layer.h"
#include "ScrollingPanel.h"
#include "UILabel.h"
#include "PopupApplyFilter.h"
#include "PopupPickColor.h"
#include "ScreenWideNavBar.h"
#include "background_operation.h"
#include "UIDropdown.h"
#include "UIButton.h"
#include "UITextField.h"
#include "UICheckbox.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "UIStackPanel.h"
#include "UIColorPicker.h"
#include "io/io_png.h"
#include "PopupContextMenu.h"
#include "UIColorInputField.h"
#include "maineditor.h"

ScreenPreprocessImage::ScreenPreprocessImage(Layer* l)
{
    target = l;
    c.dimensions = { l->w, l->h };
    c.recenter();

    filterListPanel = new PanelFilterList(this);
    filterListPanel->position = { 10, 40 };
    filterListPanel->onFilterValueChangedCallback = [this]() {
        changes = true;
    };
    wxsManager.addDrawable(filterListPanel);

    navbar = new ScreenWideNavBar(this, {
        {SDL_SCANCODE_F,
            makeNavbarSection(
                TL("vsp.nav.file"), NULL,
                {
                    //{SDL_SCANCODE_G, {"Generate", [this]() { saveToTempFile(); }}},
                    {SDL_SCANCODE_S, {TL("vsp.preprocess.nav.openineditor"), [this]() { openInEditor(); }}},
                    {SDL_SCANCODE_P, {TL("vsp.preprocess.nav.savepreset"), [this]() { promptSaveStackPreset(); }}},
                    {SDL_SCANCODE_O, {TL("vsp.preprocess.nav.loadpreset"), [this]() { promptLoadStackPreset(); }}},
                    {SDL_SCANCODE_X, {TL("vsp.cmn.close"), [this]() {
                        g_startNewMainThreadOperation([this]() {
                            g_closeScreen(this);
                        });
                    }}},
                }
            )
        }
    }, {SDL_SCANCODE_F});
    wxsManager.addDrawable(navbar);

#if MULTITHREAD_IMAGE_PROCESSING
    threadRunning = true;
    imageProcessorThread = new std::thread(&ScreenPreprocessImage::imageProcessThread, this);
#endif
}

ScreenPreprocessImage::~ScreenPreprocessImage()
{
    threadRunning = false;
    if (imageProcessorThread != NULL) {
        imageProcessorThread->join();
    }
    delete imageProcessorThread;
    delete target;
}

void ScreenPreprocessImage::render()
{
    renderWithBlurPanelsIfEnabled([this]() {this->renderCanvas(); });

    BaseScreen::render();

    drawForeground();
}

void ScreenPreprocessImage::renderCanvas()
{
    renderBackground();

    SDL_Rect layerRender = c.getCanvasOnScreenRect();

    if (processedImage != NULL) {
        processedImageMutex.lock();
        c.dimensions = { processedImage->w * downscaleW, processedImage->h * downscaleH };
        layerRender = c.getCanvasOnScreenRect();
        processedImage->render(layerRender);
        wasLocked = true;
        processedImageMutex.unlock();
    }

    if (c.scale > 8) {

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x30);
        c.drawTileGrid({ 1,1 });
    }

    if (downscaleW > 1 && downscaleH > 1) {
        SDL_Color gridColor = uint32ToSDLColor(filterListPanel->gridColorField->getColor());
        SDL_SetRenderDrawColor(g_rd, gridColor.r, gridColor.g, gridColor.b, gridColor.a);
        c.drawTileGrid({ downscaleW, downscaleH });
    }

    for (int x = 1; x < 5; x++) {
        SDL_Rect layerOutline = offsetRect(layerRender, x);
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255 / x);
        SDL_RenderDrawRect(g_rd, &layerOutline);
    }
}

void ScreenPreprocessImage::tick()
{
    BaseScreen::tick();
#if !MULTITHREAD_IMAGE_PROCESSING
    if (changes) {
        updateProcessedImage();
        changes = false;
    }
#endif
}

void ScreenPreprocessImage::defaultInputAction(SDL_Event evt)
{
    if (!c.takeInput(evt)) {
        if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN && evt.button.button == SDL_BUTTON_RIGHT) {
            std::vector<NamedOperation> ops;
            for (auto& p : builtinPresets) {
                ops.push_back({ p.name, [this, p]() { applyPreset(p); } });
            }
            g_addPopup(new PopupContextMenu(ops));
        }
    }
}

void ScreenPreprocessImage::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex) {
    if (evt_id == 0) {
        std::vector<FilterPreset> fps;
        for (auto& pp : filters) {
            FilterPreset fp{};
            fp.filterID = pp.filter->id();
            fp.options = pp.params.buildParameterMap();
            fps.push_back(fp);
        }
        std::string s = PreprocessPreset("Custom preprocessing preset", fps).serialize();
        FILE* f = platformOpenFile(name, convertStringOnWin32("w"));
        fwrite(s.c_str(), s.size(), 1, f);
        fclose(f);
    }
}

void ScreenPreprocessImage::eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex) {
    if (evt_id == 0) {
        try {
			FILE* f = platformOpenFile(name, convertStringOnWin32("r"));
			fseek(f, 0, SEEK_END);
			size_t size = ftell(f);
			fseek(f, 0, SEEK_SET);
			std::string s;
			s.resize(size);
			fread(s.data(), 1, size, f);
			fclose(f);
            PreprocessPreset p = PreprocessPreset::deserialize(s);
			applyPreset(p);
        }
        catch (...) {}
    }
}

void ScreenPreprocessImage::renderBackground()
{
    Fill::Gradient(0xFF000000, 0xFF000000, 0xFF000000, 0xFF303030).fill({ 0, 0, g_windowW, g_windowH });
}

void ScreenPreprocessImage::drawForeground()
{
    drawBottomBar();

    g_fnt->RenderString(frmt("{}x{} > {}x{}", target->w, target->h, processedSize.x, processedSize.y), 2, g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
}

void ScreenPreprocessImage::addFilter(BaseFilter* f)
{
    std::lock_guard<std::recursive_mutex> l(filterListMutex);
    filters.push_back(PreprocessImageFilter{ f, f->getParameters() });
    filterListPanel->populateFilters(&filters);
    changes = true;
}

void ScreenPreprocessImage::removeFilter(int idx)
{
    std::lock_guard<std::recursive_mutex> l(filterListMutex);
    filters.erase(filters.begin() + idx);
    filterListPanel->populateFilters(&filters);
    changes = true;
}

void ScreenPreprocessImage::moveFilter(int from, int offs)
{
    std::lock_guard<std::recursive_mutex> l(filterListMutex);
    if (from + offs >= 0 && from + offs < filters.size()) {
        auto f = filters[from];
        filters.erase(filters.begin() + from);
        filters.insert(filters.begin() + from + offs, f);
        filterListPanel->populateFilters(&filters);
        changes = true;
    }
}

void ScreenPreprocessImage::clearFilters()
{
    std::lock_guard<std::recursive_mutex> l(filterListMutex);
    filters.clear();
    filterListPanel->populateFilters(&filters);
    changes = true;
}

void ScreenPreprocessImage::applyPreset(PreprocessPreset preset)
{
    std::lock_guard<std::recursive_mutex> l(filterListMutex);
    clearFilters();
    for (auto& p : preset.filters) {
        BaseFilter* f = g_getFilterByID(p.filterID);
        if (f != NULL) {
            addFilter(f);
        }
        filters.back().params.setParametersFromParameterMap(p.options);// PopupApplyFilter::setDefaultParametersFromPreset(parameters, p);
    }
    filterListPanel->populateFilters(&filters);
    changes = true;
}

void ScreenPreprocessImage::openInEditor()
{
    g_startNewOperation([this]() {
        while (threadProcessing) {
            SDL_Delay(100);
        }
        filterListMutex.lock();
        g_addScreen(new MainEditor(processedImage->copyCurrentVariant()));
        filterListMutex.unlock();
    });
}

void ScreenPreprocessImage::promptLoadStackPreset() {
    platformTryLoadOtherFile(this, {{".preprocesspreset", "Preprocess preset"}}, "save preprocess preset", 0);
}

void ScreenPreprocessImage::promptSaveStackPreset() {
    platformTrySaveOtherFile(this, {{".preprocesspreset", "Preprocess preset"}}, "save preprocess preset", 0);
}

std::string ScreenPreprocessImage::saveToTempFile()
{
    std::string newTempPath = "temp.png";
    writePNG(convertStringOnWin32(newTempPath), processedImage);
    return newTempPath;
}

void ScreenPreprocessImage::updateProcessedImage()
{
    Layer* copy = target->copyCurrentVariant();

    filterListMutex.lock();
    auto flcopy = filters;
    XY downscale = { downscaleW, downscaleH };
    bool shouldPredownscale = preDownscale;
    filterListMutex.unlock();

    if (shouldPredownscale) {
        if (copy != NULL) {
            XY copySize = { copy->w, copy->h };
            if ((downscale.x > 1 || downscale.y > 1) && copySize.x >= downscale.x && copySize.y >= downscale.y) {
                Layer* o = copy;
                XY targetSize = { copySize.x / downscale.x, copySize.y / downscale.y };
                processedSize = targetSize;
                copy = copy->copyCurrentVariantScaled(targetSize);
                delete o;
            }
        }
    }

    for (auto& f : flcopy) {
        Layer* o = copy;
        copy = f.filter->run(copy, &f.params);
        delete o;

        if (copy == NULL || (MULTITHREAD_IMAGE_PROCESSING && !threadRunning)) {
            break;
        }
    }

    if (!shouldPredownscale) {
        if (copy != NULL) {
            XY copySize = { copy->w, copy->h };
            if ((downscale.x > 1 || downscale.y > 1) && copySize.x >= downscale.x && copySize.y >= downscale.y) {
                Layer* o = copy;
                XY targetSize = { copySize.x / downscale.x, copySize.y / downscale.y };
                processedSize = targetSize;
                copy = copy->copyCurrentVariantScaled(targetSize);
                delete o;
            }
        }
    }

    if (quantizeMode == QUANTIZEMODE_TOPALETTE) {
        quantizeToCurrentPalette(copy);
    }

    processedImageMutex.lock();
    if (processedImage != NULL) {
        Layer* pImage = processedImage;
        g_startNewMainThreadOperation([pImage]() {
            delete pImage;
        });
        processedImage = NULL;
    }
    processedImage = copy;
    processedImageMutex.unlock();
}

void ScreenPreprocessImage::imageProcessThread()
{
    while (threadRunning) {
        if (changes) {
            changes = false;
            threadProcessing = true;
            updateProcessedImage();
            threadProcessing = false;
        }

        SDL_Delay(100);
    }
}

void ScreenPreprocessImage::quantizeToCurrentPalette(Layer* target)
{
    quantizePaletteMutex.lock();
    std::vector<u32> paletteNow = quantizePalette;
    quantizePaletteMutex.unlock();
    std::vector<XYZd> paletteXYZd;
    std::transform(paletteNow.begin(), paletteNow.end(), std::back_inserter(paletteXYZd), [](u32 u) {
        auto sc = uint32ToSDLColor(u);
        return XYZd{ sc.r / 255.0, sc.g/255.0, sc.b/255.0 };
    });

    if (!paletteXYZd.empty()) {
        for (int y = 0; y < target->h; y++) {
            for (int x = 0; x < target->w; x++) {
                u32 c = target->getPixelAt({ x, y });

                int targetInd = 0;
                auto sc = uint32ToSDLColor(c);
                if (sc.a > 0) {
                    XYZd colorPos = XYZd{ sc.r / 255.0, sc.g / 255.0, sc.b / 255.0 };
                    double distanceNow = xyzdDistance(colorPos, paletteXYZd[0]);
                    for (int i = 1; i < paletteXYZd.size(); i++) {
                        double nextDistance = xyzdDistance(colorPos, paletteXYZd[i]);
                        if (nextDistance < distanceNow) {
                            targetInd = i;
                            distanceNow = nextDistance;
                            if (distanceNow == 0) {
                                break;
                            }
                        }
                    }

                    u32 setColor = modAlpha(paletteNow[targetInd], 255);
                    target->setPixel({ x,y }, setColor);
                }
            }
        }
    }
}

PanelFilterList::PanelFilterList(ScreenPreprocessImage* caller)
{
    parent = caller;
    const int b = 8;
    sizeToContent = true;

    setupDraggable();

    UITextField* txDownscaleTileW = new UITextField();
    UITextField* txDownscaleTileH = new UITextField();
    txDownscaleTileH->isNumericField = txDownscaleTileW->isNumericField = true;
    txDownscaleTileH->position = { b + 60 + 10, 50 }; 
    txDownscaleTileH->wxWidth = txDownscaleTileW->wxWidth = 50;
    txDownscaleTileW->position = { b + 10, 50 };
    txDownscaleTileH->setText("1");
    txDownscaleTileW->setText("1");
    txDownscaleTileW->onTextChangedCallback = [this, caller, txDownscaleTileW](UITextField* tf, std::string val) {
        if (val != "" && val != "0") {
            int v = std::stoi(val);
            caller->downscaleW = v;
            caller->changes = true;
        }
    };
    txDownscaleTileH->onTextChangedCallback = [this, caller, txDownscaleTileH](UITextField* tf, std::string val) {
        if (val != "" && val != "0") {
            int v = std::stoi(val);
            caller->downscaleH = v;
            caller->changes = true;
        }
    };

    UICheckbox* predownscaleCheckbox = new UICheckbox(TL("vsp.preprocess.downscalefirst"), parent->preDownscale);
    predownscaleCheckbox->onStateChangeCallback = [this, caller](UICheckbox* cb, bool state) {
        parent->preDownscale = state;
        parent->changes = true;
    };
    predownscaleCheckbox->position = { txDownscaleTileH->position.x + txDownscaleTileH->wxWidth + 10, 50 };
    wxsTarget().addDrawable(predownscaleCheckbox);
        
    wxsTarget().addDrawable(txDownscaleTileH);
    wxsTarget().addDrawable(txDownscaleTileW);

    gridColorField = new UIColorInputField(true);
    gridColorField->position = { predownscaleCheckbox->position.x + predownscaleCheckbox->getDimensions().x + 20, predownscaleCheckbox->position.y };
    gridColorField->setColor(0x10FFFFFF);
    wxsTarget().addDrawable(gridColorField);

    wxWidth = 600;
    wxHeight = 350;
    listPanel = new ScrollingPanel();
    listPanel->position = { b,20 + 30 + 50 + b };
    listPanel->wxWidth = wxWidth - b*2;
    listPanel->wxHeight = 234;
    wxsTarget().addDrawable(listPanel);

    if (g_compositeFilters.empty()) {
        loadCompositeFilters();
    }
    std::vector<BaseFilter*> filters = joinVectors({ g_filters, g_compositeFilters });

    std::vector<std::string> filterNames;
    std::transform(filters.begin(), filters.end(), std::back_inserter(filterNames), [](BaseFilter* f) { return f->name(); });

    UIDropdown* filterDropdown = new UIDropdown(filterNames);
    filterDropdown->text = TL("vsp.preprocess.addfilter");
    filterDropdown->position = { 10, 10 };
    filterDropdown->onDropdownItemSelectedCallback = [this, caller, filters](UIDropdown* dd, int idx, std::string name) {
        caller->addFilter(filters[idx]);
    };
    wxsTarget().addDrawable(filterDropdown);

    quantizeModeDropdown = new UIDropdown(std::vector<std::string>{
        TL("vsp.preprocess.matchmode.manual"),
        TL("vsp.preprocess.matchmode.defined")
    });
    
    colorListPanel = new ScrollingPanel();
    colorListPanel->wxWidth = wxWidth - b * 2;
    colorListPanel->wxHeight = 100;
    
    UIStackPanel* s = UIStackPanel::Vertical(4, { quantizeModeDropdown, colorListPanel });
    s->position = { b, 20 + 30 + 50 + 234 + 5 + b };
    wxsTarget().addDrawable(s);

    quantizeModeDropdown->onDropdownItemSelectedCallback = [this, caller, s](UIDropdown* dd, int idx, std::string name) {
        setQuantizeModeFromUI(caller, idx);
        s->recalculateLayout();
    };

    setQuantizeModeFromUI(caller, QUANTIZEMODE_MANUAL);
    s->recalculateLayout();
    populateColorList();
}

void PanelFilterList::setQuantizeModeFromUI(ScreenPreprocessImage* caller, int idx)
{
    caller->setQuantizeMode((QuantizeMode)idx);
    quantizeModeDropdown->text = quantizeModeDropdown->items[idx];
    colorListPanel->enabled = (idx == QUANTIZEMODE_TOPALETTE);
}

void PanelFilterList::populateFilters(std::vector<PreprocessImageFilter>* filters)
{
    listPanel->subWidgets.freeAllDrawables();
    int y = 0;
    int i = 0;
    for (auto& f : *filters) {
        UIButton* del = new UIButton("");
        del->icon = g_iconLayerDelete;
        del->wxWidth = 30;
        del->onClickCallback = [this, i](UIButton* b) {
            parent->removeFilter(i);
        };
        UIButton* up = new UIButton("");
        up->wxWidth = 30;
        up->icon = g_iconLayerUp;
        up->onClickCallback = [this, i](UIButton* b) {
            parent->moveFilter(i, -1);
        };
        UIButton* down = new UIButton("");
        down->wxWidth = 30;
        down->icon = g_iconLayerDown;
        down->onClickCallback = [this, i](UIButton* b) {
            parent->moveFilter(i, 1);
        };

        UIStackPanel* sp = UIStackPanel::Horizontal(2, { del, up, down, Panel::Space(10,0), new UILabel(f.filter->name()) });
        sp->position = { 2, y };
        listPanel->subWidgets.addDrawable(sp);

        y += 35;
        Panel* p = f.params.generateVerticalUI([this]() {
            this->onFilterValueChangedCallback();
        });
        p->position = { 0, y };
        listPanel->subWidgets.addDrawable(p);

        y += p->getContentBoxSize().y + 40;
        i++;
    }
}

void PanelFilterList::populateColorList()
{
    colorListPanel->subWidgets.freeAllDrawables();
    
    std::vector<Drawable*> stackPanels = {};
    UIStackPanel* buttonTargetNow = UIStackPanel::Horizontal(0, {});
    int itemsInRow = 0;
    int maxItemsInRow = 8;

    int i = 0;
    parent->quantizePaletteMutex.lock();
    for (u32& c : parent->quantizePalette) {
        UIButton* b = new UIButton();
        b->fill = Fill::Solid(c | 0xFF000000);
        b->wxWidth = 50;
        b->wxHeight = 30;
        b->onClickCallback = [this, c, i](UIButton* b) {
            PopupPickColor* p = new PopupPickColor("Add color", "");
            p->colorPicker->setColorRGB(c);
            p->onColorConfirmedCallback = [this, i](PopupPickColor* pp, u32 color) {
                parent->quantizePalette[i] = color | 0xFF000000;
                populateColorList();
                parent->changes = true;
            };
        };
        b->onRightClickCallback = [this, i](UIButton*) {
            parent->quantizePaletteMutex.lock();
            parent->quantizePalette.erase(parent->quantizePalette.begin() + i);
            parent->quantizePaletteMutex.unlock();
            auto parentc = parent;
            populateColorList();
            parentc->changes = true;
        };
        buttonTargetNow->addWidget(b);
        itemsInRow++;
        if (itemsInRow >= maxItemsInRow) {
            stackPanels.push_back(buttonTargetNow);
            buttonTargetNow = UIStackPanel::Horizontal(0, {});
            itemsInRow = 0;
        }
        i++;
    }
    parent->quantizePaletteMutex.unlock();
    stackPanels.push_back(buttonTargetNow);

    UIStackPanel* finalStack = UIStackPanel::Vertical(2, stackPanels);
    finalStack->position = { 0,0 };

    UIButton* newButton = new UIButton(TL("vsp.preprocess.addcolor"));
    newButton->onClickCallback = [this](UIButton* b) {
        PopupPickColor* p = new PopupPickColor("Add color", "");
        p->onColorConfirmedCallback = [this](PopupPickColor* pp, u32 color) {
            parent->quantizePaletteMutex.lock();
            parent->quantizePalette.push_back(color | 0xFF000000);
            parent->quantizePaletteMutex.unlock();
            populateColorList();
            parent->changes = true;
        };
        g_addPopup(p);
    };
    finalStack->addWidget(newButton);

    colorListPanel->subWidgets.addDrawable(finalStack);
}

std::string PreprocessPreset::serialize() {
    nlohmann::json j = nlohmann::json::object();
    j["name"] = name;
    auto filtersObj = nlohmann::json::array();
    for (auto& subPreset : filters) {
        auto filterObj = nlohmann::json::parse(subPreset.serialize());
        filtersObj.push_back(filterObj);
    }
    j["filters"] = filtersObj;
    return j.dump();
}

PreprocessPreset PreprocessPreset::deserialize(std::string s)
{
    PreprocessPreset p;
    try {
        nlohmann::json j = nlohmann::json::parse(s);
        p.name = j["name"].get<std::string>();
        for (auto& filterObj : j["filters"]) {
            FilterPreset fp = FilterPreset::deserialize(filterObj.dump());
            p.filters.push_back(fp);
        }
    }
    catch (...) {}
	return p;
}

void loadCompositeFilters()
{
    g_compositeFilters.push_back(new CompositeFilterBackground());
}

Layer* CompositeFilterBackground::run(Layer* src, ParameterStore* options)
{
    Layer* c = copy(src);
    u32 color = options->getColorRGB("color") | 0xFF000000;// std::stoul(options["color"], NULL, 16) | 0xFF000000;
    for (int x = 0; x < c->w; x++) {
        for (int y = 0; y < c->h; y++) {
            u32 cc = c->getPixelAt({ x,y });
            c->setPixel({ x,y }, alphaBlend(color, cc));
        }
    }

    return c;
}
