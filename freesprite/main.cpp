
#include "globals.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "BaseScreen.h"
#include "StartScreen.h"
#include "BasePopup.h"

int g_windowW = 1280;
int g_windowH = 720;
SDL_Window* g_wd;
SDL_Renderer* g_rd;
int g_mouseX = 0, g_mouseY = 0;
TextRenderer* g_fnt;

SDL_Texture* g_mainlogo = NULL;
SDL_Texture* g_iconLayerAdd = NULL;
SDL_Texture* g_iconLayerDelete = NULL;

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
            }
            x--;
        }
    }
}

//do not use
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

int main(int argc, char** argv)
{
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

    screenStack.push_back(new StartScreen());

    //load brushes
    g_brushes.push_back(new Brush1x1());
    g_brushes.push_back(new Brush3pxCircle());
    g_brushes.push_back(new Brush1pxLine());
    g_brushes.push_back(new BrushRect());
    g_brushes.push_back(new BrushRectFill());
    g_brushes.push_back(new ToolColorPicker());
    g_brushes.push_back(new ToolRectClone());
    for (BaseBrush*& brush : g_brushes) {
        brush->cachedIcon = IMGLoadToTexture(brush->getIconPath());
    }

    //MainEditor tempMainEditor(XY{ 640,480 });

    TTF_Init();
    g_fnt = new TextRenderer();

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
                        }
                    }
                    else if (evt.key.keysym.sym == SDLK_RIGHTBRACKET) {
                        if (currentScreen < screenStack.size() - 1) {
                            currentScreen++;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    break;
                case SDL_WINDOWEVENT:
                    if (evt.window.event == SDL_WINDOWEVENT_RESIZED) {
                        g_windowW = evt.window.data1;
                        g_windowH = evt.window.data2;
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

        SDL_SetRenderDrawColor(g_rd, 0,0,0,255);
        SDL_RenderClear(g_rd);

        if (!screenStack.empty()) {
            screenStack[currentScreen]->render();
        }

        for (BasePopup*& popup : popupStack) {
            popup->render();
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

        SDL_RenderPresent(g_rd);
    }

    return 0;
}
