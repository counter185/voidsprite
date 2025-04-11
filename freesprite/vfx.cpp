#include "globals.h"
#include "vfx.h"
#include "Timer64.h"
#include "main.h"

class VFX {
private:
    VFXType type;
    Timer64 timer;
    u32 duration;
    //todo: clean this up
    u32 extData1;
    SDL_Rect extData2;
    std::vector<u32> extData3;

    std::function<double(double)> interpolation = &XM1PW3P1;
public:
    VFX(VFXType t, u32 dur, u32 ext1, SDL_Rect ext2, std::vector<u32> ext3) : type(t), duration(dur), extData1(ext1), extData2(ext2), extData3(ext3) {
        timer = Timer64();
        timer.start();
    }

    void render() {
        switch (type) {
            case VFX_SCREENCLOSE:
                {
                    double percent = interpolation(timer.percentElapsedTime(duration));
                    double percentHalfTime = interpolation(timer.percentElapsedTime(duration / 2));
                    SDL_Rect screenRect = { 0,0, g_windowW, g_windowH };
                    screenRect = offsetRect(screenRect, (g_windowW / -16.0) * percentHalfTime);
                    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 255 * (1.0 - percent));
                    SDL_RenderFillRect(g_rd, &screenRect);
                }
                break;
            case VFX_BUTTONPULSE:
                {
                    for (int x = 1; x < 4; x++) {
                        double percent = interpolation(timer.percentElapsedTime(duration / x));
                        SDL_Rect pulseRect = offsetRect(extData2, 1);
                        SDL_Color col = uint32ToSDLColor(extData1);
                        SDL_SetRenderDrawColor(g_rd, col.r, col.g, col.b, col.a * (1-percent));
                        pulseRect = offsetRect(pulseRect, 20 * percent, 15 * percent);
                        SDL_RenderDrawRect(g_rd, &pulseRect);
                    }
                }
                break;
            case VFX_SCREENSWITCH:
                {
                    double animTimer = interpolation(timer.percentElapsedTime(duration));
                    double reverseAnimTimer = 1.0 - animTimer;
                    SDL_Rect rect = { 0,0,g_windowW, g_windowH };
                    XY windowOffset = { g_windowW / 16 , g_windowH / 16 };
                    rect = offsetRect(rect, windowOffset.x * -reverseAnimTimer, windowOffset.y * -reverseAnimTimer);
                    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xd0 * reverseAnimTimer);
                    SDL_RenderDrawRect(g_rd, &rect);
                }
                break;
            case VFX_COLORPICKER:
                {
                    float progress = timer.percentElapsedTime(500);
                    int pxDistance = 40 * (extData3[0] == 1 ? interpolation(progress) : (1.0 - interpolation(progress)));

                    SDL_Rect colRect2 = { extData2.x, extData2.y, 1,1 };
                    int pxDistance2 = pxDistance + 1;
                    colRect2 = offsetRect(colRect2, pxDistance2);

                    for (int x = 3; x >= 1; x--) {
                        SDL_Rect colRect = { extData2.x, extData2.y, 1,1 };
                        int nowPxDistance = pxDistance / x;
                        colRect = offsetRect(colRect, nowPxDistance);
                        hsv thsv = rgb2hsv(u32ToRGB(extData1));
                        thsv.s /= 3;
                        thsv.v += 0.4;
                        thsv.v = dxmin(1.0, thsv.v);
                        rgb trgb = hsv2rgb(thsv);
                        SDL_Color trgbColor = rgb2sdlcolor(trgb);
                        SDL_SetRenderDrawColor(g_rd, trgbColor.r, trgbColor.g, trgbColor.b, (u8)(255 * interpolation(1.0f - progress)));
                        SDL_RenderDrawRect(g_rd, &colRect);
                    }
                }
                break;
        }
    }
    bool expired() {
        return timer.elapsedTime() > duration;
    }
};

std::vector<VFX> currentVfxs;

void g_newVFX(VFXType type, u32 durationMS, u32 extData1, SDL_Rect extData2, std::vector<u32> moreExtData)
{
    if (g_config.vfxEnabled) {
        currentVfxs.push_back(VFX(type, durationMS, extData1, extData2, moreExtData));
        if (type == VFX_SCREENSWITCH) {
            screenSwitchTimer.start();
        }
    }
}

void g_renderVFX() {
    if (g_config.vfxEnabled) {
        for (int i = 0; i < currentVfxs.size(); i++) {
            currentVfxs[i].render();
            if (currentVfxs[i].expired()) {
                currentVfxs.erase(currentVfxs.begin() + i);
                i--;
            }
        }
    }
}
