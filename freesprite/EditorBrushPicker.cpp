#include "EditorBrushPicker.h"
#include "brush/BaseBrush.h"
#include "maineditor.h"
#include "UILabel.h"

EditorBrushPicker::EditorBrushPicker(MainEditor* caller) {
    this->caller = caller;

    int sc = g_config.compactEditor ? 2 : 1;

    wxWidth = 260 *sc;
    wxHeight = 210 * sc;
    borderColor = visualConfigHexU32("ui/panel/border");

    patternPanelBtn = new UIButton();
    
    patternPanelBtn->text = ">";
    patternPanelBtn->wxWidth = 40 * sc;
    patternPanelBtn->wxHeight = 24 * sc;
    patternPanelBtn->position = { wxWidth - patternPanelBtn->wxWidth - 10, 10 };
    patternPanelBtn->tooltip = "Open pattern menu";
    patternPanelBtn->icon = g_patterns[0]->cachedIcon;
    patternPanelBtn->setCallbackListener(EVENT_BRUSHPICKER_TOGGLE_PATTERN_MENU, this);
    subWidgets.addDrawable(patternPanelBtn);

    editorReplaceBtn = new UIButton();
    editorReplaceBtn->text = "R";
    editorReplaceBtn->wxWidth = 24 * sc;
    editorReplaceBtn->wxHeight = 24 * sc;
    editorReplaceBtn->position = { wxWidth - 20 - editorReplaceBtn->wxWidth - patternPanelBtn->wxWidth, 10 };
    editorReplaceBtn->tooltip = "Replace mode\nPixels will only be drawn on opaque areas";
    editorReplaceBtn->setCallbackListener(EVENT_MAINEDITOR_TOGGLEREPLACE, this);
    subWidgets.addDrawable(editorReplaceBtn);

    patternMenuPanel = new Panel();
    patternMenuPanel->enabled = false;
    patternMenuPanel->position = { wxWidth + 30, 0 };
    subWidgets.addDrawable(patternMenuPanel);

    UILabel* lbl = new UILabel("PATTERNS");
    lbl->position = { 0, 5 };
    patternMenuPanel->subWidgets.addDrawable(lbl);

    editorInvPatternBtn = new UIButton();
    editorInvPatternBtn->position = { 160, 5 };
    editorInvPatternBtn->text = "I";
    editorInvPatternBtn->wxWidth = 24 * sc;
    editorInvPatternBtn->wxHeight = 24 * sc;
    editorInvPatternBtn->tooltip = "Invert pattern\nThe pattern will be inverted.";
    editorInvPatternBtn->setCallbackListener(EVENT_MAINEDITOR_TOGGLEINVERTPATTERN, this);
    patternMenuPanel->subWidgets.addDrawable(editorInvPatternBtn);

    patternMenu = new ScrollingPanel();
    patternMenu->scrollHorizontally = false;
    patternMenu->scrollVertically = true;
    patternMenu->wxWidth = 30 * 6 * sc + 20;
    patternMenu->wxHeight = 130 * sc;
    patternMenu->position = { 0,35 };
    patternMenu->bgColor = Fill::Solid(0x80000000);
    patternMenuPanel->subWidgets.addDrawable(patternMenu);


    //create brush buttons
    XY origin = { 5,40 * sc };

    ScrollingPanel* brushPanel = new ScrollingPanel();
    brushPanel->position = origin;
    brushPanel->wxWidth = wxWidth - 10;
    brushPanel->wxHeight = wxHeight - origin.y - 5;
    brushPanel->bgColor = Fill::None();
    subWidgets.addDrawable(brushPanel);

    origin = { 0,0 };

    XY current = origin;
    XY currentSection = { 0,0 };
    int horizontalToolsPerSection = 4;

    std::map<u16, std::vector<std::pair<int, BaseBrush*>>> sectionMap;
    for (int x = 0; x < g_brushes.size(); x++) {
        BaseBrush* brush = g_brushes[x];
        XY sec = brush->getSection();
        u16 sectionCode = (sec.x << 8) + sec.y;
        sectionMap[sectionCode].push_back({ x, brush });
    }
    brushButtons.resize(g_brushes.size());

    int i = 0;
    int buttonXSpace = 30 * sc;
    for (auto& section : sectionMap) {
        for (auto& indexAndBrush : section.second) {
            int toolIndex = indexAndBrush.first;
            BaseBrush* brush = indexAndBrush.second;

            UIButton* newBtn = new UIButton();
            XY nextSection = brush->getSection();
            if (!xyEqual(currentSection, nextSection)) {
                if (currentSection.x != nextSection.x) {
                    current.y = origin.y;
                    current.x = (buttonXSpace * horizontalToolsPerSection + 10) * nextSection.x + origin.x;
                }
                else {
                    current.x = (buttonXSpace * horizontalToolsPerSection + 10) * currentSection.x + origin.x;
                    current.y += 38 * sc;
                }
                currentSection = brush->getSection();
                i = 0;
            }
            if (i++ >= horizontalToolsPerSection) {
                current.x = (buttonXSpace * horizontalToolsPerSection + 10) * currentSection.x + origin.x;
                current.y += 30 * sc;
                i = 0;
            }

            newBtn->position = current;
            current.x += buttonXSpace;
            newBtn->icon = brush->cachedIcon;
            newBtn->tooltip = brush->getName() + (brush->getTooltip() != "" ? ("\n" + brush->getTooltip()) : "");
            //newBtn->text = brush->getName();
            newBtn->wxWidth = 26 * sc;
            newBtn->wxHeight = 26 * sc;
            newBtn->colorBorder = brush->overrideRightClick() ? SDL_Color{ 0x00,0xae,0xff,0x80 } : SDL_Color{ 0xff, 0xff, 0xff, 0x50 };
            newBtn->setCallbackListener(100 + toolIndex, this);
            brushButtons[toolIndex] = newBtn;
            brushPanel->subWidgets.addDrawable(newBtn);
        }
    }
    updateActiveBrushButton(caller->currentBrush);

    //create pattern buttons
    int px = 5;
    int py = 0;
    i = 0;
    int patternButtonWSpace = 26 * sc;
    for (Pattern*& pattern : g_patterns) {
        UIButton* newBtn = new UIButton();
        if (px + patternButtonWSpace > patternMenu->wxWidth) {
            py += patternButtonWSpace;
            px = 5;
        }
        newBtn->position = XY{ px, py };
        px += patternButtonWSpace;
        newBtn->icon = pattern->cachedIcon;
        newBtn->tooltip = pattern->getName();
        //newBtn->text = brush->getName();
        newBtn->wxWidth = 24*sc;
        newBtn->wxHeight = 24*sc;
        newBtn->setCallbackListener(200 + i++, this);
        patternButtons.push_back(newBtn);
        patternMenu->subWidgets.addDrawable(newBtn);
    }
}

void EditorBrushPicker::render(XY position)
{
    if (!enabled) {
        return;
    }
    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };

    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, focused ? 0xa0 : 0x90 };
    SDL_Color colorBG2 = { 0x10, 0x10, 0x10, focused ? 0xa0 : 0x90 };
    renderGradient(r, sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1));
    if (thisOrParentFocused()) {
        renderFocusBorder(position, SDL_Color{ 255,255,255,255 });
    }

    patternMenuPanel->position = xyAdd({ wxWidth, 0 }, XY{ (int)(30 * XM1PW3P1(patternMenuTimer.percentElapsedTime(200))) , 0 });

    DraggablePanel::render(position);
}

void EditorBrushPicker::eventButtonPressed(int evt_id)
{
    if (evt_id == EVENT_BRUSHPICKER_TOGGLE_PATTERN_MENU) {
        patternMenuPanel->enabled = !patternMenuPanel->enabled;
        patternMenuTimer.start();
        patternPanelBtn->text = patternMenuPanel->enabled ? "<" : ">";
    }
    else if (evt_id == EVENT_MAINEDITOR_TOGGLEREPLACE) {
        caller->replaceAlphaMode = !caller->replaceAlphaMode;
        editorReplaceBtn->fill = caller->replaceAlphaMode ? Fill::Gradient(0xD0000000, 0xD0000000, 0x40FFFFFF, 0x40FFFFFF) : Fill::Solid(0xD0000000);
    }
    else if (evt_id == EVENT_MAINEDITOR_TOGGLEINVERTPATTERN) {
        caller->invertPattern = !caller->invertPattern;
        editorInvPatternBtn->fill = caller->invertPattern ? Fill::Gradient(0xD0000000, 0xD0000000, 0x40FFFFFF, 0x40FFFFFF) : Fill::Solid(0xD0000000);
    }
    else if (evt_id >= 200) {
        caller->currentPattern = g_patterns[evt_id - 200];
        patternMenu->subWidgets.forceUnfocus();
        updateActivePatternButton(caller->currentPattern);
    }
    else if (evt_id >= 100) {
        caller->setActiveBrush(g_brushes[evt_id - 100]);
        updateActiveBrushButton(evt_id - 100);
    }
}

void EditorBrushPicker::updateActiveBrushButton(int id)
{
    for (UIButton*& bbtn : brushButtons) {
        bool hasRightClickAction = g_brushes[bbtn->callback_id - 100]->overrideRightClick();
        bbtn->fill = hasRightClickAction ? Fill::Gradient(0xD0000000, 0xD0000000, 0xD0000000, 0xD000AEFF)
                                         : Fill::Gradient(0xD0000000, 0xD0000000, 0xD0000000, 0xD0505050);
    }
    if (id >= 0) {
        brushButtons[id]->fill = Fill::Gradient(0x80FFFFFF, 0x40FFFFFF, 0x40FFFFFF, g_brushes[id]->overrideRightClick() ? 0xD000AEFF : 0x40000000); //SDL_Color{ 0xff,0xff,0xff,0x40 };
    }
}

void EditorBrushPicker::updateActiveBrushButton(BaseBrush* id)
{
    for (int x = 0; x < g_brushes.size(); x++) {
        if (g_brushes[x] == id) {
            updateActiveBrushButton(x);
            return;
        }
    }
    updateActiveBrushButton(-1);
}

void EditorBrushPicker::updateActivePatternButton(Pattern* p)
{
    for (int x = 0; x < patternButtons.size(); x++) {
        patternButtons[x]->fill = p == g_patterns[x] ? Fill::Gradient(0x80000000, 0x80000000, 0x80FFFFFF, 0x80FFFFFF)
                                                     : FILL_BUTTON_CHECKED_DEFAULT;
    }
    patternPanelBtn->icon = p->cachedIcon;
}
