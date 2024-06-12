
#include "globals.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "BaseScreen.h"
#include "StartScreen.h"
#include "BasePopup.h"
#include "BrushFill.h"
#include "Brush1pxLinePathfind.h"
#include "BrushCircle.h"

int g_windowW = 1280;
int g_windowH = 720;
XY unscaledWindowSize = {g_windowW, g_windowH};
int renderScale = 1;
SDL_Texture* viewport = NULL;

SDL_Window* g_wd;
SDL_Renderer* g_rd;
int g_mouseX = 0, g_mouseY = 0;
TextRenderer* g_fnt;
std::vector<std::string> g_cmdlineArgs;
bool fullscreen = false;
bool g_ctrlModifier = false;
bool g_shiftModifier = false;

Timer64 screenSwitchTimer;

SDL_Texture* g_mainlogo = NULL;
SDL_Texture* g_iconLayerAdd = NULL;
SDL_Texture* g_iconLayerDelete = NULL;
SDL_Texture* g_iconLayerUp = NULL;
SDL_Texture* g_iconLayerDown = NULL;
SDL_Texture* g_iconLayerDownMerge = NULL;
SDL_Texture* g_iconEraser = NULL;
SDL_Texture* g_iconColorHSV = NULL;
SDL_Texture* g_iconColorVisual = NULL;
SDL_Texture* g_iconNavbarTabFile = NULL;
SDL_Texture* g_iconNavbarTabEdit = NULL;
SDL_Texture* g_iconNavbarTabLayer = NULL;
SDL_Texture* g_iconNavbarTabView = NULL;
SDL_Texture* g_iconComment = NULL;

std::vector<BaseBrush*> g_brushes;

std::vector<BasePopup*> popupStack;
void g_addPopup(BasePopup* a) {
    popupStack.push_back(a);
}
void g_popDisposeLastPopup(bool dispose) {
    if (dispose) {
        delete popupStack[popupStack.size() - 1];
    }
    popupStack.pop_back();
}
void g_closePopup(BasePopup* a) {
    for (int x = 0; x < popupStack.size(); x++) {
        if (popupStack[x] == a) {
            delete popupStack[x];
            popupStack.erase(popupStack.begin() + x);
        }
    }
}

int currentScreen = 0;
std::vector<BaseScreen*> screenStack;
void g_addScreen(BaseScreen* a) {
    screenStack.push_back(a);
    currentScreen = screenStack.size()-1;
    screenSwitchTimer.start();
}
void g_closeScreen(BaseScreen* screen) {
    for (int x = 0; x < screenStack.size(); x++) {
        if (screenStack[x]->isSubscreenOf() == screen) {
            g_closeScreen(screenStack[x]);
            x = 0;
        }
        if (screenStack[x] == screen) {
            delete screenStack[x];
            screenStack.erase(screenStack.begin() + x);
            if (currentScreen >= screenStack.size()) {
                currentScreen = screenStack.size() - 1;
                screenSwitchTimer.start();
            }
            x--;
        }
    }
}

//do not use
//what will happen if i do?
void g_closeLastScreen() {
    delete screenStack[screenStack.size() - 1];
    screenStack.pop_back();
}

SDL_Texture* IMGLoadToTexture(std::string path) {
    SDL_Surface* srf = IMG_Load(path.c_str());
    SDL_Texture* ret = SDL_CreateTextureFromSurface(g_rd, srf);
    SDL_FreeSurface(srf);
    return ret;
}

void UpdateViewportScaler(){
    if (viewport != NULL) {
        SDL_DestroyTexture(viewport);
    }
    g_windowW = unscaledWindowSize.x / renderScale;
    g_windowH = unscaledWindowSize.y / renderScale;
    if (renderScale == 1) {
        viewport = NULL;
        return;
    }
   
    viewport = SDL_CreateTexture(g_rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, g_windowW, g_windowH);
}

int main(int argc, char** argv)
{
    for (int arg = 1; arg < argc; arg++) {
        g_cmdlineArgs.push_back(std::string(argv[arg]));
    }

    srand(time(NULL));

    platformPreInit();
    int canInit = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    IMG_Init(IMG_INIT_AVIF | IMG_INIT_JPG | IMG_INIT_JXL | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
    g_wd = SDL_CreateWindow("void\xE2\x97\x86sprite", 50, 50, g_windowW, g_windowH, SDL_WINDOW_RESIZABLE | (_WIN32 ? SDL_WINDOW_HIDDEN : 0));
    g_rd = SDL_CreateRenderer(g_wd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    platformInit();
    SDL_SetRenderDrawBlendMode(g_rd, SDL_BLENDMODE_BLEND);

    g_mainlogo = IMGLoadToTexture("assets/mainlogo.png");
    g_iconLayerAdd = IMGLoadToTexture("assets/icon_layer_add.png");
    g_iconLayerDelete = IMGLoadToTexture("assets/icon_layer_delete.png");
    g_iconLayerUp = IMGLoadToTexture("assets/icon_layer_up.png");
    g_iconLayerDown = IMGLoadToTexture("assets/icon_layer_down.png");
    g_iconLayerDownMerge = IMGLoadToTexture("assets/icon_layer_downmerge.png");
    g_iconEraser = IMGLoadToTexture("assets/icon_eraser.png");
    g_iconColorHSV = IMGLoadToTexture("assets/icon_color_hsv.png");
    g_iconColorVisual = IMGLoadToTexture("assets/icon_color_visual.png");
    g_iconNavbarTabFile = IMGLoadToTexture("assets/tab_file.png");
    g_iconNavbarTabEdit = IMGLoadToTexture("assets/tab_edit.png");
    g_iconNavbarTabLayer = IMGLoadToTexture("assets/tab_layer.png");
    g_iconNavbarTabView = IMGLoadToTexture("assets/tab_view.png");
    g_iconComment = IMGLoadToTexture("assets/icon_message.png");

    //load brushes
    g_brushes.push_back(new Brush1x1());
    g_brushes.push_back(new Brush3pxCircle());
    g_brushes.push_back(new Brush1pxLine());
    g_brushes.push_back(new Brush1pxLinePathfind());
    g_brushes.push_back(new BrushRect());
    g_brushes.push_back(new BrushRectFill());
    g_brushes.push_back(new BrushCircle());
    g_brushes.push_back(new BrushFill());
    g_brushes.push_back(new ToolColorPicker());
    g_brushes.push_back(new ToolRectClone());
    g_brushes.push_back(new ToolSetXSymmetry());
    g_brushes.push_back(new ToolSetYSymmetry());
    g_brushes.push_back(new ToolComment());
    for (BaseBrush*& brush : g_brushes) {
        brush->cachedIcon = IMGLoadToTexture(brush->getIconPath());
    }

    //MainEditor tempMainEditor(XY{ 640,480 });

    TTF_Init();
    g_fnt = new TextRenderer();

    screenStack.push_back(new StartScreen());


    platformPostInit();

    SDL_Event evt;
    while (!screenStack.empty()) {
        while (SDL_PollEvent(&evt)) {
            switch (evt.type) {
                case SDL_QUIT:
                    //return 0;
                    break;
                case SDL_KEYDOWN:
                    if (evt.key.keysym.sym == SDLK_LEFTBRACKET) {
                        if (currentScreen != 0) {
                            currentScreen--;
                            screenSwitchTimer.start();
                        }
                    }
                    else if (evt.key.keysym.sym == SDLK_RIGHTBRACKET) {
                        if (currentScreen < screenStack.size() - 1) {
                            currentScreen++;
                            screenSwitchTimer.start();
                        }
                    }
                    else if (evt.key.keysym.sym == SDLK_F11) {
                        fullscreen = !fullscreen;
                        SDL_SetWindowFullscreen(g_wd, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                        screenSwitchTimer.start();
                    }
                    else if (evt.key.keysym.sym == SDLK_LCTRL){
                        g_ctrlModifier = true;
                    }
                    else if (evt.key.keysym.sym == SDLK_LSHIFT){
                        g_shiftModifier = true;
                    }
                    else if (evt.key.keysym.sym == SDLK_EQUALS){
                        if (g_ctrlModifier) {
                            renderScale++;
                            UpdateViewportScaler();
                        }
                    }
                    else if (evt.key.keysym.sym == SDLK_MINUS){
                        if (g_ctrlModifier){
                            if (renderScale-- <= 1){
                                renderScale = 1;
                            }
                            UpdateViewportScaler();

                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (evt.key.keysym.sym == SDLK_LCTRL){
                        g_ctrlModifier = false;
                    }
                    else if (evt.key.keysym.sym == SDLK_LSHIFT) {
                        g_shiftModifier = false;
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (evt.window.event == SDL_WINDOWEVENT_RESIZED) {
                        g_windowW = evt.window.data1;
                        g_windowH = evt.window.data2;
                        unscaledWindowSize = { g_windowW, g_windowH };
                        UpdateViewportScaler();
                    }
                    break;
                case SDL_MOUSEMOTION:
                    g_mouseX = evt.motion.x;
                    g_mouseY = evt.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    break;
            }

            if (!popupStack.empty() && popupStack[popupStack.size() - 1]->takesInput()) {
                popupStack[popupStack.size() - 1]->takeInput(evt);
            }
            else {
                if (!screenStack.empty()) {
                    screenStack[currentScreen]->takeInput(evt);
                }
            }
        }

        if (!popupStack.empty()) {
            popupStack[popupStack.size() - 1]->tick();
        }
        if (!screenStack.empty()) {
            screenStack[currentScreen]->tick();
        }

        if (viewport != NULL){
            SDL_SetRenderTarget(g_rd, viewport);
        }

        SDL_SetRenderDrawColor(g_rd, 0,0,0,255);
        SDL_RenderClear(g_rd);

        if (!screenStack.empty()) {
            screenStack[currentScreen]->render();
        }

        for (BasePopup*& popup : popupStack) {
            popup->render();
        }

        if (screenSwitchTimer.started) {
            double animTimer = XM1PW3P1(screenSwitchTimer.percentElapsedTime(800));
            if (animTimer < 1) {
                double reverseAnimTimer = 1.0 - animTimer;
                XY windowOffset = { g_windowW / 16 , g_windowH / 16 };
                SDL_Rect rect = {
                    windowOffset.x * reverseAnimTimer,
                    windowOffset.y * reverseAnimTimer,
                    g_windowW - 2 * windowOffset.x * reverseAnimTimer,
                    g_windowH - 2 * windowOffset.y * reverseAnimTimer,
                };
                SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xd0 * reverseAnimTimer);
                SDL_RenderDrawRect(g_rd, &rect);
            }
        }

        /*if (!popupStack.empty()) {
            popupStack[popupStack.size() - 1]->render();
        }*/


        //draw the screen icons
        XY screenIcons = {g_windowW, g_windowH - 10};
        screenIcons = xySubtract(screenIcons, XY{ (int)(26 * screenStack.size()), 16});
        
        for (int x = 0; x < screenStack.size(); x++) {
            BaseScreen* s = screenStack[x];
            SDL_Rect r = {
                screenIcons.x,
                screenIcons.y,
                16, 16
            };
            SDL_SetRenderDrawColor(g_rd, 255, 255, 255, x == currentScreen ? 0x80 : 0x20);
            SDL_RenderFillRect(g_rd, &r);
            screenIcons.x += 26;
        }
        if (!screenStack.empty()) {
            g_fnt->RenderString(screenStack[currentScreen]->getName(), g_windowW - 200, g_windowH - 52);
        }
        

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
        SDL_Rect temp = {g_mouseX, g_mouseY, 4, 4};
        SDL_RenderFillRect(g_rd, &temp);

        //g_fnt->RenderString("voidsprite 19.03.2024", 0, 0, SDL_Color{ 255,255,255,0x30 });

        if (viewport != NULL){
            SDL_SetRenderTarget(g_rd, NULL);
            SDL_RenderCopy(g_rd, viewport, NULL, NULL);
        }

        SDL_RenderPresent(g_rd);
    }

    return 0;
}
