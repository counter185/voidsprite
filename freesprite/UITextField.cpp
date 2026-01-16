#include "UITextField.h"
#include "FontRenderer.h"
#include "EventCallbackListener.h"
#include "TooltipsLayer.h"
#include "Notification.h"
#include "PopupContextMenu.h"

void UITextField::render(XY pos)
{
    if (!enabled) {
        return;
    }
    renderTextField(pos);
#if __ANDROID__
    if (!SDL_IsDeXMode() && focused) {
        renderOnScreenTextField();
    }
#endif
}

void UITextField::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (!enabled) {
        return;
    }
    if (evt.type == SDL_KEYDOWN) {
        switch (evt.key.scancode) {
            case SDL_SCANCODE_TAB:
                break;
            case SDL_SCANCODE_RETURN:
                if (isNumericField && numericFieldCurrentOperation != '\0') {
                    runNumericOperation();
                } else {
                    if (onTextChangedConfirmCallback != NULL) {
                        onTextChangedConfirmCallback(this, text);
                    }
                    else if (callback != NULL) {
                        callback->eventTextInputConfirm(callback_id, text);
                    }
                }
                break;
            case SDL_SCANCODE_BACKSPACE:
                if (isNumericField && numericFieldCurrentOperation != '\0') {
                    if (numericFieldOperationBuffer.empty()) {
                        numericFieldCurrentOperation = '\0';
                    } else {
                        numericFieldOperationBuffer.pop_back();
                    }
                }
                else if (!text.empty()) {
                    if (g_ctrlModifier) {
                        auto lastSpacePos = text.find_last_of(' ');
                        if (lastSpacePos == std::string::npos) {
                            lastSpacePos = 0;
                        }
                        text = text.substr(0, lastSpacePos);
                    }
                    else {
                        text = text.substr(0, text.size() - 1);
                    }

                    if (onTextChangedCallback != NULL) {
                        onTextChangedCallback(this, text);
                    }
                    else if (callback != NULL) {
                        callback->eventTextInput(callback_id, text);
                    }
                }
                break;
            case SDL_SCANCODE_DELETE:
                clearText();
                break;
            case SDL_SCANCODE_C:
                if (g_ctrlModifier) {
                    copyToClipboard();
                }
                break;
            case SDL_SCANCODE_V:
                if (g_ctrlModifier) {
                    pasteFromClipboard();
                }
                break;
        }
    }
    else if (evt.type == SDL_TEXTINPUT) {
        bool textAdded = false;
        char* nextc = (char*)evt.text.text;
        while (*nextc != '\0') {
            char c = *nextc;
            textAdded |= inputChar(c);
            nextc++;
        }
        if (textAdded) {
            if (onTextChangedCallback != NULL) {
                onTextChangedCallback(this, text);
            }
            else if (callback != NULL) {
                callback->eventTextInput(callback_id, text);
            }
        }
    }
    else if (evt.type == SDL_EVENT_TEXT_EDITING_CANDIDATES) {
        if (imeCandidates.size() == 0) {
            imeCandidatesTimer.start();
        }
        imeCandidates.clear();
        for (int x = 0; x < evt.edit_candidates.num_candidates; x++) {
            imeCandidates.push_back(evt.edit_candidates.candidates[x]);
        }
        imeCandidateIndex = evt.edit_candidates.selected_candidate;
    }
    else if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (evt.button.button == SDL_BUTTON_RIGHT) {
            g_openContextMenu({
                {TL("vsp.cmn.copy"), [this]() { copyToClipboard(); }},
                {TL("vsp.cmn.paste"), [this]() { pasteFromClipboard(); }},
                {TL("vsp.cmn.erase"), [this]() { clearText(); }},
            });
        }
    }
}

void UITextField::clearText()
{
    text = "";
    if (onTextChangedCallback != NULL) {
        onTextChangedCallback(this, text);
    }
    else if (callback != NULL) {
        callback->eventTextInput(callback_id, text);
    }
}

bool UITextField::inputChar(char c) {
    if (isNumericField) {
        if (text.empty() && c == '-' && numericAllowNegative) {
            text += c;
            return true;
        } 
        else if (numericOperations.contains(c)) {
            numericFieldOperationTimer.start();
            numericFieldCurrentOperation = c;
        }
        else if ((c >= '0' && c <= '9') || numericFieldCurrentOperation == 'x') {
            if (numericFieldCurrentOperation != '\0') {
                numericFieldOperationBuffer += c;
            } else {
                text += c;
                return true;
            }
        }
    } else {
        text += c;
        return true;
    }
    return false;
}

void UITextField::renderTextField(XY at)
{
    SDL_Rect drawrect = { at.x, at.y, wxWidth, wxHeight };
    bgFill.fill(drawrect);
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    SDL_RenderDrawRect(g_rd, &drawrect);

    if (focused) {
        double lineAnimPercent = XM1PW3P1(focusTimer.percentElapsedTime(500));
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
        drawLine(XY{ drawrect.x, drawrect.y }, XY{ drawrect.x + drawrect.w, drawrect.y }, lineAnimPercent);
        drawLine(XY{ drawrect.x, drawrect.y }, XY{ drawrect.x, drawrect.y + drawrect.h }, lineAnimPercent);
        drawLine(XY{ drawrect.x + drawrect.w, drawrect.y + drawrect.h }, XY{ drawrect.x, drawrect.y + drawrect.h }, lineAnimPercent);
        drawLine(XY{ drawrect.x + drawrect.w, drawrect.y + drawrect.h }, XY{ drawrect.x + drawrect.w, drawrect.y }, lineAnimPercent);


        SDL_SetTextInputArea(g_wd, &drawrect, 0);

        if (imeCandidates.size() > 0) {
            XY imeCandsOrigin = xyAdd(at, { 0, wxHeight });
            for (int i = 0; i < imeCandidates.size(); i++) {
                g_ttp->addTooltip(Tooltip{ imeCandsOrigin, imeCandidates[i], i == imeCandidateIndex ? SDL_Color{0,255,0,255} : SDL_Color{ 255,255,255,255 }, XM1PW3P1(imeCandidatesTimer.percentElapsedTime(400)) });
                imeCandsOrigin.y += 30;
            }

        }
    }

    if (hovered) {
        renderGradient(drawrect, 0x08FFFFFF, 0x08FFFFFF, 0x20D3F4FF, 0x20D3F4FF);
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x70);
        SDL_RenderDrawRect(g_rd, &drawrect);

        if (!tooltip.empty() && hoverTimer.percentElapsedTime(1000) == 1.0f) {
            g_ttp->addTooltip(Tooltip{ xyAdd(at, {0, wxHeight}), tooltip, {255,255,255,255}, hoverTimer.percentElapsedTime(300, 1000) });
        }
    }

    if (text.empty() && !placeholderText.empty()) {
        g_fnt->RenderString(placeholderText, at.x + 2, at.y + 2, SDL_Color{ textColor.r,textColor.g,textColor.b,(u8)(textColor.a / 4) }, fontsize);
    }

    if (!isColorField || !isValidOrPartialColor() || text.empty()) {
        XY textEP = g_fnt->RenderString(text + ((focused && numericFieldCurrentOperation == '\0') ? "_" : ""), at.x + 2, at.y + 2, SDL_Color{ textColor.r,textColor.g,textColor.b,(unsigned char)(focused ? 0xff : 0xa0) }, fontsize);

        if (numericFieldCurrentOperation != '\0') {
            textEP.x += 10;
            double separatorAnimTimer = XM1PW3P1(numericFieldOperationTimer.percentElapsedTime(400));
            SDL_SetRenderDrawColor(g_rd, 255,255,255,255);
            drawLine({textEP.x+2, at.y}, {textEP.x+2, at.y+wxHeight}, separatorAnimTimer);

            textEP.x += 10;

            textEP = g_fnt->RenderString(frmt("{} ", numericFieldCurrentOperation), textEP.x, textEP.y);
            g_fnt->RenderString(frmt("{}_", numericFieldOperationBuffer), textEP.x, textEP.y, {255,255,255,0xa0});
        }
    }
    else {
        int textPtr = 0;
        XY origin = xyAdd(at, { 2,2 });
        if (text[0] == '#') {
            origin = g_fnt->RenderString("#", origin.x, origin.y, SDL_Color{ 0x80,0x80,0x80,255 });
            textPtr++;
        }
        origin = g_fnt->RenderString(text.substr(textPtr, ixmin(2, text.size() - textPtr)), origin.x, origin.y, SDL_Color{ 255,0x32,0x32,255 }, fontsize);
        textPtr += 2;
        if (textPtr < text.size()) {
            origin = g_fnt->RenderString(text.substr(textPtr, ixmin(2, text.size() - textPtr)), origin.x, origin.y, SDL_Color{ 0x50,255,0x50,255 }, fontsize);
            textPtr += 2;
        }
        if (textPtr < text.size()) {
            origin = g_fnt->RenderString(text.substr(textPtr, ixmin(2, text.size() - textPtr)), origin.x, origin.y, SDL_Color{ 0x18,0x9A,255,255 }, fontsize);
            textPtr += 2;
        }
        if (textPtr < text.size()) {
            origin = g_fnt->RenderString(text.substr(textPtr), origin.x, origin.y, {255,255,255,255}, fontsize);
        }
        if (focused) {
            g_fnt->RenderString("_", origin.x, origin.y, {255,255,255,255}, fontsize);

        }
    }
}

void UITextField::renderOnScreenTextField()
{
    XY onScreenPos = { g_windowW / 2, g_windowH / 6 };
    onScreenPos.x -= wxWidth / 2;
    onScreenPos.y -= wxHeight / 2;

    double focusTime = focusTimer.percentElapsedTime(600);
    g_pushClip({ 0,0,g_windowW, g_windowH });
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xd0 * focusTime);
    SDL_RenderFillRect(g_rd, (SDL_Rect*)NULL);

    renderTextField(onScreenPos);
    g_popClip();
}

bool UITextField::isValidOrPartialColor()
{
    for (int x = 0; x < text.size(); x++) {
        char c = tolower(text[x]);
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c == '#' && x == 0))) {
            return false;
        }
    }
    return true;
}

void UITextField::copyToClipboard()
{
    if (SDL_SetClipboardText(text.c_str())) {
        g_addNotification(SuccessShortNotification(TL("vsp.cmn.copiedtoclipboard"), ""));
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error.clipboardcopy"), ""));
    }
}

void UITextField::pasteFromClipboard()
{
    if (char* clip = SDL_GetClipboardText()) {
        std::string t = clip;
        for (char& c : t) {
            inputChar(c);
        }
        //clean this shit up
        if (onTextChangedCallback != NULL) {
            onTextChangedCallback(this, text);
        }
        else if (callback != NULL) {
            callback->eventTextInput(callback_id, text);
        }
        SDL_free(clip);
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.clipboardtextpaste")));
    }
}

void UITextField::runNumericOperation() {
    if (isNumericField && numericFieldCurrentOperation != '\0') {
        try {
            int v1 = std::atoi(text.c_str());
            int v2 = std::atoi(numericFieldOperationBuffer.c_str());
            if (numericOperations.contains(numericFieldCurrentOperation)) {
                int result = numericOperations[numericFieldCurrentOperation](v1, numericFieldOperationBuffer);
                if (!numericAllowNegative) {
                    result = ixmax(0,result);
                }
                setText(std::to_string(result));
            } else {
                logerr(frmt("invalid operation: {}", numericFieldCurrentOperation)); 
            }
        }
        catch (std::exception& e) {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid input"));
        }
        numericFieldCurrentOperation = '\0';
        numericFieldOperationBuffer = "";
    }
}