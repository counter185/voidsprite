// freesprite.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>

#include "globals.h"
#include "FontRenderer.h"
#include "maineditor.h"

int g_windowW = 960;
int g_windowH = 540;
SDL_Window* g_wd;
SDL_Renderer* g_rd;
int g_mouseX = 0, g_mouseY = 0;
TextRenderer* g_fnt;

int main(int argc, char** argv)
{
    int canInit = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    g_wd = SDL_CreateWindow("freesprite", 50, 50, g_windowW, g_windowH, SDL_WINDOW_RESIZABLE);
    g_rd = SDL_CreateRenderer(g_wd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //SDL_CreateWindowAndRenderer(g_windowW, g_windowH, SDL_WINDOW_RESIZABLE, &g_wd, &g_rd);
    SDL_SetRenderDrawBlendMode(g_rd, SDL_BLENDMODE_BLEND);

    MainEditor tempMainEditor(XY{ 640,480 });

    TTF_Init();
    g_fnt = new TextRenderer();

    SDL_Event evt;
    while (true) {
        while (SDL_PollEvent(&evt)) {
            switch (evt.type) {
                case SDL_QUIT:
                    return 0;
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

            tempMainEditor.TakeInput(evt);
        }

        SDL_SetRenderDrawColor(g_rd, 0,0,0,255);
        SDL_RenderClear(g_rd);

        tempMainEditor.Render();

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
        SDL_Rect temp = {g_mouseX, g_mouseY, 4, 4};
        SDL_RenderFillRect(g_rd, &temp);

        SDL_RenderPresent(g_rd);
    }

}
