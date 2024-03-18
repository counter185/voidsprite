
#include "globals.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "BaseScreen.h"
#include "StartScreen.h"

int g_windowW = 1280;
int g_windowH = 720;
SDL_Window* g_wd;
SDL_Renderer* g_rd;
int g_mouseX = 0, g_mouseY = 0;
TextRenderer* g_fnt;

std::vector<BaseBrush*> g_brushes;

std::vector<BaseScreen*> screenStack;
void g_addScreen(BaseScreen* a) {
    screenStack.push_back(a);
}

int main(int argc, char** argv)
{
    platformPreInit();
    int canInit = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    IMG_Init(IMG_INIT_AVIF | IMG_INIT_JPG | IMG_INIT_JXL | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
    g_wd = SDL_CreateWindow("void\xE2\x97\x86sprite", 50, 50, g_windowW, g_windowH, SDL_WINDOW_RESIZABLE | (_WIN32 ? SDL_WINDOW_HIDDEN : 0));
    g_rd = SDL_CreateRenderer(g_wd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    platformInit();
    //SDL_CreateWindowAndRenderer(g_windowW, g_windowH, SDL_WINDOW_RESIZABLE, &g_wd, &g_rd);
    SDL_SetRenderDrawBlendMode(g_rd, SDL_BLENDMODE_BLEND);

    screenStack.push_back(new StartScreen());

    //load brushes
    g_brushes.push_back(new Brush1x1());
    g_brushes.push_back(new Brush3pxCircle());
    g_brushes.push_back(new Brush1pxLine());
    g_brushes.push_back(new BrushRect());
    for (BaseBrush*& brush : g_brushes) {
        SDL_Surface* srf = IMG_Load(brush->getIconPath().c_str());
        brush->cachedIcon = SDL_CreateTextureFromSurface(g_rd, srf);
        SDL_FreeSurface(srf);
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
                    delete screenStack.at(screenStack.size() - 1);
                    screenStack.pop_back();
                    if (screenStack.size() == 0) {
                        return 0;
                    }
                    break;
                case SDL_KEYDOWN:
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

            if (!screenStack.empty()) {
                screenStack[screenStack.size() - 1]->takeInput(evt);
            }
        }

        if (!screenStack.empty()) {
            screenStack[screenStack.size() - 1]->tick();
        }

        SDL_SetRenderDrawColor(g_rd, 0,0,0,255);
        SDL_RenderClear(g_rd);

        if (!screenStack.empty()) {
            screenStack[screenStack.size() - 1]->render();
        }

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
        SDL_Rect temp = {g_mouseX, g_mouseY, 4, 4};
        SDL_RenderFillRect(g_rd, &temp);

        g_fnt->RenderString("voidsprite 18.03.2024", 0, 0, SDL_Color{ 255,255,255,0x30 });

        SDL_RenderPresent(g_rd);
    }

    return 0;
}
