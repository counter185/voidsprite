#pragma once
#include "globals.h"
#include "drawable.h"
#include "mathops.h"

class UITextField : public Drawable
{
protected:
    std::string text = "";
public:
    bool enabled = true;
    int fontsize = 18;
    std::string tooltip = "";
    std::string placeholderText = "";
    bool isColorField = false;
    int insertPosition = 0;
    int wxWidth = 250, wxHeight = 30;
    Fill bgFill = Fill::Solid(0xFF000000);
    SDL_Color textColor = { 0xff,0xff,0xff, 0xff };
    
    std::vector<std::string> imeCandidates;
    int imeCandidateIndex = 0;
    Timer64 imeCandidatesTimer;

    bool isNumericField = false;
    bool numericAllowNegative = false;
    Timer64 numericFieldOperationTimer;
    char numericFieldCurrentOperation = '\0';
    std::string numericFieldOperationBuffer = "";
    //feel free to throw exceptions here as they will be try caught
    std::map<char, std::function<int(int, std::string)>> numericOperations = {
        {'+', [](int v0, std::string arg) { return v0+std::stoi(arg); }},
        {'-', [](int v0, std::string arg) { return v0-std::stoi(arg); }},
        {'*', [](int v0, std::string arg) { return v0*std::stoi(arg); }},
        {'/', [](int v0, std::string arg) { int v1 = std::stoi(arg); return v1 == 0 ? 0 : v0/v1; }},
        {'%', [](int v0, std::string arg) { int v1 = std::stoi(arg); return v1 == 0 ? 0 : v0%v1; }},
        {'x', [](int v0, std::string arg) { return std::stoi(arg,0,16); }},
        {'b', [](int v0, std::string arg) { return std::stoi(arg,0,2); }},
    };
    
    std::function<void(UITextField*,std::string)> onTextChangedCallback = NULL;
    std::function<void(UITextField*,std::string)> onTextChangedConfirmCallback = NULL;

    UITextField() {}
    UITextField(std::string a) {
        setText(a);
    }

    void focusIn() override {
        Drawable::focusIn();
        SDL_SetBooleanProperty(g_props, SDL_PROP_TEXTINPUT_TYPE_NUMBER, isNumericField);
        SDL_StartTextInputWithProperties(g_wd, g_props);
    }
    void focusOut() override {
        Drawable::focusOut();
        SDL_StopTextInput(g_wd);
        imeCandidates.clear();
        runNumericOperation();
    }

    bool focusable() override { return enabled; }

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
        return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
    }
    void render(XY pos) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;
    bool focusableWithTab() override { return true; }
    bool shouldMoveToFrontOnFocus() override { return true; }
    XY getDimensions() override { return { wxWidth, wxHeight }; }

    void renderTextField(XY at);
    void renderOnScreenTextField();

    bool textEmpty() { return text.empty(); }
    std::string getText() { return text; }
    void setText(std::string t, bool runCallback = true) 
    {
        text = t;
        insertPosition = text.size();
        if (runCallback && onTextChangedCallback != NULL) {
            onTextChangedCallback(this, text);
        }
    }
    void concatToText(std::string t) {
        setText(text + t);
    }
    bool inputChar(char c);
    bool isValidOrPartialColor();
    void copyToClipboard();
    void pasteFromClipboard();
    void clearText();

    void runNumericOperation();
};

class UINumberInputField : public UITextField
{
protected:
    int* target = NULL;
    Fill defaultInputFill = Fill::Solid(0xFF000000);
    Fill invalidInputFill = Fill::Gradient(0xFF600000, 0xFF300000, 0xFF600000, 0xFF300000);
public:
    std::function<bool(int)> validateFunction = NULL;

    std::function<void(int)> valueUpdatedCallback = NULL;

    UINumberInputField(int* target)
        : target(target)
    {
        isNumericField = true;
        text = std::to_string(*target);
        onTextChangedCallback = [this](UITextField* tf, std::string newText) {
            parseInput();
        };
    }
    void parseInput()
    {
        if (target != NULL) {
            try 
            {
                int val = std::stoi(text);
                if (validateFunction == NULL || validateFunction(val)) {
                    *target = val;
                    bgFill = defaultInputFill;
                    if (valueUpdatedCallback) {
                        valueUpdatedCallback(val);
                    }
                }
                else {
                    bgFill = invalidInputFill;
                }
            }
            catch (std::exception&) 
            {
                bgFill = invalidInputFill;
            }
        }
    }

    void render(XY at) override {
        if (!focused) {
            text = std::to_string(*target);
        }
        UITextField::render(at);
    }

    void focusOut() override {
        UITextField::focusOut();
        bgFill = defaultInputFill;
    }
};